# file      Assembly Utility
# date      2009/01/07
# author    kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief     �������? ��ƿ��Ƽ �Լ��� ���õ� �ҽ� ����

[BITS 64]           ; ������ �ڵ��? 64��Ʈ �ڵ��? ����

SECTION .text       ; text ����(���׸�Ʈ)�� ����

; C ����? ȣ���� �� �ֵ��� �̸��� ������(Export)

global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS, getFaultAddress
global kReadTSC
global kSwitchContext
global kSwitchContext, kHlt, kTestAndSet
; ��Ʈ�κ��� 1����Ʈ�� ����
;   PARAM: ��Ʈ ��ȣ
kInPortByte:
    push rdx        ; �Լ����� �ӽ÷� ����ϴ�? �������͸� ���ÿ� ����
                    ; �Լ��� ������ �κп��� ���ÿ� ���Ե� ���� ���� ����

    mov rdx, rdi    ; RDX �������Ϳ� �Ķ����? 1(��Ʈ ��ȣ)�� ����
    mov rax, 0      ; RAX �������͸� �ʱ�ȭ
    in al, dx       ; DX �������Ϳ� �����? ��Ʈ ��巹������? �� ����Ʈ�� �о�
                    ; AL �������Ϳ� ����, AL �������ʹ� �Լ��� ��ȯ ������ ����

    pop rdx         ; �Լ����� �����? ���� �������͸� ����
    ret             ; �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����

; ��Ʈ�� 1����Ʈ�� ��
;   PARAM: ��Ʈ ��ȣ, ������
kOutPortByte:
    push rdx        ; �Լ����� �ӽ÷� ����ϴ�? �������͸� ���ÿ� ����
    push rax        ; �Լ��� ������ �κп��� ���ÿ� ���Ե� ���� ���� ����
    
    mov rdx, rdi    ; RDX �������Ϳ� �Ķ����? 1(��Ʈ ��ȣ)�� ����
    mov rax, rsi    ; RAX �������Ϳ� �Ķ����? 2(������)�� ����
    out dx, al      ; DX �������Ϳ� �����? ��Ʈ ��巹����? AL �������Ϳ� �����?
                    ; �� ����Ʈ�� ��

    pop rax         ; �Լ����� �����? ���� �������͸� ����
    pop rdx
    ret             ; �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����

; GDTR �������Ϳ� GDT ���̺��� ����
;   PARAM: GDT ���̺��� ������ �����ϴ� �ڷᱸ���� ��巹��?
kLoadGDTR:
    lgdt [ rdi ]    ; �Ķ����? 1(GDTR�� ��巹��?)�� ���μ����� �ε��Ͽ�
                    ; GDT ���̺��� ����
    ret

; TR �������Ϳ� TSS ���׸�Ʈ ��ũ���� ����
;   PARAM: TSS ���׸�Ʈ ��ũ������ ������
kLoadTR:
    ltr di          ; �Ķ����? 1(TSS ���׸�Ʈ ��ũ������ ������)�� ���μ�����
                    ; �����Ͽ� TSS ���׸�Ʈ�� �ε�
    ret

; IDTR �������Ϳ� IDT ���̺��� ����
;   PARAM: IDT ���̺��� ������ �����ϴ� �ڷᱸ���� ��巹��?
kLoadIDTR:
    lidt [ rdi ]    ; �Ķ����? 1(IDTR�� ��巹��?)�� ���μ����� �ε��Ͽ�
                    ; IDT ���̺��� ����
    ret

; ���ͷ�Ʈ�� Ȱ��ȭ
;   PARAM: ����
kEnableInterrupt:
    sti             ; ���ͷ�Ʈ�� Ȱ��ȭ
    ret

; ���ͷ�Ʈ�� ��Ȱ��ȭ
;   PARAM: ����
kDisableInterrupt:
    cli             ; ���ͷ�Ʈ�� ��Ȱ��ȭ
    ret

; RFLAGS �������͸� �о �ǵ�����
;   PARAM: ����
kReadRFLAGS:
    pushfq                  ; RFLAGS �������͸� ���ÿ� ����
    pop rax                 ; ���ÿ� �����? RFLAGS �������͸� RAX �������Ϳ� �����Ͽ�
                            ; �Լ��� ��ȯ ������ ����
    ret

getFaultAddress:
    mov rax, cr2
    ret

; ????�� ?��?��?�� 카운?���? ?��?��?�� 반환 
;   PARAM: ?��?��    
kReadTSC:
    push rdx                ; RDX ?���??��?���? ?��?��?�� ????��
    
    rdtsc                   ; ????�� ?��?��?�� 카운?���? ?��?��?�� RDX:RAX?�� ????��
    
    shl rdx, 32             ; RDX ?���??��?��?�� ?��?�� ?��?�� 32비트 TSC 값과 RAX ?���??��?��?��
    or rax, rdx             ; ?��?�� ?��?�� 32비트 TSC 값을 OR?��?�� RAX ?���??��?��?�� 64비트 
                            ; TSC 값을 ????��
    
    pop rdx
    ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   ?��?��?�� �??�� ?��?��블리?�� ?��?��
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 콘텍?��?���? ????��?���? ????��?���? 교체?��?�� 매크�?
%macro KSAVECONTEXT 0       ; ?��?��미터�? ?��?��받�?? ?��?�� KSAVECONTEXT 매크�? ?��?��
    ; RBP ?���??��?���??�� GS ?��그먼?�� ????��?��까�?? 모두 ?��?��?�� ?��?��
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
    
    mov ax, ds      ; DS ?��그먼?�� ????��?��??? ES ?��그먼?�� ????��?��?�� ?��?��?�� 직접
    push rax        ; ?��?��?�� ?�� ?��?���?�?, RAX ?���??��?��?�� ????��?�� ?�� ?��?��?�� ?��?��
    mov ax, es
    push rax
    push fs
    push gs 
