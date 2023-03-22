#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool checkInput(char *buffer);

int main(int argc, char *argv[]) {
    FILE *infile;
    char *buffer = NULL;
    size_t bufferSize = 0;
    ssize_t charCount;
    //String path = "/bin";
    // use access to ensure the system call will succeed
    //try 1. /bin/''
    //2 /usr/bin/''
    //3 ./''

    infile = fopen(argv[1], "r");
    if (infile != NULL) {
        // batch mode

        charCount = getline(&buffer, &bufferSize, infile);
        if (charCount < 0) {
            printf("EOF");
        }

        fclose(infile);
    }

    while(1) {
        printf("tsh> ");
        charCount = getline(&buffer, &bufferSize, stdin);
        if (!checkInput(buffer))
            exit(1);

        //fork();
    }

    free(buffer);
    return 0;
}

bool checkInput(char *buffer) {
    if (buffer == NULL)
        return false;
    if (buffer == "exit")
        return false;

    return true;
}