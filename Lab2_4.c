#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Missing file arguments\n");
        exit(1);
    }
    else
    {
        // opening the first
        // file and reading
        // FILE *input_file = fopen(argv[1], "r");
        int input_file = open(argv[1], O_RDONLY);
        if (input_file < 0)
        {
            fprintf(stderr, "Error in opening the input file\n");
            exit(1);
        }
        // pmode code
        int output_file = creat(argv[2], 0644);
        if (output_file <= -1)
        {
            fprintf(stderr, "Error in creating output file\n");
            exit(1);
        }

        // dup the first file to stdin and the second file to stdout, and saves a cpy of stdIN and Stdout
        close(0);
        // storing at 4
        int input_dup = dup(input_file);

        // closing stdout
        close(1);
        // dup(input_file);
        int output_dup = dup(output_file);

        if (input_dup == -1 || output_dup == -1)
        {
            fprintf(stderr, "Error duplicating file descriptor");
            exit(1);
        }
        // creating pipe
        int pp[2];
        int flow = pipe(pp);
        int check = 0;
        if (flow == -1)
        {
            fprintf(stderr, "faulty pipeline");
            exit(1);
        }

        if (fork() != 0)
        { /* parent continues here */
            if (fork() != 0)
            {
                int status;
                // Close both ends of the pipe
                close(pp[0]);
                close(pp[1]);

                // Wait for the first child to complete
                wait(&status);

                // Wait for the second child to complete
                wait(&status);

                // Check the exit status of the child processes
                if (WEXITSTATUS(status) != 0)
                {
                    printf("One of the child processes failed with exit status %d\n", WEXITSTATUS(status));
                }
                else
                {
                    printf("Both child processes completed successfully\n");
                }

                // Exit the program
                exit(0);
            }
            // close(pp[1]);
            else
            {
                // close(1);
                // int output_dup = dup(output_file);
                // closing file1 end of pipeline
                close(pp[0]);
                close(1);
                int flow3 = dup(pp[1]);
                close(pp[1]);
                // fprintf(stderr,"Input file desc \n\n\n");

                if (flow3 == -1)
                {
                    perror("Error in redirecting stdout to pipe");
                    exit(1);
                }

                execl("./Lab2_3", "Lab2_3", (char *)NULL);

                perror("Error executing convert");
                exit(1);
            }
        }
        else
        {
            //CHILD 1
            // close the writing path so it doesnt interfere
            // closed file 2

            close(pp[1]);
            close(0);
            // redirect stdin to the read end of the pipe
            int flow2 = dup(pp[0]);
            close(pp[0]);

            if (flow2 == -1)
            {
                perror("Error in redirecting stdin to pipe");
                exit(1);
            }

            execl("./Lab2_2", "Lab2_2", (char *)NULL);

            perror("Error executing count");
            exit(1);
            /* Child process closes up input side of pipe */
        }
    }
}