#include <lib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#define SIZE 56

int main() {
    message m;
    pid_t base = getpid();
    if (fork()) {
        int status;
        wait(&status);
        assert(status == 0);
        return status;
    }
    pid_t own = getpid();
    for (int i = 0; i < sizeof(pid_t); i++) {
        for (int i = 0; i < SIZE; i++)
            m.size[i] = 0;
        for (int j = i; j + sizeof(pid_t) <= SIZE; j += sizeof(pid_t))
            *((pid_t*)(m.size + j)) = own;
        if (_syscall(PM_PROC_NR, PM_GETLCAPID, &m) == base)
            return 0; 
        for (int j = i; j + sizeof(pid_t) <= SIZE; j += sizeof(pid_t))
            if (*((pid_t*)(m.size + j)) == base)
                return 0;
    }
    return 1;
}