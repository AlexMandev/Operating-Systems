#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
	if(argc != 2) {
        errx(1, "bad args");
	}

	int pfd[2];

    if(pipe(pfd) < 0) { err(3, "couldn't pipe"); }

    pid_t pid = fork();

    if(pid < 0) { err(4, "couldn't fork"); }

    if(pid == 0) {
        // cat the file
        close(pfd[0]);
        if(dup2(pfd[1], 1) < 0) { err(5, "couldn't dup2"); }
        close(pfd[1]);
        execlp("cat", "cat", argv[1], (char*)NULL);
        
        // error by execlp
        err(6, "couldn't execlp");
    } else {
        close(pfd[1]);
        if(dup2(pfd[0], 0) < 0) { err(5, "couldn't dup2"); }
        close(pfd[0]);
        execlp("sort", "sort", (char*)NULL);
        
        err(6, "couldn't execlp");
    }
}
