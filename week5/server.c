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

int isValidPort(char *port)
{
    int result = atoi(port);
    if(result > 0 && result < 65535)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    readFile();

    int login = 0;
    char *userExist = "Account does not exist. Please try another account!\n";
    char *insertPass = "Insert password\n";
    char *notOK = "Not OK\n";
    char *ok = "OK\n";
    char *isBlocked = "Account is blocked\n";
    char *notReady = "Account not ready\n";
    char *err = "Error\n";
    user data;
    char msg[MAXLINE];
    char str_out_digit[MAXLINE];
    char str_out_alpha[MAXLINE];

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cliaddr;
    if(argc != 2)
    {
        printf("\nSyntax Error!\n\n");
        exit(1);
    }
    if(isValidPort(argv[1]) == 0)
    {
        printf("\nInvalid Port!\n\n");
        exit(1);
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    // binding newly created socket to given IP and verification
    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("\nSocket bind failed ...\n\n");
        exit(1);
    }
    printf("Socket successfully binded ...\n");

    int n;
    // listen
    if(listen(sockfd, 5) != 0)
    {
        printf("\nListen failed ...\n\n");
        exit(1);
    }
    printf("Server listening ...\n");
    len = sizeof(cliaddr);

    // accept
    connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
    if(connfd < 0)
    {
        printf("\nServer accept failed ...\n\n");
        exit(1);
    }
    printf("Server accept the client ...\n");

    while(1)
    {
        while(1)
        {
            if(isLogin == 1)
            {
                break;
            }
            read(connfd, msg, sizeof(msg));

            if(strcmp(msg, "exit") == 0)
            {
                printf("\nShuting down ... \n");
                printf("Goodbye\n\n");
                exit(1);
            }
            strcpy(data.username, msg);
            if(isExist(data.username) == 0)
            {
                write(connfd, userExist, strlen(userExist));
                continue;
            }
            write(connfd, insertPass, strlen(insertPass));
            memset(msg, 0, MAXLINE);
            break;
        }

        while(1)
        {
            if(isLogin == 1)
            {
                break;
            }
            read(connfd, msg, sizeof(msg));

            if(strcmp(msg, "exit") == 0)
            {
                printf("\nShuting down ... \n");
                printf("Goodbye\n\n");
                exit(1);
            }

            strcpy(data.password, msg);
            isLogin = 1;
            if(checkUserPassword(data.username, data.password) == 0)
            {
                isLogin = 0;
                login++;
                if(login > 3)
                {
                    userBlock(data.username);
                    write(connfd, isBlocked, strlen(isBlocked));
                    login = 0;
                    break;
                }
                write(connfd, notOK, strlen(notOK));
                continue;
            }
            if(checkUserActive(data.username) == 0)
            {
                isLogin = 0;
                write(connfd, notReady, strlen(notReady));
                break;
            }
            write(connfd, ok, strlen(ok));
            memset(msg, 0, MAXLINE);
            break;
        }

        while(1)
        {
            if(isLogin == 0)
            {
                break;
            }
            if(checkUserActive(data.username) == 0)
            {
                break;
            }
            read(connfd, msg, sizeof(msg));

            if(strcmp(msg, "exit") == 0)
            {
                printf("\nShuting down ... \n");
                printf("Goodbye\n\n");
                exit(1);
            }

            if(strcmp(msg, "bye") == 0)
            {
                isLogin = 0;
                continue;
            }
            strcpy(data.newpass, msg);
            memset(msg, 0, MAXLINE);
           
            if(passEncryption(data.newpass, str_out_digit, str_out_alpha) == 0)
            {
                break;
            }
            changePass(data.username, data.newpass);
            write(connfd, str_out_digit, sizeof(str_out_digit));
            // printf("%s\n", str_out_digit);
            write(connfd, str_out_alpha, sizeof(str_out_alpha));
            // printf("%s\n", str_out_alpha);
            memset(str_out_digit, 0, MAXLINE+1);
            memset(str_out_alpha, 0, MAXLINE+1);
            break;
        }
    }
    return 0;
}