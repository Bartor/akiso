section .data
   format: db "%d", 10, 0

section .text
    global main
    extern printf

; eax <- [sqrt(eax) + 1]
first_after_sqrt:
    push ecx
    push edx
    push ebx

    mov ebx, eax ; store the initial value in ebx
    mov ecx, 1 ; start with 1
    forloop:
        mov eax, ecx
        mul ecx      ; eax <- ecx^2
        cmp eax, ebx
        jg endloop   ; if result is bigger, end
        inc ecx      ; else, increment and go on
        jmp forloop
    endloop:
    mov eax, ecx ; load the result to eax

    pop ebx
    pop edx
    pop ecx
    ret

prime: ; if edi is prime, eax <- 0, else eax <- 1
    push ebx
    push ecx

    cmp edi, 2
    je is_prime

    mov eax, edi
    call first_after_sqrt ; we have one over sqrt(rdi) in eax
    mov ebx, eax ; save it in ebx

    mov ecx, 2
    ploop:
        mov eax, edi

        xor edx, edx
        div ecx ; edx <- eax mod ecx

        cmp edx, 0 ; if it's 0, it's not prime
        je not_prime
        inc ecx
        cmp ecx, ebx
        jle ploop ; until it's less or equal, go on
        jmp is_prime ; if it's not, it's prime
    not_prime:
        mov eax, 1
        pop ecx
        pop ebx
        ret
    is_prime:
        mov eax, 0
        pop ecx
        pop ebx
        ret

main:
    mov ebx, 10000
    mov ecx, 1
    mloop:
        inc ecx
        cmp ecx, ebx
        je mend

        mov edi, ecx
        call prime

        cmp eax, 1
        je mloop

        push ecx
        push dword format
        call printf
        add esp, 4
        pop ecx

        jmp mloop
    mend:
    mov eax, 0
    ret