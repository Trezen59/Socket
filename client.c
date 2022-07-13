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
#include <fcntl.h>


#define MAX 100
#define PORT1 8080
#define PORT2 8081



void recvfile(int connfd)
{
	char loc[MAX] = "/home/trezen/Desktop/Assignment/server/";
	char name[MAX] = "file.txt";

//	char buff[MAX];
//	bzero(buff, MAX);
	int *size = malloc(sizeof(int));
	bzero(size, sizeof(size));

	strcat(loc, name);

	if(access(loc, F_OK | R_OK) != -1)
	{
		printf("file already found\n");
	}
	else
	{
		int fd = open((char *)loc, O_RDWR | O_CREAT);
		if(fd == -1)
		{
			printf("error in opening fd\n");
		}
		
		// get length

		if((read(connfd, size, sizeof(size))) == -1)
			perror("read");
		else
			printf("read length success\n");

		printf("size recieved is %d\n", *size);

		char *buff = malloc(*size);
		bzero(buff, *size);

		int ret = read(connfd, buff, (int)*size);
		if(ret == -1)
		{
			printf("read unsuccessful\n");
		}

		//printf("buffer value after read is : %s", buff);

		int len = write(fd, buff, (int)*size);
		if(len == -1)
		{
			printf("write unsuccessful\n");

		}
		else
		{
			printf("written to file\n");
			printf("buffer content after write : %s", buff);
		}
			bzero(buff, sizeof(size));

	}
}



void *reading(void *connfd)
{
	int flag = 1;
	int *bufflen = malloc(sizeof(int));
	if(bufflen == NULL)
	{
		printf("failed to allocate memory\n");
//		close(*(int *)connfd);
//		exit(0);

	}
	
	char *buff = malloc(*bufflen);
	if(buff == NULL)
	{
		printf("failed to allocate memory\n");
		//			close(*(int *)connfd);
		//			free(bufflen);
		//exit(0);
		
	}

	while(flag) 
	{

		bzero(bufflen, sizeof(bufflen));
		// read length of buffer
		if((read(*(int *)connfd, bufflen, sizeof(bufflen))) == -1)
		{
			perror("read");
//			free(bufflen);
//			close(*(int *)connfd);
			//exit(0);
			break;
		}	
		//printf("recieved len : %d\n", *bufflen);


		bzero(buff, sizeof(buff));

		//read data 

		int ret = read(*(int *)connfd,(void *)buff, *bufflen);
		if(ret == -1)
		{
			perror("read :");
//			free(bufflen);
//			free(buff);
//			close(*(int *)connfd);
			//exit(0);
			break;
		}
		printf("\n\t\t\t%s\n", buff);


		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
//			close(*(int *)connfd);
//			free(bufflen);
//			free(buff);
			//exit(0);
			//break;
			return NULL;
		}
		bzero(buff, sizeof(buff));

	}
	free(buff);
	free(bufflen);
	pthread_exit(NULL);
}


void *writing(void *clifd)
{
	int flag = 1;

	char *buff = malloc(MAX);
	if(buff == NULL)
	{
		printf("failed to allocate memory\n");
//		close(*(int *)clifd);
//		exit(0);

	}


	int *bufflen = malloc(sizeof(int));
	
	while(flag)
	{
		printf("\n");

		int current_size = MAX;
		bzero(buff, sizeof(buff));

		int i = 0;

		while (( buff[i++] = getchar() ) != '\n')
		{
			if(i == current_size)
			{
				current_size = i + MAX;
				buff = realloc(buff, current_size);
				if(buff == NULL)
				{
					printf("failed to reallocate memory\n");
//					close(*(int *)clifd);
//					free(buff);
					//exit(0);
					break;
				}

			}
		}

		buff[i] = '\0';

		bufflen = &i;

		//printf("bufflen = %d\n", *bufflen-1);

		if((write(*(int *)clifd, bufflen, sizeof(bufflen))) == -1)
		{
			perror("write");
//			free(buff);
//			free(bufflen);
//			close(*(int *)clifd);
			//exit(0);
			break;
		}

		int wri1 = write(*(int *)clifd, buff, i);
		if(wri1 == -1)
		{
			perror("write : ");
//			close(*(int *)clifd);
//			free(buff);
//			free(bufflen);			
			//exit(0);
			break;
		}

		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
//			close(*(int *)clifd);
			//free(buff);
			//free(bufflen);
			//exit(0);
			//break;
			return NULL;
		}
		bzero(buff, MAX);
	}
	free(buff);
	free(bufflen);
	pthread_exit(NULL);
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


//	int reuse1 = 1;
//	if(setsockopt(clifd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse1, sizeof(reuse1)) < 0)
//		perror("setsockopt(SO_REUSEADDR) failed");

	bzero(&cliaddr, sizeof(cliaddr));

	// assign IP, PORT
	cliaddr.sin_family = AF_INET;
	// enter IP of server to connect
	cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // currently on loopback
	cliaddr.sin_port = htons(PORT1);

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
	else 
		printf("socket successfully created\n");

//	int reuse2 = 1;
//	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse2, sizeof(reuse2)) < 0)
//		perror("setsockopt(SO_REUSEADDR) failed");
	
	struct sockaddr_in servaddr, cli;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT2);

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
	// menu for message and file

	int choice;


	pthread_t t1, t2;
	
	int thread1, thread2;
	char choice_buff[MAX];

	do
	{
		
		bzero(choice_buff, MAX);			
		read(connfd, choice_buff, sizeof(choice_buff));
		printf("%s", choice_buff);

		printf("\n\n1) Chat.\n2) Send File.\n3) Exit.\n\n Please enter a choice : ");
		scanf("%d", &choice);

		switch(choice)
		{

			case 1:
				// message
				

				// creating threads
				printf("\n\n ********** Chat APP started ************ \n\n");
				printf("\n send 'bye' to exit chat app\n");
				
				thread1 = pthread_create(&t1, NULL, (void *)writing, (void *)&clifd);
				if(thread1 != 0)
				{
					perror("thread 1");
				}

				thread2 = pthread_create(&t2, NULL, (void *)reading, (void *)&connfd);
				if(thread2 != 0)
				{
					perror("thread 2");
				}



				pthread_join(t1, NULL);
				pthread_join(t2, NULL);
				printf("\nChat ended\n\n");
				

				break;

			case 2:
				// file
				printf("\nrecieving file \n");
				
				recvfile(connfd);

				break;

			case 3:
				// Exit 
				printf("Thank you for using this App.\n\n");
				close(connfd);
				close(sockfd);
				close(clifd);
				exit(0);

			default:
				// message
				// creating threads
				printf("\n\n ********** Chat APP started ************ \n\n");
				printf("\n send 'bye' to exit chat app\n");
				
				thread1 = pthread_create(&t1, NULL, (void *)writing, (void *)&clifd);
				if(thread1 != 0)
				{
					perror("thread 1");
				}

				thread2 = pthread_create(&t2, NULL, (void *)reading, (void *)&connfd);
				if(thread2 != 0)
				{
					perror("thread 2");
				}


				pthread_join(t1, NULL);
				pthread_join(t2, NULL);

				printf("\nChat ended\n\n");
		}
		// choice end
	}while(choice);
	// menu end

	// close the socket
	close(sockfd);

	return 0;
}

