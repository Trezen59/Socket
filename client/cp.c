// TCP client code for chatting application 

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


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
		printf("recieved len : %d\n", *bufflen);

		int len = *bufflen;
		char *buff = malloc(len);
		
		bzero(buff, sizeof(buff));
		
		//read data 

		int ret = read(*(int *)connfd,(void *)buff, len);
		if(ret == -1)
		{
			perror("read :");
			free(bufflen);
			exit(0);
		}
		//else
		//{
			printf("\nServer : %s\n", buff);

		//}

		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
			exit(0);
		}
		bzero(buff, sizeof(buff));

	}
}


void *writing(void *clifd)
{
	int flag = 1;

	while(flag)
	{
		//char buff[MAX];

		//bzero(buff, MAX);

		printf("\nClient : ");

		//		int i = 0;

		//		while((buff[i++] = getchar()) != '\n')
		//			;
		//		buff[i] = '\0';


		int current_size = MAX;

		char *buff = malloc(MAX);
		bzero(buff, sizeof(buff));

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

		int *bufflen = &i;
		printf("bufflen = %d\n", *bufflen-1);

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
		bzero(buff, MAX);
	}
}

int main()
{
	// client

	int clifd;
	struct sockaddr_in cliaddr;

	// socket create and verification
	clifd = socket(AF_INET, SOCK_STREAM, 0);

	if (clifd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	bzero(&cliaddr, sizeof(cliaddr));

	// assign IP, PORT
	cliaddr.sin_family = AF_INET;
	// enter IP of server to connect
	cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // currently on loopback
	cliaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(clifd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) == -1) 
	{
		printf("connection with the server failed...\n");
		close(clifd);
		exit(0);
	}
	else
		printf("connected to the server..\n");


	// server

	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("socket");
		exit(0);
	}


	struct sockaddr_in servaddr, cli;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8081);

	if((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0)
	{
		perror("bind : ");
		exit(0);
	}
	else
		printf("socket successfully binded\n");

	if((listen(sockfd, 5)) != 0)
	{
		perror("listen : ");
		exit(0);
	}
	else
	{
		printf("server listening\n");
	}

	socklen_t len = sizeof(cli);

	int connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if(connfd == -1)
	{
		perror("connfd : ");
		exit(0);
	}
	else
		printf("server accepted the client \n");



	printf("\n************************ Client  ************************\n\n");

	int flag = 1;
	// creating threads

	pthread_t t1, t2;

	//	while(flag)
	//	{

	int thread1 = pthread_create(&t1, NULL, (void *)writing, (void *)&clifd);
	if(thread1 != 0)
	{
		perror("thread 1");
	}

	//		sleep(3);
	int thread2 = pthread_create(&t2, NULL, (void *)reading, (void *)&connfd);
	if(thread2 != 0)
	{
		perror("thread 2");
	}
	//		sleep(3);


	pthread_join(t1, NULL);
	pthread_join(t2, NULL);	

	//	}

	// close the socket
	close(sockfd);

	return 0;
}

