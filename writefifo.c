#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
int fd;
char plcfifo[32];
char	readlin[32];
    sprintf(plcfifo,"/tmp/plcfifo");

    printf("argc = %d\n",argc);
    printf("argv = %s\n",argv[argc-1]);

    if ( argc != 2)
    {
	    printf("argc = %d\n",argc);
	    exit(1);
    }

    bzero(readlin,32);
    strcat(readlin,argv[argc-1]);


    fd = open(plcfifo, O_WRONLY);
    write(fd, readlin, strlen(readlin));
    close(fd);

    exit(0);
}
