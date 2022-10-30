#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void byName(char *name)
{

    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    he = gethostbyname(name);
    if (he == NULL)
    { // do some error checking
        printf("Not found information\n");
        exit(1);
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
        exit(1);
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

int main(int agrc, char **argv)
{
    if (agrc != 3)
    {
        printf("Wrong number of parameter!!!\n");
        exit(1);
    }
    int i;
    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    if (argv[1][0] == '1')
    {
        if (argv[2][0] >= '0' && argv[2][0] <= '9')
        {
            byAddress(argv[2]);
        }
        else
        {
            printf("Wrong parameter\n");
            exit(1);
        }
    }
    else if (argv[1][0] == '2')
    {
        if (tolower(argv[2][0]) >= 'a' && tolower(argv[2][0]) <= 'z')
        {
            byName(argv[2]);
        }
        else
        {
            printf("Wrong parameter\n");
            exit(1);
        }
    }
    else
    {
        printf("Wrong parameter\n");
    }
}