#include<stdio.h> 
#include<ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include<errno.h> 
#include <signal.h>
#include <setjmp.h>
#include<stdlib.h>

jmp_buf env;
int k;
void handler() {
 longjmp(env, 1);
 exit(1);
 }

int main(int argc,char * argv[])
{
    int fd_in,fd_out;
    int pl[2];
    int status;
    pid_t pid;
    signal(SIGALRM, handler);
    if (setjmp(env) != 0) 
    {
        kill(fd_in,SIGTERM);
        kill(fd_out,SIGTERM);
        fprintf(stderr,"read timeout in second child, killing both children");
        while ((pid = wait(&status)) > 0) {
            fprintf(stderr, "child pid=%d reaped with exit status=%d\n",pid, WEXITSTATUS(status));
        }
        exit(1);
    }
    if(argc==2)
    {
      k=-atoi(argv[1]);
      if(k<=0)
      {
        fprintf(stderr,"Error in n");
      }
     if(fork()!=0)
     {
        if(fork()!=0)
        {
            wait(&status);
            wait(&status);

        }
        else
        {
          execl("convert", "convert", (char *) 0);
        }
     }
     else
     {
        execl("count", "count",argv[1], (char *) 0);
     }

    }
    else if(argc==3)
    {
      k=-atoi(argv[1]);
      if(k<=0)
      {
        fprintf(stderr,"Error in n");
      }
        pipe(pl);
                if((fd_in=open(argv[2],O_RDONLY))<0)
        {
            fprintf(stderr, "errno=%d\n", errno);
            perror("open error in main");
        }
     if(fork()!=0)
     {
        if(fork()!=0)
        {
            close(fd_in);
            wait(&status);
            wait(&status);

        }
        else
        {
          close(fd_in);
          dup2(pl[0],0);
          close(pl[0]);
          execl("convert", "convert", (char *) 0);
        }
     }
     else
     {
        close(0);
        dup2(fd_in,0);
        close(fd_in);
        close(1);
        dup2(pl[1],1);
        close(pl[1]);
        execl("count", "count",argv[1], (char *) 0);

     }

    }
    else
    {
      k=-atoi(argv[1]);
      if(k<=0)
      {
        fprintf(stderr,"Error in n");
      }
    if((fd_in=open(argv[2],O_RDONLY))<0)
    {
        fprintf(stderr, "errno=%d\n", errno);
        perror("open error in main");
    }
    if((fd_out = creat(argv[3], 0644))<0){
    
        fprintf(stderr, "errno=%d\n", errno);
        perror("open error in main");
    }
     pipe(pl);
     if(fork()!=0)
     {
        if(fork()!=0)
        {
            close(pl[0]);
            close(pl[1]);
            wait(&status);
            wait(&status);

        }
        else
        {
          close(fd_in);
          dup2(pl[0],0);
          close(pl[0]);
          close(1);
          dup2(fd_out,1);
          close(fd_out);
          execl("convert", "convert", (char *) 0);
          printf("Normally child second excuted");

        }
     }
     else
     {
        close(0);
        dup2(fd_in,0);
        close(fd_in);
        close(1);
        dup2(pl[1],1);
        close(pl[1]);
        execl("count", "count",argv[1], (char *) 0);
        printf("Normal child one excuted");

     }
    }
     alarm(15); 
     while ((pid = wait(&status)) > 0) {
            fprintf(stderr, "child pid=%d reaped with exit status=%d\n",pid, WEXITSTATUS(status));
        }
    alarm(0);
    printf("Normal child one excuted");
    exit(0);
    return 0;
}