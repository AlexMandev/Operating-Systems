#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <sys/wait.h>
#include <unistd.h>

// cut -d: -f7 /etc/passwd | sort | uniq -c | sort -n
// => 3 pipes

int main(void) {
    int pfd1[2], pfd2[2], pfd3[2];	

    if(pipe(pfd1) < 0) err(1, "couldn't pipe");
    if(pipe(pfd2) < 0) err(1, "couldn't pipe"); 
    if(pipe(pfd3) < 0) err(1, "couldn't pipe");

    pid_t pid = fork();
    if(pid < 0) { err(2, "couldn't fork"); }
    
    if(pid == 0) {
        close(pfd1[0]);
        close(pfd2[0]);
        close(pfd2[1]);
        close(pfd3[0]);
        close(pfd3[1]);
        if(dup2(pfd1[1], 1) < 0) { err(3, "couldn't dup2"); }
        close(pfd1[1]);
        execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL);
        err(4, "couldn't execlp");
    }

    pid = fork();
    if(pid < 0) { err(2, "couldn't fork"); }

    if(pid == 0) {
        close(pfd1[1]);
        close(pfd2[0]);
        close(pfd3[0]);
        close(pfd3[1]);
        
        if(dup2(pfd1[0], 0) < 0) { err(3, "couldn't dup2"); }
        if(dup2(pfd2[1], 1) < 0) { err(3, "couldn't dup2"); }
        close(pfd1[0]);
        close(pfd2[1]);
        
        execlp("sort", "sort", (char*)NULL);
        err(4, "couldn't execlp");
    }

    pid = fork();
    if(pid < 0) { err(2, "couldn't fork"); }

    if(pid == 0) { 
        close(pfd1[0]);
        close(pfd1[1]);
        close(pfd2[1]);
        close(pfd3[0]);
        
        if(dup2(pfd2[0], 0) < 0) { err(3, "couldn't dup2"); }
        if(dup2(pfd3[1], 1) < 0) { err(3, "couldn't dup2"); }
        close(pfd2[0]);
        close(pfd3[1]);
        
        execlp("uniq", "uniq", "-c", (char*)NULL);
        err(4, "couldn't execlp"); 
    }

    pid = fork();
    if(pid < 0) { err(2, "couldn't fork"); }

    if(pid == 0) { 
        close(pfd1[0]);
        close(pfd1[1]);
        close(pfd2[0]);
        close(pfd2[1]);
        close(pfd3[1]);

        if(dup2(pfd3[0], 0) < 0) { err(3, "couldn't dup2"); }
        close(pfd3[0]);

        execlp("sort", "sort", "-n", (char*) NULL);
        err(4, "couldn't execlp");
    }

    close(pfd1[0]); close(pfd1[1]);
    close(pfd2[0]); close(pfd2[1]);
    close(pfd3[0]); close(pfd3[1]);

    wait(NULL); wait(NULL); wait(NULL); wait(NULL);

    exit(0);
}
