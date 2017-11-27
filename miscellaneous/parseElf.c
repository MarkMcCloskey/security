#include <gelf.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>

#include "parseElf.h"
#include "err.h"
#include "entropy.h"
#include "disasm.h"

#define debugParseElf 1
#define Debug(args...) if (debugParseElf){ printf("parseElf: "); printf(args);}


//need to verify format of binary
//5 classes of info: size in bytes of text sections, md5 hash of .txt section,
//entropy of the file, number of calls to dlopen()

/*------------------------
  PUBLIC FUNCTIONS
  ------------------------*/
ElfDetails *parseElf(char *binName){
	Debug("%s\n",binName);	
	GElf_Ehdr header;
	Elf *e = NULL;
	Elf_Scn *text = NULL, *dynStr = NULL, *dynSym = NULL 
		, *relPlt = NULL, *plt = NULL;
	Elf_Data *textSectionData = NULL, *dynamicStringData = NULL
		, *dynamicSymbolData = NULL, *relPltData = NULL
		, *pltData = NULL;	
	uint64_t dynStrSize = 0, dynSymSize = 0, relPltSize = 0, pltSize = 0;
	void *dynStrBuf = 0, *dynSymBuf = 0, *relPltBuf = 0, *pltBuf = 0;
	unsigned int textSectionAddr = 0, pltAddr = 0;
	extern unsigned int stringAddrs[];//make sure we can find stringAddrs here
	int fd;
	ElfDetails *deets = malloc(sizeof(ElfDetails));
	struct stat fileDetails;
	int dlopenNumber = 0, dlopenIndex = 0, dlopenAddr = 0;

	cs_insn *insn;
	int count;

	if (elf_version(EV_CURRENT) == EV_NONE){
		err("Elf library initialization failed.");		
	}

	if ((fd = open(binName,O_RDONLY,0)) < 0){
		err("Opening binary failed.");
	}else{
		Debug("binary file opened.\n");
	}

	//now that I have a file descriptor I can get the file size
	//for the entropy call
	if (fstat(fd, &fileDetails) < 0 ){
		err("fstat failed.\n");
	} else {
		Debug("file size %ld\n", fileDetails.st_size);
	}

	if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL){
		err("Elf_begin failed.");
	}else{
		Debug("elf began.\n");
	}

	if (elf_kind(e) != ELF_K_ELF){
		err("Binary is not an ELF object.");
	}else{
		Debug("Elf type confirmed.\n");
	}


	if (gelf_getehdr(e, &header) == NULL){
		err("Failure getting header.");
	}else{
		Debug("Got ELF header.\n");
	}


	if ((text = getSection(e,".text")) == NULL){
		err("Failed finding text section.");
	}
	if ((textSectionData = getSectionData(text)) == NULL){
		err("Text section data is null.");
	} else {
		textSectionAddr = getSectionStartAddress(text);
		Debug("Got text section data.\n");
		deets->sizeOfTextSection = getSectionSize(textSectionData);
		deets->textData = getSectionBuffer(textSectionData);
		deets->entropy = calculateEntropy(fd, fileDetails.st_size);	
	}

	//search .dynstr for the offset of dlopen
	if ((dynStr = getSection(e,".dynstr")) == NULL){
		err("Failed finding dynStr section.");
	}
	if ((dynamicStringData = getSectionData(dynStr)) == NULL){
		err("dynstr section data is null.");
	} else {
		Debug("Got dynstr section data.\n");
		dynStrSize = getSectionSize(dynamicStringData);
		Debug("dynStrSize: %lu\n",(unsigned long)dynStrSize);
		dynStrBuf = getSectionBuffer(dynamicStringData);
		Debug("dynStrBuf: %x\n", (unsigned int)dynStrBuf);			
		dlopenNumber = getDlopenNumber(dynStrBuf, dynStrSize);
	}

	//search .dynsym for the index of dlopen in .rel.plt
	if ((dynSym = getSection(e,".dynsym")) == NULL){
		err("Failed finding dynSym section.");
	}
	if ((dynamicSymbolData = getSectionData(dynSym)) == NULL){
		err("Dynsym section data is null.");
	} else if (dlopenNumber >= 0){
		Debug("Got dynsym section data.\n");
		dynSymSize = getSectionSize(dynamicSymbolData);
		Debug("dynSymSize: %lu\n",(unsigned long)dynSymSize);
		dynSymBuf = getSectionBuffer(dynamicSymbolData);
		Debug("dynSymBuf: %x\n", (unsigned int)dynSymBuf);
		dlopenIndex = parseDynSym(dynSymBuf, dynSymSize, dlopenNumber);	
		Debug("Dynamic Symbol index is: %d\n",dlopenNumber);
	}

	//search .rel.plt for the address of dlopen in .plt
	if ((relPlt = getSection(e,".rel.plt")) == NULL){
		err("Failed finding relPlt section");
	}
	if ((relPltData = getSectionData(relPlt)) == NULL){
		err(".rel.plt. data is null.\n");
	} else if (dlopenIndex >= 0){
		Debug("got .rel.plt section data.\n");
		relPltSize = getSectionSize(relPltData);
		Debug("relPltSize: %lu\n", (unsigned long)relPltSize);
		relPltBuf = getSectionBuffer(relPltData);
		Debug("relPltBuf: %x\n", (unsigned int)relPltBuf);	
		if ((dlopenAddr = parseRelPlt(relPltBuf, relPltSize, dlopenIndex)) >= 0){
			Debug("dlopenAddr: %x\n", (unsigned int)dlopenAddr);
		} else {
			err("dlopenAddr not found");
		}
	}

	//search .plt for the address used to call dlopen from .text
	if ((plt = getSection(e,".plt")) == NULL){
		err("Failed finding plt section.");
	}else{
		pltAddr = getSectionStartAddress(plt);
	}
	if ((pltData = getSectionData(plt)) == NULL){
		err(".plt data is null.\n");
	} else if (dlopenAddr >= 0){
		Debug("got .plt section data.\n");
		pltSize = getSectionSize(pltData);
		Debug("pltSize: %lu\n", (unsigned long)pltSize);
		pltBuf = getSectionBuffer(pltData);
		Debug("pltBuf: %x\n", (unsigned int)pltBuf);

		count = disasm(pltBuf, pltSize, pltAddr, &insn);				
		dlopenAddr = findDlopenAddr(insn, count, dlopenAddr);
		cs_free(insn, count);
	}

	//disassemble .text and count the number of dlopen calls made
	count = disasm(deets->textData, deets->sizeOfTextSection, textSectionAddr, &insn);
	deets->numDlopenCalls = countDlopens(insn, count, dlopenAddr);
	cs_free(insn,count);
	
	//resolve strings
	Elf_Scn *ro = NULL;
	Elf_Data *rodata = NULL;	
	uint64_t rodataSize = 0;
	void *rodataBuf = 0;
	unsigned int rodataAddr = 0;

	ro = getSection(e, ".rodata");
	rodata = getSectionData(ro);
	rodataSize = getSectionSize(rodata);	
	rodataBuf = getSectionBuffer(rodata);
	rodataAddr = getSectionStartAddress(ro);
	GElf_Shdr rodataHdr;

	gelf_getshdr(ro,&rodataHdr);
	if (rodataHdr.sh_type == SHT_STRTAB){
		Debug("rodata has STRTAB type.\n");
	}
	
	int j = 0, k = 0, inc = 0;
	char *st = rodataBuf;
	
	//scan through rodata	
	while (st < (char*)(rodataBuf + rodataSize)){
		j = 0;
		//at every address check for a match for one passed to dlopen
		while (stringAddrs[j]){
			//if there is one
			if (stringAddrs[j++] == (unsigned int)rodataAddr + k){
				
				inc = 0;
				//find an open pointer to store the string at
				while(deets->strings[inc]){
					//if we've already stored the string don't do so again
					if (strcmp(deets->strings[inc],st) == 0){
						break;
					}	
					inc++;
				}
				//create space for the new string to be copied to
				deets->strings[inc] = malloc(strlen(st) + 1);
				strcpy(deets->strings[inc], st);
				Debug("string stored in deets: %s\n",deets->strings[inc]);	
			}
		}
		st++;
		k++;
	}

	close(fd);
	return deets;
}

