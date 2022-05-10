#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#include <assert.h>

const size_t NR_PROCS = 50;

pid_t pids[NR_PROCS];

size_t min(size_t a, size_t b) {
    if (a < b) {
        return a;
    }
    return b;
}

int main() {

    uint8_t me = 0;
    pids[0] = getpid();
    while (fork() == 0) {
        me++;
        pids[me] = getpid();
        if (me == NR_PROCS - 1) {
            break;
        }
    }
    int status = 0;

    if (me == NR_PROCS - 1) {
        for (size_t i = 1; i < NR_PROCS; i++) {
            for (size_t j = 1; j < NR_PROCS; j++) {
                assert(getlcapid(pids[i], pids[j]) == pids[min(i, j) - 1]);
            }
        }
    }
    else {
        wait(&status);
        assert(WIFEXITED(status));
        assert(WEXITSTATUS(status) == 0);
    }
    return 0;
}
