#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// typedef struct ELEMENT ELEMENT;

typedef struct ELEMENT  // account information
{
    char username[30];  
    char password[30];
    int status;         // 0:blocked; 1:active
    int wrongCount;     // count the number of incorrect password attempts
    struct ELEMENT* next;
}ELEMENT;

ELEMENT* createElement(char* username, char* password, int status) {    // create new account element
    ELEMENT* newEle = (ELEMENT*)malloc(sizeof(ELEMENT));    // new account element
    strcpy(newEle->username, username);     // save username
    strcpy(newEle->password, password);     // save password
    newEle->status = status;                // save status
    newEle->wrongCount = 0;
    newEle->next = NULL;
}

ELEMENT* copyElement(ELEMENT* ele) {        // create a copy of account record
    ELEMENT* newEle = createElement(ele->username, ele->password, ele->status);
    newEle->wrongCount = ele->wrongCount;
}

typedef struct {    // account list
    ELEMENT* first;     // first of the list
    ELEMENT* last;      // last of the list
    int size;       // number of record
}LIST;

LIST* ini() {       // initialize account list
    LIST* list = (LIST*)malloc(sizeof(LIST));   // create new list
    list->size = 0;
    list->first = list->last  = NULL;   // there is no first and last, yet
    return list;
}

void add(LIST* list, ELEMENT* ele) {    // add new account to list
    if(list->size == 0) {   // if there isn't any account yet
        ELEMENT* newEle = copyElement(ele);             // create new account
        list->first = newEle;   // new account is the first
        list->last = newEle;    // and the last of the list
    }
    else {  // if there are some account
        ELEMENT* newEle = copyElement(ele);     // create new account
        list->last->next = newEle;    // new account to last of the list
        list->last = newEle;    
    }
    list->size++;
}

ELEMENT* findAccount(LIST* list, char* username) {  // find account by username
    ELEMENT* index = list->first;
    while (index != NULL)       // go through list of account
    {
        if(strcmp(index->username, username) == 0)  // compare username
            return index;   // return finding account
        else index = index->next;
    }
    return NULL;
}

void getAccString(char* string, char* username, char* password, int* status) {

    int uL, pL;
    uL = pL = 0;
    strcpy(username, "");
    strcpy(password, "");
    int index = 1;  // mark reading string (1:username, 2:password; 3:status)
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] != ' ') {     // username, password and status are separate by 1 space '_'
            if(index == 1) username[uL++] = string[i];     // read username
            else if(index == 2) password[pL++] = string[i];    // read password
            else {      // read status
                *status = string[i] - '0';  // convert a char to number
                break;
            }
        }
        else index++;   // done reading 1 element of account
    }
    username[uL] = '\0';    
    password[pL] = '\0';
}

void readInput(LIST* list) {    // get data from input file account.txt
    FILE* file = fopen("account.txt", "r"); // open file to read
    if(!file) {     // error when opening
        printf("Error opening file!");
        exit(-1);
    }
    char tmp[100];
    char username[30], password[30];
    int status, uL, pL;
    while (fgets(tmp, 100, file) != NULL) {     // read line by line
        getAccString(tmp, username, password, &status);
        ELEMENT* ele = createElement(username, password, status);   // create new account
        add(list, ele);     // add to list
    }
    fclose(file);   // close file
}

void updateFile(LIST* list) {
    FILE* file = fopen("account.txt", "w");     // open file to write
    if(!file) {     // error when opening
        printf("Error opening file!");
        exit(-1);
    }
    ELEMENT* index = list->first;
    while (index != NULL)   // write all account from list to file
    {
        fprintf(file, "%s %s %d\n", index->username, index->password, index->status);
        index = index->next;
    }
    fclose(file);   // close file
}

ELEMENT* updatePassword(LIST* list, char* username, char* password) {
    ELEMENT* this = findAccount(list, username);
    if(this != NULL) {
        strcpy(this->password, password);
        updateFile(list);
    }
    return this;
}

ELEMENT* updateAccState(LIST* list, char* username, int status) {
    ELEMENT* this = findAccount(list, username);
    if(this != NULL) {
        this->status = status;
        updateFile(list);
    }
    return this;
}

#endif