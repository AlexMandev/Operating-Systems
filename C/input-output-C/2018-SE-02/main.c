#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <err.h>

#define CHUNK_SIZE (2 * 1024 * 1024)

int compare(const void* f, const void* s) {
    uint32_t x = *(const uint32_t*) f;
    uint32_t y = *(const uint32_t*) s;

    return (x > y) - (x < y);
}

void merge_files(int fd1, int fd2, int out_fd) { 
    uint32_t n1;
    uint32_t n2;

    ssize_t read1 = read(fd1, &n1, 4);
    ssize_t read2 = read(fd2, &n2, 4);

    if(read1 < 0) { err(4, "couldn't read"); }
    if(read2 < 0) { err(4, "couldn't read"); }
    
    int has1 = (read1 == 4);
    int has2 = (read2 == 4);

    while(has1 && has2) {
        if (n1 <= n2) {
            if(write(out_fd, &n1, 4) < 0) { err(12, "couldn't write"); }
            if((read1 = read(fd1, &n1, 4)) < 0) { err(4, "couldn't read"); }
            has1 = (read1 == 4);
        } else {
            if(write(out_fd, &n2, 4) < 0) { err(12, "couldn't write"); }
            if((read2 = read(fd2, &n2, 4)) < 0) { err(4, "couldn't read"); }
            has2 = (read2 == 4);
        }
    }

    while(has1) {
        if (write(out_fd, &n1, 4) != 4) err(4, "couldn't write");
        read1 = read(fd1, &n1, 4);
        if (read1 < 0) err(12, "couldn't read");
        has1 = (read1 == 4);
    }
    while(has2) {
        if (write(out_fd, &n2, 4) != 4) err(4, "couldn't write");
        read2 = read(fd2, &n2, 4);
        if (read2 < 0) err(12, "couldn't read");
        has2 = (read2 == 4);
    }
}

int main(int argc, char* argv[]) {
	if(argc != 3) { errx(1, "bad args"); }

    uint32_t chunk[CHUNK_SIZE];

    int in_fd = open(argv[1], O_RDONLY);
    if(in_fd == -1) { err(2, argv[1]); }

    struct stat st;
    if(fstat(in_fd, &st) < 0) { err(3, "couldn't fstat"); }
    size_t total_numbers = st.st_size / 4;
    
    if(total_numbers <= CHUNK_SIZE) {
        if (read(in_fd, chunk, total_numbers * 4) != (ssize_t)(total_numbers * 4)) {
            err(12, "couldn't read");
        }
        
        qsort(chunk, total_numbers, 4, compare);
        
        int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(out_fd == -1) { err(2, argv[2]); }
        if(write(out_fd, chunk, total_numbers * 4) < 0) { err(4, "couldn't write"); }
        close(in_fd);
        close(out_fd);
        return 0;
    } 
    char temp1_name[] = "/tmp/sort_XXXXXX";
    char temp2_name[] = "/tmp/sort_XXXXXX";

    int temp1_fd = mkstemp(temp1_name);
    if(temp1_fd == -1) { err(4, "couldn't create temp file"); }

    int temp2_fd = mkstemp(temp2_name);
    if(temp2_fd == -1) { err(4, "couldn't create temp file"); }

    size_t remaining = total_numbers;
    int current_temp = 1;

    while(remaining > 0) { 
        size_t chunk_size = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;
        
        if(read(in_fd, chunk, chunk_size * 4) < 0) { err(5, "couldn't read"); }

        qsort(chunk, chunk_size, 4, compare);
        
        if(current_temp == 1) {
            if(write(temp1_fd, chunk, chunk_size * 4) < 0) { err(6, "couldn't write"); }
            current_temp = 2;
        } else { 
            if(write(temp2_fd, chunk, chunk_size * 4) < 0) { err(6, "couldn't write"); }
            current_temp = 1;
        }
        remaining -= chunk_size;
    }

    if(lseek(temp1_fd, 0, SEEK_SET) < 0) { err(7, "couldn't lseek"); } 
    if(lseek(temp2_fd, 0, SEEK_SET) < 0) { err(7, "couldn't lseek"); }
    
    int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(out_fd == -1) { err(2, argv[2]); }

    merge_files(temp1_fd, temp2_fd, out_fd);

    close(in_fd);
    close(temp1_fd);
    close(temp2_fd);
    close(out_fd);
    
    if(unlink(temp1_name) < 0) { err(10, "couldn't unlink temp"); }
    if(unlink(temp2_name) < 0) { err(10, "couldn't unlink temp"); }
}
