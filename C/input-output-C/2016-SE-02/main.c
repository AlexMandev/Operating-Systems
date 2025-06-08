#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>

typedef struct {
    uint32_t pos;
    uint32_t count;
} interval;

void solve(int fd1, int fd2, int fd3) {
    interval pair;
    if (read(fd1, &pair, sizeof(pair)) != sizeof(pair)) {
        err(2, "couldn't read from first file");
    }

    if (lseek(fd2, pair.pos * sizeof(uint32_t), SEEK_SET) < 0) { err(3, "couldn't lseek"); }
    
    uint32_t num;
    for(uint32_t i = 0; i < pair.count; i++) {

        if(read(fd2, &num, sizeof(num)) != sizeof(num)) { err(4, "couldn't read from second file"); }

        if(write(fd3, &num, sizeof(num)) != sizeof(num)) { err(5, "couldn't write to third file"); }
    }
}

int main(void) {
    const char* first_file = "f1";
    const char* second_file = "f2";
    const char* third_file = "f3";

    int fd1 = open(first_file, O_RDONLY);
    
    if (fd1 == -1) { err(1, first_file); }
    
    int fd2 = open(second_file, O_RDONLY);

    if (fd2 == -1) { err(1, second_file); }

    int fd3 = open(third_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if(fd3 == -1) { err(1, third_file); }

    solve(fd1, fd2, fd3);
    solve(fd1, fd2, fd3);

    close(fd1);
    close(fd2);
    close(fd3);
}
