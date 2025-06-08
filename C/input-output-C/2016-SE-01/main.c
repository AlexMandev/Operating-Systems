#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int cnt[256] = {0};

    if (argc != 2) {
        errx(1, "bad args");
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) { err(2,argv[1]); }

    char c;
    ssize_t bytes_read;
    while ((bytes_read = read(fd, &c, sizeof(c))) > 0) {
        cnt[c + 128]++;        
    }

    if (bytes_read < 0) { err(3, "read error"); }

    close(fd);

    fd = open(argv[1], O_WRONLY | O_TRUNC);

    if (fd == -1) { err(4, argv[2]); }

    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < cnt[i]; j++) {
            c = i - 128;
            if (write(fd, &c, 1) < 0) { err(5, "Couldn't write"); }
        }
    }

    close(fd);
}
