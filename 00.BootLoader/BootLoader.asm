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
; ?ï¥?ãú Í∞íÏùÑ Í≥ÑÏÇ∞?ïòÍ∏? ?úÑ?ïú Ï§?Îπ?
READYTOCALCULATEHASH:
    ; ?ï¥?ãúÍ∞íÏùÑ ?ùΩ?ñ¥?ò¨ Î©îÎ™®Î¶? ?ãú?ûë Ï£ºÏÜåÎ•? fs ?†àÏß??ä§?Ñ∞?óê ????û•
    mov ax, 0x1000
    mov fs, ax
    ; 4Î∞îÏù¥?ä∏ ?ù¥?õÑÎ∂??Ñ∞ Í≥ÑÏÇ∞?ï¥?ïº?ïòÍ∏? ?ïåÎ¨∏Ïóê index ?ó≠?ï†?ùÑ ?ïò?äî di ?†àÏß??ä§?Ñ∞Î•? 4Î°? Ï¥àÍ∏∞?ôî
    mov di, 0x04

    ; xor ?ó∞?Ç∞ Í∞íÏùÑ ????û•?ï† ?†àÏß??ä§?Ñ∞ 0?úºÎ°? Ï¥àÍ∏∞?ôî
    mov bx, 0x00
    mov cx, 0x00

; xor ?ó∞?Ç∞?ùÑ ?Üµ?ï¥ ?ï¥?ãú Í∞íÏùÑ Í≥ÑÏÇ∞?ïò?äî Î£®ÌîÑ
CALCULATEHASHLOOP:
    ; 4Î∞îÏù¥?ä∏Î•? ?ïú Î∞îÏù¥?ä∏?î© Í∞??†∏??? xor ?ó∞?Ç∞ ?ã§?ñâ
    xor bx, word [ fs: di ]
    xor cx, word [ fs: di + 2 ]

    ; ?ã§?ùå 4Î∞îÏù¥?ä∏Î•? Í∞??†∏?ò§Í∏? ?úÑ?ï¥ di 4 Ï¶ùÍ??
    add di, 0x04
    
    ; 2?Ñπ?Ñ∞(1024Î∞îÏù¥?ä∏)Î•? Î™®Îëê ?ó∞?Ç∞?ï† ?ïåÍπåÏ?? Î£®ÌîÑ ?àò?ñâ
    cmp di, 2560
    jb CALCULATEHASHLOOP

; 4Î∞îÏù¥?ä∏?ùò ?ï¥?ãúÍ∞? Ï∂úÎ†•
; ?ó¨Í∏∞ÏÑú?äî Í≥ÑÏÇ∞?êú Í∞íÍ≥º ????û•?êò?ñ¥?ûà?äî Í∞íÏùò Ï∞®Ïù¥Î•? Ï∂úÎ†•
COMPAREHASH:
    ; ÎπÑÎîî?ò§ ?ñ¥?ìú?†à?ä§ Ï£ºÏÜå?óê?Ñú Ï∂úÎ†•?ï† Ïß??†ê?ùÑ di ?†àÏß??ä§?Ñ∞?óê ????û•
    ; 3Î≤àÏß∏ Ï§? Ï≤òÏùåÎ∂??Ñ∞ Ï∂úÎ†• ?ãú?ûë
    mov di, 320
    push CHECKIMAGEMESSAGE
    push 2
    push 0
    call PRINTMESSAGE 
    ; Ï≤´Î≤àÏß? Î∞îÏù¥?ä∏ Í∞? ÎπÑÍµê (Ï∞®Ïù¥ Í≥ÑÏÇ∞) ?õÑ PRINTONEBINARY ?ï®?àòÎ•? ?Üµ?ï¥ Ï∂úÎ†•
    ; Í≥ÑÏÇ∞?ïú ?ï¥?ãú Í∞íÏùò ?ïò?úÑ 1Î∞îÏù¥?ä∏?óê ????û•?êò?ñ¥?ûà?äî ?ï¥?ãú Í∞íÏùò ?ïò?úÑ 1Î∞îÏù¥?ä∏Î•? Î∫? ?õÑ Ï∂úÎ†•
    ; bin ?åå?ùº ?Ç¥ ????û•?êú ?ï¥?ãúÍ∞íÏ?? fs: 0x00 ~ fs: 0x03?óê ????û•?êò?ñ¥?ûà?ùå

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
    call PRINTONEBYTE

    mov ah, byte[fs: 0x01]
    call PRINTONEBYTE

    mov ah, byte[fs: 0x02]
    call PRINTONEBYTE

    mov ah, byte[fs: 0x03]
    call PRINTONEBYTE

    mov di, 640

    mov ah, bl
    call PRINTONEBYTE
    
    mov ah, bh
    call PRINTONEBYTE

    mov ah, cl
    call PRINTONEBYTE

    mov ah, ch
    call PRINTONEBYTE

    jmp $

