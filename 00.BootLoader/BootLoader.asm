[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

TOTALSECTORCOUNT:   dw  0x02
KERNEL32SECTORCOUNT: dw 0x02

START:
    mov ax, 0x07C0  
    mov ds, ax      
    mov ax, 0xB800  
    mov es, ax      

    mov ax, 0x0000  
    mov ss, ax      
    mov sp, 0xFFFE  
    mov bp, 0xFFFE  

    mov si,    0                    
        
.SCREENCLEARLOOP:                   
    mov byte [ es: si ], 0          
                                    
    mov byte [ es: si + 1 ], 0x0A   

    add si, 2                       

    cmp si, 80 * 25 * 2     
                            
    jl .SCREENCLEARLOOP     
                        
    push MESSAGE1               
    push 0                      
    push 0                      
    call PRINTMESSAGE           
    add  sp, 6                  
    
;    push MESSAGE2            
;    push 1                                            
;    push 0                                            
;    call PRINTMESSAGE                                     
;    add  sp, 6                  

;    mov ax, 0xB800
;    mov es, ax

;    mov di, 160
;    add di, 28

;    mov ah, 2h
;    int 1Ah

;    mov al, ch
;    and al, 0xF0
;    shr al, 0x4
;    add al, 0x30
;    mov byte [ es: di ], al
;    add di, 2

;    mov al, ch
;    and al, 0x0F
;    add al, 0x30
;    mov byte [ es: di ], al
;    add di, 2

;    mov byte [ es: di ], ':'
;    add di, 2

;    mov al, cl
;    and al, 0xF0
;    shr al, 0x4
;    add al, 0x30
;    mov byte [ es: di ], al
;    add di, 2

;    mov al, cl
;    and al, 0x0F
;    add al, 0x30
;    mov byte [ es: di ], al
;    add di, 2

;    mov byte [ es: di ], ':'
;    add di, 2

;    mov al, dh
;    and al, 0xF0
;    shr al, 0x4
;    add al, 0x30
;    mov byte [ es: di ], al
;    add di, 2

;    mov al, dh
;    and al, 0x0F
;    add al, 0x30
;    mov byte [ es: di ], al
;    add di, 2

    push IMAGELOADINGMESSAGE         
    push 1                           
    push 0                           
    call PRINTMESSAGE                
    add  sp, 6                       
        
RESETDISK: 
    mov ax, 0
    mov dl, 0              
    int 0x13     
    jc  HANDLEDISKERROR
        
    mov si, 0x1000                  
                                    
    mov es, si                      
    mov bx, 0x0000                  
                                    
    mov di, word [ TOTALSECTORCOUNT ]

READDATA:   
    cmp di, 0              
    je  READEND            
    sub di, 0x1            

    mov ah, 0x02                        
    mov al, 0x1                         
    mov ch, byte [ TRACKNUMBER ]        
    mov cl, byte [ SECTORNUMBER ]       
    mov dh, byte [ HEADNUMBER ]         
    mov dl, 0x00                        
    int 0x13                            
    jc HANDLEDISKERROR                  
    
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
    
    add byte [ TRACKNUMBER ], 0x01      
    jmp READDATA                       

READEND:
    push LOADINGCOMPLETEMESSAGE    
    push 1                      
    push 9                    
    call PRINTMESSAGE              
    add  sp, 6 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ?��?�� 값을 계산?���? ?��?�� �?�?
READYTOCALCULATEHASH:
    ; ?��?��값을 ?��?��?�� 메모�? ?��?�� 주소�? fs ?���??��?��?�� ????��
    mov ax, 0x1000
    mov fs, ax
    ; 4바이?�� ?��?���??�� 계산?��?��?���? ?��문에 index ?��?��?�� ?��?�� di ?���??��?���? 4�? 초기?��
    mov di, 0x04

    ; xor ?��?�� 값을 ????��?�� ?���??��?�� 0?���? 초기?��
    mov bx, 0x00
    mov cx, 0x00

; xor ?��?��?�� ?��?�� ?��?�� 값을 계산?��?�� 루프
CALCULATEHASHLOOP:
    ; 4바이?���? ?�� 바이?��?�� �??��??? xor ?��?�� ?��?��
    xor bx, word [ fs: di ]
    xor cx, word [ fs: di + 2 ]

    ; ?��?�� 4바이?���? �??��?���? ?��?�� di 4 증�??
    add di, 0x04
    
    ; 2?��?��(1024바이?��)�? 모두 ?��?��?�� ?��까�?? 루프 ?��?��
    cmp di, 1024
    jb CALCULATEHASHLOOP

; 4바이?��?�� ?��?���? 출력
; ?��기서?�� 계산?�� 값과 ????��?��?��?��?�� 값의 차이�? 출력
COMPAREHASH:
    ; 비디?�� ?��?��?��?�� 주소?��?�� 출력?�� �??��?�� di ?���??��?��?�� ????��
    ; 3번째 �? 처음�??�� 출력 ?��?��
    mov di, 320
    push CHECKIMAGEMESSAGE
    push 2
    push 0
    call PRINTMESSAGE 
    ; 첫번�? 바이?�� �? 비교 (차이 계산) ?�� PRINTONEBINARY ?��?���? ?��?�� 출력
    ; 계산?�� ?��?�� 값의 ?��?�� 1바이?��?�� ????��?��?��?��?�� ?��?�� 값의 ?��?�� 1바이?���? �? ?�� 출력
    ; bin ?��?�� ?�� ????��?�� ?��?��값�?? fs: 0x00 ~ fs: 0x03?�� ????��?��?��?��?��

    cmp bx, word [fs: 0x00]
    ;jne FAIL

    cmp cx, word [fs: 0x02]
    ;jne FAIL

OKAY:
    push IMGSUCCE
    push 2
    push 10
    call PRINTMESSAGE

    jmp 0x1000:0x0000

FAIL:
    mov di, 480

    push IMGFAIL
    push 2
    push 10
    call PRINTMESSAGE

    mov ah, byte[fs: 0x00]
    call PRINTONEHEX

    mov ah, byte[fs: 0x01]
    call PRINTONEHEX

    mov ah, byte[fs: 0x02]
    call PRINTONEHEX

    mov ah, byte[fs: 0x03]
    call PRINTONEHEX

    mov di, 640

    mov ah, bl
    call PRINTONEHEX
    
    mov ah, bh
    call PRINTONEHEX

    mov ah, cl
    call PRINTONEHEX

    mov ah, ch
    call PRINTONEHEX

    jmp $

; ?�� 바이?�� 값을 16진수 ?��?���? 출력?���? ?��?�� ?��?��
; 출력?�� 값�?? ah ?���??��?��?�� ?���? ?��?��?�� ?��
; 출력?�� ?��치�?? ?��????��?�� di?�� 미리 값이 �??��?��?�� ?��?��?�� ?��
PRINTONEHEX:
    push bp       
    mov bp, sp    
            
    push es       
    push si             
    push cx
    push dx

    ; 비디?�� ?��?��?��?�� ?��?�� 주소�? es ?���??��?��?�� ????��
    mov dx, 0xB800
    mov es, dx

    ; ?��?�� 4비트�? �??��??? '0' ?��?��
    mov al, ah
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    ; 만약 ?��?�� 값이 0xA�? ?��?��?���? 출력?�� ?��?�� 0x27�? 추�??�? ?��?��
    cmp al, '9'
    ja .ISUPPERBIG
    ; 0xA�? ?���? ?��?�� ?���? 출력 코드�? ?��?��?��
    jmp .PRINTUPPERHEX

; 0xA ?��?��?�� 값을 출력?���? ?��?�� 추�??�? 0x27�? ?��?��?�� 코드 (?��?��?��코드 ?��?�� 출력?�� ?��?��)
.ISUPPERBIG: 
    add al, 0x27

; ?��?�� 4비트�? ?��?��?�� 문자�? 출력
.PRINTUPPERHEX:
    mov byte [ es: di ], al
    add di, 2

    ;?��?�� 4비트�? �??��??? '0'?�� ?��?��?�� 출력 ?��?�� 코드?�� ?��??? ?��?��
    mov al, ah
    and al, 0x0F
    add al, 0x30
    cmp al, '9'
    ja .ISLOWERBIG
    jmp .PRINTLOWERHEX

.ISLOWERBIG: 
    add al, 0x27

.PRINTLOWERHEX:
    mov byte [ es: di ], al
    add di, 2

    ; ?��?���? ?��?���? ?�� ?��?��?�� ????��?�� ?��?��?�� ?��?�� 값들?�� ?��?��?��?��?��
    pop dx      
    pop cx            
    pop si      
    pop es
    pop bp      
    ret 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
HANDLEDISKERROR:
    push DISKERRORMESSAGE  
    push 1                
    push 9          
    call PRINTMESSAGE      
    
    jmp $                  

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
    
MESSAGE1:    db 'Start', 0                                                   
;MESSAGE2:   db 'Current time:', 0

DISKERRORMESSAGE:       db  'Err', 0
IMAGELOADINGMESSAGE:    db  'Loading:', 0
LOADINGCOMPLETEMESSAGE: db  'Compl', 0

CHECKIMAGEMESSAGE:  db 'Checking:', 0
IMGSUCCE: db 'OK', 0
IMGFAIL: db 'NO',0

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    
    
times 510 - ( $ - $$ )    db    0x00                                        

db 0x55             
db 0xAA             
                    
