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
    if (result > 0 && result < 65535)
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

    int sockfd;
    struct sockaddr_in servaddr;

    int newSoket;
    struct sockaddr_in newaddr;

    socklen_t len = sizeof(newaddr);

    char msg[MAXLINE];
    pid_t pid;

    if (argc != 2)
    {
        printf("\nSyntax Error!\n\n");
        exit(1);
    }

    if (isValidPort(argv[1]) == 0)
    {
        printf("\nInvalid Port!\n\n");
        exit(1);
    }

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
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
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("\nSocket bind failed ...\n\n");
        exit(1);
    }
    printf("Socket successfully binded ...\n");

    // listen
    if (listen(sockfd, 10) != 0)
    {
        printf("\nListen failed ...\n\n");
        exit(1);
    }
    printf("Server listening ...\n");

    while (1)
    {
        // accept
        newSoket = accept(sockfd, (struct sockaddr *)&newaddr, &len);
        if (newSoket < 0)
        {
            printf("\nServer accept failed ...\n\n");
            exit(1);
        }
        printf("Server accept the client from: %s:%d\n", inet_ntoa(newaddr.sin_addr), ntohs(newaddr.sin_port));

        // create new child pid
        if ((pid = fork()) == 0)
        {
            close(sockfd);
            while (1)
            {
                while (1)
                {
                    if (isLogin == 1)
                    {
                        break;
                    }
                    recv(newSoket, msg, MAXLINE, 0);

                    if (strcmp(msg, "exit") == 0)
                    {
                        printf("\nShuting down ... \n");
                        printf("Goodbye\n\n");
                        exit(1);
                    }
                    strcpy(data.username, msg);
                    if (isExist(data.username) == 0)
                    {
                        send(newSoket, userExist, strlen(userExist), 0);
                        continue;
                    }
                    send(newSoket, insertPass, strlen(insertPass), 0);
                    memset(msg, 0, MAXLINE);
                    break;
                }

                while (1)
                {
                    if (isLogin == 1)
                    {
                        break;
                    }
                    recv(newSoket, msg, MAXLINE, 0);

                    if (strcmp(msg, "exit") == 0)
                    {
                        printf("\nShuting down ... \n");
                        printf("Goodbye\n\n");
                        exit(1);
                    }

                    strcpy(data.password, msg);
                    isLogin = 1;
                    if (checkUserPassword(data.username, data.password) == 0)
                    {
                        isLogin = 0;
                        login++;
                        if (login > 3)
                        {
                            userBlock(data.username);
                            send(newSoket, isBlocked, strlen(isBlocked), 0);
                            login = 0;
                            break;
                        }
                        send(newSoket, notOK, strlen(notOK), 0);
                        continue;
                    }
                    if (checkUserActive(data.username) == 0)
                    {
                        isLogin = 0;
                        send(newSoket, notReady, strlen(notReady), 0);
                        break;
                    }
                    send(newSoket, ok, strlen(ok), 0);
                    memset(msg, 0, MAXLINE);
                    break;
                }
            }
            close(newSoket);
            printf("disconnected with %s:%d\n", inet_ntoa(newaddr.sin_addr), ntohs(newaddr.sin_port));
            exit(0);
        }
    }

    return 0;
}
