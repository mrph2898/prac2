#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "sighndlr_and_perread.h"

#define ERR(x,y) \
    if (x == y) { \
        perror(strerror(errno)); \
        close(father_fd[0]); \
        close(son_fd[1]); \
        wait(NULL); \
        _exit(0);\
    }
 
#define PIPERR(x) \
    if (x == -1) { \
        perror(strerror(errno)); \
        return 1;\
    }

#define READ(x) ERR((func_value = perfect_read(father_fd[0], &x, sizeof(x))), -1) \
            ERR(func_value, 1)

enum {BUF_SIZE = 4096};
char *msgs[] = {"Hello. Lets begin test on ", 
                "Your answer: ",
                "CHECKED\n"};        

void
signal_handler(int s)
{
    s = s;
    signal(SIGINT, signal_handler);
    wait(0);
    _exit(0);
}   

void
read_and_print(int father_fd[2], int son_fd[2])
{
    int func_value, length_of_buf;
    READ(length_of_buf)
    char buf[length_of_buf];
    READ(buf)
    ERR(write(1, buf, length_of_buf), -1)
    putchar('\n'); 
}

int 
main(int argc, char **argv) 
{   
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, sigpipe_handlr);
    if ((argc - 1) > 1) {
        puts("A lot of arguments!!!");
        return 1;
    }
    int father_fd[2];
    int son_fd[2], pid;
    PIPERR(pipe(father_fd))    
    PIPERR(pipe(son_fd)) 
    if ((pid = fork()) == 0) {
        close(father_fd[0]);
        close(son_fd[1]);
        dup2(son_fd[0], 0);
        dup2(father_fd[1], 1);
        close(son_fd[0]);
        close(father_fd[1]);
        execl(argv[argc - 1], argv[argc - 1], NULL);
    } else {
        if (pid == -1) { return 1; }
            close(father_fd[1]);
            close(son_fd[0]);
            int amnt_of_questions;
            ERR(write(son_fd[1], "n", 1), -1) 
            ERR(read(father_fd[0], &amnt_of_questions, sizeof(amnt_of_questions)), -1)
            char statistic[amnt_of_questions];
            for (int i = 0; i < amnt_of_questions; i++) {
                statistic[i] = 'n';
            }    
            ERR(write(1, msgs[0], strlen(msgs[0])), -1) 
            ERR(write(son_fd[1], "t", 1), -1)
            read_and_print(father_fd, son_fd);
            for (int i = 0; i < amnt_of_questions; i++) {  
                ERR(write(son_fd[1], "q", 1), -1)
                ERR(write(son_fd[1], &i, sizeof(i)), -1)
                read_and_print(father_fd, son_fd);
                char answer[BUF_SIZE];
                ERR(write(1, msgs[1], strlen(msgs[1])), -1)
                putchar('\n');
                ERR(write(son_fd[1], "a", 1), -1)
                ERR(write(son_fd[1], &i, sizeof(i)), -1)
                if (!fgets(answer, BUF_SIZE, stdin)) {
                    break;
                }
                char not_answer[BUF_SIZE];
                int was_eof = 0;
                memcpy(not_answer, answer, BUF_SIZE);
                while (!strchr(not_answer, '\n')) {
                    if (!fgets(not_answer, BUF_SIZE, stdin)) {
                        was_eof = 1;
                        break;
                    }
                }
                int size_of_ans = strlen(answer);
                ERR(write(son_fd[1], &size_of_ans, sizeof(size_of_ans)), -1) 
                ERR(write(son_fd[1], answer, size_of_ans), -1)
                char verdict[2];
                ERR(read(father_fd[0], verdict, sizeof(verdict)), -1)
                if (!strcmp(verdict, "OK")) {
                    statistic[i] = '+';
                } else {
                    statistic[i] = '-';
                }
                if (was_eof) {
                    break;
                }                 
                ERR(write(1, msgs[2], sizeof(msgs[2])), -1) 
            }
        puts("\nYour statistic:");
        ERR(write(1, statistic, amnt_of_questions), -1)    
        putchar('\n');
        close(father_fd[0]);
        close(son_fd[1]);
        wait(NULL);
        exit(0);
    }
}    
