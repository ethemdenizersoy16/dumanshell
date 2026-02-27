#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <termios.h>

#define MAX_INPUT  1024 
#define MAX_ARGS 64


void custom_pipe(char** args, int command_count, int pipes[])
{
	int i;
	int fd[2];
    	int prev_read_end = 0;
        pid_t pid;	

    	for (i = 0; i < command_count; i++) {
        	
        if (i < command_count - 1) 
	{
            if (pipe(fd) == -1) return;
        }

        pid = fork();
        if (pid == 0) {
            
            if (prev_read_end != 0) {
                dup2(prev_read_end, STDIN_FILENO);
                close(prev_read_end);
            }

           
            if (i < command_count - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]); 
                close(fd[1]);
            }
			    
            execvp(args[pipes[i]], args+pipes[i]);
            perror("execvp failed");
            exit(1);
        }

        
        if (prev_read_end != 0) close(prev_read_end);
        
        if (i < command_count - 1) {
            close(fd[1]);      
            prev_read_end = fd[0]; 
        }
    }

    
    while (wait(NULL) > 0);
 


}
