section .data
numbers_size: dd 8
numbers: dd 10, 34, 55, 106, 44, 0, 45, 400

section .text
global _start
_start:

; eax: sum
xor eax, eax
; ebx: cursor
mov ebx, numbers
; ecx: count
mov ecx, 1
; edx: high 32 bits for dividend, keep 0
xor edx, edx
loop:
    add eax, [ebx]
    cmp ecx, [numbers_size]
    je end
    add ebx, 4
    inc ecx
    jmp loop
end:
div ecx
; set return value
mov ebx, eax
mov eax, 1
int 80h
