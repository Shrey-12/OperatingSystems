#include <stdio.h>



void addrToChar(int n, char returnArr[], char *address[])
{
    int index = 0;
    for (int i = n - 1; i >= 0; i--)
    {
        char *curr_address = address[i];
        char buffer_address[15];
        //converts hexadecimal to string
        sprintf(buffer_address, "%p", curr_address);
        //copied address
        for (int j = 6; j < 15; j++)
        {
            returnArr[index++] = buffer_address[j];
        }
    }
    // for (int i = 0; i < 9 * n - 1; i++)
    // {
    //     printf("%c", returnArr[i]);
    // }
}

void pr_params(int nargs, char **args)
{
    //address of Y
    char *start_address = *args;
    printf("\n\n\n\n");
    for (int i = nargs - 1; i >= 0; i--)
    {
        printf("s%d: ",i+1);
        int j = 0;
        while (*start_address != '\n')
        {
            printf("%c", *(start_address++));
        }
        printf("\n");
        start_address++;
    }
    // for (int i = 0; i < nargs; i++)
    // {
    //     char address[8];
    //     for (int j = 0; j < 8; j++)
    //     {
    //         address
    //     }
    // }
}


int main()
{
    int n;
    printf("Enter number of arguments");
    scanf("%d", &n);
    char params[500];
    //temporarily stores the addresses of H N and Y
    char *address[n];
    int major_index = 499;
    int address_index = n - 1;
    int temp_n = n;
    for (int i = 0; i < n; i++)
    {
        printf("%d\n", i);
        //initialize because uske bina error
        char s[100]={'\0'};
        scanf("%s", s);
        s[sizeof(s) / sizeof('n')] = '\n';
        //copy every string on params starting from end bit
        for (int i = sizeof(s) / sizeof('n'); i >= 0; i--)
        {
            params[major_index--] = s[i];
        }
        //store address of params Y N H
        address[temp_n--] = &params[major_index + 1];

    //     for (int i = 0; i < 500; i++)
    // {
    //     printf("%c", params[i]);
    // }
    }
    //adds address of params Y
    char *args = &params[major_index + 1];

    printf("\n\n");
    //breaking down addresses into char bits
    char addressArray[9 * n - 1]; // 26
    addrToChar(n, addressArray, address);
    for (int i = 9 * n - 2; i >= 0; i--) // 25 to 0
    {
        params[major_index--] = addressArray[i];
        printf("%c", params[major_index]);
    }



    // for (int i = 0; i < 500; i++)
    // {
    //     printf("%c", params[i]);
    // }
    pr_params(n, &args);
}
