#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WRDSH_TOK_BUFSIZE 64
#define WRDSH_TOK_DELIM " \t\r\n\a"

char *wrdsh_readline(void){
    char *line = NULL;
    ssize_t size = 0;
    
    return line;
}

char **wrdsh_parseline(char *line){
    int bufsize = WRDSH_TOK_BUFSIZE, position = 0;
}

void wrdsh_loop(void){
    int status;
    char *line;
    char **args;

    do {
        printf("wrdsh> ");
        line = wrdsh_readline();
        args = wrdsh_parseline(line);
        status = wrdsh_execute(args);

    } while (status);
}



int main(int argc, char **argv)
{

  // Run command loop.
  wrdsh_loop();

  return EXIT_SUCCESS;
}