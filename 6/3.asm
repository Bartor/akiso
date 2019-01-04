section .data
    formatin: db "%lf %c %lf", 0
    formatout: db "= %lf", 10, 0

section .bss
    float1 resb 8
    float2 resb 8
    op resb 1

section .text
    global main
    extern printf
    extern scanf

main:
    push ebx

    push float2
    push op
    push float1
    push formatin
    call scanf
    add esp, 16

    finit
    fld qword [float1]

    cmp byte [op], '+'
    je addi

    cmp byte [op], '*'
    je mult

    cmp byte [op], '/'
    je divi

    cmp byte [op], '-'
    je subt

    addi:
        fadd qword [float2]
        jmp mend

    mult:
        fmul qword [float2]
        jmp mend

    divi:
        fdiv qword [float2]
        jmp mend

    subt:
        fsub qword [float2]
        jmp mend

    mend:
        fst qword [float1]
        push dword [float1+4]
        push dword [float1]
        push formatout
        call printf
        add esp, 12

        pop ebx

        mov eax, 0
        ret