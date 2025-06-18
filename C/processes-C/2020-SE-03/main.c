#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdint.h>

typedef struct {
    char file_name[8];
    uint32_t offset;
    uint32_t length;

} element;

int main(int argc, char* argv[]) {
    if(argc != 2) { errx(1, "bad args"); }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) { err(2, argv[1]); }

    element el;
    element elements[8];
    ssize_t read_bytes;
    int elements_count = 0;
    while((read_bytes = read(fd, &el, sizeof(el))) > 0) {
        if(read_bytes != sizeof(element)) { 
            errx(3, "Invalid input file format");
        }
        if(elements_count == 8) { 
            errx(100, "too much elements in input file");
        }
        elements[elements_count++] = el;
    }
    if(read_bytes < 0) { 
        err(4, "couldn't read");
    }

    if(elements_count == 0) { 
        errx(100, "Invalid input file format");
    }

    int pipes[8][2];

    

    for(int i = 0; i < elements_count; i++) {
        if(pipe(pipes[i]) < 0) { err(8, "couldn't pipe"); }

        pid_t pid = fork();
        if(pid < 0) { err(9, "couldn't fork"); }

        if(pid == 0) { 
            for(int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            close(pipes[i][0]);

            int in_fd = open(elements[i].file_name, O_RDONLY);
            if(in_fd == -1) { err(10, elements[i].file_name); }
            
            if(lseek(in_fd, elements[i].offset * sizeof(uint16_t), SEEK_SET) < 0) { err(12, "couldn't lseek"); }

            uint16_t result = 0;
            uint16_t curr;
            for(uint32_t j = 0; j < elements[i].length; j++) {
                ssize_t bytes = read(in_fd, &curr, sizeof(curr));
                if(bytes < 0) { err(12, "couldn't read"); }
                if(bytes != sizeof(curr)) { err(15, "Invalid file format: %s", elements[i].file_name); }

                result ^= curr;
            }

            if(write(pipes[i][1], &result, sizeof(result)) < 0) { err(30, "couldn't write"); }

            close(pipes[i][1]);
            close(in_fd);

            exit(0);
        } else {
            close(pipes[i][1]);
        }
    }

    
    for(int i = 0; i < elements_count; i++) {
        wait(NULL);
    }
    
    uint16_t final = 0;
    uint16_t curr;

    for(int i = 0; i < elements_count; i++) {
        ssize_t bytes = read(pipes[i][0], &curr, sizeof(curr));
        if(bytes != sizeof(curr)) {
            err(200, "couldn't read from child pipes");
        }
        close(pipes[i][0]);
        final ^= curr;
    }
    
    

    char buffer[100];
    int len = snprintf(buffer, 100, "result: %d\n", final);

    if(write(1, buffer, len) < 0) { err(30, "couldn't write"); }
}
