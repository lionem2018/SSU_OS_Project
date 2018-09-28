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
    push 20                        
    call PRINTMESSAGE              
    add  sp, 6 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;READHASHVALUE:
;    mov ax, 0x1000                 
;    mov fs, ax

;    mov bx, word [ fs: 0x00 ]
;    mov cx, word [ fs: 0x02 ]

;PRINTBINARIES:
;    mov di, 320;
;    mov ah, bl
;    call PRINTONEBINARY

;    mov ah, bh
;    call PRINTONEBINARY

;    mov ah, cl
;    call PRINTONEBINARY
;    mov ah, ch
;    call PRINTONEBINARY

READYTOCALCULATEHASH:
    mov ax, 0x1000
    mov fs, ax
    mov di, 0x04

    mov bx, 0x00
    mov cx, 0x00

CALCULATEHASHLOOP:

    xor bl, byte [ fs: di ]
    xor bh, byte [ fs: di + 1 ]
    xor cl, byte [ fs: di + 2 ]
    xor ch, byte [ fs: di + 3 ]

    add di, 0x04
    
    cmp di, 1024
    jb CALCULATEHASHLOOP

PRINTBINARIES:
    mov di, 320

    sub bl, byte[ fs: 0x00]
    mov ah, bl
    call PRINTONEBINARY

    sub bh, byte[ fs: 0x01]
    mov ah, bh
    call PRINTONEBINARY

    sub cl, byte[ fs: 0x02]
    mov ah, cl
    call PRINTONEBINARY

    sub ch, byte[ fs: 0x03]
    mov ah, ch
    call PRINTONEBINARY

    jmp $
    ;jmp 0x1000:0x0000
    

PRINTONEBINARY:
    push bp       
    mov bp, sp    
            
    push es       
    push si             
    push cx
    push dx

    mov dx, 0xB800
    mov es, dx

    mov al, ah
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    cmp al, '9'
    ja .isUpperBig
    jmp .printUpperBinary

.isUpperBig: 
    add al, 0x27

.printUpperBinary:
    mov byte [ es: di ], al
    add di, 2

    mov al, ah
    and al, 0x0F
    add al, 0x30
    cmp al, '9'
    ja .isLowerBig
    jmp .printLowerBinary

.isLowerBig: 
    add al, 0x27

.printLowerBinary:
    mov byte [ es: di ], al
    add di, 2

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
    push 20                
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
    
MESSAGE1:    db 'MINT64 OS Boot Loader Start~!!', 0                                                   
;MESSAGE2:   db 'Current time: ', 0

DISKERRORMESSAGE:       db  'DISK Error~!!', 0
IMAGELOADINGMESSAGE:    db  'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete~!!', 0

CHECKIMAGEMESSAGE:  db 'OS Image Checking...', 0

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    
    
times 510 - ( $ - $$ )    db    0x00                                        

db 0x55             
db 0xAA             
                    