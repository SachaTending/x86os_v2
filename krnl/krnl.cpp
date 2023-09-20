#include <krnl.hpp>
#include <libc.hpp>
#include <logger.hpp>
#include <acpi.hpp>

static logger log("Kernel");
void Kernel::Main() {
    log.info("Kernel::Main() IS RUNNING YAY\n");
    ACPI::Setup();
}