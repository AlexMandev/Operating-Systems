#include <err.h>
#include <unistd.h>
#include <string.h>

int char_in_set(char c, const char* set) {
    while(*set) {
        if(*set == c) 
            return 1;
        
        set++;
    }
    return 0;
}

int find_char_position(char c, const char* set) {
    int pos = 0;
    while (*set) {
        if (*set == c) 
            return pos;
        set++;
        pos++;
    }
    return -1; 
}

void squeeze_option(const char* set1) {
    char c;
    char prev = '\0';
    int first_char = 1;

    ssize_t read_bytes;

    while((read_bytes = read(0, &c, 1)) > 0) {
        if(char_in_set(c, set1)) {
            if(first_char || c != prev) {
                if(write(1, &c, 1) < 0) { err(1, "couldn't write"); }
            }
        } else {
            if(write(1, &c, 1) < 0) { err(1, "couldn't write"); }
        }
        prev = c;
        first_char = 0;
    }
    if(read_bytes < 0) { err(2, "couldn't read"); }
}

void delete_option(const char* set1) {
    ssize_t read_bytes;
    char c;
    while((read_bytes = read(0, &c, 1)) > 0) {
        if(!char_in_set(c, set1)) {
            if(write(1, &c, 1) < 0) { err(1, "couldn't write");}
        }
    }
    if(read_bytes < 0) { err(2, "couldn't read"); }
}

void translate_option(const char* set1, const char* set2) {
    char c;
    ssize_t read_bytes;
    while((read_bytes = read(0, &c, 1)) > 0) {
        int pos = find_char_position(c, set1);
        if (pos >= 0) {
            if(write(1, &set2[pos], 1) < 0) { err(1, "couldn't write"); }
        } else {
            if(write(1, &c, 1) < 0) { err(1, "couldn't write"); } 
        }     
    }
    if(read_bytes < 0) { err(2, "couldn't read"); }
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
	    errx(256, "bad args"); 
	}

	if(strcmp(argv[1], "-d") == 0) {
        if(argc != 3) {
            errx(128, "-d option requires exactly one set.");
        }
        delete_option(argv[2]);
	} else if(strcmp(argv[1], "-s") == 0) {
        if(argc != 3) {
            errx(128, "-s option requires exactly one set.");
        }
        squeeze_option(argv[2]);
	} else {
        if(argc == 2) {
            errx(128, "translations requires two sets.");
        }
        if(argc != 3) { 
            errx(128, "too many args");
        }
        if(strlen(argv[1]) != strlen(argv[2])) {
            errx(512, "the two sets should have the same length");
        }
        translate_option(argv[1], argv[2]);
	}

	return 0;
}



