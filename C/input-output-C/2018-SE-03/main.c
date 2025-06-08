#include <unistd.h>
#include <err.h>
#include <string.h>

int is_digit(char c) {
    return c >= '1' && c <= '9';
}

void set_range(const char* format, int* start, int* end) {
    size_t len = strlen(format);
    if(len != 3 && len != 1) {
        errx(128, "Invalid format for range of characters");
    }
    if(len == 1 && !is_digit(*format)) { 
        errx(128, "Invalid format for range of characters");
    }
    if(len == 3 && (!is_digit(format[0]) || !is_digit(format[2]) ||  (format[1] != '-'))) { 
        errx(128, "Invalid format for range of characters");
    }
    if(len == 1) {
        (*start) = (*format) - '0';
        (*end) = (*format) - '0';
    } else if (len == 3) {
        (*start) = (*format) - '0';
        (*end) = format[2] - '0';
    }

}

void cut_range(int start, int end) {
     int pos = 1;
     
     char c;
     ssize_t read_bytes;
     while((read_bytes = read(0, &c, 1)) > 0) {
        if(c == '\n') {
            if(write(1, &c, 1) < 0) { err(2, "couldn't write"); }
            pos = 0;
        } else if(pos >= start && pos <= end) {
            if(write(1, &c, 1) < 0) { err(2, "couldn't write"); } 
        }
        pos++;
     }
     if(read_bytes < 0) { err(1, "couldn't read"); } 
}

void cut_fields(char delimiter, int start, int end) {
    char c;
    ssize_t read_bytes;
    int field = 1;
    int valid_field = (field >= start && field <= end);
    int first_field = 1;

    while((read_bytes = read(0, &c, 1)) > 0) {
        if(c == '\n') {
            field = 1;
            valid_field = (field >= start && field <= end);
            first_field = 1;
        } else if(c == delimiter) {
            field++;
            int next_valid = (field >= start && field <= end);

            if(valid_field && next_valid && !first_field) {
                if(write(1, &delimiter, 1) < 0) { err(2, "couldn't write"); }
            }

            valid_field = next_valid;

            if(valid_field) first_field = 0;
        } else {
            if(valid_field) {
                if(write(1, &c, 1) < 0) { err(2, "couldn't write"); }
            }
        }
    }
}

int main(int argc, char* argv[]) {
	if(argc < 3) { 
        errx(10, "bad args");
	}
    
    int start, end;

    if(strcmp(argv[1], "-c") == 0) {
        if(argc != 3) { errx(256, "-c requires one range argument"); }

        set_range(argv[2], &start, &end);
        cut_range(start, end);
    } else if(strcmp(argv[1], "-d") == 0) {
        if(argc != 5 || strcmp(argv[3], "-f") != 0) {
            errx(128, "-d requires: -d DELIM -f range");
        }
        char delim = argv[2][0];
        set_range(argv[4], &start, &end);
        cut_fields(delim, start ,end);
    } else {
        errx(512, "first arg must be -c or -d");
    }
	return 0;
}
