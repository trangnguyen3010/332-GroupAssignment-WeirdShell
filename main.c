
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

int doCommand(char * str);
int doSingleCommand (char *str);
int doPipe(char *str2, char *str1);
void copy_cmpt(char *source, int nbytes);

        int main(int argc, const char * argv[]) {
    // insert code here...
    int numOfChar = 100;
    char str[numOfChar];

    // get first command
    printf("wrdsh>");
    fgets(str, numOfChar, stdin );

    int save_in = dup(STDIN_FILENO);

    // while the input is not exit
    while (strcmp("exit\n", str) != 0){
        //elimiate the newline character at the end
        str[strcspn(str,"\n")] = 0;
        doCommand(str);
        dup2(save_in, STDIN_FILENO);
        printf("wrdsh>");
        fgets(str, numOfChar, stdin);
    }
    return 0;
}

void copy_cmpt(char *source, int nbytes) {
    char ret[nbytes*2];
    int j = 0;
    for (size_t i = 0; i < strlen(source); i++) {
        // printf("%c", source[i]);
        if ((source[i] == 'c') || (source[i] == 'm') || (source[i] == 'p') || (source[i] == 't')) {

            ret[j] = source[i];
            j++;
        }
        ret[j] = source[i];
        j++;
    }
    printf("%s", ret);
}

int doCommand(char * str){
    // token = everything from the first | to the end of the str1
    char *token = strchr(str, '|');

    int fd[2];
    pipe(fd);
    fflush(NULL);
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        // if it is a single command
        if (token == NULL) {
            doSingleCommand(str);
        }
            // do pipe
        else {
            // everything from the first | to the end of the str1 (not include |, that's why we write token + 1)
            char str1[100];
            strcpy(str1, token + 1);

            // the left most single command
            str[strlen(str) - strlen(token)] = 0;
            doPipe(str, str1);
        }
        close(fd[1]);
        exit(0);
    }
    else{
        wait(NULL);
        close(fd[1]);
        char character[10000];
        int nr = read(fd[0], character, 10000);
        //// add a null termination at the and of the string
        character[nr] = 0;
        close(fd[0]);
        if (nr > 0){
            copy_cmpt(character, nr);
        }
    }
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

        // the left most single command
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
            int status = doPipe(str1, right_part);
            close(fd[1]);
            printf("the return of doPipe: %d\n", status);
            // return the exit status of doPipe
            exit(status);
        } else {
            // catch the exit status of the child process
            // if not 0, then just return the exit status
            int status;
            wait(&status);
            if ( WEXITSTATUS(status) != 0 ) {
                int es = WEXITSTATUS(status);
                return es;
            }
            // the child is successful, do the second command and return its exit status
            else {
                close(fd[1]);  // close write end of pipe
                dup2(fd[0], STDIN_FILENO);
                // do the str2 command
                status = doSingleCommand(str2);
                close(fd[0]);
                return (status);
            }
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
            int status = doSingleCommand(str1);
            close(fd[1]);
            exit(status);
        } else {
            int status;
            wait(&status);
            if ( WEXITSTATUS(status) != 0 ) {
                int es = WEXITSTATUS(status);
                return es;
            }
            else {
                close(fd[1]);  // close write end of pipe
                dup2(fd[0], STDIN_FILENO);
                status = doSingleCommand(str2);
                close(fd[0]);
                return status;
            }
        }
    }
}

int doSingleCommand(char *str) {
    // delimiter for commands
    const char space[2] = " ";
    // number of words allowed for a command
    int numberOfWords = 100;

    fflush(stdout);
    int saveOut = dup(STDOUT_FILENO);

    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        char *myargs[numberOfWords];

        // split at "<"
        char *command;
        command = strtok(str, "<");

        // get file name. First token.
        char *file_name;
        file_name = command;

        // get command, after "<"
        command = strtok(NULL, "<");

        //select command to make into array and execute.
        char *token;
        if (command != NULL){
            // command is null if no redirection is found
            token = strtok(command, space);
        }
        else{
            // if command is null, the input is stored in file_name
            token = strtok(file_name, space);
        }

        // turn command into an array
        int counter = 0;
        while (token != NULL) {
            myargs[counter] = strdup(token);
            counter += 1;
            token = strtok(NULL, space);
        }

        myargs[counter] = NULL; // marks end of array


        if (command == NULL){
            // if no redirection
            execvp(myargs[0], myargs);
        }
        else{
            // set file to save to and execute command
            close(STDOUT_FILENO);
            int fd = open(file_name, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
            dup2(fd, STDOUT_FILENO);

            execvp(myargs[0], myargs);
            close(fd);
        }
        // if execvp is not successful, it'll reach this line
        exit(errno);

    } else {
        int status;
        wait(&status);
        int es = WEXITSTATUS(status);
        if ( es != 0) {
            // if the errno is 2, then it is a wrong command name
            if (es == 2) {
                fprintf(stderr, "Command \"%s\" is not found\n", str);
            }
        }
        dup2(saveOut, STDOUT_FILENO);
        return es;
    }
}

