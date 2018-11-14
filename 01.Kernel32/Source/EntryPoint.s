# file      EntryPoint.s
# date      2008/11/27
# author    kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief     占쏙옙호 占쏙옙占?? 커占쏙옙 占쏙옙트占쏙옙 占쏙옙占쏙옙트占쏙옙 占쏙옙占시듸옙 占쌀쏙옙 占쏙옙占쏙옙

[ORG 0x00]          ; 占쌘듸옙占쏙옙 占쏙옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 0x00占쏙옙占쏙옙 占쏙옙占쏙옙
[BITS 16]           ; 占쏙옙占쏙옙占쏙옙 占쌘듸옙占?? 16占쏙옙트 占쌘듸옙占?? 占쏙옙占쏙옙

SECTION .text       ; text 占쏙옙占쏙옙(占쏙옙占쌓몌옙트)占쏙옙 占쏙옙占쏙옙

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   占쌘듸옙 占쏙옙占쏙옙
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x1000  ; 占쏙옙호 占쏙옙占?? 占쏙옙트占쏙옙 占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙 占쏙옙藥뱄옙占??(0x10000)占쏙옙 
                    ; 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙환
    mov ds, ax      ; DS 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov es, ax      ; ES 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    
    mov ax, 0x2401
    int 0x15

    jc .A20GATEERROR
    jmp .A20GATESUCCESS

.A20GATEERROR:
    in al, 0x92
    or al, 0x02
    and al, 0xFE
    out 0x92, al

