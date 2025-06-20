#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if(argc != 4) { errx(1, "bad args"); }	

    char* endptr;
    long N = strtol(argv[2], &endptr, 10);

    if(*endptr != '\0') {
        errx(2, "second argument should be a valid number [0, 255]");
    }
    
    if(N < 0 || N > 255) {
        errx(2, "second argument should be a valid number [0, 255]");
    }
    
    int dev_urand = open("/dev/urandom", O_RDONLY);
    if(dev_urand == -1) { err(3, "/dev/urandom"); }

    int dev_null = open("/dev/null", O_WRONLY);
    if(dev_null == -1) { err(3, "/dev/null"); }

    int out_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(out_fd == -1) { err(4, argv[3]); }

    char buffer[UINT16_MAX];
    int status;
    uint16_t r;
    for(long i = 0; i < N; i++) {
        if(read(dev_urand, &r, sizeof(r)) < 0) { err(4, "couldn't read"); }
        int pfd[2];

        if(pipe(pfd) < 0) { err(4, "couldn't pipe"); }

        pid_t pid = fork();
        if(pid < 0) { err(5, "couldn't fork"); }

        if(pid == 0) {
            close(dev_urand);
            close(pfd[1]);

            dup2(dev_null, 1);
            dup2(dev_null, 2);
            close(dev_null);


            dup2(pfd[0], 0);
            close(pfd[0]);

            execl(argv[1], argv[1], (char*)NULL);
            err(6, "couldn't execl");
        }
        
        close(pfd[0]);

        if(read(dev_urand, buffer, r) != r) { err(4, "couldn't read"); }
        
        if(write(pfd[1], buffer, r) != r) { err(7, "couldn't write to pipe"); }

        close(pfd[1]);
        wait(&status);
        
        if(WIFSIGNALED(status)) {
            if(write(out_fd, buffer, r) < 0) { err(8, "couldn't write"); }
            
            close(dev_urand);
            close(dev_null);
            close(out_fd);
            return 42;
        }
    }

    close(dev_urand);
    close(dev_null);
    close(out_fd);
    return 0;
}
