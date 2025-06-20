#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>
#include <signal.h>
#include <unistd.h>

int get_pid_index(int* pids, int programs, int pid) {
    for(int i = 0; i < programs; i++) {
        if(pids[i] == pid) {
            return i;
        }
    }
    return -1;
}

void kill_all_except(int* pids, int programs, int pid) {
    for(int i = 0; i < programs; i++) {
        if(pids[i] != -1 && pids[i] != pid) {
            if(kill(pids[i], SIGTERM) < 0) { warn("Couldn't send signal to process with ID %d", pids[i]); }            
        }
    }

    for(int i = 0; i < programs; i++) {
        if(pids[i] != -1 && pids[i] != pid) {
            waitpid(pids[i], NULL, 0);
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 11) { errx(1, "bad args"); }	

    int pids[10];
    int programs = argc - 1;
    int working_programs = argc - 1;
    // start programs first time
    for(int i = 1; i < argc; i++) {
        pid_t pid = fork();
        if(pid < 0) { err(2, "couldn't fork"); }
        
        if(pid == 0) {
            execl(argv[i], argv[i], (char*)NULL);
            err(3, "couldn't execl");
        }

        pids[i - 1] = pid;
    }

    int status;
    while(1) {
        int waited_pid = wait(&status);
        
        if(WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);

            if(exit_status == 0) {
                working_programs--;
                if(working_programs == 0) { 
                    return 0;
                }
                int index = get_pid_index(pids, programs, waited_pid);
                pids[index] = -1;
            } else {
                int index = get_pid_index(pids, programs, waited_pid);
                
                pid_t new_pid = fork();
                if(new_pid < 0) { err(2, "couldn't fork"); }

                if(new_pid == 0) {
                    execl(argv[index + 1], argv[index + 1], (char*)NULL);
                    err(3, "couldn't execl");
                }

                pids[index] = new_pid;
            }
        } else if(WIFSIGNALED(status)) {
            kill_all_except(pids, programs, waited_pid); 
            int index = get_pid_index(pids, programs, waited_pid);
            return index + 1;
        }
    }

}
