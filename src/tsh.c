#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Global Variables */
char *myInitials = "JS";
bool loggingFlag = false;
FILE *logFile = NULL;

/* Function Definitions */
void printErrorMessage(char *userInitials);
char ** getArgsList(char *buffer);
bool cmdChecker(char *argsList[]);
void cmdExecutor(int cmd, char *argsList[]);
void executeCommand(char *argsList[]);


int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL;
    char **args_list;
    size_t bufferSize = 0;
    ssize_t nread;

    if (argv[1] != NULL) {   /* Batch Mode */
        infile = fopen(argv[1], "r");
        if (infile == NULL || argc > 2) {
            printErrorMessage(myInitials);
            exit(0);
        }

        while(1) {
            if ((nread = getline(&buffer, &bufferSize, infile)) == -1)
                exit(0);

            args_list = getArgsList(buffer);
            if (!cmdChecker(args_list))
                executeCommand(args_list);
        }

        fclose(infile);
        exit(0);
    }
    else {              /* Interactive mode */
        while(1) {
            printf("tsh> ");
            if ((nread = getline(&buffer, &bufferSize, stdin)) == -1)
                exit(0);

            args_list = getArgsList(buffer);
            if (!cmdChecker(args_list))
                executeCommand(args_list);
        }
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
    argsList[i] = strsep(&buffer, " \t\n");

    while (argsList[i] != NULL) {
        if (*argsList[i] != '\0') {
            if (loggingFlag == true) {
                fputs(argsList[i], logFile);
                fputs("\n", logFile);
            }
            i++;
        }
        argsList[i] = strsep(&buffer, " \t\n");
    }

    return argsList;
}

/* Checks input for built-in commands */
bool cmdChecker(char *argsList[]) {
    static char *commands[4] = {"exit\0", "cd\0", "path\0", "logging\0"};
    for (int i = 0; i < 4; i++) {
        if (strcmp(argsList[0], commands[i]) == 0) {
            cmdExecutor(i, argsList);
            return true;
        }
    }
    return false;
}

/* Checks input for built-in commands */
void cmdExecutor(int cmd, char *argsList[]) {
    if (cmd == 0) {            // exit
        if (loggingFlag == true) {
            if (fclose(logFile) != 0)
                printErrorMessage(myInitials);

            printf("file closed\n");
        }
        exit(0);
    }
    else if (cmd == 1) {      // change directory
        if (chdir(argsList[1]) == -1)
            printErrorMessage(myInitials);
    }
    else if (cmd == 2) {        // path
        static char *path[1024] = {"/bin", NULL};
        
        if (argsList[1] == NULL) {  //no args
            int i = 0;
            printf("Path is set to ");
            while (path[i] != NULL) {
                printf("%s:", path[i]);
                i++;
            }
            printf("\n");
        }
        else {  //args
            //path = strsep(&argsList[1], " :\t\n");
        }
    }
    else if (cmd == 3) {        // logging
        if (argsList[1] == NULL)
            return;
        else if (strcmp(argsList[1], "off\0") == 0) {
            loggingFlag = false;
            if (logFile == NULL)
                return;
            else if (fclose(logFile) != 0)
                printErrorMessage(myInitials);
        }
        else {
            logFile = fopen(argsList[1], "w");
            if (logFile == NULL) {
                printErrorMessage(myInitials);
                return;
            }
            else
                loggingFlag = true;
        }

    }
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