/*-----------------------
  PRIVATE FUNCTIONS
  ------------------------*/
int createFileBuffer(char *buffer, ElfDetails *deets){
	int size = 0, savedSize = 0;
	int i = 0, j = 0, temp = 0;
	
	if (deets == NULL){
		err("No information to save.");
	}
	
	SaveFile *bufPtr = (SaveFile*)buffer;
	size += sizeof(bufPtr->size);
	
	bufPtr->sizeOfTextSection = deets->sizeOfTextSection;
	size += sizeof(bufPtr->sizeOfTextSection);
	
	memcpy(bufPtr->hash, deets->md5Hash, MD5_DIGEST_LENGTH);
	size += MD5_DIGEST_LENGTH;
	
	bufPtr->numDlopenCalls = deets->numDlopenCalls;
	size += sizeof(bufPtr->numDlopenCalls);
	
	bufPtr->entropy = deets->entropy;
	size += sizeof(bufPtr->entropy);
	Debug("At this point size is %d\n", size);
	savedSize = size;
	while (deets->strings[i] && i < NUM_STRING_ADDRS){
		temp = strlen(deets->strings[i]) + 1;
		Debug("About to copy string to bufPtr\n");
		strcpy(&buffer[size + j], deets->strings[i]);
		Debug("copied string into bufPtr.\n");
		size += temp;
		j += temp;
		i++;
	}
	
	bufPtr->size = size;
	if (debugParseElf){
		printf("saveFile size: %d\n", bufPtr->size);
		printf("saveFile sizeOfTextSec: %ld\n",bufPtr->sizeOfTextSection);
		
		for (i = 0; i < MD5_DIGEST_LENGTH; i++){
			printf("%x",bufPtr->hash[i]);
		}
		printf("\n");
		printf("saveFile numDlopenCalls: %ld\n", bufPtr->numDlopenCalls);
		printf("saveFile entropy: %lf\n", bufPtr->entropy);
		printf("saveFile strings: %s\n", &buffer[savedSize]);
	}

	return size;
}

