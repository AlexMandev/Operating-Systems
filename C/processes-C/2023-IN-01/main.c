#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

const char* WORDS[] = { "tic ", "tac ", "toe\n" };

void communicate(int in_pipe, int out_pipe, int word_count) {
    int written;
    ssize_t read_bytes;

    while((read_bytes = read(in_pipe, &written, sizeof(written))) > 0) {
        if(written >= word_count) {
            // finish up and return/exit
            if(write(out_pipe, &written, sizeof(written)) < 0) { err(15, "couldn't write to pipe"); }
            close(in_pipe);
            close(out_pipe);
            exit(0);
        }

        if(write(1, WORDS[written % 3], 4) < 0) { err(10, "couldn't write to stdout"); }

        int next = written + 1;
        if(write(out_pipe, &next , sizeof(next)) < 0) { err(15, "couldn't write to pipe"); }
    }

    if(read_bytes < 0) { err(20, "couldn't read from pipe"); }
}

int main(int argc, char* argv[]) {
    if(argc != 3) { errx(1, "bad args"); }

    int pr_count = strtol(argv[1], NULL, 10);
    int word_count = strtol(argv[2], NULL, 10);

    if(pr_count < 1 || pr_count > 7) { errx(1, "bad args"); }
    if(word_count < 1 || word_count > 35) { errx(1, "bad args"); }

    int pipe_count = pr_count + 1;

    int pipes[8][2];

    for(int i = 0; i < pipe_count; i++) { 
        if(pipe(pipes[i]) < 0) { err(2, "couldn't pipe"); }
    }

    for(int i = 0; i < pr_count; i++) {
        pid_t pid = fork();
        if(pid < 0) { err(3, "couldn't fork"); }

        int in_pipe = pipes[i][0];
        int out_pipe = pipes[i + 1][1];

        // close unused pipes for child
        if(pid == 0) {
            for(int k = 0; k < pipe_count; k++) {
                if(k == i) {
                    close(pipes[k][1]);
                } else if(k == i + 1) {
                    close(pipes[k][0]);
                } else {
                    close(pipes[k][0]);
                    close(pipes[k][1]);
                }
            }
            communicate(in_pipe, out_pipe, word_count);
            return 0;
        }
    }

    // parent code
    int in_pipe = pipes[pipe_count - 1][0];
    int out_pipe = pipes[0][1];

    // close parent pipe ends
    close(pipes[0][0]);
    for(int i = 1; i < pipe_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    close(pipes[pipe_count - 1][1]);

    if(write(1, WORDS[0], 4) < 0) { err(10, "couldn't write to stdout"); }
    
    int written = 1;
    if(written >= word_count) {
        if(write(out_pipe, &written, sizeof(written)) < 0) { err(5, "couldn't write to pipe"); }
        close(in_pipe);
        close(out_pipe);

        for(int i = 0; i < pr_count; i++) { 
            wait(NULL);
        }
        return 0;
    }

    if(write(out_pipe, &written, sizeof(written)) < 0) {
        err(5, "couldn't write to first child");
    }

    communicate(in_pipe, out_pipe, word_count);
    
    close(in_pipe);
    close(out_pipe);

    for(int i = 0; i < pr_count; i++) {
        wait(NULL);
    }
    
    return 0;
}
