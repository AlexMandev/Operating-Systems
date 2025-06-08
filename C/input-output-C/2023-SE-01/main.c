#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>



int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "Invalid number of arguments.");
    }

    int src = open(argv[1], O_RDONLY);
    if (src == -1) { err(2, argv[1]); }

    int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (dest == -1) { err(3, argv[2]); }

    uint8_t byte;
    ssize_t read_bytes;
    uint8_t message[256];
    while((read_bytes = read(src, &byte, 1)) > 0) {
        if (byte != 0x55)
            continue;

        uint8_t n;

        if (read(src, &n, 1) < 0) { err(4, "Error when reading from source"); }

        uint8_t sum = byte;
        sum ^= n;
        int index = 0;
        for(int i = 3; i < n; i++) {
            if (read(src, &byte, 1) < 0) { err(4, "Error when reading from source"); }

            message[index++] = byte;
            sum ^= byte;
        }
        uint8_t checksum;

        if(read(src, &checksum, 1) < 0) { err(4, "Error when reading from source"); }

        if(checksum == sum) {
            uint8_t start = 0x55;
            if(write(dest, &start, 1) < 0) { err(5, "Error when writing to destination"); }

            if(write(dest, &n , 1) < 0) { err(5, "Error when writing to destination"); }

            if (write(dest, message, index) < 0) { err(5, "Error when writing to destination"); }
            
            if (write(dest, &checksum, 1) < 0) { err(5, "Error when writing to destination"); }
        }
    }

    if (read_bytes == -1) { err(4, "Error when reading from source"); }

    close(src);
    close(dest);
}


