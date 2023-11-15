#include <krnl.hpp>
#include <libc.hpp>
#include <logger.hpp>
#include <acpi.hpp>
#include <sched.hpp>
#include <common.hpp>
#include <spinlock.hpp>
#include <vfs_drive_mgr.hpp>
#include <vfs.hpp>

spinlock_t sp = SPINLOCK_INIT;

static logger log("Kernel");

void th1() {
    while (1);
}

void th2() {
    while (1);
}
extern fs_driver drv;
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
    size_t i = fs_driver_register(drv);
    log.info("TMPFS Drive registered: %u\n", i);
    bool m = vfs_mount(i, (char *)"/");
    log.info("TMPFS Is mounted?: %d\n", m);
    if (m) {
        dir_report r = iterate_dir(i, (char *)"/");
        log.info("%s\n", r.entries->name);
        free((void *)r.entries);
    }
    while (1);
}

void Kernel::Main() {
    log.info("Kernel::Main() IS RUNNING YAY\n");
    log.info("0x%x\n",  __builtin_return_address(0));
    ACPI::Setup();
    sched_init();
    create_task(krnl_sched, "kthread");
    sched_start();
    for(;;);
}