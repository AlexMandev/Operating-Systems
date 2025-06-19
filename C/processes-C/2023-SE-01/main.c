#include <err.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

void process_file(char* file) {
    int md5_pfd[2];
    if(pipe(md5_pfd) < 0) { err(2, "couldn't pipe"); }
    
    pid_t pid = fork();
    if(pid < 0) { err(3, "couldn't fork"); }
    
    if(pid == 0) {
        close(md5_pfd[0]);
        dup2(md5_pfd[1], 1);
        close(md5_pfd[1]);
        execlp("md5sum", "md5sum", file, (char*)NULL);
        err(4, "couldn't execlp");
    }
    
    close(md5_pfd[1]);
    
    char hash_name[4096];
    strcpy(hash_name, file);
    strncat(hash_name, ".hash", 4096 - strlen(hash_name) - 1);
    
    int hash = open(hash_name, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(hash == -1) { err(10, "%s", hash_name); }
    
    ssize_t read_bytes;
    char b;
    while((read_bytes = read(md5_pfd[0], &b, 1)) > 0) {
        if(b == ' ') {
            break;
        }
        if(write(hash, &b, 1) < 0) { err(12, "couldn't write"); }
    }
    if(read_bytes < 0) { err(15, "couldn't read from pipe"); }
    
    b = '\n';
    if(write(hash, &b, 1) < 0) { err(12, "couldn't write"); }
    
    close(md5_pfd[0]);
    wait(NULL);
    close(hash);
}

int main(int argc, char* argv[]) {
    if(argc != 2) { errx(1, "bad args"); }
    
    int find_pfd[2];
    if(pipe(find_pfd) < 0) { err(2, "couldn't pipe"); }
    
    pid_t find_pid = fork();
    if(find_pid < 0) { err(3, "couldn't fork"); }
    
    if(find_pid == 0) {
        close(find_pfd[0]);
        dup2(find_pfd[1], 1);
        close(find_pfd[1]);
        execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", "-print0", (char*)NULL);
        err(4, "couldn't execlp");
    }
    
    close(find_pfd[1]);
    
    ssize_t read_bytes;
    char b;
    char buffer[4096];
    size_t index = 0;
    int waits = 0;
    
    while((read_bytes = read(find_pfd[0], &b, 1)) > 0) {
        buffer[index++] = b;
        if(b == '\0') {
            index = 0;
            
            pid_t child_pid = fork();
            if(child_pid < 0) { err(3, "couldn't fork"); }
            
            if(child_pid == 0) {
                process_file(buffer);
                exit(0);
            }
            waits++;
        }
    }
    
    if(read_bytes < 0) { err(5, "couldn't read"); }
    
    close(find_pfd[0]);
    wait(NULL);
    
    for(int i = 0; i < waits; i++) {
        wait(NULL);
    }
    
    return 0;
}
