[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

TOTALSECTORCOUNT:   dw 1024

START:
    mov ax, 0x07C0
    mov ds, ax
    mov ax, 0xB800
    mov es, ax

    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE


; ---clean screen---

    mov si,    0
    
.SCREENCLEARLOOP:
    mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A

    add si, 2
    cmp si, 80 * 25 * 2

    jl .SCREENCLEARLOOP


; ---print message on screen---

    push MESSAGE1
    push 0
    push 0
    call PRINTMESSAGE
    add sp, 6

    push MESSAGE2
    push 1
    push 0
    call PRINTMESSAGE
    add sp, 6

    push 1
    push 14
    call PRINTTIME
    add sp, 4

    jmp $


; ---Function code---

PRINTMESSAGE:
    push bp
    mov bp, sp

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800
    mov es, ax

    mov ax, word [ bp + 6 ]
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [ bp + 4 ]
    mov si, 2
    mul si
    add di, ax

    mov si, word [ bp + 8 ]

.MESSAGELOOP:
    mov cl, byte [ si ]
    cmp cl, 0
    je .MESSAGEEND

    mov byte [ es: di ], cl
    add si, 1
    add di, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

PRINTTIME:
    push bp
    mov bp, sp

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800
    mov es, ax

    mov ax, word [ bp + 6 ]
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [ bp + 4 ]
    mov si, 2
    mul si
    add di, ax

    mov ah, 2h
    int 1Ah

    mov ah, 0x0E
    mov al, ch
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    mov byte [ es: di ], al
    add si, 1
    add di, 2

    mov al, ch
    and al, 0x0F
    add al, 0x30
    mov byte [ es: di ], al
    add si, 1
    add di, 2

    mov byte [ es: di ], ':'
    add si, 1
    add di, 2

    mov al, cl
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    mov byte [ es: di ], al
    add si, 1
    add di, 2

    mov al, cl
    and al, 0x0F
    add al, 0x30
    mov byte [ es: di ], al
    add si, 1
    add di, 2

    mov byte [ es: di ], ':'
    add si, 1
    add di, 2

    mov al, dh
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    mov byte [ es: di ], al
    add si, 1
    add di, 2

    mov al, dh
    and al, 0x0F
    add al, 0x30
    mov byte [ es: di ], al
    add si, 1
    add di, 2

    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret



MESSAGE1:    db 'MINT64 OS Boot Loader Start~!!', 0
MESSAGE2:   db 'Current time: ', 0

times 510 - ( $ - $$ )    db    0x00

db 0x55
db 0xAA
