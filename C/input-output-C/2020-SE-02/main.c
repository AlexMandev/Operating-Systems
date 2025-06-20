#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <err.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if(argc != 4) { errx(1, "bad args"); } 	

    int scl = open(argv[1], O_RDONLY);
    if(scl == -1) { err(2, argv[1]); }

    int sdl = open(argv[2], O_RDONLY);
    if(sdl == -1) { err(2, argv[2]); }

    int out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(out == -1) { err(2, argv[3]); }

    struct stat st1;
    struct stat st2;
    
    if(fstat(scl, &st1) < 0) { err(3, "couldn't fstat"); }
    if(fstat(sdl, &st2) < 0) { err(3, "couldn't fstat"); }

    if(st2.st_size % sizeof(uint16_t) != 0) { errx(4, "sdl file size not divisible by uint16_t size"); }

    if((size_t)(st1.st_size * 8) != st2.st_size / sizeof(uint16_t)) { 
        errx(4, "file elements don't match"); 
    }

    uint8_t b;
    ssize_t read_bytes;
    uint16_t sdl_num;
    while((read_bytes = read(scl, &b, 1)) > 0) { 
        for(int i = 7; i >= 0; i--) {
            if(read(sdl, &sdl_num, sizeof(uint16_t)) != sizeof(uint16_t)) { err(5, "couldn't read"); }

            if(b & (1 << i)) {
               if(write(out, &sdl_num, sizeof(uint16_t)) < 0) { err(6, "couldn't write"); } 
            }
        }
    }
    if(read_bytes < 0) { err(5, "couldn't read"); }

    close(scl);
    close(sdl);
    close(out);
}
