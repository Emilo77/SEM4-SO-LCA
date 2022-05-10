#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

#define SHORT_NAP 100
#define LEVELS 4
#define MAX_PROCS 100 // MAX_PROCS >= 2^(LEVELS+1)

pid_t parent(pid_t pid) {
    return getlcapid(pid, pid);
}

bool is_anc(pid_t proc, pid_t anc) {
    if (proc == -1)
        return false;
    return proc == anc || is_anc(parent(proc), anc);
}

int doublefork() {
    if (fork())
        return fork() == 0;
    return 2;
}

int main() {
    pid_t base = getpid();
    int segment_id = shmget (IPC_PRIVATE, sizeof(pid_t) * MAX_PROCS,
                        IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    
    pid_t *shdata = (pid_t *) shmat(segment_id, 0, 0);
    memset(shdata, 0, sizeof(pid_t) * MAX_PROCS);

    int all_procs = 1;
    for (int i = 1; i < LEVELS; i++) 
        all_procs *= 2;
    all_procs = all_procs * 2 - 1;

    int levels_left = LEVELS - 1;
    int pid = 0, add;
    while (levels_left --> 0 && (add = doublefork()) != 0)
        pid = 2 * pid + add;
    shdata[pid] = getpid();

    // Jesli jestesmy korzeniem
    if (levels_left == LEVELS - 2) {
        for (size_t i = 0; i < all_procs; i++)
            while (!shdata[i])
                usleep(SHORT_NAP);

        for (size_t i = 0; i < all_procs; i++) {
            for (size_t j = 0; j < all_procs; j++) {
                pid_t lca = getlcapid(shdata[i], shdata[j]);
                // LCA jest rodzicem kazdego procesu
                assert((is_anc(shdata[i], lca) && is_anc(shdata[j], lca))
                // Jesli jakis proces stanie sie zombie 
                    || getlcapid(shdata[i], shdata[i]) == -1 
                    || getlcapid(shdata[j], shdata[j]) == -1);
                pid_t vi = shdata[i];
                while (parent(vi) != lca && vi != lca) vi = parent(vi);
                assert(!is_anc(vi, shdata[j]) || vi == shdata[i] || vi == shdata[j]);
            }
        }

        memset(shdata, 0, sizeof(pid_t) * MAX_PROCS);

        wait(0); wait(0);
        shmdt(shdata);
    	shmctl(segment_id, IPC_RMID, 0);
    } else { // jesli jestesmy dzieckiem
        while (shdata[pid])
            usleep(SHORT_NAP);
        if (levels_left > 0) {
            wait(0);
            wait(0);
        }
    }
}

