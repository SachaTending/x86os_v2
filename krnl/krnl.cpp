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

#define hold asm volatile ("cli");
#define release asm volatile ("sti");

void th1() {
    hold
    printf("th1 started.\n");
    release
    while (1);
}

void th2() {
    hold
    printf("th2 started.\n");
    release
    while (1);
}
void syscall_setup();
extern fs_driver drv;
void krnl_sched() {
    log.info("If you see this, scheduler started succesfully!\n");
    create_task(th1, "th1");
    create_task(th2, "th2");
    log.info("Processes:\n");
    asm volatile ("hlt");
    task_struct *e = root_task;
    while (e != 0)
    {
        log.info("%d. %s EIP: 0x%08x ESP: 0x%08x\n", e->pid, e->name, e->regs.eip, e->regs.esp);
        e = e->next;
    }
    while(1);
}
void page_setup();
void Kernel::Main() {
    log.info("Kernel::Main() IS RUNNING!!!\n");
    log.info("This is prototype kernel. It can be unstable, so run this only in virtual machine\n");
    log.info("IM NOT PROPRIETARY!!! MY SOURCE CODE AVAIBLE AT https://github.com/SachaTending/x86os_v2\n");
    log.info("THIS OS(Including kernel) IS FREE TO USE!!!\n");
    log.info("Setupping paging...\n");
    page_setup();
    log.info("Setupping ACPI...\n");
    ACPI::Setup();
    syscall_setup();
    sched_init();
    create_task(krnl_sched, "kthread");
    sched_start();
    for(;;);
}