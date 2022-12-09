; Assemble and link like this:
;     nasm -f elf -gstabs pr.asm
;     ld -o pr -m elf_i386 pr.o
; Run like this:
;     ./pr

; The section is for initialized data
section .data

guardn1: db "Everything "

; Here we allocate a 16-byte buffer.
; The label name "buffer" refers to the address of the beginning of this region
; and end_of_buffer is the address of the first byte after this region
buffer: times 16 db "?"
end_of_buffer:

; We define bufer_len to the difference between the start and end
; address of the buffer, thus it is equal to the length of the buffer
; in bytes
buffer_len equ end_of_buffer - buffer

guardn2: db "is okay!",10

; The text section if for code
section .text
global _start

; This function prints to the console the decimal representation
; of its 32-bit parameter, which is passed in EAX.
print_int:

    ;; TODO Your code goes here
    ;; Your code should examine the 32-bit unsigned integer
    ;; in EAX, and convert it into an ASCII string, stored
    ;; in the 16-byte buffer, and then print it using 
    ;; the write syscall.

    ret                     ; return to caller

; This function simply prints the newline character to stdout
; by invoking syscall 4. It overwrites the first character
; of the buffer
print_newline:
    mov eax,4         ; select syscall 4, write
    mov ebx,1         ; select file descriptor 1, stdout
    mov ecx,buffer    ; point to data in buffer
    mov edx,1         ; just print one character
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

    mov eax,4
    mov ebx,1
    mov ecx,guardn1
    mov edx,11
    int 80h
    mov eax,4
    mov ebx,1
    mov ecx,guardn2
    mov edx,9
    int 80h

; We're done, tell the OS to kill us
endprogram:
    mov eax,1                 ; select exit syscall
    mov ebx,0                 ; return value
    int 80h                   ; do it
