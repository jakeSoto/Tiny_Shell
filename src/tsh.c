#include <stdio.h>
#include <stdlib.h>

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
    if (infile == NULL) {
        perror("Couldn't open file");
        exit(1);
    }

    while(1) {
        printf("tsh> ");
        charCount = getline(&buffer, &bufferSize, infile);
        if (charCount < 0) {
            printf("error");
        }
        printf("Num of characters read in: %s\n", charCount);
    }

    free(buffer);
    fclose(infile);
    return 0;
}