# file      Assembly Utility
# date      2009/01/07
# author    kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief     ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½Æ¿ï¿½ï¿½Æ¼ ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ãµï¿½ ï¿½Ò½ï¿½ ï¿½ï¿½ï¿½ï¿½

[BITS 64]           ; ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½ï¿? 64ï¿½ï¿½Æ® ï¿½Úµï¿½ï¿? ï¿½ï¿½ï¿½ï¿½

SECTION .text       ; text ï¿½ï¿½ï¿½ï¿½(ï¿½ï¿½ï¿½×¸ï¿½Æ®)ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

; C ï¿½ï¿½î¿¡ï¿½ï¿? È£ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Öµï¿½ï¿½ï¿½ ï¿½Ì¸ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½(Export)
global kInPortWord, kOutPortWord 
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS, getFaultAddress
global kReadTSC
global kSwitchContext
global kSwitchContext, kHlt, kTestAndSet
; ï¿½ï¿½Æ®ï¿½Îºï¿½ï¿½ï¿½ 1ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
;   PARAM: ï¿½ï¿½Æ® ï¿½ï¿½È£
kInPortByte:
    push rdx        ; ï¿½Ô¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ó½Ã·ï¿½ ï¿½ï¿½ï¿½ï¿½Ï´ï¿? ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½ï¿½ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½ï¿½
                    ; ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ÎºÐ¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½Ôµï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

    mov rdx, rdi    ; RDX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? 1(ï¿½ï¿½Æ® ï¿½ï¿½È£)ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    mov rax, 0      ; RAX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½Ê±ï¿½È­
    in al, dx       ; DX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½Æ® ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Ð¾ï¿½
                    ; AL ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½ï¿½ï¿½ï¿½, AL ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í´ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½È¯ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

    pop rdx         ; ï¿½Ô¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½ï¿½ï¿½ï¿½
    ret             ; ï¿½Ô¼ï¿½ï¿½ï¿½ È£ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

