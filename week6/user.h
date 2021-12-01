#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 1024

struct User
{
    char username[MAXLINE];
    char password[MAXLINE];
    char newpass[MAXLINE];
    int status;
};

typedef struct User user;

struct List
{
    user data;
    struct List *next;   
};

typedef struct List node;

void addUser(user data);
void readFile();
void writeFile();
int isExist(char username[]);
int checkUserPassword(char username[], char password[]);
int checkUserBlock(char username[]);
int checkUserActive(char username[]);
void userBlock(char username[]);
void userActive(char username[]);
void changePass(char username[], char newpass[]);
char *getUserPass(char username[]);
int passEncryption(char *str_in, char *str_out_digit, char *str_out_alpha);


