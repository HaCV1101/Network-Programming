#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define shortS 20
#define longS 60
#define MAX_L 1024
char sentString[shortS] = "Sent";
char received[shortS] = "Received";
char empty[2];
char errorString[shortS] = "Error";
char loginSuccess[shortS] = "Login success!";
char logginFail[shortS] = "Login failed!";
char accBlocked[longS] = "Account is blocked!";
char changePassSucces[longS] = "Password change successfully!";
char invalidNewPass[longS] = "New password can only contain numbers and letters!";

long long charToNum(char* s) {        // convert string to int
    int p = 1, res = 0;
    for(int i = strlen(s) - 1; i >= 0; i--) {
        res += (s[i] - '0') * p;
        p *= 10;
    }
    return res;
}

void revStr(char* str) {
    int l = strlen(str);
    char tmp;
    for(int i = 0; i < l / 2; i++) {
        tmp = str[i];
        str[i] = str[l - i - 1];
        str[l - i - 1] = tmp;
    }
}

void numToString(int x, char* res) {
    // char res[10];
    int c = 0;
    while (x != 0)
    {
        res[c++] = (x % 10) + '0';
        x /= 10;
    }
    revStr(res);
}

void splitString(char* s, char* charString, char* numString) {        // split string into number string letter string
    memset(charString, '\0', sizeof(charString));
    memset(numString, '\0', sizeof(numString));
    int lchar = 0;
    int lnum = 0;

    for(int i = 0; i < strlen(s); i++) {
        if(s[i] >= '0' && s[i] <= '9') {        // is a number
            numString[lnum] = s[i];
            lnum++;
        }
        else {              // is a letter
            charString[lchar] = s[i];
            lchar++;
        }
    }
    numString[lnum] = '\0';
    charString[lchar] = '\0';
}

/////////////////     new data type to save message
typedef struct MESSAGE {
    char numS[MAX_L];
    char charS[MAX_L];
    struct MESSAGE* next;
} MESSAGE;

/////////////////       new data type - a queue to save all messages that 1 client sent
typedef struct M_QUEUE {
    MESSAGE* first;
    MESSAGE* last;
    int count;
} M_QUEUE;

MESSAGE* newMessage(char * msg) {       // create new MESSAGE
    MESSAGE* New = (MESSAGE*)malloc(sizeof(MESSAGE));
    char numS[MAX_L];
    char charS[MAX_L];
    splitString(msg, charS, numS);
    printf("%s -- %s\n", numS, charS);
    strcpy(New->numS, numS);
    strcpy(New->charS, charS);
    New->next = NULL;
    return New;
}

M_QUEUE* createNew() {                  // create new QUEUE
    M_QUEUE* New = (M_QUEUE*)malloc(sizeof(M_QUEUE));
    New->count = 0;
    New->first = New->last = NULL;
    return New;
}

//////////      put message to last of queue
void putQueue(M_QUEUE* mQ, char* msg) {
    printf("%d  ", mQ->count);
    MESSAGE* newMsg = newMessage(msg);
    if(mQ->count == 0) {
        mQ->first = newMsg;
        mQ->last = newMsg;
    }
    else {
        mQ->last->next = newMsg;
        mQ->last = newMsg;
    }
    mQ->count++;
    printf("%d\n", mQ->count);
}

//////////      get the first message in queue
MESSAGE* popQueue(M_QUEUE* mQ) {
    MESSAGE* this = mQ->first;
    MESSAGE* next = mQ->first->next;
    mQ->first = next;
    mQ->count--;
    return this;
}

#endif