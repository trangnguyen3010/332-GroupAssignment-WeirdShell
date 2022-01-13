#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int doSingleCommand(char *str);

int doPipe(char *str2, char *str1);

int main(int argc, char *argv[]) {
    char str1[] = "wc | grep parent | cat main.c";
    char str2[] = "wc -m";
    doPipe(str2, str1);
    return 0;
}


/*
 * str2: the right-most single command
 * str1: the rest of the big command
 * example: if the big command is  a | b | c | d, then
 * str2 = a; str1 = b | c | d
 */
int doPipe(char *str2, char *str1) {
    // token = everything from the first | to the end of the str1
    char *token = strchr(str1, '|');

    // if str1 is not a single command, do the pipe in str1 then give its result to str2 command (using pipe too)
    if (token != NULL) {
        // everything from the first | to the end of the str1 (not include |, that's why we write token + 1)
        char right_part[100];
        strcpy(right_part, token + 1);

        // set the position of the first | is 0 so str1 is now just the left-most single command
        str1[strlen(str1) - strlen(token)] = 0;

        int fd[2];
        pipe(fd);
        fflush(NULL);
        int rc = fork();
        if (rc < 0) {
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            // child
            close(fd[0]);  // close read end of pipe
            dup2(fd[1], STDOUT_FILENO);
            // do the commands in right_part then give its output to left_most_command
            doPipe(str1, right_part);
            exit(0);
        } else {
            wait(NULL);
            close(fd[1]);  // close write end of pipe
            dup2(fd[0], STDIN_FILENO);
            // do the str2 command
            doSingleCommand(str2);
            exit(0);
        }
    }
    // if str1 is a single command, do pipe as normal
    else {
        int fd[2];
        pipe(fd);
        fflush(NULL);
        int rc = fork();
        if (rc < 0) {
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            // child
            close(fd[0]);  // close read end of pipe
            dup2(fd[1], STDOUT_FILENO);
            doSingleCommand(str1);
            exit(0);
        } else {
            wait(NULL);
            close(fd[1]);  // close write end of pipe
            dup2(fd[0], STDIN_FILENO);
            doSingleCommand(str2);
            exit(0);
        }
    }
    return 0;
}

int doSingleCommand(char *str) {
    // delimiter for commands
    const char space[2] = " ";
    // number of words allowed for a command
    int numberOfWords = 100;

    fflush(NULL);
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        //printf("hello, I am child (pid:%d) %s\n", (int) getpid(), str);
        char *myargs[numberOfWords];
        char *token;
        token = strtok(str, space);
        int counter = 0;

        while (token != NULL) {
            myargs[counter] = strdup(token);
            counter += 1;
            token = strtok(NULL, space);
        }

        myargs[counter] = NULL;           // marks end of array
        execvp(myargs[0], myargs);  // runs word count
    } else {
        int wc = wait(NULL);
    }
    return 0;
}