%endmacro       ; 매크�? ?��


; 콘텍?��?���? 복원?��?�� 매크�?
%macro KLOADCONTEXT 0   ; ?��?��미터�? ?��?��받�?? ?��?�� KSAVECONTEXT 매크�? ?��?��
    ; GS ?��그먼?�� ????��?���??�� RBP ?���??��?��까�?? 모두 ?��?��?��?�� 꺼내 복원
    pop gs
    pop fs
    pop rax
    mov es, ax      ; ES ?��그먼?�� ????��?��??? DS ?��그먼?�� ????��?��?�� ?��?��?��?�� 직접
    pop rax         ; 꺼내 복원?�� ?�� ?��?���?�?, RAX ?���??��?��?�� ????��?�� ?��?�� 복원
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
%endmacro       ; 매크�? ?��

; Current Context?�� ?��?�� 콘텍?��?���? ????��?���? Next Task?��?�� 콘텍?��?���? 복구
;   PARAM: Current Context, Next Context
kSwitchContext:
    push rbp        ; ?��?��?�� RBP ?���??��?���? ????��?���? RSP ?���??��?���? RBP?�� ????��
    mov rbp, rsp
    
    ; Current Context�? NULL?���? 콘텍?��?���? ????��?�� ?��?�� ?��?��
    pushfq          ; ?��?��?�� cmp?�� 결과�? RFLAGS ?���??��?���? �??���? ?��?���? ?��?��?�� ????��
    cmp rdi, 0      ; Current Context�? NULL?���? 콘텍?��?�� 복원?���? 바로 ?��?��
    je .LoadContext 
    popfq           ; ?��?��?�� ????��?�� RFLAGS ?���??��?���? 복원

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; ?��?�� ?��?��?��?�� 콘텍?��?���? ????��
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push rax            ; 콘텍?��?�� ?��?��?�� ?��?��?��?���? ?��?��?�� RAX ?���??��?���? ?��?��?�� ????��
    
    ; SS, RSP, RFLAGS, CS, RIP ?���??��?�� ?��?��???�? ?��?��
    mov ax, ss                          ; SS ?���??��?�� ????��
    mov qword[ rdi + ( 23 * 8 ) ], rax

    mov rax, rbp                        ; RBP?�� ????��?�� RSP ?���??��?�� ????��
    add rax, 16                         ; RSP ?���??��?��?�� push rbp??? Return Address�?
    mov qword[ rdi + ( 22 * 8 ) ], rax  ; ?��?��?�� 값으�? ????��
    
    pushfq                              ; RFLAGS ?���??��?�� ????��
    pop rax
    mov qword[ rdi + ( 21 * 8 ) ], rax

    mov ax, cs                          ; CS ?���??��?�� ????��
    mov qword[ rdi + ( 20 * 8 ) ], rax
    
    mov rax, qword[ rbp + 8 ]           ; RIP ?���??��?���? Return Address�? ?��?��?��?�� 
    mov qword[ rdi + ( 19 * 8 ) ], rax  ; ?��?�� 콘텍?��?�� 복원 ?��?�� ?�� ?��?���? ?��출한 
                                        ; ?��치로 ?��?��?���? ?��
    
    ; ????��?�� ?���??��?���? 복구?�� ?�� ?��?��?��?���? 발생?��?�� ?��처럼 ?��머�?? 콘텍?��?���? 모두 ????��
    pop rax
    pop rbp
    
    ; �??�� ?���?분에 SS, RSP, RFLAGS, CS, RIP ?���??��?���? ????��?��?���?�?, ?��?�� ?��?��?��
    ; push 명령?���? 콘텍?��?���? ????��?���? ?��?�� ?��?��?�� �?�?
    add rdi, ( 19 * 8 )
    mov rsp, rdi
    sub rdi, ( 19 * 8 )
    
    ; ?��머�?? ?���??��?���? 모두 Context ?��료구조에 ????��
    KSAVECONTEXT

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; ?��?�� ?��?��?��?�� 콘텍?��?�� 복원
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.LoadContext:
    mov rsp, rsi
    
    ; Context ?��료구조에?�� ?���??��?���? 복원
    KLOADCONTEXT
    iretq

; ���μ����� ���� ��
;   PARAM: ����
kHlt:
    hlt     ; ���μ����� ��� ���·� ���Խ�Ŵ
    hlt
    ret
    
; �׽�Ʈ�� ������ �ϳ��� ������� ó��
;	Destination�� Compare�� ���Ͽ� ���ٸ�, Destination�� Source ���� ����
;   PARAM: ���� ������ ��巹��(Destination, rdi), ���� ��(Compare, rsi), 
;          ������ ��(Source, rdx) 
kTestAndSet:
    mov rax, rsi        ; �� ��° �Ķ������ ���� ���� RAX �������Ϳ� ���� 
    
    ; RAX �������Ϳ� ����� ���� ���� ù ��° �Ķ������ �޸� ��巹���� ����
    ; ���Ͽ� �� ���� ���ٸ� �� ��° �Ķ������ ���� ù ��° �Ķ���Ͱ� ����Ű��
    ; ��巹���� ����
    lock cmpxchg byte [ rdi ], dl
    je .SUCCESS         ; ZF ��Ʈ�� 1�̸� ���ٴ� ���̹Ƿ� .SUCCESS�� �̵�
    
.NOTSAME:               ; Destination�� Compare�� �ٸ� ���
    mov rax, 0x00
    ret
    
.SUCCESS:               ; Destination�� Compare�� ���� ���
    mov rax, 0x01
    ret


