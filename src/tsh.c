#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

bool checkInput(char *buffer);
void execInput(char *command);

int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL;
    size_t bufferSize = 0;
    ssize_t charCount;
    // use access to ensure the system call will succeed
    //try 1. /bin/''
    //2 /usr/bin/''
    //3 ./''

    /* Batch Mode */
    infile = fopen(argv[1], "r");
    if (infile != NULL) {
        charCount = getline(&buffer, &bufferSize, infile);
        if (charCount < 0) {
            perror("EOF");
            exit(0);
        }
        fclose(infile);
    }

    /* Interactive mode */
    while(1) {
        printf("tsh> ");
        charCount = getline(&buffer, &bufferSize, stdin);
        if (charCount < 0)
            exit(0);
        bool input;
        if(!(input = checkInput(buffer)))
            exit(0);
    }

    free(buffer);
    return 0;
}

bool checkInput(char *buffer) {
    char *delims[3] = {"/", "-", " "};
    char *token = strtok(buffer, delims);
    
    while (token != NULL) {
        printf("%s\n", token);
        
        execInput(token);
        token = strtok(NULL, "\n");
    }
    return true;
}

void execInput(char *command) {
    char* argsList[] = {"ls", NULL};
    pid_t pid = fork();

    if (pid < 0) {
        printf("A fork error has occurred.\n");
        exit(0);
    }
    else if (pid == 0) {    // Child process
        int execReturn = execvp(command, argsList);
        if (execReturn < 0) {
            printf("Execvp() call failed.\n");
            exit(0);
        }
    }
    else {                  // Parent process
        int awaitReturn = wait(0);
        if (awaitReturn < 0) {
            printf("Wait() call failed.\n");
            exit(0);
        }
    }
}