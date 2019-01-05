section .data
    formin: db "%d", 0
    formout: db "%08x%08x%08x%08x", 10, 0
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
        je mend

        pinsrd xmm1, ecx, 0

        dec ecx

        cmp ecx, 0
        je insert1
        jmp insertn

        insert1:
            pinsrd xmm1, [one], 2
            jmp mend

        insertn:
            pinsrd xmm1, ecx, 2
            pmuldq xmm0, xmm1 ;todo jezeli overflow, to przejsc na mnozenie 64 bitowych...?
            jmp mloop

    mend:
    pextrd eax, xmm0, 0
    pextrd edx, xmm0, 2

    pinsrd xmm1, edx, 0

    pmuldq xmm0, xmm1

    pextrd [res], xmm0, 0
    pextrd [res+4], xmm0, 1

    push dword [res]
    push dword [res+4]
    push dword [res+8]
    push dword [res+12]
    push formout
    call printf
    add esp, 20

    mov eax, 0
    ret