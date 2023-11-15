# Target for LAI(Lightweit AML Intreptitator)
lai = lai
OBJ += $(lai)/core/error.o $(lai)/core/eval.o $(lai)/core/exec-operand.o $(lai)/core/libc.o $(lai)/core/ns.o $(lai)/core/object.o 
OBJ += $(lai)/core/opregion.o $(lai)/core/os_methods.o $(lai)/core/variable.o $(lai)/core/vsnprintf.o $(lai)/core/exec.o

OBJ += $(lai)/helpers/sci.o $(lai)/helpers/pc-bios.o $(lai)/helpers/resource.o $(lai)/helpers/pm.o $(lai)/helpers/pci.o
OBJ += $(lai)/drivers/timer.o

CFLAGS += -I $(lai)/include
