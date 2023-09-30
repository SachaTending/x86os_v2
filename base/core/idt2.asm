[GLOBAL idt_flush]
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

%macro isr_err_stub 1
isr_stub_%+%1:
    cli
    push byte %1
    jmp exception_handler
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    cli
    push byte 0
    push byte %1
    jmp exception_handler
%endmacro

    isr_no_err_stub 0
    isr_no_err_stub 1
    isr_no_err_stub 2
    isr_no_err_stub 3
    isr_no_err_stub 4
    isr_no_err_stub 5
    isr_no_err_stub 6
    isr_no_err_stub 7
    isr_err_stub    8
    isr_no_err_stub 9
    isr_err_stub    10
    isr_err_stub    11
    isr_err_stub    12
    isr_err_stub    13
    isr_err_stub    14
    isr_no_err_stub 15
    isr_no_err_stub 16
    isr_err_stub    17
    isr_no_err_stub 18
    isr_no_err_stub 19
    isr_no_err_stub 20
    isr_no_err_stub 21
    isr_no_err_stub 22
    isr_no_err_stub 23
    isr_no_err_stub 24
    isr_no_err_stub 25
    isr_no_err_stub 26
    isr_no_err_stub 27
    isr_no_err_stub 28
    isr_no_err_stub 29
    isr_err_stub    30
    isr_no_err_stub 31

%assign i 32
%rep 64
isr_no_err_stub i
%assign i i+1
%endrep

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    64 
    dd isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1 
%endrep

extern irq_handler

; This is our common IRQ stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
exception_handler:
    pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds               ; Lower 16-bits of eax = ds.
    push eax                 ; Save the data segment descriptor

    mov ax, 0x10             ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push esp    	     ; Push a pointer to the current top of stack - this becomes the registers_t* parameter.
    call irq_handler         ; Call into our C code.
    add esp, 4		     ; Remove the registers_t* parameter.

    pop ebx                  ; Reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx

    popa                     ; Pops edi,esi,ebp...
    add esp, 8               ; Cleans up the pushed error code and pushed ISR number
    iret                     ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
.end: