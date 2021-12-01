#include "user.h"

node *head = NULL;


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

void changePass(char username[], char newpass[])
{
    node *tmp;
    tmp = head;
    while(tmp != NULL)
    {
        if(strcmp(username, tmp->data.username) == 0)
        {
            strcpy(tmp->data.password, newpass);
            writeFile();
        }
        tmp = tmp->next;
    }
}

int passEncryption(char *str_in, char *str_out_digit, char *str_out_alpha)
{
    int alpha_index = 0;
    int digit_index = 0;

    for(int i = 0; i < strlen(str_in); i++)
    {
        if(isdigit(str_in[i]))
            str_out_digit[digit_index++] = str_in[i];
        else if(isalpha(str_in[i]))
            str_out_alpha[alpha_index++] = str_in[i];
        else return 0;
    }

    str_out_alpha[alpha_index] = '\0';
    str_out_digit[digit_index] = '\0';
    return 1;
}
