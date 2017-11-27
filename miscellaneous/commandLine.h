#define ANALYZE_BINARY 1
#define SAVE_INFO 2
#define EXIT 3
#define FAKE_STRING_LENGTH 4
#define TRUE_STRING_LENGTH 1023
#define BINARY_NAME_LENGTH 1023
void clearInput();
char *getBinaryName();
void getPassword();
void getUsername();
char *parseCommandLine(int *argc, char *argv[]);
void printHelp();
void printPrompt();
int promptUser();
void validateUsernamePassword();
