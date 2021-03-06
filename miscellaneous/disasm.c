#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "disasm.h"
#include "err.h"

#define debugDisasm 0
#define Debug(args...) if (debugDisasm){ printf("disasm: "); printf(args);}
//static void print_insn_detail(csh ud, cs_mode mode, cs_insn *ins);
//static void print_string_hex(char *comment, unsigned char *str, size_t len);
//static csh handle;

unsigned int stringAddrs[NUM_STRING_ADDRS];

/* Disasm will take as input a pointer to the code to be disassembled, an int that is the size of the code,
the address where the code starts, and a the address of a cs_insn structure where the instructions should be stored.
It will then dissasemble the code and return the number of instructions. Don't forget to cs_free(insn, numInstructions)*/
int disasm(unsigned char *code, unsigned int codeSize, int codeStartAddress, cs_insn **insn){
	Debug("Disasm called with code %x and codeSize %i\n",(unsigned int)code, codeSize);
	csh handle = 0;
	size_t count = 0;
	if(cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK){
		err("cs open failed");
	}
	cs_option(handle,CS_OPT_DETAIL, CS_OPT_ON);
	count = cs_disasm(handle,code, codeSize, codeStartAddress,0,insn);
	return count;

}

/* findDlopenAddr takes as input a cs_insn pointer to an instruction array, size_t count which is the
number of instructions, and the address of dlopenAddr. It will return the address used to call dlopen
in the .text section or -1 on error.*/
int findDlopenAddr(cs_insn *insn, size_t count, int dlopenAddr){
	Debug("findDlopenAddr called with count %d dlopenAddr: %x\n", (int)count, (unsigned int)dlopenAddr);
	size_t j = 0;
	char hex[9] = "";
	sprintf(hex, "%x",dlopenAddr);
	Debug("hex string: %s\n",hex);
	
	for (j = 0; j < count; j++){
		if (strstr(insn[j].op_str, hex) != NULL){
			Debug("findDlopenAddr returning %x\n",(unsigned int)insn[j].address);
			return insn[j].address;
		}
	}
	return -1;
}

//REQUIREMENT 7
/* countDlopens takes as input a cs_insn array holding the dissasembly of the .text section, size_t count
which is the number of instructions in the array, and an int that is the address of the dlopen call in the
.text section. It will return an int the number of times dlopen is called in the .text section. */
int countDlopens(cs_insn *insn, size_t count, int dlopenAddr/*, unsigned int *strAddrs*/){
	Debug("countDlopens called with count: %d dlopenAddr: %x\n", count, (unsigned int)dlopenAddr); 
	size_t j, i = 0;
	int numDlopens = 0;
	char hex[9] = "";
	char *stringStr = NULL;
	unsigned int stringAddr;
	sprintf(hex, "%x",dlopenAddr);
	Debug("hex string: %s\n",hex);
	//strAddrs = stringAddrs;//point the passed pointer to the array of string addresses		
	for (j = 0; j < count; j++){
		if (strstr(insn[j].op_str, hex) != NULL){
			numDlopens++;
			stringStr = strstr(insn[j-1].op_str,"0x");
			Debug("String string is %s\n", stringStr);
			stringAddr = strtol(stringStr, NULL, 16);
			Debug("Hex string addr is %s\n", (char*)stringAddr);
			Debug("StringAddr is %x\n",stringAddr);
			stringAddrs[i++] = stringAddr;//store string addr for later resolution
		}
	}
	Debug("countDlopens returning %d\n",numDlopens);
	return numDlopens;
}


