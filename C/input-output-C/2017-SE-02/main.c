#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>

void read_from_fd(int fd, int numbered, int* line) {
    char c;
    char buffer[20]; // for counting lines
    int newline = 1;
    ssize_t read_bytes;
    while((read_bytes = read(fd, &c, sizeof(c))) > 0) {
        if(newline && numbered) {
            int len = snprintf(buffer, sizeof(buffer), "%u ", (*line)++);
            if (write(1, buffer, len) < 0) { err(2, "write error to stdout"); }
        }

        if (write(1, &c, 1) < 0) { err(2, "write error to stdout"); }

        if (c == '\n') {
            newline = 1;
        } else {
            newline = 0;
        }

    }
    if(read_bytes < 0) { err(1, "read error"); }

}

int main(int argc, char* argv[]) {
    int numbered = 0;
    int line = 1;
    if (argc >= 2 && strcmp(argv[1], "-n") == 0) {
        numbered = 1;
    }

    int i = 1;
    if(numbered) { i++; }

    for(; i < argc; i++) {
        if(strcmp(argv[i], "-") == 0) { read_from_fd(0, numbered, &line); }
        else {
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) { warn(argv[i]); continue; }
            
            read_from_fd(fd, numbered, &line);
            close(fd);
        }
    }
}
