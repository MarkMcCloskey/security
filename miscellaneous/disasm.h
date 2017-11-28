//REQUIREMENT 15
#include <capstone/capstone.h>//here to squelch compiler complaints
#define NUM_STRING_ADDRS 1024//this  should be enough for most binaries in this project would be better dynamically allocated
int disasm(unsigned char *code, unsigned int codeSize, int codeStartAddress, cs_insn **insn);
int countDlopens(cs_insn *insn, size_t count, int dlopenAddr);
int findDlopenAddr(cs_insn *insn, size_t count, int dlopenAddr);
