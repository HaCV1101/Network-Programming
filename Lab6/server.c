#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>

#include "lib.h"

#define PORT 5550
#define BACKLOG 20
#define BUFF_SIZE 1024

int listen_sock, conn_sock; /* file descriptors */
struct sockaddr_in server;	/* server's address information */
struct sockaddr_in client;	/* client's address information */
pid_t pid;
int sin_size;

List *list;

/* Handler process signal*/
// void sig_chld(int signo);

/*
 * Receive and echo message to client
 * [IN] sockfd: socket descriptor that connects to client
 */
// void echo(int sockfd);

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	/* Wait the child process terminate */
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("\nChild %d terminated\n", pid);
}

int login(int signedIn)
{
	if (signedIn == 1)
	{
		printf("Already logged in with an account\n");
		return signedIn;
	}
	char buff[BUFF_SIZE];
	char res[256];

	recv(conn_sock, buff, sizeof(buff), 0);

	Account *now = findAccount(list, buff);
	recv(conn_sock, buff, sizeof(buff), 0);
	if (now == NULL)
		strcpy(res, "Account not exists");
	else
	{

		if (strcmp(now->password, buff) == 0)
			strcpy(res, "success");
		else
			strcpy(res, "Wrong password");
	}

	send(conn_sock, res, sizeof(res), 0);

	if (strcmp(res, "success") == 0)
		return 1;
	else
		return 0;
}

int logout(int signedIn)
{
	if (signedIn == 1)
		return 0;
	else
		return 1;
}

void echo(int sockfd)
{
	char buff[BUFF_SIZE];
	int signedIn = 0;
	int check = 0;
	while (1)
	{
		if (check == 1)
			break;
		recv(conn_sock, buff, sizeof(buff), 0);
		switch (buff[0])
		{
		case '1':
			signedIn = login(signedIn);
			break;

		case '2':
			signedIn = logout(signedIn);
			break;

		case '3':
			check = 1;
			break;

		default:
			break;
		}
	}

	close(sockfd);
}

int createServer()
{
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		printf("socket() error\n");
		return 0;
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listen_sock, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);
}

int Run()
{
	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("\nError: ");
				return 0;
			}
		}

		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();

		/* fork() is called in child process */
		if (pid == 0)
		{
			close(listen_sock);
			printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			echo(conn_sock);
			exit(0);
		}

		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
}

int main()
{
	createServer();
	list = init();
	readFile(list);
	Run();
}