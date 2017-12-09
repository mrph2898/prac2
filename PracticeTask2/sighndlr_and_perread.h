#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#ifndef SIGHNDLR_AND_PERREAD
#define SIGHNDLR_AND_PERREAD
void 
sigpipe_handlr(int s)
{
    s = s;
    signal(SIGPIPE, sigpipe_handlr);
    _exit(0);
}

int 
perfect_read(int fd, void *buf, int size)
{
    int cur_readed, cur_size = 0;
    while (cur_size != size) {
        if ((cur_readed = read(fd, (char *)buf + cur_size, size - cur_size)) == -1 ) {
            return -1;
        } else if (cur_readed == 0) {
            return 1;
        }
        cur_size += cur_readed;
    }
    return 0;
}
#endif
