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

;;;;;;;;;;;;;;;;;;;;;;
; ---clean screen--- ;
;;;;;;;;;;;;;;;;;;;;;;

    mov si,    0
    
.SCREENCLEARLOOP:
    mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A

    add si, 2
    cmp si, 80 * 25 * 2

    jl .SCREENCLEARLOOP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ---print message on screen--- ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; print start message

    push MESSAGE1
    push 0
    push 0
    call PRINTMESSAGE
    add sp, 6

; print current time ---------------

    push MESSAGE2
    push 1
    push 0
    call PRINTMESSAGE
    add sp, 6

    push 1
    push 14
    call PRINTTIME
    add sp, 4

; print message for loading os image

    push IMAGELOADINGMESSAGE
    push 2
    push 0
    call PRINTMESSAGE
    add sp, 6


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; --- load os image from disk--- ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; reset disk

RESETDISK:
    ; call BIOS reset function
    mov ax, 0
    mov dl, 0
    int 0x13

    jc HANDLEDISKERROR

    ; read sector from disk
    mov si, 0x1000
    mov es, si
    mov bx, 0x0000

    mov di, word [ TOTALSECTORCOUNT ]

READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

    ; call BIOS read function
    mov ah, 0x02
    mov al, 0x1
    mov ch, byte [ TRACKNUMBER ]
    mov cl, byte [ SECTORNUMBER ]
    mov dh, byte [ HEADNUMBER ]
    mov dl, 0x00
    int 0x13
    jc HANDLEDISKERROR

    ; calculate address, track, head, sector
    add si, 0x0020
    mov es, si

    mov al, byte [ SECTORNUMBER ]
    add al, 0x01
    mov byte [ SECTORNUMBER ], al
    cmp al, 19
    jl READDATA
    
    xor byte [ HEADNUMBER ], 0x01
    mov byte [ SECTORNUMBER ], 0x01

    cmp byte [ HEADNUMBER ], 0x00
    jne READDATA

READEND:
    push LOADINGCOMPLETEMESSAGE
    push 2
    push 20
    call PRINTMESSAGE
    add sp, 6

    jmp 0x1000:0x0000


;;;;;;;;;;;;;;;;;;;;;;;
; ---Function code--- ;
;;;;;;;;;;;;;;;;;;;;;;;

; print disk error message function

HANDLEDISKERROR:
    push DISKERRORMESSAGE
    push 2
    push 20
    call PRINTMESSAGE

    jmp $

; print message fuction

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

; print current time (HH:MM:SS)

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
    add di, 2

    mov al, ch
    and al, 0x0F
    add al, 0x30
    mov byte [ es: di ], al
    add di, 2

    mov byte [ es: di ], ':'
    add di, 2

    mov al, cl
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    mov byte [ es: di ], al
    add di, 2

    mov al, cl
    and al, 0x0F
    add al, 0x30
    mov byte [ es: di ], al
    add di, 2

    mov byte [ es: di ], ':'
    add di, 2

    mov al, dh
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    mov byte [ es: di ], al
    add di, 2

    mov al, dh
    and al, 0x0F
    add al, 0x30
    mov byte [ es: di ], al
    add di, 2

    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret


;;;;;;;;;;;;;;;;;;;;
; ---other data--- ;
;;;;;;;;;;;;;;;;;;;;

MESSAGE1:    db 'MINT64 OS Boot Loader Start~!!', 0
MESSAGE2:   db 'Current time: ', 0

DISKERRORMESSAGE:   db 'DISK Error~!!', 0
IMAGELOADINGMESSAGE:    db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db 'Complete~!!', 0

SECTORNUMBER:   db 0x02
HEADNUMBER: db 0x00
TRACKNUMBER:    db 0x00

times 510 - ( $ - $$ )    db    0x00

db 0x55
db 0xAA
