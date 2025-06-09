#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t uid;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t start_time;
    uint32_t end_time;
} session;

typedef struct {
    uint32_t uid;
    uint32_t max_duration;
    int has_long_session;
} user_info;


int main(int argc, char* argv[]) {
    if(argc != 2) { errx(1, "bad args"); }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) { err(2, argv[1]); }

    session sessions[16384];
    user_info users[2048];

    uint32_t durations[16384];
    int sessions_count = 0;
    int user_count = 0;

    session s;
    ssize_t read_bytes;
    while((read_bytes = read(fd, &s, sizeof(s))) > 0) {
        if(read_bytes != sizeof(s)) {
            errx(3, "Invalid file format");
        }
        sessions[sessions_count] = s;
        durations[sessions_count] = s.end_time - s.start_time;
        sessions_count++;
    }
    if(read_bytes < 0) { err(4, "couldn't read"); }

    close(fd);
    

    if(sessions_count == 0) { return 0; }

    double sum = 0;

    for(int i = 0;  i < sessions_count; i++) { 
        sum += durations[i];
    }

    double avg = sum / sessions_count;

    double dispersion = 0.0;

    for(int i = 0; i < sessions_count; i++) {
        double diff = durations[i] - avg;
        dispersion += diff * diff;
    }
    dispersion /= sessions_count;

    for(int i = 0; i < sessions_count; i++) {
        uint32_t uid = sessions[i].uid;
        uint32_t duration = durations[i];

        int user_index = -1;

        for(int j = 0; j < user_count; j++) {
            if(users[j].uid == uid) {
                user_index = j;
                break;
            }
        }

        if(user_index == -1) {
            user_index = user_count++;
            users[user_index].uid = uid;
            users[user_index].max_duration = duration;
            users[user_index].has_long_session = 0;
        } else {
            if(duration > users[user_index].max_duration) {
                users[user_index].max_duration = duration;
            }
        }

        double duration_squared = (double) duration * duration;
        if (duration_squared > dispersion) {
            users[user_index].has_long_session = 1;
        }
    }
    char buffer[4096];
    for(int i = 0; i < user_count; i++) {
        if(users[i].has_long_session) {
            int len = snprintf(buffer, 4096, "%u %u\n", users[i].uid, users[i].max_duration);
            if(write(1, buffer, len) < 0) { err(10, "couldn't write to stdout"); }
        }
    }
}
