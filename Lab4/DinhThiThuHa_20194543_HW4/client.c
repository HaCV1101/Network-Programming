/*UDP Echo Client*/
#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "lib.h"
// #define SERV_PORT 5550
// #define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

/// global variables
int client_sock;
char buff[BUFF_SIZE];
struct sockaddr_in server_addr;
int bytes_sent, bytes_received;
int sin_size = sizeof(struct sockaddr);
;

int SERV_PORT;
char SERV_IP[50];

int loggedIn;
char currentAcc[30];
int logSend = 0;
// char numString[BUFF_SIZE];
// char charString[BUFF_SIZE];

void printMenu()
{ // program menu, print after every function done
	printf("-----------------------------\n");
	printf("CLIENT SIDE\n");
	printf("-----------------------------\n");
	printf("You are currently signed in with account %s. \n", currentAcc);
	printf("1. Change password\n");
	printf("2. Send chat\n");
	printf("3. Check received message (check all messages that other has sent)\n");
	printf("Your choice (1-3, leave blank to quit, \"bye\" to sign out), enter to confirm: ");
}

void inputString(char *string, char *alert)
{
	printf("%s : ", alert);
	fgets(string, 50, stdin);
	string[strlen(string) - 1] = '\0';
	if (strlen(string) == 0)
	{
		if (loggedIn == 0)
		{
			printf("Exiting ...\n");
			sendto(client_sock, "exit", 6, 0, (struct sockaddr *)&server_addr, sin_size); // send first message to server

			exit(0);
		}
		else
			return;
	}
}

void Login(int *log)
{ // Sign in

	strcpy(buff, "login");
	sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size); // send first message to server

	char password[50];
	inputString(currentAcc, "Enter username, leave blank to exit");									   // ask client to input username
	sendto(client_sock, currentAcc, sizeof(currentAcc), 0, (struct sockaddr *)&server_addr, sin_size); // send username to server
	recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);		   // receive result
	// printf("%s\n", buff);
	if (strcmp(buff, "ok") != 0) // not ok - result some error => print error
		printf("%s\n", buff);
	else
	{
		// strcpy(currentAcc, string);		// save current username
		inputString(password, "Enter password, leave blank to exit"); // ask client to input password
		// sendto(client_sock, "password", 10, 0, (struct sockaddr *) &server_addr, sin_size);	// send password to server
		sendto(client_sock, password, sizeof(password), 0, (struct sockaddr *)&server_addr, sin_size); // send password to server
		recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);	   // receive result
		if (strcmp(buff, "Wrong password") == 0)
		{ // wrong password
			printf("%s\n%s\n", buff, logginFail);
		}
		else if (strcmp(buff, "ok") == 0)
		{			  // login success
			*log = 1; // mark that this client has logged into an account
			printf("%s\n", loginSuccess);
		}
		else
		{
			printf("%s\n", buff);
		}
	}
}

void Case1()
{ // change password
	char password[50];

	inputString(password, "Enter new password(only contains numbers and letters), leave blank to cancel");
	if (strlen(password) == 0)
	{
		sendto(client_sock, "cancel", 10, 0, (struct sockaddr *)&server_addr, sin_size);
		printf("Canceled!\n");
	}
	sendto(client_sock, "password", 10, 0, (struct sockaddr *)&server_addr, sin_size);			   // send new password to server
	sendto(client_sock, password, sizeof(password), 0, (struct sockaddr *)&server_addr, sin_size); // send new password to server
	recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);	   // receive result
	// printf("%s\n", buff);
	if (strcmp(buff, "ok") != 0)
	{ // not ok - result some error => print error
		printf("%s\n", buff);
	}
	else
	{ // login success
		printf("%s\n", changePassSucces);
		recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive result
		printf("%s\n", buff);
		recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive result
		printf("%s\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", buff);
	}
}

void Case2()
{ // send message
	char msg[BUFF_SIZE];

	inputString(msg, "Enter message (except \"bye\"), leave blank to cancel");
	if (strlen(msg) == 0)
	{
		sendto(client_sock, "cancel", 10, 0, (struct sockaddr *)&server_addr, sin_size);
		printf("Canceled!\n");
		return;
	}
	sendto(client_sock, "message", 10, 0, (struct sockaddr *)&server_addr, sin_size);		   // send new password to server
	sendto(client_sock, msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, sin_size);	   // send message to server
	recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive result
	// printf("%s\n", buff);
	if (strcmp(buff, "ok") != 0)
	{ // not ok - result some error => print error
		printf("%s\n", buff);
	}
	else
	{ // message sending success
		printf("Message sent!\n");
	}
}

void Case3()
{																							   // show received message
	recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive number of messages waiting to receive
	int msgCount = charToNum(buff);															   // message count
	if (msgCount == 0)
		printf("Current have 0 message\n");
	else
	{
		int count = 1;
		char string[BUFF_SIZE];
		while (count <= msgCount)
		{
			strcpy(string, "");
			recvfrom(client_sock, string, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive number string
			printf("%d.  %s\n", count, string);
			strcpy(string, "");
			recvfrom(client_sock, string, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive letter string
			printf("     %s\n", string);
			count++;
		}
	}
}

void startConnect(char *ip, int port)
{ // initial and start connection with server

	strcpy(SERV_IP, ip);
	SERV_PORT = port;
	// Step 1: Construct a UDP socket
	if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	// Step 3: Start communicating with server
	strcpy(buff, "start");
	sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size); // send first message to server
	logSend = 1;
	recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size); // receive result
	if (strcmp(buff, "connect") == 0)
	{
		printf("Connected\n");
	}
}

int main(int argc, char *argv[])
{

	if (argc == 3)
	{
		startConnect(argv[1], charToNum(argv[2]));
	}
	else
	{
		printf("Wrong argument\n");
		exit(0);
	}

	loggedIn = 0;

	char c[6];
	while (1)
	{
		// printf("%d log\n", loggedIn);
		if (loggedIn == 0)
		{ // not log in => log in
			Login(&loggedIn);
			continue;
		}
		//// already logged in
		printMenu();		// print Menu
		fgets(c, 6, stdin); // choose option
		c[strlen(c) - 1] = '\0';
		if (strcmp(c, "bye") == 0)
		{ // user enter "bye" and log out
			printf("Goodbye %s\n", currentAcc);
			sendto(client_sock, c, sizeof(c), 0, (struct sockaddr *)&server_addr, sin_size); // send option to server
			loggedIn = 0;
			continue;
		}
		printf("%s\n", c);
		if (strlen(c) != 1)
			continue; // not an valid option
		if (c[0] < '1' || c[0] > '3')
			continue;															 // not an valid option
		sendto(client_sock, c, 1, 0, (struct sockaddr *)&server_addr, sin_size); // send option to server
		switch (c[0])															 // solving cases
		{
		case '1':
		{
			Case1();
			break;
		}
		case '2':
		{
			Case2();
			break;
		}
		case '3':
		{
			Case3();
			break;
		}

		default:
			break;
		}
	}
	close(client_sock);
}
