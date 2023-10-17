#include <krnl.hpp>
#include <libc.hpp>
#include <logger.hpp>
#include <acpi.hpp>
#include <sched.hpp>
#include <common.hpp>

static logger log("Kernel");

void th1() {
    while (1);
}

void th2() {
    while (1);
}

void krnl_sched() {
    log.info("Im in schedule.\n");
    create_task(th1, "th1");
    create_task(th2, "th2");
    log.info("Processes:\n");
    asm volatile ("hlt");
    task_struct *e = root_task;
    while (e != 0)
    {
        log.info("%d. %s EIP: 0x%x ESP: 0x%x\n", e->pid, e->name, e->regs.eip, e->regs.esp);
        e = e->next;
    }
    
    while (1);
}

void Kernel::Main() {
    log.info("Kernel::Main() IS RUNNING YAY\n");
    ACPI::Setup();
    sched_init();
    create_task(krnl_sched, "kthread");
    sched_start();
    for(;;);
}