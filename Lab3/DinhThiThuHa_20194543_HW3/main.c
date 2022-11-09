#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lib.h"
void byName(char *name)
{

    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    he = gethostbyname(name);
    if (he == NULL)
    { // do some error checking
        printf("Not found information\n");
        return;
    }

    printf("Name: %s\n", he->h_name);
    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_addr));
    addr_list = (struct in_addr **)he->h_addr_list;
    if (addr_list[0] == NULL)
        return;
    printf("Alias IP: \n");
    for (int i = 0; addr_list[i] != NULL; i++)
    {
        printf("%s\n", inet_ntoa(*addr_list[i]));
    }
}

void byAddress(char *address)
{

    struct hostent *he;
    struct in_addr **name_list;
    struct in_addr addr;
    inet_aton(address, &addr);
    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL)
    { // do some error checking
        printf("Not found infomation\n");
        return;
    }
    printf("IP Address: %s\n", inet_ntoa(*(struct in_addr *)he->h_addr));
    printf("Official name: %s\n", he->h_name);
    name_list = (struct in_addr **)he->h_aliases;
    if (name_list[0] == NULL)
        return;
    printf("Alias name: \n");
    for (int i = 0; name_list[i] != NULL; i++)
    {
        printf("%s\n", inet_ntoa(*name_list[i]));
    }
}

