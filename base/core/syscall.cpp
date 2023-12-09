#include <idt.hpp>
#include <sched.hpp>

void syscall_int(registers_t *reg) {
    switch (reg->eax) {
        case 0:
            kill_task(getpid());
    }
}

void syscall_setup() {
    IDT::SetStub(syscall_int, 0x81);
}