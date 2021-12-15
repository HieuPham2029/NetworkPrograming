#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "user.h"

void *connection_handler(void *);

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

    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in newaddr;

    socklen_t len = sizeof(newaddr);

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

    int no_threads = 0;
    pthread_t threads[3];
    while (no_threads < 3)
    {
        printf("Listening ...\n");
        // accept
        int client_socket = accept(sockfd, (struct sockaddr *)&newaddr, &len);
        if (pthread_create(&threads[no_threads], NULL, connection_handler, &client_socket) < 0)
        {
            printf("Cannot create thread for connection handler\n");
            exit(1);
        }
        if (client_socket < 0)
        {
            printf("\nServer accept failed ...\n\n");
            exit(1);
        }
        printf("Server accept the client from: %s:%d\n", inet_ntoa(newaddr.sin_addr), ntohs(newaddr.sin_port));
        no_threads++;
    }
    int k = 0;
    for (k = 0; k < 3; k++)
    {
        pthread_join(threads[k], NULL);
    }
    close(sockfd);
    return 0;
}

void *connection_handler(void *client_socket)
{
    int newSoket = *(int *)client_socket;
    int n;
    int login = 0;
    int isLogin = 0;
    int isSignOunt = 0;
    user data;
    char *userExist = "Account does not exist. Please try another account!\n";
    char *insertPass = "Insert password\n";
    char *notOK = "Not OK\n";
    char *ok = "OK\n";
    char *isBlocked = "Account is blocked\n";
    char *notReady = "Account not ready\n";
    char *err = "Error\n";
    char msg[MAXLINE];

    while (1)
    {
        n = recv(newSoket, msg, MAXLINE, 0);
        msg[n] = '\0';
        printf("%s\n", msg);
        if (strcmp(msg, "signout") == 0)
        {
            isLogin = 0;
            continue;
        }
        memset(msg, 0, MAXLINE);
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
            //printf("%s\n", msg);
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
    return 0;
}