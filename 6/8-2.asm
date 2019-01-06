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
    fld1
    fld qword [input]
    fld1
    fld qword [input]
    fld qword [input] ; stack = x x 1 x 1
    fmulp ; stack = x*x 1 x 1
    faddp ; stack = x*x+1 x 1
    fsqrt ; stack = sqrt(x*x+1) x 1
    faddp ; stack = sqrt(x*x+1)+x 1
    fyl2x ; stack = 1*log2(sqrt(x*x+1)+x)
    fldl2e ; stack = log2(e) log2(sqrt(x*x+1)+x)
    fdivp ; stack =  log2(sqrt(x*x+1)+x)/log2(e)

    fst qword [input]

    push dword [input+4]
    push dword [input]
    push formout
    call printf
    add esp, 12

    xor eax, eax
    ret