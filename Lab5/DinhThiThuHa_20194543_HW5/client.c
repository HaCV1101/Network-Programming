/*TCP Client*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "lib.h"

#define BUFF_SIZE 8192

char SERVER_ADDR[150];
int SERVER_PORT;
int client_sock;
char buff[BUFF_SIZE];
struct sockaddr_in server_addr; /* server's address information */
int msg_len, bytes_sent, bytes_received;

int Send(char* msg) {			/// send msg to server	
	bytes_sent = send(client_sock, msg, sizeof(msg), 0);
	if(bytes_sent <= 0) return 1;
	return 0;
}

int ReceiveData() {				/// receive msg from server to buff variable
	bytes_received = recv(client_sock, buff, BUFF_SIZE, 0); 
	if(bytes_received <= 0 || strcmp(buff, errorString) == 0 || strcmp(buff, sentError) == 0) return 1;
	else return 0;
}


void Case1() {
	char String[BUFF_SIZE];
	while(1) {
		printf("Nhap xau (bo trong de thoat chuc nang, enter de nhap): \n");
		fgets(String, BUFF_SIZE, stdin);
		String[strlen(String) - 1] = '\0';
		if(strlen(String) == 0) {		// check string length
			Send(sentDone);				// send "Done" signal
			break;
		}
		Send(String);		// send string
		ReceiveData();		// receive respone
		if(strcmp(buff, okString) != 0) {	// error occur
			printf("Xau co chua ky tu khong phai chu so hoac chu cai\n");
			continue;
		}

		Send(okString);		// send "ok" signal for server to start sending result	
		ReceiveData();		// receive result string 1
		if(strcmp(buff, emptyString) != 0) printf("%s\n", buff);

		Send(okString);		// send "ok" signal for server to start sending result	
		ReceiveData();	// receive result string 2
		if(strcmp(buff, emptyString) != 0) printf("%s\n", buff);
	}
	
}

void Case2() {
	char filename[BUFF_SIZE];
	printf("Nhap ten file (bo trong de thoat chuc nang, enter de nhap): ");
	fgets(filename, BUFF_SIZE, stdin);		// input file name
	filename[strlen(filename) - 1] = '\0';
	FILE* f = fopen(filename, "r");
	if(!f) {		// error when open file
		printf("Loi khi mo file\n");
		Send(errorString);	// send server "error" signal
		return;
	}
	Send(filename);			// send file name
	int check = 0;

	ReceiveData();			// receive respone
	if(strcmp(buff, okString) != 0) {		// "error" signal
		printf("Loi server\n");
		return;
	}

	while (fgets(buff, BUFF_SIZE, f) != NULL) {     // read line by line
		Send(buff);		// send line
		if(ReceiveData() == 1) {		// "error" signal received
			printf("Loi doc file\n");
			check = 1;
			break;
		}
    }
	if(check == 0) Send(sentDone);		// sent all file content

}

void printMenu() {
	printf("MENU\n");
	printf("------------------------------------\n");
	printf("1. Gui xau bat ky\n");
	printf("2. Gui noi dung file\n");
	printf("3. Thoat chuong trinh\n");
	printf("Chon: ");
}

int main(int argc, char* argv[]){

	if(argc != 3) {
		printf("Wrong argument\n");
		exit(0);
	}
	strcpy(SERVER_ADDR, argv[1]); 
	SERVER_PORT = charToNum(argv[2]);

	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately!\n");
		return 0;
	}


	char c[6];
		
	//Step 4: Communicate with server			
	while(1){
		printMenu();
		fgets(c, 6, stdin);	// choose option
		c[strlen(c) - 1] = '\0';
		if(strlen(c) != 1) continue;				// not an valid option
		if(c[0] < '1' || c[0] > '3') continue;		// not an valid option

		Send(c);		// send option to server
		if(c[0] == '3') {		// exit program
			printf("Exitting ...\n");
			break;
		}

		switch (c[0])
		{
			case '1':{
				Case1();
				break;
			}

			case '2': {
				Case2();
				break;
			}
				
			default:
				break;
		}
	}

	printf("#####################################\n");
	
	//Step 4: Close socket
	close(client_sock);
}
