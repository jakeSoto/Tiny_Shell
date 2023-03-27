#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char ** getArgsList(char *buffer);
bool cmdChecker(char *argsList[]);
void executeCommand(char *argsList[]);

int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL;
    char **args_list;
    size_t bufferSize = 0;
    ssize_t nread;

    /* Interactive mode */
    while(1) {
        printf("tsh> ");
        if ((nread = getline(&buffer, &bufferSize, stdin)) == -1)
            exit(0);

        args_list = getArgsList(buffer);
        if (!cmdChecker(args_list))
            executeCommand(args_list);
    }

    free(buffer);
    free(args_list);
    return 0;
}


/*  Returns character array containing arguments
    from input seperated by a space */
char ** getArgsList(char *buffer) {
    int i = 0;
    static char *argsList[50];
    argsList[i] = strsep(&buffer, " \t\n");

    while (argsList[i] != NULL) {
        if (*argsList[i] != '\0')
            i++;
        argsList[i] = strsep(&buffer, " \t\n");
    }

    return argsList;
}

/* Checks input for built-in commands */
bool cmdChecker(char *argsList[]) {
    int i = 0;
    static char *commands[4] = {"exit\0", "cd\0", "path\0", "logging\0"};

    while (argsList[i] != NULL) {
        for (int j = 0; j < 4; j++) {
            if (strcmp(argsList[i], commands[j]) == 0) {
                if (j == 0)
                    exit(0);
                else if (j == 1) {
                    if (chdir(argsList[1]) == -1)
                        perror("Failed to change directories");
                    return true;
                }
            }
        }
        i++;
    }
    return false;
}

/* Executes input given from interactive mode 
   after forking to a child process */
void executeCommand(char *argsList[]) {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr,"Fork Failed\n");
		exit(0);
    }
    else if (pid == 0) {    // Child process
        int execStatus = execvp(argsList[0], argsList);
        if (execStatus == -1) {
            fprintf(stderr,"Exec Failed\n");
		    exit(0);
        }
    }
    else {                  // Parent process
        pid_t pid2 = wait(NULL);
        if (pid2 < 0) {
            fprintf(stderr,"Wait Failed\n");
		    exit(0);
        }
        fflush(stderr);
    }
}