.A20GATESUCCESS
    cli             ; 占쏙옙占싶뤄옙트占쏙옙 占쌩삼옙占쏙옙占쏙옙 占쏙옙占싹듸옙占쏙옙 占쏙옙占쏙옙
    lgdt [ GDTR + 4 ]   ; GDTR 占쌘료구占쏙옙占쏙옙 占쏙옙占싸쇽옙占쏙옙占쏙옙 占쏙옙占쏙옙占싹울옙 GDT 占쏙옙占싱븝옙占쏙옙 占싸듸옙

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 占쏙옙호占쏙옙占쏙옙 占쏙옙占쏙옙
    ; Disable Paging, Disable Cache, Internal FPU, Disable Align Check, 
    ; Enable ProtectedMode
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, 0x4001003B ; PG=0, CD=1, NW=0, AM=0, WP=1, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax        ; CR0 占쏙옙트占쏙옙 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占시뤄옙占쌓몌옙 占쏙옙占쏙옙占싹울옙 
                        ; 占쏙옙호 占쏙옙占쏙옙 占쏙옙환   0x4001003B

    ; 커占쏙옙 占쌘듸옙 占쏙옙占쌓몌옙트占쏙옙 0x00占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占싹댐옙 占쏙옙占쏙옙占쏙옙 占쏙옙체占싹곤옙 EIP占쏙옙 占쏙옙占쏙옙 0x00占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占썹설占쏙옙
    ; CS 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占쏙옙 : EIP
    jmp dword 0x18: ( PROTECTEDMODE - $$ + 0x10004 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; 占쏙옙호 占쏙옙占쏙옙 占쏙옙占쏙옙
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]               ; 占쏙옙占쏙옙占쏙옙 占쌘듸옙占?? 32占쏙옙트 占쌘듸옙占?? 占쏙옙占쏙옙
PROTECTEDMODE:
    mov ax, 0x20        ; 占쏙옙호 占쏙옙占?? 커占싸울옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌓몌옙트 占쏙옙크占쏙옙占싶몌옙 AX 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov ds, ax          ; DS 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov es, ax          ; ES 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov fs, ax          ; FS 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov gs, ax          ; GS 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    
    ; 占쏙옙占쏙옙占쏙옙 0x00000000~0x0000FFFF 占쏙옙占쏙옙占쏙옙 64KB 크占쏙옙占?? 占쏙옙占쏙옙
    mov ss, ax          ; SS 占쏙옙占쌓몌옙트 占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov esp, 0xFFFE     ; ESP 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 0xFFFE占쏙옙 占쏙옙占쏙옙
    mov ebp, 0xFFFE     ; EBP 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 0xFFFE占쏙옙 占쏙옙占쏙옙
    
    ; 화占썽에 占쏙옙호 占쏙옙占쏙옙 占쏙옙환占실억옙占쌕댐옙 占쌨쏙옙占쏙옙占쏙옙 占쏙옙쨈占??.
    push ( SWITCHSUCCESSMESSAGE - $$ + 0x10004 )    ; 占쏙옙占쏙옙占?? 占쌨쏙옙占쏙옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占시울옙 占쏙옙占쏙옙
    push 3                                          ; 화占쏙옙 Y 占쏙옙표(2)占쏙옙 占쏙옙占시울옙 占쏙옙占쏙옙
    push 0                                          ; 화占쏙옙 X 占쏙옙표(0)占쏙옙 占쏙옙占시울옙 占쏙옙占쏙옙
    call PRINTMESSAGE                               ; PRINTMESSAGE 占쌉쇽옙 호占쏙옙
    add esp, 12                                     ; 占쏙옙占쏙옙占쏙옙 占식띰옙占쏙옙占?? 占쏙옙占쏙옙

    jmp dword 0x18: 0x10200               ; 占쏙옙占쏙옙 占쏙옙치占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   占쌉쇽옙 占쌘듸옙 占쏙옙占쏙옙
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 占쌨쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙求占?? 占쌉쇽옙
;   占쏙옙占시울옙 x 占쏙옙표, y 占쏙옙표, 占쏙옙占쌘울옙
PRINTMESSAGE:
    push ebp        ; 占쏙옙占싱쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙(BP)占쏙옙 占쏙옙占시울옙 占쏙옙占쏙옙
    mov ebp, esp    ; 占쏙옙占싱쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙(BP)占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙(SP)占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
    push esi        ; 占쌉쇽옙占쏙옙占쏙옙 占쌈시뤄옙 占쏙옙占쏙옙求占?? 占쏙옙占쏙옙占쏙옙占싶뤄옙 占쌉쇽옙占쏙옙 占쏙옙占쏙옙占쏙옙 占싸분울옙占쏙옙
    push edi        ; 占쏙옙占시울옙 占쏙옙占쌉듸옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
    push eax
    push ecx
    push edx
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; X, Y占쏙옙 占쏙옙표占쏙옙 占쏙옙占쏙옙 占쌨몌옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占쏙옙占??
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Y 占쏙옙표占쏙옙 占싱울옙占쌔쇽옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占쏙옙
    mov eax, dword [ ebp + 12 ] ; 占식띰옙占쏙옙占?? 2(화占쏙옙 占쏙옙표 Y)占쏙옙 EAX 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov esi, 160                ; 占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙트 占쏙옙(2 * 80 占시뤄옙)占쏙옙 ESI 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mul esi                     ; EAX 占쏙옙占쏙옙占쏙옙占싶울옙 ESI 占쏙옙占쏙옙占쏙옙占싶몌옙 占쏙옙占싹울옙 화占쏙옙 Y 占쏙옙藥뱄옙占?? 占쏙옙占??
    mov edi, eax                ; 占쏙옙占쏙옙 화占쏙옙 Y 占쏙옙藥뱄옙占쏙옙占?? EDI 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    
    ; X 占승료를 占싱울옙占쌔쇽옙 2占쏙옙 占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占쏙옙
    mov eax, dword [ ebp + 8 ]  ; 占식띰옙占쏙옙占?? 1(화占쏙옙 占쏙옙표 X)占쏙옙 EAX 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mov esi, 2                  ; 占쏙옙 占쏙옙占쌘몌옙 占쏙옙타占쏙옙占쏙옙 占쏙옙占쏙옙트 占쏙옙(2)占쏙옙 ESI 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
    mul esi                     ; EAX 占쏙옙占쏙옙占쏙옙占싶울옙 ESI 占쏙옙占쏙옙占쏙옙占싶몌옙 占쏙옙占싹울옙 화占쏙옙 X 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占??
    add edi, eax                ; 화占쏙옙 Y 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占쏙옙 X 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占쌔쇽옙
                                ; 占쏙옙占쏙옙 占쏙옙占쏙옙 占쌨몌옙 占쏙옙藥뱄옙占쏙옙占?? 占쏙옙占??
                                
    ; 占쏙옙占쏙옙占?? 占쏙옙占쌘울옙占쏙옙 占쏙옙藥뱄옙占??      
    mov esi, dword [ ebp + 16 ] ; 占식띰옙占쏙옙占?? 3(占쏙옙占쏙옙占?? 占쏙옙占쌘울옙占쏙옙 占쏙옙藥뱄옙占??)

