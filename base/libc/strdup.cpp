#include <libc.hpp>
char *strdup(char *str) {
    char *buf = new char[strlen(str)];
    memset(buf, 0, strlen(str));
    memcpy(buf, str, strlen(str));
    return buf;
}