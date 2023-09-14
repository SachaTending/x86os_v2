#include <stdint.h>
int memcmp(uint8_t * data1, uint8_t * data2, int n) {
    for (int i=0;i<n;i++) {
        if (data1[i] != data2[i]) return true;
    }
    return false;
}
int memcmpn(uint8_t * data1, uint8_t * data2, int n) {
    return !memcmp(data1, data2, n);
}