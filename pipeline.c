#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
    int fd[2];
    pipe(fd);
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
	// child
	close(fd[0]);  // close read end of pipe

	// if do exec now, the program we run will have write end of pipe
	// as its standard output 
    char* myargs[3];
    myargs[0] = strdup("cat");
    myargs[1] = strdup("pipeline.c");
    
    myargs[2] = NULL;           // marks end of array
    execvp(myargs[0], myargs);  // runs word count
    dup2(fd[1],STDOUT_FILENO);
    } else {
        // parent goes down this path (original process)
	//
	// do a second fork here, if want to run a different program
	// for the backend of the pipeline, and have the child from
	// this second fork execute the following ...
	close(fd[1]);  // close write end of pipe
    dup2(fd[0],STDIN_FILENO);
    char* myargs[3];
    myargs[0] = strdup("grep");
    myargs[1] = strdup("parent");
    myargs[2] = NULL;           // marks end of array

    execvp(myargs[0], myargs);  // runs word count
    
	// if do exec now the program we run will
	// have the read end of pipe as standard input
	//
	// char *message;
	// size_t size = 100;
	// message = (char *) malloc (size + 1);
	// int nr = getline(&message, &size, stdin);
	// if (nr > 0) printf("%s", message);
    //     int wc = wait(NULL);
    // }
    }
    return 0;
}