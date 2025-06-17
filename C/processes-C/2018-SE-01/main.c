#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

// find {argv[1]} -type f -printf '%T@ %p\n' | sort -n | tail -1 | cut -d' ' -f2-

int main(int argc, char* argv[]) {
    if(argc != 2) { errx(1, "bad args"); }
   
    int pfd1[2], pfd2[2], pfd3[2];	

    if(pipe(pfd1) < 0 || pipe(pfd2) < 0 || pipe(pfd3) < 0) {
        err(2, "couldn't pipe");
    }

    pid_t pid = fork();
    if(pid < 0) { err(3, "couldn't fork"); }

    if(pid == 0) {
        close(pfd1[0]);
        close(pfd2[0]); close(pfd2[1]);
        close(pfd3[0]); close(pfd3[1]);
        
        if(dup2(pfd1[1], 1) < 0) { err(37, "couldn't dup2"); }
        close(pfd1[1]);
        
        execlp("find", "find", argv[1], "-type", "f", "-printf", "%T@ %p\n", NULL);
        err(4, "couldn't execlp");
    }

    pid = fork();
    if(pid < 0) { err(3, "couldn't fork"); }
    
    if(pid == 0) {
        close(pfd1[1]);
        close(pfd2[0]);
        close(pfd3[0]); close(pfd3[1]);
        
        if(dup2(pfd1[0], 0) < 0) { err(37, "couldn't dup2"); }
        close(pfd1[0]);
        if(dup2(pfd2[1], 1) < 0) {err(37, "couldn't dup2"); }
        close(pfd2[1]);
        
        execlp("sort", "sort", "-n", NULL);
        err(5, "couldn't execlp");
    }

    pid = fork();
    if(pid < 0) { err(3, "couldn't fork"); }
 
    if(pid == 0) {
        close(pfd1[0]); close(pfd1[1]);
        close(pfd2[1]);
        close(pfd3[0]);
        
        if(dup2(pfd2[0], 0) < 0) { err(37, "couldn't dup2"); }
        close(pfd2[0]);
        if(dup2(pfd3[1], 1) < 0) { err(37, "couldn't dup2"); }
        close(pfd3[1]);
        
        execlp("tail", "tail", "-1", NULL);
        err(6, "couldn't execlp");
    }

    pid = fork();
    if(pid < 0) { err(3, "couldn't fork"); }
    
    if(pid == 0) {
        close(pfd1[0]); close(pfd1[1]);
        close(pfd2[0]); close(pfd2[1]);
        close(pfd3[1]);
        
        if(dup2(pfd3[0], 0) < 0) { err(37, "couldn't dup2"); }
        close(pfd3[0]);
        
        execlp("cut", "cut", "-d ", "-f2-", NULL);
        err(7, "couldn't execlp");
    }

    close(pfd1[0]); close(pfd1[1]);
    close(pfd2[0]); close(pfd2[1]);
    close(pfd3[0]); close(pfd3[1]);

    wait(NULL); wait(NULL);
    wait(NULL); wait(NULL);

    return 0;
}
