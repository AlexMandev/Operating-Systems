#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void log_timestamps(int log_fd, long before, long after, int exit_code) {   
    char buffer[100];

    int len = snprintf(buffer, 100, "%ld %ld %d\n", before, after, exit_code);
    if(write(log_fd, buffer, len) < 0) { err(200, "couldn't write"); }
}

int run_program(char* program, char** cmd_args, int log_fd, int seconds) { 
    int status;
    int exit_code = 0;
    time_t before = time(NULL);
    pid_t pid = fork();
    if(pid < 0) { err(37, "couldn't fork"); }

    

    if(pid == 0) {
        execv(program, cmd_args);
        err(100, "couldn't execv");
    }
    
    wait(&status);
    time_t after = time(NULL);

    if(WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    } else if(WIFSIGNALED(status)) {
        exit_code = 129;
    }

    log_timestamps(log_fd, before, after, exit_code);
    
    if(exit_code != 0 && (after - before) < seconds) {
        return 1;
    }
    return 0;
    
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        errx(1, "bad args");
    }

    if(strlen(argv[1]) != 1 || argv[1][0] < '1' || argv[1][0] > '9') {
        errx(1, "bad args");
    }

    int seconds = argv[1][0] - '0';

    int log = open("run.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(log == -1) { err(2, "run.log"); }

    char** cmd_args = malloc((argc - 2 + 1) * sizeof(char*));
    
    for(int i = 0; i < argc - 2; i++) {
        cmd_args[i] = argv[i + 2];
    }

    cmd_args[argc - 2] = NULL;
    
    int last = 0;
    while(1) {
        int res = run_program(argv[2], cmd_args, log, seconds);

        if(res && last) {
            break;
        }
        if(res) {
            last = 1;
        } else {
            last = 0;
        }
    }

    free(cmd_args);
    close(log);
}
