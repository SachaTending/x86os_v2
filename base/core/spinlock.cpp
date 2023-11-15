#include <spinlock.hpp>
#include <logger.hpp>
#include <stdint.h>
/*
Whole source code(with some changes) are taken from lyre
Original author of this src is mintsuki(and contributors)
*/

static logger log("spinlock");

__attribute__((noinline)) void spinlock_acquire(spinlock_t *lock) {
    volatile size_t deadlock_counter = 0;
    for (;;) {
        if (spinlock_test_and_acq(lock)) {
            break;
        }
        if (++deadlock_counter >= 100000000) {
            goto deadlock;
        }
        asm volatile ("pause");
    }
    lock->last_acquirer = __builtin_return_address(0);
    return;

deadlock:
    log.critical("Deadlock occurred at %llx on lock %llx whose last acquirer was %llx\n", __builtin_return_address(0), lock, lock->last_acquirer);
}