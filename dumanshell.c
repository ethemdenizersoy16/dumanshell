#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "history.h"/*a circular array for implementing the command history*/
#include "read_input.h"
#include <termios.h>
#include "pipe.h"
#include <signal.h>
#include "redirection.h"
#include <fcntl.h>
#include <sys/stat.h>


#define MAX_INPUT  1024 
#define MAX_ARGS 64 
#define MAX_PIPES 100


int main()
{
	char* input; 
	input = (char*) malloc(sizeof(char) * MAX_INPUT);
	char* args[MAX_ARGS];
	char* token;
	
	int i = 0;
	int command_count = 1;
	int pipes[MAX_PIPES];

	pid_t pid;

	int fd[2];
	
	struct redirection redirect[MAX_PIPES];

	pipes[0] = 0;
	
	printf("Hello and welcome to dumanshell.\nThis is a custom shell I am building/have built\n");
	while(1){
		
		struct sigaction sa_new;
		
		int saved_fd[] = {-1, -1};
		
		
		sa_new.sa_handler = SIG_IGN;
		sigemptyset(&sa_new.sa_mask);
		sa_new.sa_flags = 0;
		
		sigaction(SIGINT, &sa_new, NULL);			
		
		
		command_count = 1;

		printf("dumanshell:D>");
		
		fflush(stdout);
		
		read_input(input);
	
		history_add(input);		
		
		if(strlen(input) == 0) continue;
		
		
		if(strcmp(input, "exit") == 0)
		{

			printf("Exiting dumanshell, bye bye \n");	
			break;

		}


		 
		token = strtok(input," ");/*here we tokenize from the input, and our delimiter is a space*/
		
		
		for(i = 0;i < MAX_ARGS && token != NULL;i++)
		{
			if(strcmp(token, "|") == 0)
			{
				if(command_count == MAX_PIPES)
				{
					break;
				}


				args[i] = NULL;
				pipes[command_count++] = i+1;
			}
			else if(strcmp(token, "<") == 0)
			{
				token = strtok(NULL," ");
				redirect[command_count-1].input_fl = token;	
				args[i] = '\0';			
			}
			else if(strcmp(token,">") == 0)
			{
				token = strtok(NULL," ");
				redirect[command_count-1].output_fl = token;
				redirect[command_count-1].output_type = 1;/*overwrite*/
				args[i] = '\0';			
			}
			else if(strcmp(token,">>") == 0)
			{
				token = strtok(NULL," ");
				redirect[command_count-1].output_fl = token;
				redirect[command_count-1].output_type = 2;/*append*/
				args[i] = '\0';			

			}
			else
			{
				redirect[command_count-1].input_fl = NULL;
				redirect[command_count-1].output_fl = NULL;
				args[i] = token;
			}
 
				token = strtok(NULL," ");
			/*strtok internally stores where the last token ended, it is used as the default value here,
			 hence why we used NULL in the string parameter*/
		}
		args[i] = NULL;	

		if(command_count > 1)
		{
			custom_pipe(args,command_count,pipes, redirect);
			continue;

		}


		if(redirect[0].input_fl)
		        {
				
				saved_fd[0] = dup(STDIN_FILENO);	
				fd[0] = open(redirect[0].input_fl, O_RDONLY);
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
		if(redirect[0].output_fl)
			{
			
				saved_fd[1] = dup(STDOUT_FILENO);
				if(redirect[0].output_type == 1)
				{
					fd[1] = open(redirect[0].output_fl, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if(fd[1] < 0)
					{
						perror("Open failed");
						exit(1);
					}
					dup2(fd[1],STDOUT_FILENO);
					close(fd[1]);

				}
				else if(redirect[0].output_type == 2)
				{		
					
					fd[1] = open(redirect[0].output_fl, O_WRONLY | O_CREAT | O_APPEND, 0644);
					if(fd[1] < 0)
					{
						perror("Open failed");
						exit(1);
					}
					dup2(fd[1],STDOUT_FILENO);
					close(fd[1]);

				}

			}
		if(strcmp(args[0],"history") == 0)
		{

			history_print();
			
			if(saved_fd[0] != -1)
			{
				dup2(saved_fd[0],STDIN_FILENO);
				close(saved_fd[0]);
			}
			if(saved_fd[1] != -1)
			{
				dup2(saved_fd[1],STDOUT_FILENO);
				close(saved_fd[1]);
			}			
			continue;

		}
		if(strcmp(args[0],"cd") == 0)
		{
			if(args[1] == NULL) perror("missing argument for cd");
			else if(chdir(args[1]) != 0)
			{
				perror("No such file or directory");
			}

			if(saved_fd[0] != -1)
			{
				dup2(saved_fd[0],STDIN_FILENO);
				close(saved_fd[0]);
			}
			if(saved_fd[1] != -1)
			{
				dup2(saved_fd[1],STDOUT_FILENO);
				close(saved_fd[1]);
			}			
			continue;
		}
		if(strcmp(args[0],"echo") == 0)
		{
			if(args[1])
			{
				printf("%s\n",args[1]);
				fflush(stdout);
			}
			else{
				perror("Please provide a valid string");

			}
		
		
			if(saved_fd[0] != -1)
			{
				dup2(saved_fd[0],STDIN_FILENO);
				close(saved_fd[0]);
			}
			if(saved_fd[1] != -1)
			{
				dup2(saved_fd[1],STDOUT_FILENO);
				close(saved_fd[1]);
			}			
			continue;
			
		}
		
		
		pid = fork(); 

		if(pid == 0){/*you are the child*/
			struct sigaction sa_default;
			sa_default.sa_handler = SIG_DFL;
			sigemptyset(&sa_default.sa_mask);
			sa_default.sa_flags = 0;

			sigaction(SIGINT, &sa_default, NULL);			
			
			execvp(args[0],args);
			
			perror("execvp failed");
			exit(EXIT_FAILURE);
			continue;
		}
		else if(pid > 0)/*you are the parent*/
		{
			int status;
			waitpid(pid,&status,0); 
		
			if(saved_fd[0] != -1)
			{
				dup2(saved_fd[0],STDIN_FILENO);
				close(saved_fd[0]);
			}
			if(saved_fd[1] != -1)
			{
				dup2(saved_fd[1],STDOUT_FILENO);
				close(saved_fd[1]);
			}			
		
			if(status != 0)
			{ 
				if (WIFEXITED(status)) 
				{
					int exit_code = WEXITSTATUS(status);
					if (exit_code != 0)
				       	{
        					fprintf(stderr,"Command failed with exit code: %d\n",exit_code);
  					}
				} 
						else if (WIFSIGNALED(status)) 
					
					{
						if(WTERMSIG(status) == SIGINT) printf("\n");
						fprintf(stderr,"Command killed by signal: %d\n", WTERMSIG(status));
					}
				continue;
			
			}

			
		}
		else{ 
			perror("for failed");
		}


		

	}
	free(input);
   	return 0;
}
