#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if(argc != 2) { err(2, "bad args"); }

    int fd = open("named_pipe", O_RDONLY);
    if(fd == -1) { err(123, "named_pipe"); } 

    if(dup2(fd, 0) < 0) { err(3, "couldn't dup2"); }
    close(fd);

    execl(argv[1], argv[1], (char*)NULL);
    err(123456, "couldn't execl");
}
