#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include "redirection.h"
#include <fcntl.h>
#include "history.h"



#define MAX_INPUT  1024 
#define MAX_ARGS 64
#define MAX_PIPES 100

void custom_pipe(char** args, int command_count, int pipes[], struct redirection redirect[])
{
	int i, status;
	int fd[2];
    	int prev_read_end = STDIN_FILENO;
	int is_killed = 0;
	
	pid_t pid;
	pid_t child_pids[MAX_PIPES];	
		

    	for (i = 0; i < command_count; i++) {
        	
        if (i < command_count - 1) 
	{
            if (pipe(fd) == -1) return;
        }

        pid = fork();
        if (pid == 0) {
            struct sigaction sa_default;
	    sa_default.sa_handler = SIG_DFL;
	    sigemptyset(&sa_default.sa_mask);
	    sa_default.sa_flags = 0;
	    
	    
	    sigaction(SIGINT,&sa_default, NULL);




            if (prev_read_end != 0) {
                dup2(prev_read_end, STDIN_FILENO);
                close(prev_read_end);
            }

           
            if (i < command_count - 1) {
                dup2(fd[1], STDOUT_FILENO);
                
		close(fd[0]); 
                close(fd[1]);
            }
		if(args[pipes[i]])
	{
	    if(strcmp(args[pipes[i]],"exit") == 0)
	    {
			exit(0);
		}
				
		if(strcmp(args[pipes[i]],"history") == 0)
		{

			history_print();
			
			continue;

		}
		if(strcmp(args[pipes[i]],"cd") == 0)
		{
			fprintf(stderr, "Can't use cd in a pipe\n");
			exit(1);
		}
	    			
		if(redirect[i].input_fl)
		        {
				
				fd[0] = open(redirect[i].input_fl, O_RDONLY);
				if(fd[0] < 0)
				{
					if(fd[0] < 0)
					{
						perror("Open failed");
						exit(1);
					}
				
				}
				dup2(fd[0],STDIN_FILENO);
				close(fd[0]);
			}		
		if(redirect[i].output_fl)
			{
			
				if(redirect[i].output_type == 1)
				{
					fd[1] = open(redirect[i].output_fl, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if(fd[1] < 0)
					{
						perror("Open failed");
						exit(1);
					}
					dup2(fd[1],STDOUT_FILENO);
					close(fd[1]);

				}
				else if(redirect[i].output_type == 2)
				{		
					
					fd[1] = open(redirect[i].output_fl, O_WRONLY | O_CREAT | O_APPEND, 0644);
					if(fd[1] < 0)
					{
						perror("Open failed");
						exit(1);
					}
					dup2(fd[1],STDOUT_FILENO);
					close(fd[1]);

				}
			}
            
		
	    execvp(args[pipes[i]], args+pipes[i]);

	    perror("Execvp failed");
	    exit(EXIT_FAILURE);
	}
		else
	{
		perror("Missing command");
		exit(EXIT_FAILURE);
		}
        }
	child_pids[i] = pid;
        
        if (prev_read_end != 0) close(prev_read_end);
        
        if (i < command_count - 1) {
            close(fd[1]);      
            prev_read_end = fd[0]; 
        }
    }

    
    for(i = 0;i < command_count;i++)
    {


			waitpid(child_pids[i],&status,0);
			if(WIFSIGNALED(status) && !is_killed)
			{

				int sig = WTERMSIG(status);
				if(sig == SIGINT){
				       	printf("\n");
					fflush(stdout);
				}
				is_killed = 1;
				if(sig != 13) fprintf(stderr,"Command killed by signal: %d\n", sig);
			}

			else if(i == command_count -1)
		  	  {
	
			if (WIFEXITED(status)) 
			{
               			int exit_code = WEXITSTATUS(status);
                		if (exit_code != 0) 
				{
					fprintf(stderr,"Pipeline failed at end with code: %d\n", exit_code);
				}
            		}
		}
    }
	  
 


}
