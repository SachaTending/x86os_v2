# libc
base = base/libc
OBJ += $(base)/memcmp.o $(base)/memset.o $(base)/memcpy.o
OBJ += $(base)/atoi.o $(base)/itoa.o
OBJ += $(base)/strcmp.o $(base)/strcpy.o $(base)/strlen.o $(base)/printf.o
OBJ += base/memory/malloc.o base/core/putc.o base/core/logging.o
OBJ += base/core/cxx_support.o
OBJ += base/other/acpi.o