void menu()
{
    printf("USER MANAGEMENT PROGRAM\n");
    printf("------------------------------------\n");
    printf("1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("7. Homepage with domain name\n");
    printf("8. Homepage with IP address\n");
    printf("Your choice (1-6, other to quit):\n");
}
int main()
{
    char n[255];
    List *list = init();
    readFile(list);
    char curAcc[50];
    int signin = 0;

    while (1)
    {
        menu();
        fgets(n, 255, stdin);
        n[strlen(n) - 1] = '\0';
        if (strlen(n) != 1)
        {
            exit(0);
        }
        char username[50];
        char password[50];
        char activation_code[10];
        char newPassword[50];
        char homepage[50];
        switch (n[0])
        {
        case '1':
        {
            printf("Username: ");
            fgets(username, 50, stdin);
            username[strlen(username) - 1] = 0;
            if (checkInput(username) == 0)
            {
                printf("This username is invalid\n\n");
                break;
            }
            Account *acc = findAccount(list, username);
            if (acc != NULL)
            {
                printf("Account existed\n\n");
            }
            else
            {
                printf("Password: ");
                fgets(password, 50, stdin);
                password[strlen(password) - 1] = 0;
                if (checkInput(password) == 0)
                {
                    printf("This password is invalid\n\n");
                    break;
                }
                printf("Homepage: ");
                fgets(homepage, 50, stdin);
                homepage[strlen(homepage) - 1] = 0;
                if (checkInput(homepage) == 0)
                {
                    printf("This homepage is invalid\n\n");
                    break;
                }
                acc = createAcc(username, password, 2, homepage);
                addList(list, acc);
                updateFile(list);
                printf("Successful registration. Activation required.\n\n");
            }
            break;
        }
        case '2':
        {
            printf("Username: ");
            fgets(username, 50, stdin);
            username[strlen(username) - 1] = 0;
            if (checkInput(username) == 0)
            {
                printf("This username is invalid\n\n");
                break;
            }
            Account *acc = findAccount(list, username);
            if (acc == NULL)
            {
                printf("You are not register\n\n");
                break;
            }
            if (acc->status == 1)
            {
                printf("Account %s was actived\n\n", username);
                break;
            }
            else if (acc->status == 0)
            {
                printf("Account %s was blocked\n\n", username);
                break;
            }
            printf("Password: ");
            fgets(password, 50, stdin);
            password[strlen(password) - 1] = 0;
            if (checkInput(password) == 0)
            {
                printf("This password is invalid\n\n");
                break;
            }
            if (strcmp(acc->password, password) != 0)
            {
                acc->wrongCount++;
                if (acc->wrongCount < 4)
                {
                    printf("Password is incorrect\n\n");
                }
                else
                {
                    printf("Password is incorrect. Account is blocked\n\n");
                    acc->status = 0;
                    updateFile(list);
                }
                break;
            }
            printf("Activation code: ");
            fgets(activation_code, 10, stdin);
            activation_code[strlen(activation_code) - 1] = 0;
            if (checkInput(activation_code) == 0)
            {
                printf("This activation code is invalid\n\n");
                break;
            }
            if (strcmp(activation_code, "20194543") != 0)
            {
                acc->wrongCode++;
                if (acc->wrongCode < 5)
                {
                    printf("Account is not activated\n\n");
                }
                else
                {
                    printf("Activation code is incorrect. Account is blocked\n\n");
                    acc->status = 0;
                    updateFile(list);
                }
                break;
            }
            printf("Account is activated\n\n");
            acc->status = 1;
            updateFile(list);
            break;
        }

        case '3':
        {
            if (signin == 1)
            {
                printf("Already sign in with account %s \n\n", curAcc);
                break;
            }
            printf("Username: ");
            fgets(username, 50, stdin);
            username[strlen(username) - 1] = '\0';
            if (checkInput(username) == 0)
            {
                printf("This username is invalid\n\n");
                break;
            }
            Account *acc = findAccount(list, username);
            if (acc == NULL)
            {
                printf("Cannot find account\n\n");
            }
            else if (acc->status == 1)
            {
                if (strcmp(username, curAcc) != 0)
                {
                    Account *nowAcc = findAccount(list, curAcc);
                    if (nowAcc != NULL)
                        nowAcc->wrongCount = 0;
                }
                printf("Password: ");
                fgets(password, 50, stdin);
                password[strlen(password) - 1] = '\0';
                if (checkInput(password) == 0)
                {
                    printf("This password is invalid\n\n");
                    break;
                }
                if (strcmp(password, acc->password) != 0)
                {
                    strcpy(curAcc, username);
                    acc->wrongCount++;
                    if (acc->wrongCount < 4)
                    {
                        printf("Password is incorrect\n\n");
                        break;
                    }
                    else
                    {
                        printf("Password is incorrect. Account is blocked\n\n");
                        acc->status = 0;
                        updateFile(list);
                        break;
                    }
                }
                printf("Hello %s\n\n", username);
                strcpy(curAcc, username);
                acc->wrongCount = 0;
                signin = 1;
            }
            else if (acc->status == 0)
            {
                printf("Account %s was blocked\n\n", username);
                break;
            }
            else
            {
                printf("Account %s is not active\n\n", username);
                break;
            }

            break;
        }
        case '4':
        {
            if (signin == 0)
            {
                printf("Please sign in to use this funtion!\n\n");
                break;
            }
            printf("Username: ");
            fgets(username, 50, stdin);
            username[strlen(username) - 1] = 0;
            if (checkInput(username) == 0)
            {
                printf("This username is invalid\n\n");
                break;
            }
            Account *acc = findAccount(list, username);
            if (acc == NULL)
            {
                printf("Cannot file account\n\n");
            }
            else
            {
                if (acc->status == 1)
                {
                    printf("Account %s is active\n\n", username);
                }
                else if (acc->status == 0)
                {
                    printf("Account %s is blocked\n\n", username);
                }
                else
                {
                    printf("Account %s is idle\n\n", username);
                }
            }
            break;
        }

        case '5':
        {
            if (signin == 0)
            {
                printf("Please sign in to use this funtion!\n\n");
                break;
            }
            printf("Username: ");
            fgets(username, 50, stdin);
            username[strlen(username) - 1] = 0;
            if (checkInput(username) == 0)
            {
                printf("This username is invalid\n\n");
                break;
            }
            Account *acc = findAccount(list, username);
            if (acc == NULL)
            {
                printf("Cannot file account\n\n");
                break;
            }
            if (strcmp(username, curAcc) != 0)
            {
                printf("Account is not sign in\n\n");
                break;
            }

            printf("Please input current password\n");
            printf("Password: ");
            fgets(password, 50, stdin);
            password[strlen(password) - 1] = 0;
            if (checkInput(password) == 0)
            {
                printf("This password is invalid\n\n");
                break;
            }
            if (strcmp(acc->password, password) != 0)
            {
                acc->wrongCount++;
                if (acc->wrongCount < 4)
                {
                    printf("Current password is incorrect. Please try again\n\n");
                }
                else
                {
                    printf("Current password is incorrect. Account is blocked\n\n");
                    acc->status = 0;
                    updateFile(list);
                }
                break;
            }
            printf("New password: ");
            fgets(newPassword, 50, stdin);
            newPassword[strlen(newPassword) - 1] = 0;
            if (checkInput(newPassword) == 0)
            {
                printf("New password is invalid\n\n");
                break;
            }
            if (strcmp(acc->password, newPassword) == 0)
            {
                printf("Password is not changed\n\n");
                break;
            }
            printf("Password is changed\n\n");
            strcpy(acc->password, newPassword);
            updateFile(list);

            break;
        }
        case '6':
        {
            if (signin == 0)
            {
                printf("Please sign in to use this funtion!\n\n");
                break;
            }
            printf("Username: ");
            fgets(username, 50, stdin);
            username[strlen(username) - 1] = 0;
            if (checkInput(username) == 0)
            {
                printf("This username is invalid\n\n");
                break;
            }
            Account *acc = findAccount(list, username);
            if (acc == NULL)
            {
                printf("Cannot file account\n\n");
            }
            else
            {
                if (strcmp(username, curAcc) != 0)
                {
                    printf("Account is not sign in\n\n");
                }
                else
                {
                    printf("Goodbye %s\n\n", username);
                    signin = 0;
                    strcpy(curAcc, "");
                }
            }

            break;
        }
        case '7':
        {
            if (signin == 0)
            {
                printf("Please sign in to use this funtion!\n\n");
                break;
            }
            Account *acc = findAccount(list, curAcc);

            if (tolower(acc->homepage[0]) >= 'a' && tolower(acc->homepage[0]) <= 'z')
            {
                byName(acc->homepage);
            }
            else
            {
                printf("Can't use this function!!!\n");
            }
            break;
        }
        case '8':
        {
            if (signin == 0)
            {
                printf("Please sign in to use this funtion!\n\n");
                break;
            }
            Account *acc = findAccount(list, curAcc);

            if (acc->homepage[0] >= '0' && acc->homepage[0] <= '9')
            {
                byAddress(acc->homepage);
            }
            else
            {
                printf("Can't use this function!!!\n");
            }
            break;
        }
        default:
            printf("Thanks for using our program <3\n\n");
            exit(0);
        }
    }
}