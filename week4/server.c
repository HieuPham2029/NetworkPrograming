
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
    int sockfd;
    user data;
    char *userExist = "Account does not exist. Please try another account!\n";
    char *insertPass = "Insert password\n";
    char *notOK = "Not OK\n";
    char *ok = "OK\n";
    char *isBlocked = "Account is blocked\n";
    char *notReady = "Account not ready\n";
    char *err = "Error\n";
    char msg[MAXLINE];
    // char client[MAXLINE];
    char str_out_digit[MAXLINE];
    char str_out_alpha[MAXLINE];

    struct sockaddr_in servaddr, cliaddr;
    if(argc != 2)
    {
        printf("\nSyntax Error!\n\n");
        return 1;
    }
    if(isValidPort(argv[1]) == 0)
    {
        printf("\nInvalid Port!\n\n");
        return 1;
    }

    // creating socket file desciptor
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("\nSocket creation failed\n\n");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("\nBind failed\n\n");
        exit(EXIT_FAILURE);
    }
    int len, n;
    len = sizeof(cliaddr);  //len is value/resuslt
    int login = 0;
    while(1)
    {
        while(1)
        {
            if(isLogin == 1)
            {
                break;
            }
            n = recvfrom(sockfd, (char *)msg, MAXLINE,
                        MSG_WAITALL, (struct sockaddr *) &cliaddr, 
                        &len);

            msg[n] = '\0';
            if(strcmp(msg, "exit") == 0)
            {
                printf("\nShuting down ... \n");
                printf("Goodbye\n\n");
                exit(1);
            }
            if(strcmp(msg, "bye") == 0)
            {
                isLogin = 0;
            }
            strcpy(data.username, msg);
            if(isExist(data.username) == 0)
            {
                sendto(sockfd, (const char *)userExist, strlen(userExist),
                    MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                    len);
                continue;
            }
            sendto(sockfd, (const char *)insertPass, strlen(insertPass),
                MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len);
            memset(msg, 0, MAXLINE);
            break;
        }

        while(1)
        {
            if(isLogin == 1)
            {
                break;
            }
            n = recvfrom(sockfd, (char *)msg, MAXLINE,
                        MSG_WAITALL, (struct sockaddr *) &cliaddr, 
                        &len);

            msg[n] = '\0';
            if(strcmp(msg, "exit") == 0)
            {
                printf("\nShuting down ... \n");
                printf("Goodbye\n\n");
                exit(1);
            }
            if(strcmp(msg, "bye") == 0)
            {
                isLogin = 0;
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
                    sendto(sockfd, (const char *)isBlocked, strlen(isBlocked),
                        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                        len);
                    login = 0;
                    break;
                }
                sendto(sockfd, (const char *)notOK, strlen(notOK),
                    MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                    len);
                continue;
            }
            if(checkUserActive(data.username) == 0)
            {
                isLogin = 0;
                sendto(sockfd, (const char *)notReady, strlen(notReady),
                    MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                    len);
                break;
            }
            sendto(sockfd, (const char *)ok, strlen(ok),
                MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len);
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
            n = recvfrom(sockfd, (char *)msg, MAXLINE,
                        MSG_WAITALL, (struct sockaddr *) &cliaddr, 
                        &len);

            msg[n] = '\0';
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
            sendto(sockfd, (char *)str_out_digit, strlen(str_out_digit),
                MSG_CONFIRM, (struct sockaddr *) &cliaddr, 
                len);
            
            sendto(sockfd, (char *)str_out_alpha, strlen(str_out_alpha),
                MSG_CONFIRM, (struct sockaddr *) &cliaddr, 
                len);

            memset(str_out_digit, 0, MAXLINE+1);
            memset(str_out_alpha, 0, MAXLINE+1);
            break;
        }
    }
    return 0;
}