.MESSAGELOOP:               ; 占쌨쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙求占?? 占쏙옙占쏙옙
    mov cl, byte [ esi ]    ; ESI 占쏙옙占쏙옙占쏙옙占싶곤옙 占쏙옙占쏙옙키占쏙옙 占쏙옙占쌘울옙 占쏙옙치占쏙옙占쏙옙 占쏙옙 占쏙옙占쌘몌옙 
                            ; CL 占쏙옙占쏙옙占쏙옙占싶울옙 占쏙옙占쏙옙
                            ; CL 占쏙옙占쏙옙占쏙옙占싶댐옙 ECX 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 1占쏙옙占쏙옙트占쏙옙 占실뱄옙
                            ; 占쏙옙占쌘울옙占쏙옙 1占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙球퓐占?? ECX 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 1占쏙옙占쏙옙트占쏙옙 占쏙옙占??

    cmp cl, 0               ; 占쏙옙占쏙옙占?? 占쏙옙占쌘울옙 0占쏙옙 占쏙옙
    je .MESSAGEEND          ; 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 0占싱몌옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙퓸占쏙옙占쏙옙占??
                            ; 占실뱄옙占싹므뤄옙 .MESSAGEEND占쏙옙 占싱듸옙占싹울옙 占쏙옙占쏙옙 占쏙옙占?? 占쏙옙占쏙옙

    mov byte [ edi + 0xB8000 ], cl  ; 0占쏙옙 占싣니띰옙占?? 占쏙옙占쏙옙 占쌨몌옙 占쏙옙藥뱄옙占?? 
                                    ; 0xB8000 + EDI 占쏙옙 占쏙옙占쌘몌옙 占쏙옙占??
    
    add esi, 1              ; ESI 占쏙옙占쏙옙占쏙옙占싶울옙 1占쏙옙 占쏙옙占싹울옙 占쏙옙占쏙옙 占쏙옙占쌘울옙占쏙옙 占싱듸옙
    add edi, 2              ; EDI 占쏙옙占쏙옙占쏙옙占싶울옙 2占쏙옙 占쏙옙占싹울옙 占쏙옙占쏙옙 占쌨몌옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙치占쏙옙 占싱듸옙
                            ; 占쏙옙占쏙옙 占쌨모리댐옙 (占쏙옙占쏙옙, 占쌈쇽옙)占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占실므뤄옙 占쏙옙占쌘몌옙 占쏙옙占쏙옙狗占쏙옙占??
                            ; 2占쏙옙 占쏙옙占쌔억옙 占쏙옙

    jmp .MESSAGELOOP        ; 占쌨쏙옙占쏙옙 占쏙옙占?? 占쏙옙占쏙옙占쏙옙 占싱듸옙占싹울옙 占쏙옙占쏙옙 占쏙옙占쌘몌옙 占쏙옙占??

