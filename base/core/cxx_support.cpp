#include <libc.hpp>

void *operator new(unsigned int size) {
    return malloc(size);
}

void *operator new[](unsigned int size) {
    return malloc(size);
}

void operator delete(void *ptr) {
    free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}

void operator delete(void *ptr, unsigned long size) {
    free(ptr);
    (void)size;
}

// Support for LAI

extern "C" {
    void laihost_free(void *ptr, size_t size) {
        free(ptr);
    }
    void *laihost_malloc(size_t size) {
        void *out = malloc(size);
        //printf("DBG: OUT: 0x%x SIZE: %d\n", out, size);
        return out;
    }
    void *laihost_realloc(void *ptr, size_t new_size, size_t old_size) {
        void *n = malloc(new_size);
        memcpy(n, ptr, old_size);
        free(ptr);
        return n;
    }
}