; The section is for initialized data
section .data

guardn1: db "Everything "

; Here we allocate a 16-byte buffer.
; The label name "buffer" refers to the address of the beginning of this region
; and end_of_buffer is the address of the first byte after this region
buffer: times 16 db "?"
end_of_buffer:

; We define buffer_len to the difference between the start and end
; address of the buffer, thus it is equal to the length of the buffer
; in bytes
buffer_len equ end_of_buffer - buffer

guardn2: db "is okay!", 10

; The text section if for code
section .text
global _start

; This function prints to the console the decimal representation
; of its 32-bit parameter, which is passed in EAX.
print_int:
    xor ebx, ebx            ; cursor for [buffer] - begins from 0
    cmp eax, 0              ; is "0"?
    jne non_zero
    mov byte [end_of_buffer - 1], 48
    inc ebx
    jmp print
non_zero:
    mov ecx, 10             ; divisor - constant 10
    cmp eax, 0
    je print
    xor edx, edx            ; dividend high 32 bits - constant 0
    div ecx                 ; eax /= 10, edx %= 10
    add edx, 48             ; convert remaindar to ASCII
    ; mov [buffer + ebx], edx
    mov ecx, end_of_buffer  ; tmp
    sub ecx, 1
    sub ecx, ebx
    mov [ecx], dl
    inc ebx                 ; increment cursor
    jmp non_zero
print:
    mov eax, 4              ; write
    ; mov ecx, buffer
    mov ecx, end_of_buffer  ; pointer
    sub ecx, ebx
    mov edx, ebx            ; len
    mov ebx, 1              ; stdout
    int 80h
    ret                     ; return to caller

; This function simply prints the newline character to stdout
; by invoking syscall 4. It overwrites the first character
; of the buffer
print_newline:
    mov eax, 4         ; select syscall 4, write
    mov ebx, 1         ; select file descriptor 1, stdout
    mov ecx, buffer    ; point to data in buffer
    mov edx, 1         ; just print one character
    mov byte [ecx], 10 ; we want the newline character
    int 80h            ; do it
    ret               ; return to caller

; The entrypoint of the program. Just print some numbers
_start:
    ; erase the buffer
    mov al, '?'
    mov ecx, buffer_len
    mov edi, buffer
    rep stosb

    mov eax, 0
    call print_int
    call print_newline

    mov eax, 1
    call print_int
    call print_newline


    mov eax, 34
    call print_int
    call print_newline


    mov eax, 1024
    call print_int
    call print_newline


    mov eax, 9999
    call print_int
    call print_newline


    mov eax, 32777
    call print_int
    call print_newline

    mov eax, 20343343
    call print_int
    call print_newline

    mov eax, 2147483646
    call print_int
    call print_newline

    mov eax, 2
    call print_int
    call print_newline

    mov eax, 4
    mov ebx, 1
    mov ecx, guardn1
    mov edx, 11
    int 80h
    mov eax, 4
    mov ebx, 1
    mov ecx, guardn2
    mov edx, 9
    int 80h

; We're done, tell the OS to kill us
endprogram:
    mov eax, 1                ; select exit syscall
    mov ebx, 0                ; return value
    int 80h                   ; do it
