#ifndef MAIN
#define MAIN

char *check_path(char *command);
char *getlineterminal();
char **commandss(char *ch);
void execute_commands(char **commands);
void change_directory(char **commands);
int main();

#endif