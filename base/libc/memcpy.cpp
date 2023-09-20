#include <stdint.h>
extern "C" int memcpy(void *dst, const void *src, int num) {
    int out = 0;
    int a = num;
    uint8_t *dst2 = (uint8_t *)dst;
    uint8_t *src2 = (uint8_t *)src;
    while (a--) {
        dst2[out] = src2[out];
        out++;
    }
    return out;
}