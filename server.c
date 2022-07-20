// server app

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX 100
#define PORT1 8080
#define PORT2 8081


// function to recieve file

void recvfile(int connfd)
{
	// file location and name
	char loc[MAX] = "/home/trezen/Desktop/task/";
	char name[MAX] = "file1.txt";
	
	// init size to store size of file
	int *size = malloc(sizeof(int));
	bzero(size, sizeof(int));
	
	// join location and name 
	strcat(loc, name);

	// check if file exists at a given location
	if(access(loc, F_OK | R_OK) != -1)
	{
		printf("file already found\n");
		//return;

	}
	else
	{
		// open and create file if file not available
		int fd = open((char *)loc, O_RDWR | O_CREAT);
		if(fd == -1)
		{
			printf("error in opening fd\n");
		}
		
		// get file size
		if((read(connfd, size, sizeof(int))) == -1)
			perror("read");
		else
			printf("read length success\n");

		printf("size recieved is %d\n", *size);

		char *buff = (char *)malloc(*size);
		bzero(buff, *size);

		// read file into buff
		int ret = read(connfd, buff, *size);
		if(ret == -1)
		{
			printf("read unsuccessful\n");
		}

		//printf("buffer value after read is : %s", buff);
		
		// write the contents of buff to file
		int len = write(fd, buff, *size);
		if(len == -1)
		{
			printf("write unsuccessful\n");

		}
		else
		{
			printf("written to file\n");
//			printf("buffer content after write : %s", buff);
		}
		bzero(buff, *size);
	
	}
		free(size);
}


// function to send the file
void sendfile(int clifd)
{
	// location and name of file
	char loc[MAX] = "/home/trezen/";
	char name[MAX] = "input.txt";

	// file size
	struct stat st;
		
	strcat(loc, name);

	// check if file exists
	if(access(loc, F_OK | R_OK) != -1)
	{
		printf("file is found and can be read..\n");
		int fd = open((char *)loc, O_RDONLY);
		if(fd == -1)
		{
			printf("error in opening fd\n");
		}

		// get file size
		stat(loc, &st);
		int *size = malloc(sizeof(int)); 
		*size = st.st_size;

		// init buffer with file size
		char *buff = (char *)malloc(*size);
		bzero(buff, *size);

		printf("size of file is %d\n", *size);
		
		// send length		
		write(clifd, size, sizeof(int));

		// store contents of file into buffer
		int len = read(fd, buff, *size);
		buff[len] = '\0';

		printf("lenth read = %d\n", len);
//		printf("file contents : \n%s", buff);
		
		// send file
		int ret = write(clifd, buff, *size);
		if(ret == -1)
		{
			printf("write unsuccessful\n");
		}
		else
			printf("write successful. File sent\n");

		bzero(buff, *size);
		free(size);
	}
	else
	{
		printf("file not found\n");
	}
}



void *reading(void *connfd)
{
	int flag = 1;

	// declare bufflen to store length of msg
	int *bufflen = malloc(sizeof(int));
	if(bufflen == NULL)
	{
		printf("failed to allocate memory - bufflen - func reading \n");
		close(*(int *)connfd);
		exit(0);

	}
	bzero(bufflen, sizeof(int));

	while(flag)
	{

		bzero(bufflen, sizeof(int));		

		// read length of buffer
		if((read(*(int *)connfd, bufflen, sizeof(int))) == -1)
		{
			perror("read");
			//free(bufflen);
			//exit(0);
			break;
		}	
		printf("recieved length : %d\n", *bufflen);

		char *buff = malloc(*bufflen);
		if(buff == NULL)
		{
			printf("failed to allocate memory.. buff - func reading\n");
			//close(*(int *)connfd);
			//free(bufflen);
			//exit(0);

		}

		bzero(buff, *bufflen);

		//read data into buff
		int ret = read(*(int *)connfd, (void *)buff, *bufflen);
		if(ret == -1)
		{
			perror("read :");
			//free(bufflen);
			//free(buff);
			//exit(0);
			break;
		}
		printf("\n\t\t\t %s\n", buff);
		
		// compare and check for "bye" to exit chat
		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
			//free(buff);
			//free(bufflen);
			//exit(0);
			break;
			//return NULL;
		}
		bzero(buff, *bufflen);
		free(buff);
	}

	free(bufflen);
	pthread_exit(NULL);
}


