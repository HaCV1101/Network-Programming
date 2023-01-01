#ifndef __LIB_H__
#define __LIB_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// struct Account Account;
typedef struct Account
{
    char username[50];
    char password[50];
    int status;
    int wrongCount;
    int wrongCode;
    struct Account *next;
} Account;
Account *createAcc(char *username, char *password, int status)
{
    Account *newAcc = (Account *)malloc(sizeof(Account));
    strcpy(newAcc->username, username);
    strcpy(newAcc->password, password);
    newAcc->status = status;
    newAcc->wrongCount = 0;
    newAcc->wrongCode = 0;
    newAcc->next = NULL;
}
Account *copyAcc(Account *acc)
{
    Account *newAcc = createAcc(acc->username, acc->password, acc->status);
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
    FILE *file = fopen("account.txt", "r");
    if (!file)
    {
        printf("Error opening file!!! ");
        exit(-1);
    }
    char temp[127];
    char username[50];
    char password[50];
    int status, uLength, pLength;
    while (fgets(temp, 127, file) != NULL)
    {
        strcpy(username, "");
        strcpy(password, "");
        uLength = 0;
        pLength = 0;
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
                else
                {
                    status = temp[i] - '0';
                    break;
                }
            }
            else
                index++;
        }
        username[uLength] = '\0';
        password[pLength] = '\0';
        Account *acc = createAcc(username, password, status);
        addList(list, acc);
    }
    fclose(file);
}
void updateFile(List *list)
{
    FILE *file = fopen("account.txt", "w");
    if (!file)
    {
        printf("Error opening file!!!");
        exit(-1);
    }
    Account *index = list->head;
    while (index != NULL)
    {
        fprintf(file, "%s %s %d\n", index->username, index->password, index->status);
        index = index->next;
    }
    fclose(file);
}
#endif