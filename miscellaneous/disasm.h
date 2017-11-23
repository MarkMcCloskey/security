#include <capstone/capstone.h>//here to squelch compiler complaints
int disasm(unsigned char *code, unsigned int codeSize, int dlopenAddr);
int countDlopens(cs_insn *insn, size_t count, int dlopenAddr);
