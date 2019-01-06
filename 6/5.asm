bits 16
org 0x7c00

boot:
    mov ax, 0x2401
    int 0x15
    mov ax, 0x13        ; wlaczenie trybu graficznego 320x200
    int 0x10
    cli
    lgdt [gdt_pointer]  ; ustawienie tablicy GDT
    mov eax, cr0        ; wlaczenie trybu chronionego
    or eax,0x1
    mov cr0, eax
    jmp CODE_SEG:fract
gdt_start:              ; tablica GDT
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:
gdt_pointer:
    dw gdt_end - gdt_start
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

bits 32
.data:
    CntrA: dd 0
    CntrB: dd 0

    X: dw 0
    Y: dw 0

fract:
    mov di, 0
    mov dword [CntrB], -510*256
    mov word [X], 0

    @@LoopHoriz:
        mov dword [CntrB], -270*256
        mov word [Y], 200

    @@LoopVert:
        xor ecx, ecx
        xor edx, edx
        mov si, 32-1

    @@LoopFractal:
        mov eax, ecx
        imul eax, eax
        mov ebx, edx
        imul ebx, ebx
        sub eax, ebx
        add eax, dword [CntrA]
        mov ebx, ecx
        imul ebx, edx
        sal ebx, 1
        add ebx, dword [CntrB]
        sar eax, 8
        sar ebx, 8
        mov ecx, eax
        mov edx, ebx
        imul eax, eax
        imul ebx, ebx
        add eax, ebx
        sar eax, 8
        cmp eax, 1024
        jg Break
        dec si
        jnz @@LoopFractal

    Break:
        mov ax, si
        mov byte [di], al

        add dword [CntrB], 720
        add di, 320
        dec word [Y]
        jnz @@LoopVert
        add dword [CntrA], 568
        inc word [X]
        mov di, 0
        add di, word [X]
        cmp word [X], 320
        jnz @@LoopHoriz
        ret
    halt:
        cli
        hlt

times 510 - ($-$$) db 0
dw 0xaa55

