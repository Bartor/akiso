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

prime: ; if rdi is prime, eax <- 0, else eax <- 1
    push ebx

    mov eax, edi
    call first_after_sqrt ; we have one over sqrt(rdi) in eax
    mov ebx, eax ; save it in ebx

    mov ecx, 2
    ploop:
        mov eax, edi
        div ecx ; edx <- eax mod ecx
        cmp edx, 0
        je not_prime
        inc ecx
        cmp ecx, ebx
        jle ploop
        jmp is_prime
    not_prime:
        mov eax, 1
        pop ebx
        ret
    is_prime:
        mov eax, 0
        pop ebx
        ret

main:
    ;mov eax, 74
    ;call first_after_sqrt
    mov edi, 9
    call prime

    push eax
    push dword format
    call printf
    add esp, 8

    mov eax, 0
    ret