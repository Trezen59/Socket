// dynamic memory allocation example. using malloc and realloc.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MAX 2

int main()
{
	int MAX = 2;
	int current_size = 0;

	char *buff = malloc(MAX);
	current_size = MAX;

	printf("\nEnter string : ");
	
	int i = 0;

	while (( buff[i++] = getchar() ) != '\n')
	{
		if(i == current_size)
		{
			current_size = i + MAX;
			buff = realloc(buff, current_size);
		}
	}

	buff[i] = '\0';

	printf("\nString value : %s \n",buff);

	return 0;
}
	
