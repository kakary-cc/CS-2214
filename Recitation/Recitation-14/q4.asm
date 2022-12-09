section .data
num_array:
    dd 45
    dd 99
    dd 8190
    ; ...
    dd 0

section .text
global _start
_start:

; eax: sum
xor eax, eax
; ebx: largest element
xor ebx, ebx
; ecx: cursor
mov ecx, num_array

loop:
    add eax, [ecx]
    cmp [ecx], 0

    cmp ebx, [ecx]
    jbe skip
    mov ebx, [ecx]
    skip:

    je end
    add ecx, 4
    jmp loop

end:
mov eax, 1
mov ebx, 0
int 80h
