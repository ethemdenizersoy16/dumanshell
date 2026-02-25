#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "history.h"/*a circular array for implementing the command history*/
#include "read_input.h"
#include <termios.h>

#define MAX_INPUT  1024 
#define MAX_ARGS 64 

int main()
{
	char* input; 
	input = (char*) malloc(sizeof(char) * MAX_INPUT);
	char* args[MAX_ARGS];
	char* token;
	
	int i = 0;
	pid_t pid;
	
	printf("Hello and welcome to dumanshell.\nThis is a custom shell I am building/have built\n");
	while(1){
		


		printf("dumanshell:D>");
		
		fflush(stdout);
		
		read_input(input);
	
		history_add(input);		
		
		if(strlen(input) == 0) continue;
		
		
		if(strcmp(input, "exit") == 0)
		{

			printf("exiting dumanshell, bye bye \n");	
			break;

		}


		if(strcmp(input,"history") == 0)
		{

			history_print();
			
			continue;

		}
		 
		token = strtok(input," ");/*here we tokenize from the input, and our delimiter is a space*/
		
		
		for(i = 0;i < MAX_ARGS && token != NULL;i++)
		{
			args[i] = token;
			token = strtok(NULL," ");
			/*strtok internally stores where the last token ended, it is used as the default value here,
			 hence why we used NULL in the string parameter*/
		}
		args[i] = NULL;

		if(strcmp(args[0],"cd") == 0)
		{
			if(args[1] == NULL) perror("missing argument for cd");
			else if(chdir(args[1]) != 0)
			{
				perror("no such file or directory");
			}

			continue;
		}
		if(strcmp(args[0],"cd") == 0)
		{
			if(args[1])
			{
				printf("%s\n",args[1]);
				fflush(stdout);
			}
			else{
				perror("please provide a valid string");

			}
			continue;
			
		}
		
		
		pid = fork(); 

		if(pid == 0){/*you are the child*/
			execvp(args[0],args);
			
			perror("execvp failed");
			
			exit(EXIT_FAILURE);
			continue;
		}
		else if(pid > 0)/*you are the parent*/
		{
			int status;
			waitpid(pid,&status,0); 
		       	
			if(status != 0){ 
				printf("Exit status :%d\n",status);
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
