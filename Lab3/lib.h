#ifndef __LIB_H__
#define __LIB_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct Account
{
    char username[50];
    char password[50];
    int status;
    char homepage[50];
    int wrongCount;
    int wrongCode;
    struct Account *next;

} Account;
Account *createAcc(char *username, char *password, int status, char *homepage)
{
    Account *newAcc = (Account *)malloc(sizeof(Account));
    strcpy(newAcc->username, username);
    strcpy(newAcc->password, password);
    strcpy(newAcc->homepage, homepage);
    newAcc->status = status;
    newAcc->wrongCount = 0;
    newAcc->wrongCode = 0;
    newAcc->next = NULL;
}
Account *copyAcc(Account *acc)
{
    Account *newAcc = createAcc(acc->username, acc->password, acc->status, acc->homepage);
    newAcc->wrongCount = acc->wrongCount;
    newAcc->wrongCode = acc->wrongCode;
}
typedef struct
{
    Account *head;
    Account *tail;
    int size;
} List;

List *init()
{
    List *list = (List *)malloc(sizeof(List));
    list->size = 0;
    list->head = list->tail = NULL;
    return list;
}
void addList(List *list, Account *acc)
{
    if (list->size == 0)
    {
        Account *newAcc = copyAcc(acc);
        list->head = newAcc;
        list->tail = newAcc;
        list->size++;
    }
    else
    {
        Account *newAcc = copyAcc(acc);
        Account *tail = list->tail;
        tail->next = newAcc;
        list->tail = newAcc;
        list->size++;
    }
}
Account *findAccount(List *list, char *username)
{
    Account *index = list->head;
    while (index != NULL)
    {
        if (strcmp(index->username, username) == 0)
        {
            return index;
        }
        else
            index = index->next;
    }
}
int checkInput(char *input)
{
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == ' ')
        {
            return 0;
        }
    }
    return 1;
    // return strstr(input, " ") == NULL;
}
void readFile(List *list)
{
    FILE *file = fopen("nguoidung.txt", "r");
    if (!file)
    {
        printf("Error opening file!!! ");
        exit(-1);
    }
    char temp[127];
    char username[50];
    char password[50];
    char homepage[50];
    int status, uLength, pLength, hLength;
    while (fgets(temp, 127, file) != NULL)
    {
        temp[strlen(temp) - 1] = '\0';
        strcpy(username, "");
        strcpy(password, "");
        strcpy(homepage, "");

        uLength = 0;
        pLength = 0;
        hLength = 0;
        int index = 1;
        for (int i = 0; i < strlen(temp); i++)
        {
            if (temp[i] != ' ')
            {
                if (index == 1)
                {
                    username[uLength++] = temp[i];
                }
                else if (index == 2)
                {
                    password[pLength++] = temp[i];
                }
                else if (index == 3)
                {
                    status = temp[i] - '0';
                }

                else
                {
                    homepage[hLength++] = temp[i];
                }
            }
            else
                index++;
        }
        username[uLength] = '\0';
        password[pLength] = '\0';
        homepage[hLength] = '\0';
        Account *acc = createAcc(username, password, status, homepage);
        addList(list, acc);
    }
    fclose(file);
}
void updateFile(List *list)
{
    FILE *file = fopen("nguoidung.txt", "w");
    if (!file)
    {
        printf("Error opening file!!!");
        exit(-1);
    }
    Account *index = list->head;
    while (index != NULL)
    {
        fprintf(file, "%s %s %d %s\n", index->username, index->password, index->status, index->homepage);
        index = index->next;
    }
    fclose(file);
}
#endif