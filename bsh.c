/**
 * bsh, or Basic Shell, is a simple shell for UNIX created by yours truly, in C.
 * 
 * @author  -   Jørgen Mo Opsahl
 * 
 * @todo moving around the file system, see if user has root privileges (# vs $), creating a proper prompt (with location, name etc.)
 **/

#include <unistd.h> // exec
#include <stdbool.h>
#include <stdio.h> // printf, perror
#include <string.h>
#include <ctype.h> // strcmp, strtok
#include <sys/wait.h> // waitpid
#include <signal.h>   // Signal

void parseAndExecute();
void execute(char ** args);

const int INPUTBUFFER = 80;  ///< Input buffer
const int MAXARGUMENTS = 50; ///< Max. arguments
const char* DELIMITERS = "\t\n\a "; ///< characters used to split the user input into arguments

bool gEXITFLAG = false;

/**
 * main
 **/
int main(){
    printf("Basic Shell!\n");
    char userInput[INPUTBUFFER];

    do{
        signal(SIGINT, SIG_IGN); // Ignore signals (ctrl-c )
        userInput[0] = '\0';    
        do{
            printf("> ");
            fgets(userInput, INPUTBUFFER, stdin);   // Reads the command
        }while (isalnum(userInput[0]) == 0);    // Loop as long as the first char isn't alphanumeric
        
        parseAndExecute(userInput);


    }while(!gEXITFLAG);                          // see if the exit flag has been set


    return 0;
}


/**
 * Parse the input, and execute the command.
 **/
void parseAndExecute(char* input){
    char* parsed[MAXARGUMENTS];
    int argumentCounter = 0;
    char* token = strtok(input, DELIMITERS);

    while(token != NULL && argumentCounter < MAXARGUMENTS){
        parsed[argumentCounter] = token;
        argumentCounter++;
        token = strtok(NULL, DELIMITERS); // Continue to use the string, finds the next substring.
    }

    parsed[argumentCounter] = NULL; // NULL terminates the array (needed for execvp)

    if(strcmp("exit", parsed[0]) == 0){
        gEXITFLAG = true;
    }
    else{
        
        execute(parsed);
    }
    
}

/**
 * Execute a command, with arguments
 **/
void execute(char ** args){
    int processStatus;
    pid_t newProcess = fork(); // create child process, PID
    if(newProcess == 0){ // Child process
        signal(SIGINT, SIG_DFL); // Signal default behavior
        if(execvp(args[0], args) == -1){ // Error
            perror("bsh");
        } 
    }
    else if(newProcess > 0){ // Parent process
        do{
            waitpid(newProcess, &processStatus, WUNTRACED);

        }while (!WIFEXITED(processStatus) && !WIFSIGNALED(processStatus)); // Loops as long as the process hasn't exited normally or abnormally
        
    }
    else{ // Error
        perror("bsh");
    }
}