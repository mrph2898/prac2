#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "sighndlr_and_perread.h"

#define ERR(x) \
    if (x == -1) {\
        perror(strerror(errno));\
        return -1;\
    } 

enum { BUF_SIZE = 4096 }; 
int numeric_ans(char *, int);
int permutation_ans(char *, int);
int str_ans(char *, int);

typedef int (*ArrayOfPtrsToFunc)(char *str, int num);
struct Testing_Array {
    char *question;
    char *answer;
    ArrayOfPtrsToFunc funct;
};

const char *theme = "Math";    
struct Testing_Array mas[] = { {"135 + 230 = ", "365", numeric_ans},
                               {"400 - 289 = ", "111", numeric_ans},
                               {"20 * 5 = ", "100", numeric_ans}, 
                               {"200 / 50 = ", "4", numeric_ans}, 
                               {"Who formulate theorem about 2 cathetus and hypothenus:", "Pifagor", str_ans}, 
                               {"Write natural numbers between 0 and 8:", "2 4 6", permutation_ans}, 
                               {"sin(pi/2) = ", "1", numeric_ans}, 
                               {"e^(ln(15)) = ", "15", numeric_ans}, 
                               {"x^2 + 2*x + 1 = 0; What is x? ", "-1", numeric_ans} }; 
const char *commands = "QqAatTnN";
const char *comments[3] = {"Incorrect answer!!!", "Good job!!!", (char *)0};

int                                      //function for qsort( *,*,*, comp)
comp (const int *i, const int *j)
{
    return *i - *j;
}

int 
count_amnt_of_num(char *buf) 
{
    int i = 0;
    char *p = buf;
    while (strchr(p, ' ')) {
        p = strchr(p, ' ') + 1; 
        i++;
    }
    return i + 1;
}

int
permutation_ans(char *usr_ans, int num)      //Checking trueness of answer permutation type.
                                             //For example "2 4 6" == "2 6 4" if question was: 
                                             //Write all even numbers in [2,6]
{
    char *ptr_to_enter;
    if ((ptr_to_enter = strchr(usr_ans, '\n')) != 0) {
        usr_ans[ptr_to_enter - usr_ans] = '\0';
    }
    int amnt_of_int_in_ans;
    int amnt_of_int_in_cor_ans;
    char answ[strlen(mas[num].answer)];
    strcpy(answ, mas[num].answer);
    char *p1 = &answ[0];
    char *p2 = usr_ans;
    int i;
    amnt_of_int_in_cor_ans = count_amnt_of_num(p1); 
    amnt_of_int_in_ans = count_amnt_of_num(p2); 
    int ans[amnt_of_int_in_ans];                    //Now answer is
    int cor_ans[amnt_of_int_in_cor_ans];            //an array of integers
    if (amnt_of_int_in_cor_ans != amnt_of_int_in_ans) {
        return 0;
    } else {
        for (i = 0; i < amnt_of_int_in_ans; i++) {
            sscanf(p1, "%d", &ans[i]);
            p1 = strchr(p1, ' ') + 1; 
            sscanf(p2, "%d", &cor_ans[i]);
            p2 = strchr(p2, ' ') + 1; 
        }
        qsort(ans, amnt_of_int_in_ans, sizeof (int), (int(*) (const void *, const void *)) comp);
        qsort(cor_ans, amnt_of_int_in_ans, sizeof (int), (int(*) (const void *, const void *)) comp);
        if (!(memcmp(ans, cor_ans, (amnt_of_int_in_ans * sizeof(ans[0])) ))) {
            return 1;
        } else { 
            return 0; 
        }
    }
}

int
numeric_ans(char *usr_ans, int num)     //Checking trueness of answer numeric type.
                                        //For example "2" == "2.0" == "02.0" if question was: 
                                        //1 + 1 = 
{
    char *ptr_to_enter;
    if ((ptr_to_enter = strchr(usr_ans, '\n')) != 0) {
        usr_ans[ptr_to_enter - usr_ans] = '\0';
    }
    char *end_ptr1, *end_ptr2;
    float a = strtof(usr_ans, &end_ptr1);
    float b = strtof(mas[num].answer, &end_ptr2);
    if (*end_ptr1) {
        return 0;
    }
    if (errno == ERANGE) {
        return 0;
    }
    if (a == b) { 
        return 1;
    } else {
        return 0;
    }
}

int 
str_ans(char *usr_ans, int num)         //Checking trueness of answer string type. Only compare     
{        
    if (!strcasecmp(usr_ans, mas[num].answer)) {     //100% Correct
        return 1;
    } else {                                   //May be correct
        char *ptr_to_enter = strchr(usr_ans, '\n');
        if (!ptr_to_enter) {                           //100% Incorrect
            return 0;
        } else {
            usr_ans[ptr_to_enter - usr_ans] = '\0';    //Delete \n from answer    
            if (!strcasecmp(usr_ans, mas[num].answer)) {  //100% Correct
                return 1;
            } else {                                   //100% Incorrect
                return 0;
            }
        }
    }
}

int
give_question_or_check_ans(int n, int questions_amnt)       //0 - give question; 1 - check ans
{
    int q_num, ret_val;
    char answer[BUF_SIZE];
    if ((ret_val = perfect_read(0, &q_num, sizeof(q_num))) == 1) {
        fputs("You didn't write a number of question!", stderr);
        _exit(0);
    } else if (( q_num > questions_amnt ) || ( q_num < 0 )) {
        fputs("There is no question with this number!!!", stderr);
        return 0;
    } else ERR(ret_val) 
    if (!n) {        
        int length = strlen(mas[q_num].question);
        ERR(write(1, &length, sizeof(length)))
        ERR(write(1, mas[q_num].question, length))
        return 1;
    } else {
        int length_of_usr_ans;
        if (perfect_read(0, &length_of_usr_ans, sizeof(length_of_usr_ans))) {
            return -1;     // Need to exit
        }   
        if (perfect_read(0, answer, length_of_usr_ans)) {
            return -1;     // Need to exit
        }   
        if (mas[q_num].funct(answer, q_num)) {
            ERR(write(1, "OK", 2))
        } else {
            ERR(write(1, "NO", 2))
        }
        return 1;
    }
}

int 
main(void)
{
    int questions_amnt = (sizeof(mas) / sizeof(*mas));                               
    signal(SIGPIPE, sigpipe_handlr);
    char cmd; 
    char *p_to_command;
    int not_so_many_diagn = 0;
    while (read(0, &cmd, sizeof(cmd)) != 0) {   //While not EOF(CTRL+D)
        if (!(p_to_command = strchr(commands, cmd))) {
            if (!not_so_many_diagn) {
                fputs("Wrong command", stderr);
                not_so_many_diagn = 1;
            }
            continue;
        }
        int len;
        switch (*p_to_command) {
        case 'Q':
        case 'q':
            if (give_question_or_check_ans(0, questions_amnt) == -1) { return 0; }
            break;
        case 'A':
        case 'a':
            if (give_question_or_check_ans(1, questions_amnt) == -1) { return 0; }
            break;
        case 't':
        case 'T':
            len = strlen(theme);
            ERR(write(1, &len, sizeof(len)))
            ERR(write(1, theme, len))
            break;
        case 'n':
        case 'N': 
            ERR(write(1, &questions_amnt, sizeof(questions_amnt)))
            break;
        }
    }
    return 0;
}
