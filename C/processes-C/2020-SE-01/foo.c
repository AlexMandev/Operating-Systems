#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if(argc != 2) { err(1, "bad args"); }

    int fd = open("named_pipe", O_WRONLY);
    if(fd == -1) { err(2, "named_pipe"); }

    if(dup2(fd, 1) < 0) { err(37, "couldn't dup2"); }
    close(fd);

    execlp("cat", "cat", argv[1], (char*)NULL);
    err(100, "couldn't execlp");
}
