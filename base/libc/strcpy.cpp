#include <libc.hpp>
int strcpy(char *dst, const char *src) {
    int out = 0;
    int a = strlen(src);
    while (a--) {
        dst[out] = src[out];
        out++;
    }
    return out;
}