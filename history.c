#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "history.h"

#define MAX_HIST 1000


char* history[MAX_HIST];
int start = 0, end = 0, count = 0;

void history_add(char* input)
{
	if(count == 0)
	{
		history[start] = strdup(input);
		count++;
	}
	else if(count < MAX_HIST)
	{
		
		end = (end+1)%MAX_HIST;
		history[end] = strdup(input);/*uses malloc to allocate new space for the string to be copied, safer than strcpy*/
		count++;
	}
	else
	{
		free(history[start]);
		start = (start+1)%MAX_HIST;
		end = (end+1)%MAX_HIST;
		history[end] = strdup(input);
	}
}

void history_print()
{	
	int i;
	for(i = start; i != end; i = (i+1)%MAX_HIST)
	{
		printf("%d) %s\n",i%MAX_HIST,history[i]);
	}
	printf("%d) %s\n",i%MAX_HIST,history[end]);


}

const char* history_fetch(int step)
{
	if(step > count)
	{
		return NULL;
	}
	return history[(end-step+1 + MAX_HIST)%MAX_HIST];

}

