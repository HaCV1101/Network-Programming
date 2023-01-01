#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#include "lib.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024

int client_sock;
char buff[BUFF_SIZE + 1];
struct sockaddr_in server_addr; /* server's address information */
int msg_len, bytes_sent, bytes_received;

int signin = 0;
char myUsername[256];

void clear_stdin_buff()
{
	char ch;
	while ((ch = getchar() != EOF && ch != '\n'))
		;
}

int connectToServer()
{
	// Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	// Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		exit(0);
	}
}

void Menu()
{
	printf("1. Sign in\n");
	printf("2. Sign outn\n");
	printf("3. Exit\n");
	printf("Choice: ");
}

void SignIn()
{
	char username[256], password[256];
	char res[256];
	printf("Username: ");
	scanf("%s", username);

	send(client_sock, username, sizeof(username), 0);

	printf("Password: ");
	scanf("%s", password);

	send(client_sock, password, sizeof(password), 0);

	recv(client_sock, res, sizeof(res), 0);

	if (strcmp(res, "success"))
	{
		signin = 1;
		printf("Login success\n");
		strcpy(myUsername, username);
	}
	else
		printf("%s\n", res);
}

void SignOut()
{
	signin = 0;
}

void Run()
{
	char n;
	while (1)
	{
		Menu();
		scanf("%c", &n);
		clear_stdin_buff();

		send(client_sock, &n, sizeof(n), 0);
		switch (n)
		{
		case '1':
			/* code */
			SignIn();
			break;

		case '2':
			SignOut();
			break;

		case '3':
			exit(0);

		default:
			break;
		}
	}
}

int main()
{

	connectToServer();
	Run();
}
