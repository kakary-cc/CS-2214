; The data section is for initialized data
section .data

; The string is simply a byte array, created with db.
; Note that the string includes a newline character (10).
prompt: db "Please enter your name: "
prompt_end:
greet: db "Nice to meet you, "
greet_end:
input: db "abcdefghijklmnopqrst"
input_end:

; The EQU syntax declares a new compile-time symbol,
; similar to a label, but in this case hello_text_len
; designates a difference between addresses, not an address.
prompt_len equ prompt_end-prompt
greet_len equ greet_end-greet
input_len equ input_end-input

; The text section is for code
section .text
global _start

; Special symbol _start identifies entry point
_start:

; Print the prompt
    mov eax, 4                ; select write syscall
    mov ebx, 1                ; file descriptor: stdout
    mov ecx, prompt           ; address of string
    mov edx, prompt_len       ; length of string
    int 80h

; Taking the name
    mov eax, 3                ; select read syscall
    mov ebx, 0                ; file descriptor: stdin
    mov ecx, input            ; address of string
    mov edx, input_len        ; length of string
    int 80h

; Greeting
    mov edx, eax              ; length of string readed
    mov eax, 4
    mov ebx, 1
    mov ecx, greet
    add edx, greet_len        ; length of greet + input
    int 80h

; Done
    mov eax, 1
    mov ebx, 0
    int 80h
