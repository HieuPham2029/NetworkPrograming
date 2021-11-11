#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_USERNAME_LEN 30
#define MAX_PASSWORD_LEN 30
#define ACTIVE_CODE "20184103"

struct User
{
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int status;
};

typedef struct User user;

struct List
{
    user data;
    struct List *next;   
};

typedef struct List node;

node *head = NULL;
int isLogin = 0;
char currentUser[MAX_USERNAME_LEN];

/*----------------------------------------------------------------*/

void addUser(user data)
{
    node *newUser, *tmp;
    newUser = (node *)malloc(sizeof(node));
    if(head == NULL)
    {
        head = newUser;
        strcpy(head->data.username, data.username);
        strcpy(head->data.password, data.password);
        head->data.status = data.status;
        head->next = NULL;
    }
    else
    {
        tmp = head;
        while(tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = newUser;
        strcpy(newUser->data.username, data.username);
        strcpy(newUser->data.password, data.password);
        newUser->data.status = data.status;
        newUser->next = NULL;
    }
}

void readFile()
{
    user data;
    FILE *fptr;
    fptr = fopen("account.txt", "r");
    if(fptr == NULL)
    {
        printf("\nFailed to load data file\n\n");
        exit(1);
    }
    else
    {
        while(fscanf(fptr, "%s %s %d", data.username, data.password, &data.status) != EOF)
        {
            addUser(data);
        }
        fclose(fptr);
    }
}

void writeFile()
{
    FILE *fptr;
    fptr = fopen("account.txt", "w");
    if(fptr == NULL)
    {
        printf("\nFailed to write data file\n\n");
        exit(1);
    }
    else
    {
        node *tmp;
        tmp = head;
        while(tmp != NULL)
        {
            fprintf(fptr, "%s %s %d\n", tmp->data.username, tmp->data.password, tmp->data.status);
            tmp = tmp->next;
        }
    }
    fclose(fptr);
}

int isExist(char username[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            return 1;
        }  
        tmp = tmp->next;
    }
    return 0;
}

int checkUserPassword(char username[], char password[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            if(strcmp(password, tmp->data.password) == 0)
            {
                return 1;
            }
            return 0;
        }
        tmp = tmp->next;
    }
}

int checkUserBlock(char username[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            if(tmp->data.status == 0)
            { 
                return 1;
            }
            else return 0;
        }
        tmp = tmp->next;
    }
}

int checkUserActive(char username[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            if(tmp->data.status == 1)
            { 
                return 1;
            }
            else return 0;
        }
        tmp = tmp->next;
    }
}

void userBlock(char username[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            tmp->data.status = 0;
            writeFile();
            return;
        }
        tmp = tmp->next;
    }
}

void userActive(char username[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            tmp->data.status = 1;
            writeFile();
        }
        tmp = tmp->next;
    }
}

