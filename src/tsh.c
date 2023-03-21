#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    pid_t pid;
    string path = "/bin";
    // use access to ensure the system call will succeed
    //try 1. /bin/''
    //2 /usr/bin/''
    //3 ./''

    while(1) {
        fprintf("tsh> ");
        getline();
    }

    return 0;
}