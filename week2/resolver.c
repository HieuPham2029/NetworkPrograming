#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void convertIP(char *param)
{
    struct hostent *host;
    struct in_addr addr;
    printf("IP address: %s\n", param);
    printf("\nConverting ... \n\n");
    addr.s_addr = inet_addr(param);
    if (addr.s_addr == INADDR_NONE) 
    {
        printf("Not found information\n");
        return;
    } 
    else host = gethostbyaddr((char *) &addr, 4, AF_INET);
    if (host == NULL) 
    {
        printf("Not found information\n");
        return;
    }
    printf("Offcial name: %s \n", host->h_name);
    char **pAlias;
    pAlias = host->h_aliases;
    if(*pAlias == NULL)
    {
        printf("Cannot find Alias name!!!\n");
        return;
    }
    printf("Alias name:\n");
    for (pAlias; *pAlias != NULL; pAlias++) 
    {
        printf("%s\n", *pAlias);
    }
}

void convertDomain(char *param)
{
    struct hostent *host;
    printf("Official name: %s\n", param);
    printf("\nConverting ... \n\n");
    host = gethostbyname(param);
    if (host == NULL) 
    {
        printf("Not found information\n");
        return;
    }
    printf("Offcial IP: %s \n", inet_ntoa(*((struct in_addr *)host->h_addr)));
    char **pAlias;
    pAlias = host->h_aliases;
    if(*pAlias == NULL)
    {
        printf("Cannot find Alias IP!!!\n");
        return;
    }
    printf("Alias IP:\n");
    for (pAlias; *pAlias != NULL; pAlias++) 
    {
        printf("%s\n", inet_ntoa(*((struct in_addr *)pAlias)));
    }
}


int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Syntax error!!!\n");
        exit(1);
    }
    switch (atoi(argv[1]))
    {
    case 1: 
        if(!atoi(argv[2]))
        {
            printf("Wrong parameter\n");
            return 0;
        }
        convertIP(argv[2]);
        break;
    case 2: 
        if(atoi(argv[2]))
        {
            printf("Wrong parameter\n");
            return 0;
        }
        convertDomain(argv[2]);
        break;
    default:
        printf("Syntax error!!!\n");
        exit(1);
        break;
    }
    return 0;
}
