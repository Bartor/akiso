; to chyba mialo dzialac inaczej

section .data
    formin: db "%d", 0
    formout: db "%d", 10, 0
    one: dd 0x1

section .bss
    res resb 16

section .text
    global main
    extern printf
    extern scanf

main:
    push res
    push formin
    call scanf
    add esp, 8

    mov ecx, dword [res]

    pinsrd xmm0, ecx, 0
    dec ecx
    pinsrd xmm0, ecx, 2

    mloop:
        dec ecx
        cmp ecx, 0
        je insert11

        pinsrd xmm1, ecx, 0

        dec ecx

        cmp ecx, 0
        je insert1
        jmp insertn

        insert11:
            pinsrd xmm1, [one], 0

        insert1:
            pinsrd xmm1, [one], 2
            jmp mend

        insertn:
            pinsrd xmm1, ecx, 2
            pmuldq xmm0, xmm1
            jmp mloop

    mend:
    pextrd eax, xmm0, 0
    pextrd edx, xmm0, 2

    mul edx

    push eax
    push formout
    call printf
    add esp, 8

    mov eax, 0
    ret