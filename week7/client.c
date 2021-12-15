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
void signIn();

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
    if (result > 0 && result < 65535)
        return 1;
    return 0;
}

int exitClient(char param[])
{
    if (param[0] == '\n' || param[0] == '\0')
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
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 97 && str[i] <= 122)
        {
            str[i] = str[i] - 32;
        }
    }
    if (strcmp(str, "BYE") == 0)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    readFile();
    int login = 0;
    user data;
    char msg[MAXLINE];

    int sockfd;
    struct sockaddr_in servaddr;

    int choice;

    if (argc != 3)
    {
        printf("\nSyntax Error!\n\n");
        return 1;
    }

    if (isValidIpAddress(argv[1]) == 0)
    {
        printf("\nInvalid IP address!\n\n");
        return 1;
    }

    if (isValidPort(argv[2]) == 0)
    {
        printf("\nInvalid Port!\n\n");
        return 1;
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
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    // connect
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("\nConnection with the server failed ...\n\n");
        exit(1);
    }
    printf("Connected to the server ...\n");
    do
    {
        printf("\n----------------------------------------\n");
        printf("\n\tUSER MANAGEMENT PROGRAM\n");
        printf("\n----------------------------------------\n");
        printf("\n1. Sign in\n");
        printf("2. Sign out\n");
        printf("Your choice (1-2, other to quit): ");
        scanf("%d%*c", &choice);

        switch (choice)
        {
        case 1:
            if (isLogin == 0)
            {
                send(sockfd, "signin", strlen("signin"), 0);
                do
                {
                    if (isLogin == 1)
                    {
                        break;
                    }
                    printf("Username: ");
                    gets(msg);
                    strcpy(data.username, msg);
                    if (exitClient(msg) == 1)
                    {
                        send(sockfd, "exit", strlen("exit"), 0);
                        exit(1);
                    }

                    send(sockfd, msg, MAXLINE, 0);
                    memset(msg, 0, MAXLINE);

                    recv(sockfd, msg, MAXLINE, 0);
                    printf("Server: %s\n", msg);
                    memset(msg, 0, MAXLINE);
                } while (isExist(data.username) == 0);

                do
                {

                    if (isLogin == 1)
                    {
                        break;
                    }
                    printf("Password: ");
                    gets(msg);
                    strcpy(data.password, msg);
                    if (exitClient(msg) == 1)
                    {
                        send(sockfd, "exit", strlen("exit"), 0);
                        exit(1);
                    }
                    isLogin = 1;
                    send(sockfd, msg, MAXLINE, 0);
                    memset(msg, 0, MAXLINE);

                    recv(sockfd, msg, MAXLINE, 0);

                    printf("Server: %s\n", msg);
                    if (checkUserActive(data.username) == 0)
                    {
                        isLogin = 0;
                    }
                    memset(msg, 0, MAXLINE);
                    if (checkUserPassword(data.username, data.password) == 0)
                    {
                        login++;
                        isLogin = 0;
                        if (login > 3)
                        {
                            userBlock(data.username);
                            login = 0;
                            break;
                        }
                    }
                } while (checkUserPassword(data.username, data.password) == 0);
            }
            else
                printf("Please sign out your account first!\n\n");
            break;
        case 2:
            if (isLogin == 1)
            {
                printf("Signing out...\n");
                send(sockfd, "signout", strlen("signout"), 0);
                isLogin = 0;
            }
            else
                printf("You are not sign in.\n");
            break;
        default:
            exit(1);
        }
    } while (choice == 1 || choice == 2);

    close(sockfd);
    return 0;
}
