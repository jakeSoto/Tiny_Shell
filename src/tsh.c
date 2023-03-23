#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void execInput(char *buffer);

int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL;
    size_t bufferSize = 0;
    ssize_t charCount;

    /* Batch Mode 
    infile = fopen(argv[1], "r");
    if (infile != NULL) {
        charCount = getline(&buffer, &bufferSize, infile);
        if (charCount < 0) {
            perror("EOF");
            exit(0);
        }
        fclose(infile);
    } */

    /* Interactive mode */
    while(1) {
        printf("tsh> ");
        charCount = getline(&buffer, &bufferSize, stdin);
        if (charCount < 0) {
            exit(0);
        }
        execInput(buffer);
    }

    free(buffer);
    return 0;
}


/* Executes input given from interactive mode 
   after forking to a child process */
static void execInput(char *buffer) {
    pid_t pid = fork();

    if (pid < 0) {
        printf("A fork error has occurred.\n");
        exit(0);
    }
    else if (pid == 0) {    // Child process
        int i = 0;
        char *argsList[10];
        char *token = strtok(buffer, " \n");

        while (token != NULL) {
            argsList[i] = token;
            i++;
            token = strtok(NULL, " \n");
        }
        argsList[i] = NULL;

        if (execvp(argsList[0], argsList) < 0) {
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
