CMPT 332\
Assignment 1\
October 1, 2021\
Heather Caswell | Trang Nguyen\
hec626 | len054\

# Weird Shell

### Startup:
Use “make run” to compile and start the wrdsh.

### Summary of functions:

int doCommand(char * str):
- str: the full command
- Determines if the command is a pipe, or a single command. Redirection counts as a single command.
- If command is a pipe, it splits the pipe and sends the pieces through doPipe.
  
int doSingleCommand (char *str):
- str: the single command. (example: ls -a, output.txt < cat main.c, echo cmpt)
- Splits command into an array of arguments
- Determines whether the command is a redirection or not, then executes the command as given.

int doPipe(char *str2, char *str1): str1- The left command of the pipe str2- the right command of the pipe
- Takes split pipe strings and executes the commands from right to left.

void copyCmpt(char *source, int n):
- source: string to add the repeated letters to.
- n: size of the source string
- Takes in sources string and prints out the same string but with the letters ‘c’ ‘m’ ‘p’ ‘t’ repeated

### Known issues:
• Is not capable of distinguishing between an invalid command and a command that is missing arguments.
