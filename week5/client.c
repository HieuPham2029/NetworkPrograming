#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "user.h"

int isLogin = 0;

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    return 0;
}

int isValidPort(char *port)
{
    int result = atoi(port);
    if(result > 0 && result < 65535)
        return 1;
    return 0;
}

int exitClient(char param[])
{
    if(param[0] == '\n' || param[0] == '\0')
    {
        printf("\nShuting down ... \n");
        printf("Goodbye\n\n");
        return 1;
    }
    return 0;
}

int signOut(char param[])
{
    char str[MAXLINE];
    strcpy(str, param);
    for(int i = 0; i < strlen(str); i++)
    {
        if(str[i] >= 97 && str[i] <= 122)
        {
            str[i] = str[i] - 32;
        }
    }
    if(strcmp(str, "BYE") == 0)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    readFile();

    struct sockaddr_in servaddr, cliaddr;
    user data;
    int sockfd;
    int login = 0;
    char msg[MAXLINE];
    char client[MAXLINE];
    char str_out_digit[MAXLINE];
    char str_out_alpha[MAXLINE];

    if(argc != 3)
    {
        printf("\nSyntax Error!\n\n");
        return 1;
    }
    if(isValidIpAddress(argv[1]) == 0)
    {
        printf("\nInvalid IP address!\n\n");
        return 1;
    }
    if(isValidPort(argv[2]) == 0)
    {
        printf("\nInvalid Port!\n\n");
        return 1;
    }

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("\nSocket creation failed ...\n\n");
        exit(1);
    }
    printf("Socket successfully created ...\n");
    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    // connect
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("\nConnection with the server failed ...\n\n");
        exit(1);
    }
    printf("Connected to the server ...\n");
    printf("\n\t\tSign in\n\n");
    int n, len;
    while(1)
    {
        do
        {
            if(isLogin == 1)
            {
                break;
            }
            printf("Username: ");
            gets(msg);
            strcpy(data.username, msg);
            if(exitClient(msg) == 1)
            {
                write(sockfd, "exit", strlen("exit"));
                exit(1);
            }

            write(sockfd, msg, sizeof(msg));
            memset(msg, 0, MAXLINE);
            
            read(sockfd, msg,sizeof(msg));
            printf("Server: %s\n", msg);
            memset(msg, 0, MAXLINE);
        } while (isExist(data.username) == 0); 

        do
        {
            if(isLogin == 1)
            {
                break;
            }
            printf("Password: ");
            gets(msg);
            strcpy(data.password, msg);
            if(exitClient(msg) == 1)
            {
                write(sockfd, "exit", strlen("exit"));
                exit(1);
            }
            isLogin = 1;
            write(sockfd, msg, sizeof(msg));
            memset(msg, 0, MAXLINE);

            read(sockfd, msg, sizeof(msg));

            printf("Server: %s\n", msg);
            if(checkUserActive(data.username) == 0)
            {
                isLogin = 0;
            }
            memset(msg, 0, MAXLINE);
            if(checkUserPassword(data.username, data.password) == 0)
            {
                login++;
                isLogin = 0;
                if(login > 3)
                {
                    userBlock(data.username);
                    login = 0;
                    break;
                }
            }
        } while (checkUserPassword(data.username, data.password) == 0); 

        do
        {
            if(isLogin == 0)
            {
                break;
            }
            if(checkUserActive(data.username) == 0)
            {
                break;
            }
            printf("New password: ");
            gets(msg);
            strcpy(data.newpass, msg);
            if(exitClient(msg) == 1)
            {
                write(sockfd, "exit", strlen("exit"));
                exit(1);
            }

            if(signOut(msg) == 1)
            {
                write(sockfd, "bye", strlen("bye"));
                printf("\nSigning out ... \n");
                printf("Goodbye %s\n\n", data.username);
                isLogin = 0;
                continue;
            }

            write(sockfd, msg, sizeof(msg));

            memset(msg, 0, MAXLINE);

            read(sockfd, msg, sizeof(msg));
            if(strcmp(msg, "Error") == 0)
            {
                printf("Sever: %s\n\n", msg);
                continue;
            }
           
            printf("Server: \n");

            printf("%s\n", msg);
            memset(msg, 0, MAXLINE);

            read(sockfd, msg, sizeof(msg));
        
            printf("%s\n\n", msg);
            memset(msg, 0, MAXLINE);
            changePass(data.username, data.newpass);
            break;
        } while (1);    
    }
    return 0;
}