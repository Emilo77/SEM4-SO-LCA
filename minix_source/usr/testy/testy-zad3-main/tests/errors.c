#include <unistd.h>
#include <errno.h>
#include <assert.h>

int main() {
    errno = 0;
    assert(getlcapid(-1, -1) == -1);
    assert(errno == EINVAL);
    errno = 0;
    assert(getlcapid(1, 1) == -1);
    assert(errno == ESRCH);
}
