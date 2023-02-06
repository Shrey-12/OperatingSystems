#include <stdio.h>

int main()
{
    int non_alphabetic_count = 0;
    char c;

    while ((c = getchar()) != EOF)
    {
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
        {
            non_alphabetic_count++;
        }
        putchar(c);
    }

    fprintf(stderr, "Number of non-alphabetic characters: %d\n", non_alphabetic_count);
    return 0;
}

/*getchar and putchar functions in C language operate on the standard input (stdin) and 
standard output (stdout) streams, respectively. They do not modify the content 
of the actual file. Instead, they are used to read characters from stdin and write 
characters to stdout one by one. The characters read by getchar remain in the 
input buffer until they are consumed by a subsequent read operation.*/