void fuzzFile(char *buffer, int size){
	char fuzz = 0xA5;
	int i = 0;
	for (i = 0; i < size; i++){
		buffer[i] = buffer[i] ^ fuzz;
	}	
}

void saveFile(char *buffer, int size){
	FILE *outfile;

	outfile = fopen("elfData.bin","ab+");
	fwrite(buffer, sizeof(char), size, outfile);
	fflush(outfile);
	fclose(outfile);

}

Elf_Scn *getSection(Elf *elf, char *wantedSection){
	Debug("getSection called.\n");
	Elf_Scn *section = NULL;
	GElf_Shdr sectionHeader;
	//void *textSectionData = NULL;
	char *sectionName = NULL;
	size_t sectionHeaderStrIdx = 0;

	if (elf_getshdrstrndx(elf, &sectionHeaderStrIdx) != 0){
		err("Getting section header str index failed.");
	}

	while((section = elf_nextscn(elf,section)) != NULL){

		if (gelf_getshdr(section, &sectionHeader) != &sectionHeader){
			err("Getting section header failed.");
		}

		if((sectionName = elf_strptr(elf, sectionHeaderStrIdx
						, sectionHeader.sh_name)) == NULL){
			err("Getting string pointer failed.");
		} else {

			//Debug("Found section: %s\n",sectionName);
		}

		//stop looking when we find the wanted section
		if (strcmp(sectionName, wantedSection) == 0 ){

			Debug("Found section: %s\n",sectionName);
			Debug("Section address: %x\n", (unsigned int)sectionHeader.sh_addr);
			return section;
		}	

	}
	//return null pointer if fail
	return NULL;	

}

Elf_Data *getSectionData(Elf_Scn *section){
	Elf_Data *elfData = NULL;
	Debug("getSectionData called.\n");
	elfData = elf_getdata(section,elfData);
	return elfData;

}		

unsigned int getSectionStartAddress(Elf_Scn *section){
	GElf_Shdr sectionHeader;

	if (gelf_getshdr(section, &sectionHeader) != &sectionHeader){
		err("Getting section header failed.");
	}
	return sectionHeader.sh_addr;
}

