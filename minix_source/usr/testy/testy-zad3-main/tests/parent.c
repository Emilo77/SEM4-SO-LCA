#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#include <assert.h>

#define NR_PROC 5

int main ()
{
    pid_t pid;
    int i;

    /* tworzenie procesów potomnych */
    for (i = 1; i <= NR_PROC; i++) {
        pid_t father = getpid();
        int status = 0;
        switch (pid = fork()) {
            case -1:
                printf("Error in fork\n");
            case 0: /* proces potomny */
                assert(getlcapid(getpid(), getpid()) == father);
                break;
            default: /* proces macierzysty */
                wait(&status);
                assert(WIFEXITED(status));
                assert(WEXITSTATUS(status) == 0);
                return 0;
        }
    }
    return 0;

}