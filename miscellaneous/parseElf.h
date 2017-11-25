#include <libelf.h>

typedef struct elfDetails {
	uint64_t sizeOfTextSection;
 	void  *textData;
	unsigned char *md5Hash;
	uint64_t numDlopenCalls;
	double entropy;
} ElfDetails;

ElfDetails *parseElf(char *binaryName);
int getDlopenNumber(void *sectionData, uint64_t sectionSize);
Elf_Scn *getSection(Elf *elf, char *wantedSection);
void *getSectionBuffer(Elf_Data *data);
Elf_Data *getSectionData(Elf_Scn *section);
unsigned int getSectionStartAddress(Elf_Scn *section);
uint64_t getSectionSize(Elf_Data *data);
void destroyElfDetails(ElfDetails *deets);
int parseDynSym(void *sectionData, uint64_t sectionSize, unsigned int dlopenNum);
int parseRelPlt(void *sectionData, uint64_t sectionSize, unsigned int index);
void printElf(ElfDetails *deets);
