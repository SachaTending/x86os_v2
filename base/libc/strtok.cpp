#include <libc.hpp>

int strtok(const char *str, char tok) {
    for (int i=0;i<strlen(str);i++) {
        if (str[i] == tok) {
            return i;
        }
    }
    return 0;
}