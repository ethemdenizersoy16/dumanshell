#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include "history.h"
#include <termios.h>

#define MAX_INPUT  1024
#define MAX_ARGS 64 

static struct termios orig_termios;/*variable for original terminal attributes*/

void disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enable_raw_mode()
{
	tcgetattr(STDIN_FILENO, &orig_termios);

	struct termios raw_termios = orig_termios;
	
	raw_termios.c_lflag &= ~(ECHO | ICANON);
	raw_termios.c_cc[VMIN] = 1;
	raw_termios.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios);
}

void read_input(char* buffer)
{
	int len = 0,step = 0/*step counter for scrolling command history*/, cursor = 0;
	
	char c;
	char* buffer_backup;
	buffer[len] = '\0';
	buffer_backup = (char*) malloc(sizeof(char) * MAX_INPUT);
	
	
	enable_raw_mode();
	while(read(STDIN_FILENO, &c,1) == 1) 
	{
		if(c == '\n')
		{
			buffer[len] = '\0';
			printf("\n");	
			break;
		 }
		else if(c == 127 || c == '\b')
		      {
			if(len > 0 && cursor > 0)
			{
				int i,n;
				cursor--;
				printf("\033[D");
				fflush(stdout);
				for(i = cursor;i != len;i++)
				{

					buffer[i] = buffer[i+1];
					printf("%c",buffer[i]);
					fflush(stdout);

				}
				printf("\033[C");
				fflush(stdout);
				
				printf("\b \b");
				fflush(stdout);
				buffer[--len] = '\0';
				
				n = len - cursor;
				if(n > 0)
				{
					printf("\033[%dD",n);
					fflush(stdout);
					}


			}
			else if(len == 0) {/*if there is nothing to delete play a bell sound*/
				printf("\a");
				fflush(stdout);
			}

		}
		else if(c == 27)
		{
			char seq[2];
			if(read(STDIN_FILENO, &seq[0],1)==1 && read(STDIN_FILENO,&seq[1],1) == 1 )
			{
				if(seq[0] == '[')
					{
						const char* prev;
						if(seq[1] == 'A')
						{
							int i;
							if(step == 0)
							{	
								strcpy(buffer_backup,buffer);
								
							}
							step++;
							prev = history_fetch(step);
							if(prev)
							{   
								
								int n;

								n = len- cursor;
								if(n > 0)
									{
									printf("\033[%dC",n);
									fflush(stdout);
								}
								
								for(i = 0;i<len;i++) printf("\b \b");
								
								len = snprintf(buffer,MAX_INPUT,"%s",prev);
								cursor = len;
				
								printf("%s",buffer);
								fflush(stdout);

							}
							else{
							       	step--;
							}
							continue;
						}

						if(seq[1] == 'B')
						{
							int i;
							if(step > 1){
								step--;
								prev = history_fetch(step);
								if(prev)
								{    
									int n;
									
									n = len - cursor;

									if(n > 0)
									{
										printf("\033[%dC",n);
										fflush(stdout);
									}
									
									for(i = 0;i<len;i++) printf("\b \b");
									len = snprintf(buffer, MAX_INPUT,  "%s",prev);
									cursor = len;
				
									printf("%s",buffer);
									fflush(stdout);

								}	
								else step++;
								continue;
								}
							if(step == 1)/*load what the user had previously written*/
							{	
								int n;

								n = len - cursor;

								if(n >0)
								{
									printf("\033[%dC",n);
									fflush(stdout);

								}
								step--;
								for(i = 0;i<len;i++) printf("\b \b");
								len = snprintf(buffer,MAX_INPUT,"%s",buffer_backup);
								cursor = len;
				
								printf("%s",buffer);
								fflush(stdout);	
								
							}
						}
						if(seq[1] == 'C')
						{
							if(cursor < len)
							{
								cursor++;
								printf("\033[C");
								fflush(stdout);
							
							}
							continue;	
						}
						if(seq[1] == 'D')
						{
							if(cursor > 0)
							{		
								cursor--;
				
								printf("\033[D");
								fflush(stdout);
								
							}
							continue;
						}
				}	
			}	
		}
		else if(len < MAX_INPUT -1 && c >= 32 && c<= 126 ){/* handles non ASCII characters*/
			int i,n;
			char temp;
			for(i = cursor; buffer[i] != '\0';i++)
				{
					temp = buffer[i];
					buffer[i] = c;
				
					printf("%c",c);
					fflush(stdout);
					c = temp;
					
			}
			cursor++;
			buffer[len++] = c;
			
			printf("%c",c);
			fflush(stdout);
			
			buffer[len] = '\0';
			
			n = len - cursor;
			if(n > 0)/*put the cursor in its original place*/
			{	
				printf("\033[%dD",n);
				fflush(stdout);
			}
			
			
		}
		else
		{
			printf("\a");
			fflush(stdout);
		}
		step = 0;
		
		      	      
	}


	free(buffer_backup);
	disable_raw_mode();	
}

