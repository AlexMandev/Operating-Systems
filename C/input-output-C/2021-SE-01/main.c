#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if(argc != 3) { errx(1, "bad args"); }

    int in = open(argv[1], O_RDONLY);
    if(in == -1) { err(2, argv[1]); }

    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(out == -1) { err(2, argv[2]); }

    uint8_t b;
    ssize_t read_bytes;

    while((read_bytes = read(in, &b, 1)) > 0) { 
        uint16_t new_byte = 0;

        for(int i = 7; i >= 0; i--) { 
            new_byte <<= 2;

            if(b & (1 << i)) {
                new_byte |= 2;
            } else {
                new_byte |= 1;
            }
        }

        if(write(out, &new_byte, sizeof(new_byte)) < 0) {  err(4, "couldn't write"); }
    }
    if(read_bytes < 0) { err(3, "couldn't read"); }

    close(in);
    close(out);
}
