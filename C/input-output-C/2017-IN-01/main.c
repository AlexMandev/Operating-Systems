#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>

typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t reserved;
} sequence;

int main(int argc, char* argv[]) {
    if (argc != 5) { errx(1, "bad args"); }

    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) { err(2, argv[1]);}
    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1) { err(2, argv[2]);}
    
    int fd3 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd3 == -1) { err(2, argv[3]);}
    int fd4 = open(argv[4], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd4 == -1) { err(2, argv[4]);}
    
    struct stat st;
    if(fstat(fd2, &st) < 0) { err(2, "couldn't fstat");}

    if(st.st_size % sizeof(sequence) != 0) { errx(3, "invalid format of .idx file"); }
    
    if(fstat(fd1, &st) < 0) { err(2, "couldn't fstat");}

    off_t size_of_dat = st.st_size;
    
    sequence seq;
    ssize_t read_bytes;
    int pos = 0;
    while((read_bytes = read(fd2, &seq, sizeof(seq))) > 0) {
        if(read_bytes != sizeof(seq)) { err(4, "error while reading"); }

        if(seq.offset > size_of_dat) { errx(6, "logic error of offset in .idx file"); }

        if(lseek(fd1, seq.offset, SEEK_SET) < 0) { err(5, "couldn't lseek"); }

        char c;
        int broken = 0;
    
        for(uint16_t i = 0; i < seq.length; i++) {
            if(read(fd1, &c, sizeof(c)) != 1) { err(3, "couldn't read"); }
            
            if(i == 0 && (c < 'A' || c > 'Z')) {
                broken = 1;
                break;
            }

            if(write(fd3, &c, sizeof(c)) < 0) { err(7, "couldn't write"); }
                    
        }
        
        if (broken == 0) {
            seq.offset = pos;
            pos += seq.length;
            if(write(fd4, &seq, sizeof(seq)) < 0) { err(7, "couldn't write"); }
        }

    }
    if (read_bytes < 0) { err(3, "couldn't read"); }

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
}
