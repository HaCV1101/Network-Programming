/*TCP Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "lib.h"

// #define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 1   /* Number of allowed connections */
#define BUFF_SIZE 1024

char numString[BUFF_SIZE];
char charString[BUFF_SIZE];

int PORT;
 
int listen_sock, conn_sock; /* file descriptors */
char recv_data[BUFF_SIZE];
int bytes_sent, bytes_received;
struct sockaddr_in server; /* server's address information */
struct sockaddr_in client; /* client's address information */
int sin_size;

int Send(char* msg) {			/// send msg to server	
	bytes_sent = send(conn_sock, msg, sizeof(msg), 0);
	if(bytes_sent <= 0) return 1;
	return 0;
}

int ReceiveData() {				/// receive msg from server to recv_data variable
	bytes_received = recv(conn_sock, recv_data, BUFF_SIZE, 0);
	if(bytes_received <= 0 || strcmp(recv_data, errorString) == 0 || strcmp(recv_data, sentError) == 0) return 1;
	else return 0;
}

int main(int argc, char* argv[])
{
	if(argc != 2) {
		printf("Wrong argument\n");
		exit(0);
	}
	PORT = charToNum(argv[1]);

	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			//receives message from client
			ReceiveData();		// receive user's option choice
			char choice = recv_data[0];
			printf("Client choose %c %d!\n", choice, choice);
			if(choice == '3') {			// user disconnected
				printf("Client exit!\n");
				break;
			}

			if(choice == '1') {		// sending string choice
				while(1) {
					ReceiveData();		// receive string
					printf("%s\n", recv_data);
					if(strcmp(recv_data, sentDone) == 0) {		// if receive "done" signal, break
						printf("Done func1\n");
						break;
					}

					if(checkString(recv_data) == 0)  {			// check if valid string
						Send(sentError);		// invalid string => send error
						continue;
					}
					Send(okString);			// send "ok" signal
					splitString(recv_data, charString, numString);		// split string into numbers and letters
					printf("%s -- %s\n", numString, charString);

					ReceiveData();		// receive if ok to send
					//send numbers string
					if(strlen(numString) == 0) Send(emptyString);
					else	Send(numString);

					ReceiveData();		// receive if ok to send
					// send letters string
					if(strlen(charString) == 0) Send(emptyString);
					else 	Send(charString);

				}
				continue;
				
			}
			else {
				if(ReceiveData() == 1) {	/// check if there's any filename was sent
					Send(errorString);
					continue;
				}
				Send(okString);				/// send "ok" signal to start reading file 
				printf("Noi dung file %s: \n", recv_data);
				while (1)		// read line by line
				{
					if(ReceiveData() == 1) {		/// if error occurs
						Send(errorString);
						break;
					}
					if(strcmp(recv_data, sentDone) == 0) break;		/// read all file content
					Send(okString);			// send "ok" signal to continue reading file
					printf("%s\n", recv_data);		// print file content
				}
				printf("Done func2\n");
			}
			
		}//end conversation
		close(conn_sock);	
		break;
	}
	
	close(listen_sock);
	return 0;
}
