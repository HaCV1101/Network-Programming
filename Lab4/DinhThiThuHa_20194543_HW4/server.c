/*UDP Echo Server*/
#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "lib.h"
#include "account.h"

// #define PORT 5550  /* Port that will be opened */
#define BUFF_SIZE 1024

//// global variables
int server_sock; /* file descriptors */
char buff[BUFF_SIZE];
int bytes_sent, bytes_received;
struct sockaddr_in server;					 /* server's address information */
struct sockaddr_in client, client1, client2; /* client's address information */
int sin_size = sizeof(struct sockaddr_in);
int PORT;

char acc1[50], acc2[50];
char firstAcc[50];
int firstLog = 0;

LIST *accList;
M_QUEUE *Q1;
M_QUEUE *Q2;
int step01;
int step02;

int type01, type02;

int checkString(char *string)
{ // check if a valid password (only numbers and letters) or not
	int l = strlen(string);
	for (int i = 0; i < l; i++)
	{
		if (string[i] < '0' || string[i] > '9')						  // numbers
			if (tolower(string[i]) < 'a' || tolower(string[i]) > 'z') // letters
				return 0;											  // invalid string
	}
	return 1; // valid string
}

void Signin(int user)
{
	if (user == 1)
	{
		switch (step01)
		{
		case 1:
		{
			// bytes_received = recvfrom(server_sock, buff, 50, 0, (struct sockaddr *) &client1, &sin_size);
			// buff[bytes_received] = '\0';
			printf("[Client 1 with port %d] says: %s with acc %s\n", ntohs(client1.sin_port), "need login", buff);
			ELEMENT *this = findAccount(accList, buff);
			if (this == NULL)
			{
				sendto(server_sock, "Account not existed", longS, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				step01 = 0;
				type01 = 0;
			}
			else
			{
				strcpy(acc1, buff);
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				step01++;
			}
			break;
		}

		case 2:
		{
			// bytes_received = recvfrom(server_sock, buff, 50, 0, (struct sockaddr *) &client1, &sin_size);
			// buff[bytes_received] = '\0';
			ELEMENT *this = findAccount(accList, acc1);
			if (strcmp(buff, this->password) == 0)
			{
				if (this->status == 0)
				{
					strcpy(acc1, "");
					sendto(server_sock, accBlocked, sizeof(accBlocked), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
					step01 = 0;
					type01 = 0;
					break;
				}

				if (strlen(firstAcc) == 0)
					strcpy(firstAcc, this->username);
				else
				{
					if (strcmp(firstAcc, this->username) == 0 && firstLog == 2)
					{
						strcpy(acc1, "");
						sendto(server_sock, "Other has logged in to this account", longS, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
						step01 = 0;
						type01 = 0;
						break;
					}
				}
				if (firstLog == 0)
					firstLog = 1;
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				printf("Client 1 login success into %s\n", acc1);
				step01 = 0;
				type01 = 0;
				break;
			}
			else
			{
				sendto(server_sock, "wrong password", shortS, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				step01 = 0;
				type01 = 0;
			}
		}

		default:
			break;
		}
	}

	else
	{
		switch (step02)
		{
		case 1:
		{
			// bytes_received = recvfrom(server_sock, buff, 50, 0, (struct sockaddr *) &client2, &sin_size);
			// buff[bytes_received] = '\0';
			printf("[Client 2 with port %d] says: %s with acc %s\n", ntohs(client2.sin_port), "need login", buff);
			ELEMENT *this = findAccount(accList, buff);
			if (this == NULL)
			{
				sendto(server_sock, "Account not existed", longS, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				step02 = 0;
				type02 = 0;
			}
			else
			{
				strcpy(acc2, buff);
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				step02++;
			}
			break;
		}

		case 2:
		{
			// bytes_received = recvfrom(server_sock, buff, 50, 0, (struct sockaddr *) &client2, &sin_size);
			// buff[bytes_received] = '\0';
			ELEMENT *this = findAccount(accList, acc2);
			if (strcmp(buff, this->password) == 0)
			{
				if (this->status == 0)
				{
					strcpy(acc2, "");
					sendto(server_sock, accBlocked, sizeof(accBlocked), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
					step02 = 0;
					type02 = 0;
					break;
				}

				if (strlen(firstAcc) == 0)
					strcpy(firstAcc, this->username);
				else
				{
					if (strcmp(firstAcc, this->username) == 0 && firstLog == 1)
					{
						strcpy(acc2, "");
						sendto(server_sock, "Other has logged in to this account", longS, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
						step02 = 0;
						type02 = 0;
						break;
					}
				}
				if (firstLog == 0)
					firstLog = 2;
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				printf("Client 2 login success into %s\n", acc2);
				step02 = 0;
				type02 = 0;
				break;
			}
			else
			{
				sendto(server_sock, "wrong password", shortS, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				step02 = 0;
				type02 = 0;
			}
		}

		default:
			break;
		}
	}
}

void changePassword(int user)
{

	if (user == 1)
	{
		switch (step01)
		{
		case 1:
		{
			// recvfrom(server_sock, buff, 10, 0, (struct sockaddr *) &client1, &sin_size);
			if (strcmp(buff, "cancel") == 0)
			{
				step01 = 0;
				type01 = 0;
			}
			else
			{
				step01++;
			}
			break;
		}
		// case 2: {step01++; break;}
		case 2:
		{
			// recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *) &client1, &sin_size);
			if (checkString(buff) == 0)
				sendto(server_sock, "Invalid password", shortS, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
			else
			{
				ELEMENT *this = findAccount(accList, acc1);
				strcpy(this->password, buff);
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				updateFile(accList);
				char numS[50], charS[50];
				splitString(buff, charS, numS);
				sendto(server_sock, numS, sizeof(numS), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				sendto(server_sock, charS, sizeof(charS), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				printf("Done\n");
			}
			step01 = 0;
			type01 = 0;
			break;
		}

		default:
			break;
		}
	}
	else
	{
		switch (step02)
		{
		case 1:
		{
			// recvfrom(server_sock, buff, 10, 0, (struct sockaddr *) &client2, &sin_size);
			if (strcmp(buff, "cancel") == 0)
			{
				step02 = 0;
				type02 = 0;
			}
			else
			{
				step02++;
			}
			break;
		}
		// case 2: {step02++; break;}
		case 2:
		{
			// recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *) &client2, &sin_size);
			if (checkString(buff) == 0)
				sendto(server_sock, "Invalid password", shortS, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
			else
			{
				ELEMENT *this = findAccount(accList, acc2);
				strcpy(this->password, buff);
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				updateFile(accList);
				char numS[50], charS[50];
				splitString(buff, charS, numS);
				sendto(server_sock, numS, sizeof(numS), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				sendto(server_sock, charS, sizeof(charS), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				printf("Done\n");
			}
			step02 = 0;
			type02 = 0;
			break;
		}

		default:
			break;
		}
	}
}

void sendMessage(int user)
{
	if (user == 1)
	{
		switch (step01)
		{
		case 1:
		{
			// recvfrom(server_sock, buff, 10, 0, (struct sockaddr *) &client1, &sin_size);
			if (strcmp(buff, "cancel") == 0)
			{
				step01 = 0;
				type01 = 0;
			}
			else
				step01++;
			break;
		}
		// case 2: {step01++; break;}
		case 2:
		{
			// recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *) &client1, &sin_size);
			if (checkString(buff) == 0)
				sendto(server_sock, errorString, sizeof(errorString), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
			else
			{
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
				putQueue(Q1, buff);
				printf("Done\n");
			}
			step01 = 0;
			type01 = 0;
			break;
		}

		default:
			break;
		}
	}
	else
	{
		switch (step02)
		{
		case 1:
		{
			// recvfrom(server_sock, buff, 10, 0, (struct sockaddr *) &client2, &sin_size);
			if (strcmp(buff, "cancel") == 0)
			{
				step02 = 0;
				type02 = 0;
			}
			else
				step02++;
			break;
		}
		// case 2: {step02++; break;}
		case 2:
		{
			// recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *) &client2, &sin_size);
			if (checkString(buff) == 0)
				sendto(server_sock, errorString, sizeof(errorString), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
			else
			{
				sendto(server_sock, "ok", 3, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
				putQueue(Q2, buff);
				printf("Done\n");
			}
			step02 = 0;
			type02 = 0;
			break;
		}

		default:
			break;
		}
	}
}

void showMessage(int user)
{
	char count[10];
	if (user == 1)
	{
		numToString(Q2->count, count);
		sendto(server_sock, count, 10, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
		while (Q2->count != 0)
		{
			MESSAGE *now = popQueue(Q2);
			if (strlen(now->numS) != 0)
				sendto(server_sock, now->numS, sizeof(now->numS), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
			else
				sendto(server_sock, empty, sizeof(empty), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
			if (strlen(now->charS) != 0)
				sendto(server_sock, now->charS, sizeof(now->charS), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
			else
				sendto(server_sock, empty, sizeof(empty), 0, (struct sockaddr *)&client1, sizeof(struct sockaddr));
		}
		step01 = 0;
		type01 = 0;
		printf("Done\n");
	}
	else
	{
		numToString(Q1->count, count);
		sendto(server_sock, count, 10, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
		while (Q1->count != 0)
		{
			MESSAGE *now = popQueue(Q1);
			if (strlen(now->numS) != 0)
				sendto(server_sock, now->numS, sizeof(now->numS), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
			else
				sendto(server_sock, empty, sizeof(empty), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
			if (strlen(now->charS) != 0)
				sendto(server_sock, now->charS, sizeof(now->charS), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
			else
				sendto(server_sock, empty, sizeof(empty), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr));
		}
		step02 = 0;
		type02 = 0;
		printf("Done\n");
	}
}

void startServer(int port)
{
	PORT = port;

	// Step 1: Construct a UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);		 /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY puts your IP address automatically */
	bzero(&(server.sin_zero), 8);		 /* zero the rest of the structure */

	if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}

	printf("Server start at port %d\n", PORT);

	// Step 3: Communicate with clients

	Q1 = createNew();
	Q2 = createNew();

	memset(&client, '\0', sizeof(client));
	memset(&client1, '\0', sizeof(client1));
	memset(&client2, '\0', sizeof(client2));

	recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client1, &sin_size);
	printf("Client 1 with port %d connected\n", ntohs(client1.sin_port));

	recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client2, &sin_size);
	printf("Client 2 with port %d connected\n", ntohs(client2.sin_port));
	sendto(server_sock, "connect", 10, 0, (struct sockaddr *)&client1, sizeof(struct sockaddr)); /* send to the client2 */
	sendto(server_sock, "connect", 10, 0, (struct sockaddr *)&client2, sizeof(struct sockaddr)); /* send to the client2 */
}

int main(int argc, char *argv[])
{

	if (argc == 2)
	{
		startServer(charToNum(argv[1]));
	}
	else
	{
		printf("Wrong argument\n");
		exit(0);
	}

	/// create the account list
	accList = ini();
	readInput(accList); // read from input file account.txt
	strcpy(firstAcc, "");

	// char numString[BUFF_SIZE], charString[BUFF_SIZE];

	step01 = step02 = 0;
	type01 = type02 = 0;

	while (1)
	{

		if (strlen(acc1) == 0 && strlen(acc2) == 0)
		{
			strcpy(firstAcc, "");
			firstLog = 0;
		}

		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client, &sin_size);
		buff[bytes_received] = '\0';

		if (ntohs(client.sin_port) == ntohs(client1.sin_port))
		{

			switch (step01)
			{
			case 0:
			{
				if (strlen(buff) == 1)
				{
					type01 = buff[0] - '0';
					step01++;
					if (type01 == 3)
					{
						showMessage(1);
						step01 = 0;
						type01 = 0;
					}
				}
				else if (strcmp(buff, "bye") == 0)
				{
					printf("Client 1 [%s:%d] signout\n", inet_ntoa(client1.sin_addr), ntohs(client1.sin_port));
					strcpy(acc1, "");
					strcpy(firstAcc, acc2);
					firstLog = 2;
					sendto(server_sock, empty, sizeof(empty), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr)); /* send to the client2 */
					step01 = 0;
					type01 = 0;
				}
				else if (strcmp(buff, "exit") == 0)
				{
					printf("Client 1 [%s:%d] disconnected\n", inet_ntoa(client1.sin_addr), ntohs(client1.sin_port));
					step01 = 0;
				}
				else
					step01++;
				break;
			}
			default:
			{
				switch (type01)
				{
				case 0:
				{
					Signin(1);
					break;
				}
				case 1:
				{
					changePassword(1);
					break;
				}
				case 2:
				{
					sendMessage(1);
					break;
				}
				// case 3: {	showMessage(1); 	break;	}
				default:
					break;
				}
				break;
			}
			}

			printf("1 %d %d with %s\n", type01, step01, buff);
		}

		else
		{
			switch (step02)
			{
			case 0:
			{
				if (strlen(buff) == 1)
				{
					type02 = buff[0] - '0';
					step02++;
					if (type02 == 3)
					{
						showMessage(2);
						step02 = 0;
						type02 = 0;
					}
				}
				else if (strcmp(buff, "bye") == 0)
				{
					printf("Client 2 [%s:%d] signout\n", inet_ntoa(client2.sin_addr), ntohs(client2.sin_port));
					strcpy(acc2, "");
					strcpy(firstAcc, acc1);
					firstLog = 1;
					sendto(server_sock, empty, sizeof(empty), 0, (struct sockaddr *)&client2, sizeof(struct sockaddr)); /* send to the client2 */
					step02 = 0;
					type02 = 0;
				}
				else if (strcmp(buff, "exit") == 0)
				{
					printf("Client 2 [%s:%d] disconnected\n", inet_ntoa(client2.sin_addr), ntohs(client2.sin_port));
					step02 = 0;
				}
				else
					step02++;
				break;
			}
			default:
			{
				switch (type02)
				{
				case 0:
				{
					Signin(2);
					break;
				}
				case 1:
				{
					changePassword(2);
					break;
				}
				case 2:
				{
					sendMessage(2);
					break;
				}
				// case 3: {	showMessage(2); 	break;	}
				default:
					break;
				}
				break;
			}
			}
			printf("2 %d %d with %s\n", type02, step02, buff);
		}
	}

	close(server_sock);
}