/* pulled from the Captsone-Engine source code to print detailed instructions.*/
/*
static void print_insn_detail(csh ud, cs_mode mode, cs_insn *ins)
{

	int count, i;
	cs_x86 *x86;
	// detail can be NULL on "data" instruction if SKIPDATA option is turned ON
	if (ins->detail == NULL)
		return;

	x86 = &(ins->detail->x86);
	print_string_hex("\tPrefix:", x86->prefix, 4);
	print_string_hex("\tOpcode:", x86->opcode, 4);
	printf("\trex: 0x%x\n", x86->rex);
	printf("\taddr_size: %u\n", x86->addr_size);
	printf("\tmodrm: 0x%x\n", x86->modrm);
	printf("\tdisp: 0x%x\n", x86->disp);

	// SIB is not available in 16-bit mode
	if ((mode & CS_MODE_16) == 0) {
		printf("\tsib: 0x%x\n", x86->sib);
		if (x86->sib_base != X86_REG_INVALID)
			printf("\t\tsib_base: %s\n", cs_reg_name(handle, x86->sib_base));
		if (x86->sib_index != X86_REG_INVALID)
			printf("\t\tsib_index: %s\n", cs_reg_name(handle, x86->sib_index));
		if (x86->sib_scale != 0)
			printf("\t\tsib_scale: %d\n", x86->sib_scale);
	}

	// SSE code condition
	if (x86->sse_cc != X86_SSE_CC_INVALID) {
		printf("\tsse_cc: %u\n", x86->sse_cc);
	}

	// AVX code condition
	if (x86->avx_cc != X86_AVX_CC_INVALID) {
		printf("\tavx_cc: %u\n", x86->avx_cc);
	}

	// AVX Suppress All Exception
	if (x86->avx_sae) {
		printf("\tavx_sae: %u\n", x86->avx_sae);
	}
	
	// AVX Rounding Mode
	if (x86->avx_rm != X86_AVX_RM_INVALID) {
		printf("\tavx_rm: %u\n", x86->avx_rm);
	}

	count = cs_op_count(ud, ins, X86_OP_IMM);
	if (count) {
		printf("\timm_count: %u\n", count);
		for (i = 1; i < count + 1; i++) {
			int index = cs_op_index(ud, ins, X86_OP_IMM, i);
			printf("\t\timms[%u]: 0x%" PRIx64 "\n", i, x86->operands[index].imm);
		}
	}

	if (x86->op_count)
		printf("\top_count: %u\n", x86->op_count);
	for (i = 0; i < x86->op_count; i++) {
		cs_x86_op *op = &(x86->operands[i]);
		switch((int)op->type) {
			case X86_OP_REG:
				printf("\t\toperands[%u].type: REG = %s\n", i, cs_reg_name(handle, op->reg));
				break;
			case X86_OP_IMM:
				printf("\t\toperands[%u].type: IMM = 0x%" PRIx64 "\n", i, op->imm);
				break;
			case X86_OP_MEM:
				printf("\t\toperands[%u].type: MEM\n", i);
				if (op->mem.segment != X86_REG_INVALID)
					printf("\t\t\toperands[%u].mem.segment: REG = %s\n", i, cs_reg_name(handle, op->mem.segment));
				if (op->mem.base != X86_REG_INVALID)
					printf("\t\t\toperands[%u].mem.base: REG = %s\n", i, cs_reg_name(handle, op->mem.base));
				if (op->mem.index != X86_REG_INVALID)
					printf("\t\t\toperands[%u].mem.index: REG = %s\n", i, cs_reg_name(handle, op->mem.index));
				if (op->mem.scale != 1)
					printf("\t\t\toperands[%u].mem.scale: %u\n", i, op->mem.scale);
				if (op->mem.disp != 0)
					printf("\t\t\toperands[%u].mem.disp: 0x%" PRIx64 "\n", i, op->mem.disp);
				break;
			default:
				break;
		}

		// AVX broadcast type
		if (op->avx_bcast != X86_AVX_BCAST_INVALID)
			printf("\t\toperands[%u].avx_bcast: %u\n", i, op->avx_bcast);
		// AVX zero opmask {z}
		if (op->avx_zero_opmask != false)
			printf("\t\toperands[%u].avx_zero_opmask: TRUE\n", i);
		printf("\t\toperands[%u].size: %u\n", i, op->size);
	}
	printf("\n");
}
*/

/* Pulled from the Capstone-Engine source code because print_insn_detail needed it.*/
/*
static void print_string_hex(char *comment, unsigned char *str, size_t len)
{
	unsigned char *c;
	printf("%s", comment);
	for (c = str; c < str + len; c++) {
		printf("0x%02x ", *c & 0xff);
	}
	printf("\n");
}*/
