#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct {
    uint16_t offset;
    uint8_t original;
    uint8_t new_byte;
} triple;

int main(int argc, char* argv[]) {
    if(argc != 4) { errx(1, "bad args"); }
    
    int patch_fd = open(argv[1], O_RDONLY);
    if(patch_fd == -1) { err(2, argv[1]); }
    
    int first_fd = open(argv[2], O_RDONLY);
    if(first_fd == -1) { err(2, argv[2]); }
    
    int second_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(second_fd == -1) { err(2, argv[3]); }
    
    struct stat st;
    if(fstat(patch_fd, &st) < 0) { err(3, "couldn't fstat patch"); }
    if(st.st_size % sizeof(triple) != 0) { errx(4, "Invalid patch file format."); }
    
    if(fstat(first_fd, &st) < 0) { err(3, "couldn't fstat first"); }
    off_t first_size = st.st_size;
    
    uint8_t b;
    ssize_t read_bytes;
    while((read_bytes = read(first_fd, &b, sizeof(b))) > 0) {
        if(write(second_fd, &b, sizeof(b)) < 0) { err(7, "couldn't write"); }
    }
    if(read_bytes < 0) { err(8, "couldn't read"); }
    if(lseek(first_fd, 0, SEEK_SET) < 0) { err(5, "couldn't lseek first"); }
    
    triple t;
    ssize_t patch_read;
    while((patch_read = read(patch_fd, &t, sizeof(t))) > 0) {
        if(patch_read != sizeof(triple)) {
            err(5, "couldn't read whole patch entry");
        }
        if(t.offset >= first_size) {
            errx(6, "Patch offset doesn't match file size.");
        }
        
        if(lseek(first_fd, t.offset, SEEK_SET) < 0) { err(5, "couldn't lseek"); }
        if(read(first_fd, &b, sizeof(b)) != 1) { err(8, "couldn't read original byte"); }
        
        if(t.original != b) {
            errx(6, "Patch original byte doesn't match file byte."); 
        }
        
        if(lseek(second_fd, t.offset, SEEK_SET) < 0) { err(5, "couldn't lseek second"); }
        if(write(second_fd, &t.new_byte, sizeof(t.new_byte)) < 0) { err(7, "couldn't write"); }
    }
    
    if(patch_read < 0) { err(8, "couldn't read patch"); }
    
    close(patch_fd);
    close(first_fd);
    close(second_fd);
    
    return 0;
}
