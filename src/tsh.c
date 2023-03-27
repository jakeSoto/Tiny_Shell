#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void execInput(char *buffer);

int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL;
    size_t bufferSize = 0;
    ssize_t nread;

    /* Interactive mode */
    while(1) {
        printf("tsh> ");
        if ((nread = getline(&buffer, &bufferSize, stdin)) == -1)
            exit(0);
        if (strcmp(buffer, "exit\n") == 0)
            exit(0);
        else
            execInput(buffer);
    }

    free(buffer);
    return 0;
}


/* Executes input given from interactive mode 
   after forking to a child process */
void execInput(char *buffer) {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr,"Fork Failed\n");
		exit(0);
    }
    else if (pid == 0) {    // Child process
        int i = 0;
        char *argsList[50];
        argsList[i] = strsep(&buffer, " \t\n");

        while (argsList[i] != NULL) {
            if (*argsList[i] != '\0')
                i++;
            argsList[i] = strsep(&buffer, " \t\n");
        }

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