; ?ïú Î∞îÏù¥?ä∏ Í∞íÏùÑ 16ÏßÑÏàò ?òï?ÉúÎ°? Ï∂úÎ†•?ïòÍ∏? ?úÑ?ïú ?ï®?àò
; Ï∂úÎ†•?ï† Í∞íÏ?? ah ?†àÏß??ä§?Ñ∞?óê ?ã¥Í≤? ?ûà?ñ¥?ïº ?ï®
; Ï∂úÎ†•?ï† ?úÑÏπòÎ?? ?Çò????Ç¥?äî di?äî ÎØ∏Î¶¨ Í∞íÏù¥ Ïß??†ï?êò?ñ¥ ?ûà?ñ¥?ïº ?ï®
PRINTONEBYTE:
    push bp       
    mov bp, sp    
            
    push es       
    push si             
    push cx
    push dx

    ; ÎπÑÎîî?ò§ ?ñ¥?ìú?†à?ä§ ?ãú?ûë Ï£ºÏÜåÎ•? es ?†àÏß??ä§?Ñ∞?óê ????û•
    mov dx, 0xB800
    mov es, dx

    ; ?ÉÅ?úÑ 4ÎπÑÌä∏Î•? Í∞??†∏??? '0' ?çî?ï®
    mov al, ah
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    ; ÎßåÏïΩ ?ï¥?ãπ Í∞íÏù¥ 0xAÍ∞? ?Ñò?äî?ã§Î©? Ï∂úÎ†•?ùÑ ?úÑ?ï¥ 0x27Î•? Ï∂îÍ??Î°? ?çî?ï®
    cmp al, '9'
    ja .ISUPPERBIG
    ; 0xAÎ•? ?ÑòÏß? ?ïä?äî ?ã§Î©? Ï∂úÎ†• ÏΩîÎìúÎ°? ?†ê?îÑ?ï®
    jmp .PRINTUPPERHEX

; 0xA ?ù¥?ÉÅ?ùò Í∞íÏùÑ Ï∂úÎ†•?ïòÍ∏? ?úÑ?ï¥ Ï∂îÍ??Î°? 0x27Î•? ?çî?ïò?äî ÏΩîÎìú (?ïÑ?ä§?Ç§ÏΩîÎìú ?ÉÅ?ùò Ï∂úÎ†•?ùÑ ?úÑ?ï¥)
.ISUPPERBIG: 
    add al, 0x27

; ?ÉÅ?úÑ 4ÎπÑÌä∏Î•? ?ïò?Çò?ùò Î¨∏ÏûêÎ°? Ï∂úÎ†•
.PRINTUPPERHEX:
    mov byte [ es: di ], al
    add di, 2

    ;?ïò?úÑ 4ÎπÑÌä∏Î•? Í∞??†∏??? '0'?ùÑ ?çî?ïò?ó¨ Ï∂úÎ†• ?ù¥?ïò ÏΩîÎìú?äî ?úÑ??? ?èô?ùº
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

    ; ?ï®?àòÎ•? ?Åù?Ç¥Í∏? ?†Ñ ?ä§?Éù?óê ????û•?ï¥ ?ëê?óà?çò ?ù¥?†Ñ Í∞íÎì§?ùÑ ?êò?èå?†§?Üì?ùå
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
                    
