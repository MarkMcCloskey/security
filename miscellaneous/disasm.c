#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "disasm.h"
#include "err.h"

#define debug 1
#define Debug(args...) if (debug) printf("disasm: "); printf(args);

int disasm(unsigned char *code, unsigned int codeSize, int dlopenAddr){
	Debug("Disasm called with code %x and codeSize %i\n",(unsigned int)code, codeSize);
	csh handle;
	cs_insn *insn;
	size_t count;
	int numDlopens = 0;
	if(cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK){
		err("cs open failed");
	}

	count = cs_disasm(handle,code, codeSize, 0,0,&insn);
	numDlopens = countDlopens(insn, count, dlopenAddr);
	/*
	if (count > 0){
		size_t j;
		for (j = 0; j < count; j++){
			//printf("0x%"PRIx32":\t%s\t\t%s\n",(unsigned int)insn[j].address, insn[j].mnemonic
			//	, insn[j].op_str);
				
			printf("%s\n",insn[j].op_str);
		}
		cs_free(insn,count);	
	} else {
		err("cs_disasm failed.");
	}*/
	cs_free(insn,count);
	cs_close(&handle);
	return numDlopens;

}

int countDlopens(cs_insn *insn, size_t count, int dlopenAddr){
	Debug("countDlopens called with count: %d dlopenAddr: %x\n", count, (unsigned int)dlopenAddr); 
	size_t j;
	int numDlopens = 0;
	char hex[9];
	sprintf(hex, "%x",dlopenAddr);
	Debug("hex string: %s\n",hex);
	
	for (j = 0; j < count; j++){
		if (strstr(insn[j].op_str, hex) != NULL){
			numDlopens++;
		}
	}
	Debug("countDlopens returning %d\n",numDlopens);
	return numDlopens;
}

