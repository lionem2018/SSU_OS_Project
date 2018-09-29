[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

TOTALSECTORCOUNT:   dw  0x02

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
; ?΄? κ°μ κ³μ°?κΈ? ?? μ€?λΉ?
READYTOCALCULATEHASH:
    ; ?΄?κ°μ ?½?΄?¬ λ©λͺ¨λ¦? ?? μ£Όμλ₯? fs ? μ§??€?°? ????₯
    mov ax, 0x1000
    mov fs, ax
    ; 4λ°μ΄?Έ ?΄?λΆ??° κ³μ°?΄?Ό?κΈ? ?λ¬Έμ index ?­? ? ?? di ? μ§??€?°λ₯? 4λ‘? μ΄κΈ°?
    mov di, 0x04

    ; xor ?°?° κ°μ ????₯?  ? μ§??€?° 0?Όλ‘? μ΄κΈ°?
    mov bx, 0x00
    mov cx, 0x00

; xor ?°?°? ?΅?΄ ?΄? κ°μ κ³μ°?? λ£¨ν
CALCULATEHASHLOOP:
    ; 4λ°μ΄?Έλ₯? ? λ°μ΄?Έ?© κ°?? Έ??? xor ?°?° ?€?
    xor bx, word [ fs: di ]
    xor cx, word [ fs: di + 2 ]

    ; ?€? 4λ°μ΄?Έλ₯? κ°?? Έ?€κΈ? ??΄ di 4 μ¦κ??
    add di, 0x04
    
    ; 2?Ή?°(1024λ°μ΄?Έ)λ₯? λͺ¨λ ?°?°?  ?κΉμ?? λ£¨ν ??
    cmp di, 1024
    jb CALCULATEHASHLOOP

; 4λ°μ΄?Έ? ?΄?κ°? μΆλ ₯
; ?¬κΈ°μ? κ³μ°? κ°κ³Ό ????₯??΄?? κ°μ μ°¨μ΄λ₯? μΆλ ₯
COMPAREHASH:
    ; λΉλ?€ ?΄?? ?€ μ£Όμ?? μΆλ ₯?  μ§?? ? di ? μ§??€?°? ????₯
    ; 3λ²μ§Έ μ€? μ²μλΆ??° μΆλ ₯ ??
    mov di, 320
    push CHECKIMAGEMESSAGE
    push 2
    push 0
    call PRINTMESSAGE 
    ; μ²«λ²μ§? λ°μ΄?Έ κ°? λΉκ΅ (μ°¨μ΄ κ³μ°) ? PRINTONEBINARY ?¨?λ₯? ?΅?΄ μΆλ ₯
    ; κ³μ°? ?΄? κ°μ ?? 1λ°μ΄?Έ? ????₯??΄?? ?΄? κ°μ ?? 1λ°μ΄?Έλ₯? λΊ? ? μΆλ ₯
    ; bin ??Ό ?΄ ????₯? ?΄?κ°μ?? fs: 0x00 ~ fs: 0x03? ????₯??΄??

    cmp bx, word [fs: 0x00]
    jne FAIL

    cmp cx, word [fs: 0x02]
    jne FAIL

OKAY:
    push IMGSUCCE
    push 2
    push 10
    call PRINTMESSAGE

    jmp 0x1000:0x0004

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

; ? λ°μ΄?Έ κ°μ 16μ§μ ??λ‘? μΆλ ₯?κΈ? ?? ?¨?
; μΆλ ₯?  κ°μ?? ah ? μ§??€?°? ?΄κ²? ??΄?Ό ?¨
; μΆλ ₯?  ?μΉλ?? ?????΄? di? λ―Έλ¦¬ κ°μ΄ μ§?? ??΄ ??΄?Ό ?¨
PRINTONEHEX:
    push bp       
    mov bp, sp    
            
    push es       
    push si             
    push cx
    push dx

    ; λΉλ?€ ?΄?? ?€ ?? μ£Όμλ₯? es ? μ§??€?°? ????₯
    mov dx, 0xB800
    mov es, dx

    ; ?? 4λΉνΈλ₯? κ°?? Έ??? '0' ??¨
    mov al, ah
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    ; λ§μ½ ?΄?Ή κ°μ΄ 0xAκ°? ???€λ©? μΆλ ₯? ??΄ 0x27λ₯? μΆκ??λ‘? ??¨
    cmp al, '9'
    ja .ISUPPERBIG
    ; 0xAλ₯? ?μ§? ?? ?€λ©? μΆλ ₯ μ½λλ‘? ? ??¨
    jmp .PRINTUPPERHEX

; 0xA ?΄?? κ°μ μΆλ ₯?κΈ? ??΄ μΆκ??λ‘? 0x27λ₯? ??? μ½λ (??€?€μ½λ ?? μΆλ ₯? ??΄)
.ISUPPERBIG: 
    add al, 0x27

; ?? 4λΉνΈλ₯? ??? λ¬Έμλ‘? μΆλ ₯
.PRINTUPPERHEX:
    mov byte [ es: di ], al
    add di, 2

    ;?? 4λΉνΈλ₯? κ°?? Έ??? '0'? ???¬ μΆλ ₯ ?΄? μ½λ? ???? ??Ό
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

    ; ?¨?λ₯? ??΄κΈ? ?  ?€?? ????₯?΄ ??? ?΄?  κ°λ€? ??? €??
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
                    
