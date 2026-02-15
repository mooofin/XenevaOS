[BITS 64]
[GLOBAL _start]
[EXTERN _AuMain]

SECTION .text

_start:
    ; logic: The bootloader (Microsoft x64 ABI) passes arguments in RCX, RDX, R8, R9.
    ;        The Kernel (System V ABI) expects arguments in RDI, RSI, RDX, RCX.
    ;        We need to move the first argument (KERNEL_BOOT_INFO*) from RCX to RDI.

    mov rdi, rcx    ; Move KERNEL_BOOT_INFO* to 1st SysV arg register
    
    ; Ensure stack is 16-byte aligned before call
    and rsp, -16

    call _AuMain

    cli
.hang:
    hlt
    jmp .hang
