#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

void check_arg(const char* arg) {
    if(strlen(arg) != 1 || (arg[0] < '1' || arg[0] > '9')) {
        err(1, "bad args");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3) { errx(1, "bad args"); }
    
    check_arg(argv[1]); 
    check_arg(argv[2]);

    int n = argv[1][0] - '0';
    int d = argv[2][0] - '0';

    int pipes[2][2];

    if(pipe(pipes[0]) < 0 || pipe(pipes[1]) < 0) { err(2, "couldn't pipe"); }

    pid_t pid = fork();
    if(pid < 0) { err(3, "couldn't fork"); }

   
    if(pid == 0) {
        close(pipes[0][1]);
        close(pipes[1][0]);
        char signal = 0;
        for(int i = 0; i < n; i++) {
            // wait for signal to DONG
            if(read(pipes[0][0], &signal, 1) < 0) { err(4, "couldn't read"); }

            if(write(1, "DONG", 4) < 0) { err(5, "couldn't write"); }

            // give signal to sleep
            if(write(pipes[1][1], &signal, 1) < 0) { err(5, "couldn't write"); }
            
        }
        close(pipes[0][0]);
        close(pipes[1][1]);
        
        exit(0);
    }
    
    // parent process
    close(pipes[0][0]);
    close(pipes[1][1]);

    char signal;
    for(int i = 0; i < n; i++) {
        // wait for signal to write
            
        if(write(1, "DING", 4) < 0) { err(5, "couldn't write"); }

        signal = 1;
        // signal child to write DONG
        if(write(pipes[0][1], &signal, 1) < 0) { err(5, "couldn't write"); }
        
        // wait for signal to sleep
        if(read(pipes[1][0], &signal, 1) < 0) { err(4, "couldn't read"); }
            
        sleep(d);

    }

    close(pipes[0][1]);
    close(pipes[1][0]);

    wait(NULL);
}
