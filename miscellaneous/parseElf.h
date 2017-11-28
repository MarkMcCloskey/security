//REQUIREMENT 13
#include <libelf.h>
#include <openssl/md5.h>
#define NUM_STRING_ADDRS 1024
typedef struct elfDetails {
	uint16_t machineType;
	uint64_t sizeOfTextSection;
 	void  *textData;
	unsigned char *md5Hash;
	uint64_t numDlopenCalls;
	double entropy;
	char *strings[NUM_STRING_ADDRS];
} ElfDetails;
//REQUIREMENT 8
typedef struct saveFile {
	int size;
	long sizeOfTextSection;
	uint16_t machineType;
	unsigned char hash[MD5_DIGEST_LENGTH];
	long numDlopenCalls;
	double entropy;
	char *strings;
} SaveFile;

void saveFile(char *buffer, int size);
int createFileBuffer(char *buffer, ElfDetails *deets);
ElfDetails *parseElf(char *binaryName);
void fuzzFile(char *buffer, int size);
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
