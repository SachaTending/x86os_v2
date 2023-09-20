# Files and dirs
init/ - Init files
     main.cpp - Init main code
     init.asm - Pre-init code and multiboot header

base/
     include/ - Include files
     memory/ - Memory management stuff
            malloc.cpp - malloc and free
     libc/ - Simple and basic libc implementation
     drivers/ - Drivers
     core/ - Core files, needed for kernel to work
          psfdraw.cpp - Draws text using psf font
          putc.cpp - implemintation of putc

iso/ - ISO root files, also contains bootloader files

targets/ - Makefile files, typically these files adds target objects files

docs/ - Documentation

link.ld - LinkerScript.
Makefile - Makefile for kernel