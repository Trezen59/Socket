// server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX 100
#define PORT 8080

void *reading(void *connfd)
{
	int flag = 1;

	while(flag)
	{
		
		int *bufflen = malloc(sizeof(int));
		bzero(bufflen, sizeof(bufflen));		
			
		// read length of buffer
		if((read(*(int *)connfd, bufflen, sizeof(bufflen))) == -1)
		{
			perror("read");
		}	
		printf("recieved length : %d\n", *bufflen);

	
		int len  = *bufflen;
	
		char *buff = malloc(len);
		
		bzero(buff, sizeof(buff));
		
		//read data 
		int ret = read(*(int *)connfd, (void *)buff, len);
		if(ret == -1)
		{
			perror("read :");
			free(bufflen);
			exit(0);
		}
		//else
		//{
			printf("\nClient : %s\n", buff);
		//}

		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
			exit(0);
		}
		bzero(buff, sizeof(buff));
		free(buff);
	}
}


void *writing(void *clifd)
{
	int flag = 1;
	
	while(flag)
	{
		//char buff[MAX];

		//bzero(buff, MAX);

		printf("\nServer : ");

		//		int i = 0;

		//		while((buff[i++] = getchar()) != '\n')
		//			;
		//		buff[i] = '\0';


		int current_size = MAX;

		char *buff = malloc(MAX);
		bzero(buff, MAX);
	
		int i = 0;

		while (( buff[i++] = getchar() ) != '\n')
		{
			if(i == current_size)
			{
				current_size = i + MAX;
				buff = realloc(buff, current_size);
				perror("realloc");
			}
		}

		buff[i] = '\0';

		printf("buffer value = %s", buff);
		
		int *bufflen = &i;
		printf("bufflen = %d\n", *bufflen);

		if((write(*(int *)clifd, bufflen, sizeof(bufflen))) == -1)
		{
			perror("write");
		}
		
		int wri1 = write(*(int *)clifd, buff, i);
		if(wri1 == -1)
		{
			perror("write : ");
			exit(0);
		}

		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
			exit(0);
		}
		
	}
}

int main()
{
	int sockfd, connfd;
	socklen_t len;	
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		printf("socket creation failed...\n");
		close(sockfd);
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	memset(&servaddr, '\0', sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// long integer from host byte order to network byte order.
	servaddr.sin_port = htons(PORT);	// short integer from host byte order to network byte order.

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		close(sockfd);
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// server listens
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		close(sockfd);
		exit(0);
	}
	else
		printf("Server listening..\n");

	len = sizeof(cli);
	// Accept the data from client
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);

	if (connfd < 0) {

		printf("server accept failed...\n");
		close(sockfd);
		exit(0);
	}
	else{
		printf("server accepted the client...\n");
	}


	// client
	int clifd;
	clifd = socket(AF_INET, SOCK_STREAM, 0);
	if (clifd == -1)
	{
		perror("socket");
	}
	else
		printf("socket created..\n");

	struct sockaddr_in cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	cliaddr.sin_port = htons(8081);

	if((connect(clifd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)))== -1)
	{
		perror("connect : ");
		exit(0);
	}
	else
		printf("connected to socket\n");



	printf("\n****************** Server  ***************************\n\n");

	int flag = 1;

	// creating threads

	pthread_t t1, t2;


	int thread1 = pthread_create(&t1, NULL, (void *)reading, (void *)&connfd);
	if(thread1 != 0)
	{
		perror("thread1");
	}

	int thread2 = pthread_create(&t2, NULL, (void *)writing, (void *)&clifd);
	if(thread2 != 0)
	{
		perror("thread2");
	}


	pthread_join(t1, NULL);
	pthread_join(t2, NULL);


	// After chatting close the socket
	close(sockfd);
	close(clifd);
	return 0;
}
