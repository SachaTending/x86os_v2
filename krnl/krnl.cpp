#include <krnl.hpp>
#include <libc.hpp>
#include <logger.hpp>
#include <acpi.hpp>
#include <sched.hpp>
#include <common.hpp>

static logger log("Kernel");

void krnl_sched() {
    log.info("Im in schedule.\n");
    while (1);
}

void Kernel::Main() {
    log.info("Kernel::Main() IS RUNNING YAY\n");
    //ACPI::Setup();
    sched_init();
    create_task(krnl_sched, "kthread");
    sched_start();
    for(;;);
}