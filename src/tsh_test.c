#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char** getArgList(char *buffer);
void execInput(char *argsList[]);

int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL; //buf[80]
    size_t bufferSize = 0;  //150 - 200
    ssize_t nread;
    char *args_list[50] = {NULL};

    /* Interactive mode */
    while(1) {
        printf("tsh> ");
        if ((nread = getline(&buffer, &bufferSize, stdin)) == -1)
            exit(0);
        if (strcmp(buffer, "exit\n") == 0)
            exit(0);
        else {
            int i = 0;
            args_list = getArgList(buffer);
            printf("%s\n", args_list[i]);
            //execInput(args_list);
        }
    }

    free(buffer);
    return 0;
}

char** getArgList(char *buffer) {
    int i = 0;
    char *argList[50];
    argList[i] = strsep(&buffer, " \t\n");

    while (argList[i] != NULL) {
        if (*argList[i] != '\0')
            i++;
        argList[i] = strsep(&buffer, " \t\n");
    }

    return argList;
}



/* Executes input given from interactive mode 
   after forking to a child process */
void execInput(char *argsList[]) {
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