.MESSAGEEND:
    pop edx     ; 占쌉쇽옙占쏙옙占쏙옙 占쏙옙占쏙옙占?? 占쏙옙占쏙옙 EDX 占쏙옙占쏙옙占쏙옙占싶븝옙占쏙옙 EBP 占쏙옙占쏙옙占쏙옙占싶깍옙占쏙옙占쏙옙 占쏙옙占시울옙
    pop ecx     ; 占쏙옙占쌉듸옙 占쏙옙占쏙옙 占싱울옙占쌔쇽옙 占쏙옙占쏙옙
    pop eax     ; 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占?? 占쏙옙占쏙옙占싶곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙
    pop edi     ; 占쌘료구占쏙옙(Last-In, First-Out)占싱므뤄옙 占쏙옙占쏙옙(push)占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙
    pop esi     ; 占쏙옙占쏙옙(pop) 占쌔억옙 占쏙옙
    pop ebp     ; 占쏙옙占싱쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙(BP) 占쏙옙占쏙옙
    ret         ; 占쌉쇽옙占쏙옙 호占쏙옙占쏙옙 占쏙옙占쏙옙 占쌘듸옙占쏙옙 占쏙옙치占쏙옙 占쏙옙占쏙옙

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 占싣뤄옙占쏙옙 占쏙옙占쏙옙占싶듸옙占쏙옙 8byte占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占싹깍옙 占쏙옙占쏙옙 占쌩곤옙
align 8, db 0

; GDTR占쏙옙 占쏙옙占쏙옙 8byte占쏙옙 占쏙옙占쏙옙占싹깍옙 占쏙옙占쏙옙 占쌩곤옙
dw 0x0000
; GDTR 占쌘료구占쏙옙 占쏙옙占쏙옙
GDTR:
    dw GDTEND - GDT - 1         ; 占싣뤄옙占쏙옙 占쏙옙치占싹댐옙 GDT 占쏙옙占싱븝옙占쏙옙 占쏙옙체 크占쏙옙
    dd ( GDT - $$ + 0x10004 )   ; 占싣뤄옙占쏙옙 占쏙옙치占싹댐옙 GDT 占쏙옙占싱븝옙占쏙옙 占쏙옙占쏙옙 占쏙옙藥뱄옙占??

; GDT 占쏙옙占싱븝옙 占쏙옙占쏙옙
GDT:
    ; 占쏙옙(NULL) 占쏙옙크占쏙옙占쏙옙, 占쌥듸옙占?? 0占쏙옙占쏙옙 占십깍옙화占쌔억옙 占쏙옙
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    ; IA-32e ?占쏙옙?? ?占쏙옙占쏙옙?占쏙옙? ???? 占쏙옙占쏙옙占쏙옙占쏙옙??占쏙옙占쏙옙 ?占쏙옙占쏙옙占쏙옙???占쏙옙??
    IA_32eCODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    ; IA-32e ?占쏙옙?? ?占쏙옙占쏙옙?占쏙옙? ?????? 占쏙옙占쏙옙占쏙옙占쏙옙??占쏙옙占쏙옙 ?占쏙옙占쏙옙占쏙옙???占쏙옙??
    IA_32eDATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
        db 0x00         ; Base [31:24]

    ; 占쏙옙호 占쏙옙占?? 커占싸울옙 占쌘듸옙 占쏙옙占쌓몌옙트 占쏙옙크占쏙옙占쏙옙
    CODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    ; 占쏙옙호 占쏙옙占?? 커占싸울옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌓몌옙트 占쏙옙크占쏙옙占쏙옙
    DATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
GDTEND:

; 占쏙옙호 占쏙옙占쏙옙 占쏙옙환占실억옙占쌕댐옙 占쌨쏙옙占쏙옙
SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success~!!', 0

times 508 - ( $ - $$ )  db  0x00    ; 512占쏙옙占쏙옙트占쏙옙 占쏙옙占쌩깍옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占싸븝옙占쏙옙 0占쏙옙占쏙옙 채占쏙옙
