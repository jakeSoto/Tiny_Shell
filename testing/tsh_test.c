#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *myInitials = "JS";
bool loggingFlag = false;
FILE *logFile = NULL;
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

/* Prints error message with the user's initials */
void printErrorMessage(char *userInitials) {
    char error_message[30] = "An error has occurred (from ";
    strcat(error_message, userInitials);
    strcat(error_message, ")\n");
    write(STDERR_FILENO, error_message, strlen(error_message));
}

/*  Returns character array containing arguments
    from input seperated by white space */
char ** getArgsList(char *buffer) {
    int i = 0;
    static char *argsList[50];

    if (loggingFlag)
        fputs(buffer, logFile);

    argsList[i] = strsep(&buffer, " :\t\n");
    while (argsList[i] != NULL) {
        if (*argsList[i] != '\0')
            i++;
        argsList[i] = strsep(&buffer, " :\t\n");
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
                if (j == 0)             // exit
                    exit(0);
                else if (j == 1) {      // change directory
                    if (chdir(argsList[1]) == -1)
                        printErrorMessage(myInitials);
                    return true;
                }
                else if (j == 2) {      // path
                    static char *path[1024] = {"/bin"};
                    int pathCounter = 0;

                    if (argsList[i+1] != NULL) {
                        do {
                            pathCounter++;
                            path[pathCounter] = argsList[i++];
                        } while (argsList[i] != NULL);
                    }
                    else {
                        pathCounter = 0;
                        printf("path is set to ");
                        while (path[pathCounter] != NULL) {
                            printf("%s:", path[pathCounter]);
                            pathCounter++;
                        }
                        printf("\n");
                    }
                    
                    return true;
                }
                else if (j == 3) {      // logging
                    if (argsList[i+1] == NULL)
                        return false;
                    else if (strcmp(argsList[i+1], "off\0") == 0) {
                        loggingFlag = false;
                        
                        if (logFile == NULL)
                            return true;
                        else if (fclose(logFile) != 0)
                            printErrorMessage(myInitials);
                    }
                    else {
                        logFile = fopen(argsList[i+1], "w");
                        loggingFlag = true;
                    }
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

    if (pid < 0)
        printErrorMessage(myInitials);
    else if (pid == 0) {    // Child process
        int execStatus = execvp(argsList[0], argsList);
        if (execStatus == -1)
            printErrorMessage(myInitials);
    }
    else {                  // Parent process
        pid_t pid2 = wait(NULL);
        if (pid2 < 0)
            printErrorMessage(myInitials);
    }
}
