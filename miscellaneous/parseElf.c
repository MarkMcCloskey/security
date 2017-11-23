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

#define debug 1
#define Debug(args...) if (debug) printf("parseElf: "); printf(args);


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
	Elf_Data *textSectionData = NULL, *dynamicStringData = NULL
		, *dynamicSymbolData = NULL, *relPltData = NULL
		, *plt = NULL;	
	uint64_t dynStrSize = 0, dynSymSize = 0, relPltSize = 0, pltSize = 0;
	void *dynStrAddr = 0, *dynSymAddr = 0, *relPltAddr = 0, *pltAddr = 0;
	int fd;
	ElfDetails *deets = malloc(sizeof(ElfDetails));
	struct stat fileDetails;
	int dlopenNumber = 0, dlopenIndex = 0, dlopenAddr = 0;
	
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
	
	if ((textSectionData = getSectionData(e ,".text")) == NULL){
		err("Text section data is null.");
	} else {
		Debug("Got text section data.\n");
	}
	
	if ((dynamicStringData = getSectionData(e ,".dynstr")) == NULL){
		err("dynstr section data is null.");
	} else {
		Debug("Got dynstr section data.\n");
		dynStrSize = getSectionSize(dynamicStringData);
		Debug("dynStrSize: %lu\n",(unsigned long)dynStrSize);
		dynStrAddr = getSectionStartAddress(dynamicStringData);
		Debug("dynStrAddr: %x\n", (unsigned int)dynStrAddr);			
		dlopenNumber = getDlopenNumber(dynStrAddr, dynStrSize);
	}

	if ((dynamicSymbolData = getSectionData(e ,".dynsym")) == NULL){
		err("Dynsym section data is null.");
	} else if (dlopenNumber >= 0){
		Debug("Got dynsym section data.\n");
		dynSymSize = getSectionSize(dynamicSymbolData);
		Debug("dynSymSize: %lu\n",(unsigned long)dynSymSize);
		dynSymAddr = getSectionStartAddress(dynamicSymbolData);
		Debug("dynSymAddr: %x\n", (unsigned int)dynSymAddr);
		dlopenIndex = parseDynSym(dynSymAddr, dynSymSize, dlopenNumber);	
		Debug("Dynamic Symbol index is: %d\n",dlopenNumber);
	}
	
	if ((relPltData = getSectionData(e,".rel.plt")) == NULL){
		err(".rel.plt. data is null.\n");
	} else if (dlopenIndex >= 0){
		Debug("got .rel.plt section data.\n");
		relPltSize = getSectionSize(relPltData);
		Debug("relPltSize: %lu\n", (unsigned long)relPltSize);
		relPltAddr = getSectionStartAddress(relPltData);
		Debug("relPltAddr: %x\n", (unsigned int)relPltAddr);	
		if ((dlopenAddr = parseRelPlt(relPltAddr, relPltSize, dlopenIndex)) >= 0){
			Debug("dlopenAddr: %x\n", (unsigned int)dlopenAddr);
		} else {
			err("dlopenAddr not found");
		}
	}

	if ((plt = getSectionData(e,".plt")) == NULL){
		err(".plt data is null.\n");
	} else if (dlopenAddr >= 0){
		Debug("got .plt section data.\n");
		pltSize = getSectionSize(plt);
		Debug("pltSize: %lu\n", (unsigned long)pltSize);
		pltAddr = getSectionStartAddress(plt);
		Debug("pltAddr: %x\n", (unsigned int)pltAddr);
		deets->numDlopenCalls = disasm(pltAddr, pltSize, dlopenAddr);				
	}
	

	
	deets->sizeOfTextSection = getSectionSize(textSectionData);
	deets->textData = getSectionStartAddress(textSectionData);
	deets->entropy = calculateEntropy(deets->textData, fileDetails.st_size);		
	close(fd);
	return deets;
}

/*-----------------------
PRIVATE FUNCTIONS
------------------------*/
Elf_Data *getSectionData(Elf *elf, char *wantedSection){
	Debug("getSectionData called.\n");
	
	Elf_Scn *section = NULL;
	Elf_Data *elfData = NULL;
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
	
		//stop looking when we find the text section
		if (strcmp(sectionName, wantedSection) == 0 ){
			
			Debug("Found section: %s\n",sectionName);
			elfData = elf_getdata(section,elfData);
			return elfData;
		}	

	}
	//return null pointer if fail
	return NULL;	
}		

void *getSectionStartAddress(Elf_Data *data){
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
		err("getTextSectionSize called with NULL pointer.");
	}
	size = data->d_size;
	Debug("getTextSectionSize found size of %" PRIu64 "\n",size);
	return size;
}

void destroyElfDetails(ElfDetails *deets){
	Debug("destroyElfDetails called.\n");
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
