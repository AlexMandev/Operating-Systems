#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <err.h>

typedef struct {
    uint16_t offset;
    uint8_t original;
    uint8_t new_byte;
} triple;

int main(int argc, char* argv[]) {
    if(argc != 4) { errx(1, "bad args"); }

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) { err(2, argv[1]); }
    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) { err(2, argv[2]); }
    int fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd3 == -1) { err(2, argv[3]); }

    
    
    struct stat st1;
    struct stat st2;
    if(fstat(fd1, &st1) < 0 || fstat(fd2, &st2) < 0) { err(3, "couldn't fstat"); }

    if(st1.st_size != st2.st_size) { errx(4, "files are different sizes"); }
     
    ssize_t bytes_first = 0;
    ssize_t bytes_second = 0;
    triple t;
    t.offset = 0;
    while((bytes_first = read(fd1, &t.original, 1)) > 0 &&
           (bytes_second = read(fd2, &t.new_byte, 1)) > 0) {
        if(t.original != t.new_byte) {
            if(write(fd3, &t, sizeof(t)) < 0) { err(100, "couldn't write"); }
        }
        t.offset++;
    }
    if(bytes_first < 0 || bytes_second < 0) { err(5, "couldn't read"); }

    close(fd1);
    close(fd2);
    close(fd3);
}
