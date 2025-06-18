#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if(argc != 3) { errx(1, "bad args"); } 	

    int pfd[2];

    if(pipe(pfd) < 0) { err(2, "couldn't pipe"); }

    int fd_out = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(fd_out == -1) { err(3, argv[2]); }

    pid_t pid = fork();
    if(pid < 0) { err(4, "couldn't fork"); }

    if(pid == 0) { 
        close(pfd[0]);

        if(dup2(pfd[1], 1) < 0) { err(5, "couldn't dup2"); }
        close(pfd[1]);

        execlp("cat", "cat", argv[1], (char*)NULL);
        err(6, "couldn't execlp");
    }

    close(pfd[1]);

    uint8_t b;
    ssize_t read_bytes;

    while((read_bytes = read(pfd[0], &b, 1)) > 0) {
        if(b == 0x55) continue;
        else if(b == 0x7D) {
            int read_result = read(pfd[0], &b, 1);
            if(read_result < 0) {
                err(7, "couldn't read");
            } else if(read_result == 0) {
                errx(100, "Invalid format of input bytes: escape character is not followed by anything.");
            }
            b ^= 0x20;
            if(write(fd_out, &b, 1) < 0) { err(8, "couldn't write"); }
        } else {
            if(write(fd_out, &b, 1) < 0) { err(8, "couldn't write"); } 
        }
    }
    if(read_bytes < 0) { err(7, "couldn't read"); }

    close(pfd[0]);
    close(fd_out);
    
    wait(NULL);
}
