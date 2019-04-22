#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;
    char * plcfifo = "/tmp/plcfifo";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

    /* write "START" to the FIFO */
    fd = open(plcfifo, O_WRONLY);
    write(fd, "START", sizeof("START"));
    close(fd);

    /* remove the FIFO */
    unlink(plcfifo);

    return 0;
}
