#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>

void exec_command(char* command) {
    pid_t pid = fork();

    if(pid < 0) { err(3, "couldn't fork"); }

    char command_str[5001];
    command_str[0] = '\0';

    strcat(command_str, "/bin/");
    strcat(command_str, command);

    if(pid == 0) {
        execl(command_str, command, (char*) NULL);
        
        err(4, "couldn't execl");
    } else {
        wait(NULL);
    }
}

int main(void) {
   char prompt[] = ">>";

   char buffer[4096];

    while(1) {
        if(write(1, prompt, 2) < 0) { err(1, "couldn't write prompt"); }

        int read_bytes = read(0, buffer, sizeof(buffer));
        
        if(read_bytes < 0) { err(2, "couldn't read command"); }
        else if(read_bytes == 0) { continue; } 
        
        if(buffer[read_bytes - 1] == '\n') {
            buffer[read_bytes - 1] = '\0';
        } else {
            buffer[read_bytes] = '\0';
        }

        if(strcmp(buffer, "exit") == 0) { break; }
    
        exec_command(buffer);
    }

    return 0;
}
