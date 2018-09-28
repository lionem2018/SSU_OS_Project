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
    push 17                     
    call PRINTMESSAGE              
    add  sp, 6 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 단순 저장된 해시값을 출력하기 위한 테스트 코드
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

; 해시 값을 계산하기 위한 준비
READYTOCALCULATEHASH:
    ; 해시값을 읽어올 메모리 시작 주소를 fs 레지스터에 저장
    mov ax, 0x1000
    mov fs, ax
    ; 4바이트 이후부터 계산해야하기 때문에 index 역할을 하는 di 레지스터를 4로 초기화
    mov di, 0x04

    ; xor 연산 값을 저장할 레지스터 0으로 초기화
    mov bx, 0x00
    mov cx, 0x00

; xor 연산을 통해 해시 값을 계산하는 루프
CALCULATEHASHLOOP:
    ; 4바이트를 한 바이트씩 가져와 xor 연산 실행
    xor bl, byte [ fs: di ]
    xor bh, byte [ fs: di + 1 ]
    xor cl, byte [ fs: di + 2 ]
    xor ch, byte [ fs: di + 3 ]

    ; 다음 4바이트를 가져오기 위해 di 4 증가
    add di, 0x04
    
    ; 2섹터(1024바이트)를 모두 연산할 때까지 루프 수행
    cmp di, 1024
    jb CALCULATEHASHLOOP

; 4바이트의 해시값 출력
; 여기서는 계산된 값과 저장되어있는 값의 차이를 출력
PRINTBINARIES:
    ; 비디오 어드레스 주소에서 출력할 지점을 di 레지스터에 저장
    ; 3번째 줄 처음부터 출력 시작
    mov di, 320
    
    ; 첫번째 바이트 값 비교 (차이 계산) 후 PRINTONEBINARY 함수를 통해 출력
    ; 계산한 해시 값의 하위 1바이트에 저장되어있는 해시 값의 하위 1바이트를 뺀 후 출력
    ; bin 파일 내 저장된 해시값은 fs: 0x00 ~ fs: 0x03에 저장되어있음
    sub bl, byte[ fs: 0x00]
    mov ah, bl
    call PRINTONEBINARY

    ; 두번째 바이트 값 비교 후 출력
    sub bh, byte[ fs: 0x01]
    mov ah, bh
    call PRINTONEBINARY

    ; 세번째 바이트 값 비교 후 출력
    sub cl, byte[ fs: 0x02]
    mov ah, cl
    call PRINTONEBINARY
    
    ; 네번째 바이트 값 비교 후 출력
    sub ch, byte[ fs: 0x03]
    mov ah, ch
    call PRINTONEBINARY
    jmp 0x1000:0x0004
    

; 한 바이트 값을 16진수 형태로 출력하기 위한 함수
; 출력할 값은 ah 레지스터에 담겨 있어야 함
; 출력할 위치를 나타내는 di는 미리 값이 지정되어 있어야 함
PRINTONEBINARY:
    push bp       
    mov bp, sp    
            
    push es       
    push si             
    push cx
    push dx

    ; 비디오 어드레스 시작 주소를 es 레지스터에 저장
    mov dx, 0xB800
    mov es, dx

    ; 상위 4비트를 가져와 '0' 더함
    mov al, ah
    and al, 0xF0
    shr al, 0x4
    add al, 0x30
    ; 만약 해당 값이 0xA가 넘는다면 출력을 위해 0x27를 추가로 더함
    cmp al, '9'
    ja .isUpperBig
    ; 0xA를 넘지 않는 다면 출력 코드로 점프함
    jmp .printUpperBinary

; 0xA 이상의 값을 출력하기 위해 추가로 0x27를 더하는 코드 (아스키코드 상의 출력을 위해)
.isUpperBig: 
    add al, 0x27

; 상위 4비트를 하나의 문자로 출력
.printUpperBinary:
    mov byte [ es: di ], al
    add di, 2

    ;하위 4비트를 가져와 '0'을 더하여 출력 이하 코드는 위와 동일
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

    ; 함수를 끝내기 전 스택에 저장해 두었던 이전 값들을 되돌려놓음
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
    push 17             
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
    
MESSAGE1:    db 'BootLoader Start', 0                                                   
;MESSAGE2:   db 'Current time: ', 0

DISKERRORMESSAGE:       db  'Error', 0
IMAGELOADINGMESSAGE:    db  'OS IMG Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete', 0

CHECKIMAGEMESSAGE:  db 'OS IMG Checking...', 0

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    
    
times 510 - ( $ - $$ )    db    0x00                                        

db 0x55             
db 0xAA             
                    
