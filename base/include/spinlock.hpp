#pragma once

typedef struct {
    int lock;
    void *last_acquirer;
} spinlock_t;

static inline bool spinlock_test_and_acq(spinlock_t *lock) {
    return __sync_bool_compare_and_swap(&lock->lock, 0, 1);
}

static inline void spinlock_release(spinlock_t *lock) {
    lock->last_acquirer = 0;
    __atomic_store_n(&lock->lock, 0, __ATOMIC_SEQ_CST);
}


void spinlock_acquire(spinlock_t *lock);
void spinlock_acquire_no_dead_check(spinlock_t *lock);

#define SPINLOCK_INIT {0, 0}