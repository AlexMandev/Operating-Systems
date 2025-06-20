#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct {
    uint32_t magic;
    uint8_t header_version;
    uint8_t data_version;
    uint16_t count;
    uint32_t reserved1;
    uint32_t reserved2;
} header;

typedef struct {
    uint16_t offset;
    uint8_t original_byte;
    uint8_t new_byte;
} data_v0;

typedef struct {
    uint32_t offset;
    uint16_t original_word;
    uint16_t new_word;
} data_v1;

void validate_header(header h) {
    if(h.magic != 0xEFBEADDE) { errx(100, "invalid patch file format"); }
    if(h.header_version != 0x01) { errx(100, "invalid patch file format - header version is not 0x01"); }
    if(h.data_version != 0x00 && h.data_version != 0x01) { errx(100, "invalid patch file format - invalid data version"); }
}

void copy_file(int f1_fd, int f2_fd) {
    char buffer[4096];
    ssize_t read_bytes;
    
    while((read_bytes = read(f1_fd, buffer, sizeof(buffer))) > 0) {
        if(write(f2_fd, buffer, read_bytes) != read_bytes) { err(6, "couldn't write"); }
    }
    if(read_bytes < 0) { err(5, "couldn't read"); }
}

void process_v0(int patch_fd, int f1_fd, int f2_fd, int file_size) {
    copy_file(f1_fd, f2_fd); 
    
    data_v0 data;
    ssize_t read_bytes;
    uint8_t b;
    while((read_bytes = read(patch_fd, &data, sizeof(data))) > 0) {
        if(read_bytes != sizeof(data)) {
            err(5, "couldn't read the whole data entry from patch file");
        }

        if(data.offset >= file_size) {
            errx(100, "patch data entry offset doesn't match file size");
        }

        if(lseek(f1_fd, data.offset, SEEK_SET) < 0) { err(7, "coudln't lseek"); }
        if(read(f1_fd, &b, 1) < 0) { err(5, "couldn't read"); }

        if(data.original_byte != b) { errx(100, "patch data entry original byte doesn't match the file byte"); }

        if(lseek(f2_fd, data.offset, SEEK_SET) < 0) { err(7, "couldn't lseek"); }
        if(write(f2_fd, &data.new_byte, 1) < 0) { err(6, "couldn't write"); }
    }
    if(read_bytes < 0) { err(5, "couldn't read");}
}

void process_v1(int patch_fd, int f1_fd, int f2_fd, uint32_t file_size) {
    copy_file(f1_fd, f2_fd);
    
    data_v1 data;
    ssize_t read_bytes;
    uint16_t word;
    while((read_bytes = read(patch_fd, &data, sizeof(data))) > 0) {
        if(read_bytes != sizeof(data)) {
            err(5, "couldn't read the whole data entry from patch file");
        }

        if(data.offset * sizeof(uint16_t) >= file_size) {
            errx(100, "patch data entry offset doesn't match file size");
        }

        if(lseek(f1_fd, data.offset * sizeof(word), SEEK_SET) < 0) { err(7, "coudln't lseek"); }
        if(read(f1_fd, &word, sizeof(word)) < 0) { err(5, "couldn't read"); }

        if(data.original_word != word) { errx(100, "patch data entry original byte doesn't match the file byte"); }

        if(lseek(f2_fd, data.offset * sizeof(word), SEEK_SET) < 0) { err(7, "couldn't lseek"); }
        if(write(f2_fd, &data.new_word, sizeof(data.new_word)) < 0) { err(6, "couldn't write"); }
    }
    if(read_bytes < 0) { err(5, "couldn't read");}
}

int main(int argc, char* argv[]) {
    if(argc != 4) { errx(1, "bad args"); }

    int patch_fd = open(argv[1], O_RDONLY);
    if(patch_fd == -1) { err(2, argv[1]); }

    int f1_fd = open(argv[2], O_RDONLY);
    if(f1_fd == -1) { err(2, argv[2]); }

    int f2_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(f2_fd == -1) { err(2, argv[3]); }

    header h;
    ssize_t read_bytes;
    if((read_bytes = read(patch_fd, &h, sizeof(h))) < 0) { err(3, "couldn't read"); }
    if(read_bytes != sizeof(h)) { errx(100, "invalid patch file format"); }

    validate_header(h);
    
    struct stat st;
    if(fstat(f1_fd, &st) < 0) { err(3, "couldn't fstat"); }

    if(h.data_version == 0x01 && st.st_size % sizeof(uint16_t) != 0) {
        errx(30, "invalid format of first file");
    }

    if(h.data_version == 0x00) {
        process_v0(patch_fd, f1_fd, f2_fd, st.st_size);
    } else if(h.data_version == 0x01) {
        process_v1(patch_fd, f1_fd, f2_fd, st.st_size);
    }

    close(patch_fd);
    close(f1_fd);
    close(f2_fd);
}
