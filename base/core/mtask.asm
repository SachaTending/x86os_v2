global task_main

task_main:
    mov esp, eax
    call ebx
    mov ebx, eax
    xor eax, eax
    int 129
    jmp $