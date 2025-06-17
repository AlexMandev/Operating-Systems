#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void execute_command(const char* command, char args[][5], int arg_count) {
    pid_t pid = fork();
    if(pid < 0) { err(37, "couldn't fork"); }

    if(pid == 0) {
        if(arg_count == 1) {
            execlp(command, command, args[0], (char*)NULL);
        } else {
            execlp(command, command, args[0], args[1], (char*)NULL);
        }
        err(6006, "couldn't execlp");
    } else {
        wait(NULL);
    }
}

int main(int argc, char* argv[]) {
    if(argc > 2) { errx(1, "bad args"); }

    const char* command = (argc == 2 ? argv[1] : "echo");

    if(strlen(command) > 4) { errx(4, "command too long"); }

    char c;
    char arg_buffer[5];
    int buffer_pos = 0;
    char args[2][5];
    int arg_count = 0;

    ssize_t read_bytes = 0;

    while((read_bytes = read(0, &c, 1)) > 0) {
        if(c == ' ' || c == '\n') {
            if(buffer_pos > 0) {
                arg_buffer[buffer_pos] = '\0';
                strcpy(args[arg_count], arg_buffer);
                arg_count++;
                buffer_pos = 0;

                if(arg_count == 2) {
                    execute_command(command, args, 2);
                    arg_count = 0;

                }
            }
        } else {
            if(buffer_pos >= 4) { 
                errx(4, "arg too long");
            }
            arg_buffer[buffer_pos++] = c;
        }
    }

    if(read_bytes) { err(10, "couldn't read"); }

    if(buffer_pos > 0) {
        arg_buffer[buffer_pos] = '\0';
        strcpy(args[arg_count], arg_buffer);
        arg_count++;
    }

    if(arg_count > 0) {
        execute_command(command, args, arg_count);
    }
}