void *getSectionBuffer(Elf_Data *data){
	Debug("getSectionStartAddress called.\n");
	void *textData = 0;

	if ((textData = data->d_buf) == 0){
		err("Failure getting text section data.");
	} else {
		Debug("Pointer to text data retrieved.\n");
	}
	return textData;	
}

uint64_t getSectionSize(Elf_Data *data){
	Debug("getSectionSize called.\n");
	uint64_t size = 0;

	if (data == NULL){
		err("getSectionSize called with NULL pointer.");
	}
	size = data->d_size;
	Debug("getSectionSize found size of %" PRIu64 "\n",size);
	return size;
}

void destroyElfDetails(ElfDetails *deets){
	Debug("destroyElfDetails called.\n");
	int j = 0;
	while(deets->strings[j] && j < NUM_STRING_ADDRS){
		free(deets->strings[j++]);
	}
	
	free(deets);

}

/* returns the offset of dlopen if successful or -1 if failure.*/
int getDlopenNumber(void *sectionData, uint64_t sectionSize){
	Debug("getDlopenNumber called with address %lu and size %lu\n", (unsigned long)sectionData, (unsigned long)sectionSize);
	unsigned int i = 0, temp = 0;

	while (i < sectionSize){
		temp = strlen(sectionData + i);

		if (temp == 0){
			i++;
		} else {
			Debug("%d %s\n",i, (char*)(sectionData + i));
			if (strcmp(sectionData + i, "dlopen") == 0){
				Debug("dlopen found with offset: %d\n", i);
				return i;
			}	
			i += temp;
		}
	}

	//dlopen not found return fail
	return -1;
}

/*returns the index of dlopen in the dynamic symbol table or -1 if not found. */
int parseDynSym(void *sectionData, uint64_t sectionSize, unsigned int dlopenNum){
	Debug("parseDynSym called with address: %lu size: %lu dlopenNum: %d\n"
			, (unsigned long)sectionData, (unsigned long)sectionSize, dlopenNum);
	Elf32_Sym *symbol = (Elf32_Sym*)sectionData;
	Elf32_Sym *endAddress = sectionData + sectionSize;	
	Debug("Size of Sym struct: %x\n",sizeof(*symbol));
	int index = 0;

	while(symbol < endAddress){
		Debug("Symbol %x, sectiondata+sectionSize %x\n",(unsigned int)symbol,(unsigned int) (sectionData + sectionSize));
		Debug("Symbol name: %x, value: %x\n",symbol->st_name, symbol->st_value);

		if (symbol->st_name == dlopenNum){
			return index;
		}

		symbol++;
		index++;
		Debug("New symbol %x\n",(unsigned int) symbol);	
	}
	//fail if not found	
	return -1;
}

/* returns the address of dlopen in .got.plt if found or -1 if fail. */
int parseRelPlt(void *sectionData, uint64_t sectionSize, unsigned int index){
	Debug("parseDynSym called with address: %lu size: %lu index: %d\n"
			, (unsigned long)sectionData, (unsigned long)sectionSize, index);
	Elf32_Rel *relo = (Elf32_Rel*)sectionData;
	Elf32_Rel *endAddr = relo + sectionSize;

	while (relo < endAddr){
		Debug("relo offset: %x, relo info: %x sym: %x\n",(unsigned int)relo->r_offset
				,(unsigned int) relo->r_info, (unsigned int)ELF32_R_SYM(relo->r_info));

		if (ELF32_R_SYM(relo->r_info) == index){
			return relo->r_offset;
		}
		relo++;
	}
	return -1;
}

void printElf(ElfDetails *deets){
	printf("Size of text section: %lu\n", (unsigned long)deets->sizeOfTextSection);
	printf("Number of dlopen calls: %lu\n", (unsigned long)deets->numDlopenCalls);
	printf("Entropy of file: %lf\n", deets->entropy);
	int i = 0;
	printf("Strings passed to dlopen:\n");
	while (deets->strings[i]){
		printf("%s\n",deets->strings[i++]);
	}
}