void menu()
{
    printf("\n----------------------------------------\n");
    printf("\n\tUSER MANAGEMENT PROGRAM\n");
    printf("\n----------------------------------------\n");
    printf("\n1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("Your choice (1-6, other to quit): ");
}

void Register()
{
    user data;
    if(isLogin != 0)
    {
        printf("\nPlease sign out first!\n");
        return;
    }
    do
    {
        do
        {
            printf("Username: ");
            scanf("%s", data.username);
            if(strlen(data.username) > MAX_USERNAME_LEN)
            {
                printf("Username too long. Please try again!\n\n");
            }
        }while(strlen(data.username) > MAX_USERNAME_LEN);
        if(isExist(data.username))
        {
            printf("Account existed!\n\n");
        }
    }while(isExist(data.username));
    do
    {
        printf("Password: ");
        scanf("%s", data.password);
        if(strlen(data.password) > MAX_PASSWORD_LEN)
        {
            printf("Password too long. Please try again!\n\n");
        }        
    } while (strlen(data.password) > MAX_PASSWORD_LEN);
    data.status = 2;
    addUser(data);
    printf("\nSuccessful registration. Activation required.\n");
    printf("Welcome %s!\n\n", data.username);
    writeFile();
}

void Activate()
{
    user data;
    int login = 0;
    int scan = 0;
    char code[30];
    printf("Username: ");
    scanf("%s", data.username);
    if(!isExist(data.username))
    {
        printf("\nPlease register your account first!\n\n");
    }
    else
    {
        if(checkUserBlock(data.username))
        { 
            printf("Account is blocked. Can't activate!\n");
            return;
        }
        if(checkUserActive(data.username))
        {
            printf("Account is already activated.\n");
            return;
        }
        do
        {
            printf("Password: ");
            scanf("%s", data.password);
            if(!checkUserPassword(data.username, data.password))
            {
                printf("\nPassword is incorrect!\n\n");
                login++;
            }
            if(login >= 3)
            {
                break;
            }
        } while (!checkUserPassword(data.username, data.password));
        if(login >= 3)
        {
            printf("\nEnter the wrong password too many times.\nAccount is blocked!\n\n");
            userBlock(data.username);
            return;
        }
        else
        {
            do
            {
                printf("Code: ");
                scanf("%s", code);
                if(strcmp(code, ACTIVE_CODE) != 0)
                {
                    printf("\nActivation code is incorrect!\n\n");
                    scan++;
                }
                if(scan >= 4)
                {
                    break;
                }
            } while (strcmp(code, ACTIVE_CODE) != 0);
            if(scan >= 4)
            {    
                printf("\nEnter the wrong activation code too many times.\nAccount is blocked!\n\n");
                userBlock(data.username);
                return;
            }
            printf("\nAccount is activated!\n\n");
            userActive(data.username);
        }
    }
}

void SignIn()
{
    user data;
    int login = 0;
    if(isLogin == 1)
    {
        printf("Please sign out your account first!\n\n");
        return;
    }
    printf("Username: ");
    scanf("%s", data.username);
    if(!isExist(data.username))
    {
        printf("\nPlease register your account first!\n\n");
    }
    else
    {
        if(checkUserBlock(data.username))
        {
            printf("Your account is blocked. Cannot sign in!!!\n\n");
            return;
        }
        do
        {
            printf("Password: ");
            scanf("%s", data.password);
            if(!checkUserPassword(data.username, data.password))
            {
                printf("\nPassword is incorrect!\n\n");
                login++;
            }
            if(login >= 3)
            {
                break;
            }
        } while (!checkUserPassword(data.username, data.password));
        if(login >= 3)
        {
            printf("\nEnter the wrong password too many times.\nAccount is blocked!\n\n");
            userBlock(data.username);
            return;
        }
        isLogin = 1;
        strcpy(currentUser, data.username);
        printf("\nSigning in ...\nHello %s!\n\n", data.username);
    }
}

void Search()
{
    user data;
    if(isLogin == 0)
    {
        printf("Please sign in to use this feature!\n");
        return;
    }
    printf("Search: ");
    scanf("%s", data.username);
    if(!isExist(data.username))
    {
        printf("\nCannot find account!\n");
        return;
    }
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(tmp->data.username, data.username) == 0)
        {
            switch(tmp->data.status)
            {
                case 0:
                    printf("\nAccount is blocked!\n");
                break;
                case 1:
                    printf("\nAccount is active!\n");
                break;
                case 2:
                    printf("\nAccount is idle. Activation required!\n");
                break;
            }
            return;
        }
        tmp = tmp->next;
    }
}

void ChangePassword()
{
    user data;
    int login = 0;
    char pass[MAX_PASSWORD_LEN];
    char confirmpass[MAX_PASSWORD_LEN];
    if(isLogin == 0)
    {
        printf("Please sign in to use this feature!\n");
        return;
    }
    printf("Change your password\n");
    do
    {
        printf("Username: ");
        scanf("%s", data.username);
        if(!isExist(data.username))
        {
            printf("Cannot find account\n");
        }
    }while(!isExist(data.username));
    do
    {
        printf("Password: ");  
        scanf("%s", data.password);
        if(!checkUserPassword(data.username, data.password))
        {
            printf("\nCurrent password is incorrect. Please try again\n\n");
            login++;
        }
        if(login >= 3)
        {
            break;
        }
    } while (!checkUserPassword(data.username, data.password));
    if(login >= 3)
    {
        printf("\nEnter the wrong password too many times.\nAccount is blocked!\n\n");
        userBlock(data.username);
        return;
    }
    do
    {
        printf("New password: ");
        scanf("%s", pass);
        if(strcmp(pass, data.password) == 0)
        {
            printf("The new password must be different from the old password\n");
        }
    }while(strcmp(pass, data.password) == 0);
    do
    {
        printf("Confirm password: ");
        scanf("%s", confirmpass);
        if(strcmp(pass, confirmpass) != 0)
        {
            printf("Confirm password is incorrect\n");
        }
    } while (strcmp(pass, confirmpass) != 0);
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(tmp->data.username, data.username) == 0)
        {
            strcpy(tmp->data.password, pass);
            writeFile();
        }
        tmp = tmp->next;
    }
    printf("Password is changed\n");
}

void SignOut()
{
    user data;
    if(isLogin == 0)
    {
        printf("You are not sign in.\n");
        return;
    }
    do
    {
        printf("Username: ");
        scanf("%s", data.username);
        if(strcmp(data.username, currentUser) != 0)
        {
            printf("You are not sign in.\n");
        }
    } while (strcmp(data.username, currentUser) != 0);
    isLogin = 0;
    strcpy(currentUser, "");
    printf("Signing out ...\n");
    printf("Goodbye %s!\n", data.username);
}

int main(int argc, char const *argv[])
{
    int choice;
    readFile();
    while(1)
    {
        menu();
        scanf("%d", &choice);
        printf("\n");
        switch(choice)
        {
            case 1: 
                Register();
                break;
            case 2: 
                Activate();
                break;
            case 3: 
                SignIn();
                break;
            case 4:
                Search();
                break;
            case 5:
                ChangePassword();
                break;
            case 6:
                SignOut();
                break;
            default:
                return 0;
        }
    }
    return 0;
}