void *writing(void *clifd)
{
	int flag = 1;
	
	// init buffer with a set size
	char *buff = malloc(MAX);
	if(buff == NULL)
	{
		printf("failed to allocate memory.. - buff - func writing\n");
		close(*(int *)clifd);
		exit(0);

	}


	int *bufflen = malloc(sizeof(int));

	while(flag)
	{

		printf("\n");

		int current_size = MAX;

		bzero(buff, MAX);

		int i = 0;

		// get the message and store it in buffer
		while (( buff[i++] = getchar() ) != '\n')
		{
			if(i == current_size)
			{
				current_size = i + MAX;
				// reallocate memory if set mem size exceeded
				buff = realloc(buff, current_size);
				if(buff == NULL)
				{
					printf("failed to reallocate memory..\n");
					//close(*(int *)clifd);
					//free(buff);
					//exit(0);
					break;
				}
			}
		}

		buff[i] = '\0';

		//printf("buffer value = %s", buff);
		printf("i = %d\n", i);
		// store message length in bufflen
		*bufflen = i;

		//printf("bufflen = %d\n", *bufflen);
		
		// send the bufflen 
		if((write(*(int *)clifd, bufflen, sizeof(int))) == -1)
		{
			perror("write");
			//free(buff);
			//free(bufflen);
			//exit(0);
			break;
		}
		
		// write the content
		int wri1 = write(*(int *)clifd, buff, i);
		if(wri1 == -1)
		{
			perror("write : ");
			//close(*(int *)clifd);
			//free(buff);
			//free(bufflen);
			//exit(0);
			break;
		}
	
		// compare msg for "bye" to exit chat
		if(!(strncmp("bye", buff , 3)))
		{
			flag = 0;
			//free(buff);
			//free(bufflen);
			//close(*(int *)clifd);
			//exit(0);
			break;
			//return NULL;

		}
		bzero(buff, i);
	}

	free(buff);
	free(bufflen);
	pthread_exit(NULL);
}


int create_socket()
{
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		close(sockfd);
		exit(0);
	}
	else
	{
		printf("Socket successfully created..\n");
		return sockfd;
	}
}


// main function
int main()
{
	// server

	int sockfd, connfd;
	socklen_t len;	
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = create_socket();

	// to reuse socket with same address
	int reuse1 = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse1, sizeof(reuse1)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");

	memset(&servaddr, '\0', sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// long integer from host byte order to network byte order.
	servaddr.sin_port = htons(PORT1);	// short integer from host byte order to network byte order.

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

	// create a socket
	clifd = create_socket();

	// to reuse socket with same address
	int reuse2 = 1;
	if (setsockopt(clifd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse2, sizeof(reuse2)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");

	struct sockaddr_in cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));

	// address structure
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	cliaddr.sin_port = htons(PORT2);
	
	// connect to server
	if((connect(clifd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)))== -1)
	{
		perror("connect");
		exit(0);
	}
	else
		printf("connected to socket\n");



//	printf("\n****************** Server  ***************************\n\n");

	pthread_t t1, t2;

	int thread1, thread2;
	int choice;

	char choice_buff[MAX];

	// menu 

	do
	{
		
//		bzero(choice_buff, MAX);			
//		read(connfd, choice_buff, sizeof(choice_buff));
//		printf("%s", choice_buff);
		
		
		printf("\n1) Chat.\n2) Send File.\n3) Recieve file.\n4) Exit.\n\n Please enter a choice : ");
		scanf("%d", &choice);
	
		// send the choice to other app
		char *c;
		asprintf(&c, "%d", choice);	
		strcpy(choice_buff, "\n The choice selected is ");
		strcat(choice_buff, c);

		write(clifd, choice_buff, sizeof(choice_buff));
		bzero(choice_buff, MAX);


		// switch case for multiple choices
		switch(choice)
		{

			case 1:
				// message

				printf("\n\n  ***********Chat APP started**********\n\n");
				printf("\nsend 'bye' to exit chat app\n");
				// creating threads
				thread1 = pthread_create(&t1, NULL, (void *)reading, (void *)&connfd);
				if(thread1 != 0)
				{
					perror("thread1");
				}

				thread2 = pthread_create(&t2, NULL, (void *)writing, (void *)&clifd);
				if(thread2 != 0)
				{
					perror("thread2");
				}

				// terminating the threads
				pthread_join(t1, NULL);
				pthread_join(t2, NULL);


				printf("Chat ended\n\n");

				break;

			case 2:
				// send file
				printf("\nsending file\n");

				sendfile(clifd);
				
				break;

			case 3:
				// recieve file
				printf("recieving file..\n");

				recvfile(connfd);
				
				break;

			case 4:
				// Exit 
				printf("Thank you for using this App.\n\n");
				close(connfd);
				close(sockfd);
				close(clifd);
				exit(0);

			default:
				// message
				printf("\n\n  ***********Chat APP started**********\n\n");
				printf("\nsend 'bye' to exit chat app\n");
				// creating threads
				thread1 = pthread_create(&t1, NULL, (void *)reading, (void *)&connfd);
				if(thread1 != 0)
				{
					perror("thread1");
				}

				thread2 = pthread_create(&t2, NULL, (void *)writing, (void *)&clifd);
				if(thread2 != 0)
				{
					perror("thread2");
				}

				// execute and terminate the thread
				pthread_join(t1, NULL);
				pthread_join(t2, NULL);

				printf("\nChat Ended\n");

		}
		// choice end
	}while(choice);
	// menu end


	// close the socket
	close(sockfd);
	close(clifd);
	return 0;
}
