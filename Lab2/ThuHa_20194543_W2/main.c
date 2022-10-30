#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Kiểm tra xem sâu ký tự có phải địa chỉ IP không
 * Nếu là địa chỉ IP : trả về 1
 * Nếu không phải: trả về 0
 */
int is_IP_address(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if ((str[i] < '0' || '9' < str[i]) && str[i] != '.')
        {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Wrong number of parameter\n");
        exit(1);
    }
    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    if (argv[1][0] == '1')
    {
        if (is_IP_address(argv[2]))
        {
            // nếu là địa chỉ IP
            // biến đổi địa chỉ IP từ dạng "số và dấu chấm" thành dạng nhị phân
            inet_aton(argv[2], &addr);

            // tìm kiếm thông tin của địa chỉ IP
            he = gethostbyaddr(&addr, sizeof(addr), AF_INET);

            // nếu không tìm thấy
            if (he == NULL)
            {
                printf("Not found information\n");
                exit(1);
            }

            // ngược lại, in thông tin
            printf("Official name: %s\n", he->h_name);
            if (he->h_aliases[0] != NULL)
            {
                printf("Alias name:\n");
                for (int i = 0; he->h_aliases[i] != NULL; i++)
                {
                    printf("%s\n", he->h_aliases[i]);
                }
            }
        }
        else
        {
            printf("Wrong parameter\n");
            exit(1);
        }
    }
    else if (argv[1][0] == '2')
    {
        if (!is_IP_address(argv[2]))
        {
            // nếu là tên miền
            // tìm kiếm thông tin của tiên miền

            he = gethostbyname(argv[2]);

            // nếu không tìm thấy
            if (he == NULL)
            {
                printf("Not found information\n");
                exit(1);
            }

            // ngược lại, in thông tin
            addr_list = (struct in_addr **)he->h_addr_list;
            printf("Official IP: %s\n", inet_ntoa(*addr_list[0]));
            if (addr_list[1] != NULL)
            {
                printf("Alias IP:\n");
                for (int i = 1; addr_list[i] != NULL; i++)
                {
                    printf("%s\n", inet_ntoa(*addr_list[i]));
                }
            }
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
    return 0;
}