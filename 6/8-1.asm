section .data
    formin: db "%lf", 0
    formout: db "%lf", 10, 0

section .bss
    input resb 8

section .text
    global main
    extern printf
    extern scanf

main:
    push input
    push formin
    call scanf
    add esp, 8

    finit
    fld qword [input] ; stack = x
    fldl2e ; stack = log2e x
    fyl2x ; stack = x*log2e
    f2xm1 ; stack = 2^(x*log2e)-1
    fld1 ; stack = 1 2^(x*log2e)-1
    faddp ; stack = 2^(x*log2e)
    fld qword [input] ; stack = x 2^(x*log2e)
    fchs ; stack = -x 2^(x*log2e)
    fldl2e ; stack = log2e -x 2^(x*log2e)
    fyl2x ; stack = -x*log2e 2^(x*log2e)
    f2xm1 ; stack = 2^(-x*log2e)-1 2^(x*log2e)
    fld1 ; stack = 1 2^(-x*log2e)-1 2^(x*log2e)
    faddp ; stack = 2^(-x*log2e) 2^(x*log2e)
    fsubp ; stack =  2^(-x*log2e)-2^(x*log2e)
    fchs ; stack =  2^(x*log2e)-2^(-x*log2e)
    fld1
    fld1
    faddp ; stack = 2 2^(x*log2e)-2^(-x*log2e)
    fdivp ; stack = (2^(x*log2e)-2^(-x*log2e))/2

    fst qword [input]

    push dword [input+4]
    push dword [input]
    push formout
    call printf
    add esp, 12

    xor eax, eax
    ret