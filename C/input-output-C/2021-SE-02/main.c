#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if(argc != 3) { errx(1, "bad args"); }

    int in = open(argv[1], O_RDONLY);
    if(in == -1) { err(2, argv[1]); }

    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(out == -1) { err(2, argv[2]); }

    struct stat st;
    if(fstat(in, &st) < 0) { err(3, "couldn't fstat"); }

    if(st.st_size % sizeof(uint16_t) != 0) { errx(4, "invalid file format"); }

    uint16_t num;
    ssize_t read_bytes;

    while((read_bytes = read(in, &num, sizeof(num))) > 0) {
        uint8_t new_byte = 0;
        for(int i = 7; i >= 0; i--) {
            uint8_t info = (num >> (i * 2)) & 3;

            if(info == 2) {
                new_byte |= (1 << i);
            } else if(info == 1) {
                // nothing
            } else {
                // wrong encoding
                errx(7, "wrong manchester encoding");
            }
        }

        if(write(out, &new_byte, 1) < 0) { err(6, "couldn't write"); }
    }
    if(read_bytes < 0) { err(5, "couldn't read"); }

    close(in);
    close(out);
}