; ï¿½ï¿½Æ®ï¿½ï¿½ 1ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½
;   PARAM: ï¿½ï¿½Æ® ï¿½ï¿½È£, ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
kOutPortByte:
    push rdx        ; ï¿½Ô¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ó½Ã·ï¿½ ï¿½ï¿½ï¿½ï¿½Ï´ï¿? ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½ï¿½ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½ï¿½
    push rax        ; ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ÎºÐ¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½Ôµï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    
    mov rdx, rdi    ; RDX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? 1(ï¿½ï¿½Æ® ï¿½ï¿½È£)ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    mov rax, rsi    ; RAX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? 2(ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    out dx, al      ; DX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½Æ® ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿? AL ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?
                    ; ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½

    pop rax         ; ï¿½Ô¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½ï¿½ï¿½ï¿½
    pop rdx
    ret             ; ï¿½Ô¼ï¿½ï¿½ï¿½ È£ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

; Æ÷Æ®·ÎºÎÅÍ 2¹ÙÀÌÆ®¸¦ ÀÐÀ½
;   PARAM: Æ÷Æ® ¹øÈ£
kInPortWord:
    push rdx        ; ÇÔ¼ö¿¡¼­ ÀÓ½Ã·Î »ç¿ëÇÏ´Â ·¹Áö½ºÅÍ¸¦ ½ºÅÃ¿¡ ÀúÀå
                    ; ÇÔ¼öÀÇ ¸¶Áö¸· ºÎºÐ¿¡¼­ ½ºÅÃ¿¡ »ðÀÔµÈ °ªÀ» ²¨³» º¹¿ø
    
    mov rdx, rdi    ; RDX ·¹Áö½ºÅÍ¿¡ ÆÄ¶ó¹ÌÅÍ 1(Æ÷Æ® ¹øÈ£)¸¦ ÀúÀå
    mov rax, 0      ; RAX ·¹Áö½ºÅÍ¸¦ ÃÊ±âÈ­
    in ax, dx       ; DX ·¹Áö½ºÅÍ¿¡ ÀúÀåµÈ Æ÷Æ® ¾îµå·¹½º¿¡¼­ µÎ ¹ÙÀÌÆ®¸¦ ÀÐ¾î
                    ; AX ·¹Áö½ºÅÍ¿¡ ÀúÀå, AX ·¹Áö½ºÅÍ´Â ÇÔ¼öÀÇ ¹ÝÈ¯ °ªÀ¸·Î »ç¿ëµÊ
    
    pop rdx         ; ÇÔ¼ö¿¡¼­ »ç¿ëÀÌ ³¡³­ ·¹Áö½ºÅÍ¸¦ º¹¿ø
    ret             ; ÇÔ¼ö¸¦ È£ÃâÇÑ ´ÙÀ½ ÄÚµåÀÇ À§Ä¡·Î º¹±Í
    
; Æ÷Æ®¿¡ 2¹ÙÀÌÆ®¸¦ ¾¸
;   PARAM: Æ÷Æ® ¹øÈ£, µ¥ÀÌÅÍ
kOutPortWord:
    push rdx        ; ÇÔ¼ö¿¡¼­ ÀÓ½Ã·Î »ç¿ëÇÏ´Â ·¹Áö½ºÅÍ¸¦ ½ºÅÃ¿¡ ÀúÀå
    push rax        ; ÇÔ¼öÀÇ ¸¶Áö¸· ºÎºÐ¿¡¼­ ½ºÅÃ¿¡ »ðÀÔµÈ °ªÀ» ²¨³» º¹¿ø
    
    mov rdx, rdi    ; RDX ·¹Áö½ºÅÍ¿¡ ÆÄ¶ó¹ÌÅÍ 1(Æ÷Æ® ¹øÈ£)¸¦ ÀúÀå
    mov rax, rsi    ; RAX ·¹Áö½ºÅÍ¿¡ ÆÄ¶ó¹ÌÅÍ 2(µ¥ÀÌÅÍ)¸¦ ÀúÀå    
    out dx, ax      ; DX ·¹Áö½ºÅÍ¿¡ ÀúÀåµÈ Æ÷Æ® ¾îµå·¹½º¿¡ AX ·¹Áö½ºÅÍ¿¡ ÀúÀåµÈ
                    ; µÎ ¹ÙÀÌÆ®¸¦ ¾¸
    
    pop rax         ; ÇÔ¼ö¿¡¼­ »ç¿ëÀÌ ³¡³­ ·¹Áö½ºÅÍ¸¦ º¹¿ø
    pop rdx
    ret             ; ÇÔ¼ö¸¦ È£ÃâÇÑ ´ÙÀ½ ÄÚµåÀÇ À§Ä¡·Î º¹±Í

; GDTR ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ GDT ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
;   PARAM: GDT ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Ú·á±¸ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½å·¹ï¿½ï¿?
kLoadGDTR:
    lgdt [ rdi ]    ; ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? 1(GDTRï¿½ï¿½ ï¿½ï¿½å·¹ï¿½ï¿?)ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Îµï¿½ï¿½Ï¿ï¿½
                    ; GDT ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    ret

; TR ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ TSS ï¿½ï¿½ï¿½×¸ï¿½Æ® ï¿½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
;   PARAM: TSS ï¿½ï¿½ï¿½×¸ï¿½Æ® ï¿½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
kLoadTR:
    ltr di          ; ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? 1(TSS ï¿½ï¿½ï¿½×¸ï¿½Æ® ï¿½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½
                    ; ï¿½ï¿½ï¿½ï¿½ï¿½Ï¿ï¿½ TSS ï¿½ï¿½ï¿½×¸ï¿½Æ®ï¿½ï¿½ ï¿½Îµï¿½
    ret

; IDTR ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ IDT ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
;   PARAM: IDT ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Ú·á±¸ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½å·¹ï¿½ï¿?
kLoadIDTR:
    lidt [ rdi ]    ; ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? 1(IDTRï¿½ï¿½ ï¿½ï¿½å·¹ï¿½ï¿?)ï¿½ï¿½ ï¿½ï¿½ï¿½Î¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Îµï¿½ï¿½Ï¿ï¿½
                    ; IDT ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    ret

; ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ È°ï¿½ï¿½È­
;   PARAM: ï¿½ï¿½ï¿½ï¿½
kEnableInterrupt:
    sti             ; ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ È°ï¿½ï¿½È­
    ret

; ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½È°ï¿½ï¿½È­
;   PARAM: ï¿½ï¿½ï¿½ï¿½
kDisableInterrupt:
    cli             ; ï¿½ï¿½ï¿½Í·ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½È°ï¿½ï¿½È­
    ret

; RFLAGS ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½Ð¾î¼­ ï¿½Çµï¿½ï¿½ï¿½ï¿½ï¿½
;   PARAM: ï¿½ï¿½ï¿½ï¿½
kReadRFLAGS:
    pushfq                  ; RFLAGS ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½ï¿½ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½ï¿½
    pop rax                 ; ï¿½ï¿½ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? RFLAGS ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ RAX ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¿ï¿½
                            ; ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½È¯ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    ret

getFaultAddress:
    mov rax, cr2
    ret

; ????ž„ ?Š¤?ƒ¬?”„ ì¹´ìš´?„°ë¥? ?½?–´?„œ ë°˜í™˜ 
;   PARAM: ?—†?Œ    
kReadTSC:
    push rdx                ; RDX ? ˆì§??Š¤?„°ë¥? ?Š¤?ƒ?— ????ž¥
    
    rdtsc                   ; ????ž„ ?Š¤?ƒ¬?”„ ì¹´ìš´?„°ë¥? ?½?–´?„œ RDX:RAX?— ????ž¥
    
    shl rdx, 32             ; RDX ? ˆì§??Š¤?„°?— ?žˆ?Š” ?ƒ?œ„ 32ë¹„íŠ¸ TSC ê°’ê³¼ RAX ? ˆì§??Š¤?„°?—
    or rax, rdx             ; ?žˆ?Š” ?•˜?œ„ 32ë¹„íŠ¸ TSC ê°’ì„ OR?•˜?—¬ RAX ? ˆì§??Š¤?„°?— 64ë¹„íŠ¸ 
                            ; TSC ê°’ì„ ????ž¥
    
    pop rdx
    ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   ?ƒœ?Š¤?¬ ê´?? ¨ ?–´?…ˆë¸”ë¦¬?–´ ?•¨?ˆ˜
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ì½˜í…?Š¤?Š¸ë¥? ????ž¥?•˜ê³? ???? ‰?„°ë¥? êµì²´?•˜?Š” ë§¤í¬ë¡?
%macro KSAVECONTEXT 0       ; ?ŒŒ?¼ë¯¸í„°ë¥? ? „?‹¬ë°›ì?? ?•Š?Š” KSAVECONTEXT ë§¤í¬ë¡? ? •?˜
    ; RBP ? ˆì§??Š¤?„°ë¶??„° GS ?„¸ê·¸ë¨¼?Š¸ ???? ‰?„°ê¹Œì?? ëª¨ë‘ ?Š¤?ƒ?— ?‚½?ž…
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov ax, ds      ; DS ?„¸ê·¸ë¨¼?Š¸ ???? ‰?„°??? ES ?„¸ê·¸ë¨¼?Š¸ ???? ‰?„°?Š” ?Š¤?ƒ?— ì§ì ‘
    push rax        ; ?‚½?ž…?•  ?ˆ˜ ?—†?œ¼ë¯?ë¡?, RAX ? ˆì§??Š¤?„°?— ????ž¥?•œ ?›„ ?Š¤?ƒ?— ?‚½?ž…
    mov ax, es
    push rax
    push fs
    push gs 
%endmacro       ; ë§¤í¬ë¡? ?


; ì½˜í…?Š¤?Š¸ë¥? ë³µì›?•˜?Š” ë§¤í¬ë¡?
%macro KLOADCONTEXT 0   ; ?ŒŒ?¼ë¯¸í„°ë¥? ? „?‹¬ë°›ì?? ?•Š?Š” KSAVECONTEXT ë§¤í¬ë¡? ? •?˜
    ; GS ?„¸ê·¸ë¨¼?Š¸ ???? ‰?„°ë¶??„° RBP ? ˆì§??Š¤?„°ê¹Œì?? ëª¨ë‘ ?Š¤?ƒ?—?„œ êº¼ë‚´ ë³µì›
    pop gs
    pop fs
    pop rax
    mov es, ax      ; ES ?„¸ê·¸ë¨¼?Š¸ ???? ‰?„°??? DS ?„¸ê·¸ë¨¼?Š¸ ???? ‰?„°?Š” ?Š¤?ƒ?—?„œ ì§ì ‘
    pop rax         ; êº¼ë‚´ ë³µì›?•  ?ˆ˜ ?—†?œ¼ë¯?ë¡?, RAX ? ˆì§??Š¤?„°?— ????ž¥?•œ ?’¤?— ë³µì›
    mov ds, ax
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp        
%endmacro       ; ë§¤í¬ë¡? ?

; Current Context?— ?˜„?ž¬ ì½˜í…?Š¤?Š¸ë¥? ????ž¥?•˜ê³? Next Task?—?„œ ì½˜í…?Š¤?Š¸ë¥? ë³µêµ¬
;   PARAM: Current Context, Next Context
kSwitchContext:
    push rbp        ; ?Š¤?ƒ?— RBP ? ˆì§??Š¤?„°ë¥? ????ž¥?•˜ê³? RSP ? ˆì§??Š¤?„°ë¥? RBP?— ????ž¥
    mov rbp, rsp
    
    ; Current Contextê°? NULL?´ë©? ì½˜í…?Š¤?Š¸ë¥? ????ž¥?•  ?•„?š” ?—†?Œ
    pushfq          ; ?•„?ž˜?˜ cmp?˜ ê²°ê³¼ë¡? RFLAGS ? ˆì§??Š¤?„°ê°? ë³??•˜ì§? ?•Š?„ë¡? ?Š¤?ƒ?— ????ž¥
    cmp rdi, 0      ; Current Contextê°? NULL?´ë©? ì½˜í…?Š¤?Š¸ ë³µì›?œ¼ë¡? ë°”ë¡œ ?´?™
    je .LoadContext 
    popfq           ; ?Š¤?ƒ?— ????ž¥?•œ RFLAGS ? ˆì§??Š¤?„°ë¥? ë³µì›

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; ?˜„?ž¬ ?ƒœ?Š¤?¬?˜ ì½˜í…?Š¤?Š¸ë¥? ????ž¥
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push rax            ; ì½˜í…?Š¤?Š¸ ?˜?—­?˜ ?˜¤?”„?…‹?œ¼ë¡? ?‚¬?š©?•  RAX ? ˆì§??Š¤?„°ë¥? ?Š¤?ƒ?— ????ž¥
    
    ; SS, RSP, RFLAGS, CS, RIP ? ˆì§??Š¤?„° ?ˆœ?„œ???ë¡? ?‚½?ž…
    mov ax, ss                          ; SS ? ˆì§??Š¤?„° ????ž¥
    mov qword[ rdi + ( 23 * 8 ) ], rax

    mov rax, rbp                        ; RBP?— ????ž¥?œ RSP ? ˆì§??Š¤?„° ????ž¥
    add rax, 16                         ; RSP ? ˆì§??Š¤?„°?Š” push rbp??? Return Addressë¥?
    mov qword[ rdi + ( 22 * 8 ) ], rax  ; ? œ?™¸?•œ ê°’ìœ¼ë¡? ????ž¥
    
    pushfq                              ; RFLAGS ? ˆì§??Š¤?„° ????ž¥
    pop rax
    mov qword[ rdi + ( 21 * 8 ) ], rax

    mov ax, cs                          ; CS ? ˆì§??Š¤?„° ????ž¥
    mov qword[ rdi + ( 20 * 8 ) ], rax
    
    mov rax, qword[ rbp + 8 ]           ; RIP ? ˆì§??Š¤?„°ë¥? Return Addressë¡? ?„¤? •?•˜?—¬ 
    mov qword[ rdi + ( 19 * 8 ) ], rax  ; ?‹¤?Œ ì½˜í…?Š¤?Š¸ ë³µì› ?‹œ?— ?´ ?•¨?ˆ˜ë¥? ?˜¸ì¶œí•œ 
                                        ; ?œ„ì¹˜ë¡œ ?´?™?•˜ê²? ?•¨
    
    ; ????ž¥?•œ ? ˆì§??Š¤?„°ë¥? ë³µêµ¬?•œ ?›„ ?¸?„°?Ÿ½?Š¸ê°? ë°œìƒ?–ˆ?„ ?•Œì²˜ëŸ¼ ?‚˜ë¨¸ì?? ì½˜í…?Š¤?Š¸ë¥? ëª¨ë‘ ????ž¥
    pop rax
    pop rbp
    
    ; ê°??ž¥ ?ë¶?ë¶„ì— SS, RSP, RFLAGS, CS, RIP ? ˆì§??Š¤?„°ë¥? ????ž¥?–ˆ?œ¼ë¯?ë¡?, ?´? „ ?˜?—­?—
    ; push ëª…ë ¹?–´ë¡? ì½˜í…?Š¤?Š¸ë¥? ????ž¥?•˜ê¸? ?œ„?•´ ?Š¤?ƒ?„ ë³?ê²?
    add rdi, ( 19 * 8 )
    mov rsp, rdi
    sub rdi, ( 19 * 8 )
    
    ; ?‚˜ë¨¸ì?? ? ˆì§??Š¤?„°ë¥? ëª¨ë‘ Context ?žë£Œêµ¬ì¡°ì— ????ž¥
    KSAVECONTEXT

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; ?‹¤?Œ ?ƒœ?Š¤?¬?˜ ì½˜í…?Š¤?Š¸ ë³µì›
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.LoadContext:
    mov rsp, rsi
    
    ; Context ?žë£Œêµ¬ì¡°ì—?„œ ? ˆì§??Š¤?„°ë¥? ë³µì›
    KLOADCONTEXT
    iretq

; ÇÁ·Î¼¼¼­¸¦ ½¬°Ô ÇÔ
;   PARAM: ¾øÀ½
kHlt:
    hlt     ; ÇÁ·Î¼¼¼­¸¦ ´ë±â »óÅÂ·Î ÁøÀÔ½ÃÅ´
    hlt
    ret
    
; Å×½ºÆ®¿Í ¼³Á¤À» ÇÏ³ªÀÇ ¸í·ÉÀ¸·Î Ã³¸®
;	Destination°ú Compare¸¦ ºñ±³ÇÏ¿© °°´Ù¸é, Destination¿¡ Source °ªÀ» »ðÀÔ
;   PARAM: °ªÀ» ÀúÀåÇÒ ¾îµå·¹½º(Destination, rdi), ºñ±³ÇÒ °ª(Compare, rsi), 
;          ¼³Á¤ÇÒ °ª(Source, rdx) 
kTestAndSet:
    mov rax, rsi        ; µÎ ¹øÂ° ÆÄ¶ó¹ÌÅÍÀÎ ºñ±³ÇÒ °ªÀ» RAX ·¹Áö½ºÅÍ¿¡ ÀúÀå 
    
    ; RAX ·¹Áö½ºÅÍ¿¡ ÀúÀåµÈ ºñ±³ÇÒ °ª°ú Ã¹ ¹øÂ° ÆÄ¶ó¹ÌÅÍÀÇ ¸Þ¸ð¸® ¾îµå·¹½ºÀÇ °ªÀ»
    ; ºñ±³ÇÏ¿© µÎ °ªÀÌ °°´Ù¸é ¼¼ ¹øÂ° ÆÄ¶ó¹ÌÅÍÀÇ °ªÀ» Ã¹ ¹øÂ° ÆÄ¶ó¹ÌÅÍ°¡ °¡¸®Å°´Â
    ; ¾îµå·¹½º¿¡ »ðÀÔ
    lock cmpxchg byte [ rdi ], dl
    je .SUCCESS         ; ZF ºñÆ®°¡ 1ÀÌ¸é °°´Ù´Â ¶æÀÌ¹Ç·Î .SUCCESS·Î ÀÌµ¿
    
.NOTSAME:               ; Destination°ú Compare°¡ ´Ù¸¥ °æ¿ì
    mov rax, 0x00
    ret
    
.SUCCESS:               ; Destination°ú Compare°¡ °°Àº °æ¿ì
    mov rax, 0x01
    ret


