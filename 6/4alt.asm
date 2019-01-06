; wersja alternatywna, bo mozna to zadanie rozumiec na dwa sposoby

section .data
    debug: db "eax=%x, edx=%x", 10, 0
    formin: db "%d", 0
    formout: db "%08x%08x%08x%08x", 10, 0
    one: dd 0x1

section .bss
    input resd 1
    result resd 4

section .text
    global main
    extern printf
    extern scanf

main:
    push input
    push formin
    call scanf
    add esp, 8

    mov ecx, dword [input]

    movd xmm0, ecx ; xmm0 = counter
    dec ecx
    movd xmm1, ecx ; xmm1 = counter - 1
    mloop: ; dh = data high, dl = data low
        movdqa xmm2, xmm0 ; xmm2 = xmm0 (_ dh _ dl)
        psrldq xmm2, 64 ; (_ _ _ dh)
        pmuludq xmm0, xmm1 ; xmm0 = (dh * b dl * b)
        pmuludq xmm2, xmm1 ; xmm2 = (_ _ dh * b)

        pslldq xmm0, 64 ; xmm0 = (dl * b _ _)
        psrldq xmm0, 32 ; xmm0 = (_ dl * b _)
        pslldq xmm2, 64 ; xmm2 = (dh * b _ _)

        pinsrd xmm0, [one], 3
        pinsrd xmm2, [one], 1
        pmaddwd xmm2, xmm0 ; skomplikowane mnożenie, ale powinno być ok

        pshufd xmm0, xmm2, 0b00100001 ; rozmieszanie wyników z xmm2 w xmm0

        dec ecx
        cmp ecx, 1
        je mend

        movd xmm1, ecx ; aktualizacja xmm1

        jmp mloop
    mend:
        movdqa oword [result], xmm0

        push dword [result]
        push dword [result+4]
        push dword [result+8]
        push dword [result+12]
        push formout
        call printf
        add esp, 20

    mov eax, 0
    ret