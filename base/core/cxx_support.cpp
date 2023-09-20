#include <libc.hpp>

void *operator new(size_t size) {
    return malloc(size);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) {
    free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}

// Support for LAI

extern "C" {
    void laihost_free(void *ptr, size_t size) {
        free(ptr);
    }
    void *laihost_malloc(size_t size) {
        return malloc(size);
    }
    void *laihost_realloc(void *ptr, size_t new_size, size_t old_size) {
        void *n = malloc(new_size);
        memcpy(n, ptr, old_size);
        free(ptr);
        return n;
    }
}