#include <libc.hpp>
int strcmp(const char *dst, char *src) {
    for (int i=0;i<strlen((const char *)src);i++) {
        if (src[i] != dst[i]) return true;
    }
    return false;
}
int strncmp(const char *dst, char *src) {
    return !strcmp(dst, src);
}