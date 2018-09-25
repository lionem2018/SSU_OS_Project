# file      EntryPoint.s
# date      2008/11/27
# author    kkamagui 
#           Copyright(c)2008 All rights reserved by kkamagui
# brief     ��ȣ ��� Ŀ�� ��Ʈ�� ����Ʈ�� ���õ� �ҽ� ����

[ORG 0x00]          ; �ڵ��� ���� ��巹���� 0x00���� ����
[BITS 16]           ; ������ �ڵ�� 16��Ʈ �ڵ�� ����

SECTION .text       ; text ����(���׸�Ʈ)�� ����

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x1000  ; ��ȣ ��� ��Ʈ�� ����Ʈ�� ���� ��巹��(0x10000)�� 
                    ; ���׸�Ʈ �������� ������ ��ȯ
    mov ds, ax      ; DS ���׸�Ʈ �������Ϳ� ����
    mov es, ax      ; ES ���׸�Ʈ �������Ϳ� ����
    
    cli             ; ���ͷ�Ʈ�� �߻����� ���ϵ��� ����
    lgdt [ GDTR ]   ; GDTR �ڷᱸ���� ���μ����� �����Ͽ� GDT ���̺��� �ε�

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; ��ȣ���� ����
    ; Disable Paging, Disable Cache, Internal FPU, Disable Align Check, 
    ; Enable ProtectedMode
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax        ; CR0 ��Ʈ�� �������Ϳ� ������ ������ �÷��׸� �����Ͽ� 
                        ; ��ȣ ���� ��ȯ

    ; Ŀ�� �ڵ� ���׸�Ʈ�� 0x00�� �������� �ϴ� ������ ��ü�ϰ� EIP�� ���� 0x00�� �������� �缳��
    ; CS ���׸�Ʈ ������ : EIP
    jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; ��ȣ ���� ����
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]               ; ������ �ڵ�� 32��Ʈ �ڵ�� ����
PROTECTEDMODE:
    mov ax, 0x10        ; ��ȣ ��� Ŀ�ο� ������ ���׸�Ʈ ��ũ���͸� AX �������Ϳ� ����
    mov ds, ax          ; DS ���׸�Ʈ �����Ϳ� ����
    mov es, ax          ; ES ���׸�Ʈ �����Ϳ� ����
    mov fs, ax          ; FS ���׸�Ʈ �����Ϳ� ����
    mov gs, ax          ; GS ���׸�Ʈ �����Ϳ� ����
    
    ; ������ 0x00000000~0x0000FFFF ������ 64KB ũ��� ����
    mov ss, ax          ; SS ���׸�Ʈ �����Ϳ� ����
    mov esp, 0xFFFE     ; ESP ���������� ��巹���� 0xFFFE�� ����
    mov ebp, 0xFFFE     ; EBP ���������� ��巹���� 0xFFFE�� ����
    
    ; ȭ�鿡 ��ȣ ���� ��ȯ�Ǿ��ٴ� �޽����� ��´�.
    push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )    ; ����� �޽����� ��巹���� ���ÿ� ����
    push 3                                          ; ȭ�� Y ��ǥ(2)�� ���ÿ� ����
    push 0                                          ; ȭ�� X ��ǥ(0)�� ���ÿ� ����
    call PRINTMESSAGE                               ; PRINTMESSAGE �Լ� ȣ��
    add esp, 12                                     ; ������ �Ķ���� ����

    jmp dword 0x08: 0x10200               ; ���� ��ġ���� ���� ���� ����


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   �Լ� �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; �޽����� ����ϴ� �Լ�
;   ���ÿ� x ��ǥ, y ��ǥ, ���ڿ�
PRINTMESSAGE:
    push ebp        ; ���̽� ������ ��������(BP)�� ���ÿ� ����
    mov ebp, esp    ; ���̽� ������ ��������(BP)�� ���� ������ ��������(SP)�� ���� ����
    push esi        ; �Լ����� �ӽ÷� ����ϴ� �������ͷ� �Լ��� ������ �κп���
    push edi        ; ���ÿ� ���Ե� ���� ���� ���� ������ ����
    push eax
    push ecx
    push edx
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; X, Y�� ��ǥ�� ���� �޸��� ��巹���� �����
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Y ��ǥ�� �̿��ؼ� ���� ���� ��巹���� ����
    mov eax, dword [ ebp + 12 ] ; �Ķ���� 2(ȭ�� ��ǥ Y)�� EAX �������Ϳ� ����
    mov esi, 160                ; �� ������ ����Ʈ ��(2 * 80 �÷�)�� ESI �������Ϳ� ����
    mul esi                     ; EAX �������Ϳ� ESI �������͸� ���Ͽ� ȭ�� Y ��巹�� ���
    mov edi, eax                ; ���� ȭ�� Y ��巹���� EDI �������Ϳ� ����
    
    ; X �·Ḧ �̿��ؼ� 2�� ���� �� ���� ��巹���� ����
    mov eax, dword [ ebp + 8 ]  ; �Ķ���� 1(ȭ�� ��ǥ X)�� EAX �������Ϳ� ����
    mov esi, 2                  ; �� ���ڸ� ��Ÿ���� ����Ʈ ��(2)�� ESI �������Ϳ� ����
    mul esi                     ; EAX �������Ϳ� ESI �������͸� ���Ͽ� ȭ�� X ��巹���� ���
    add edi, eax                ; ȭ�� Y ��巹���� ���� X ��巹���� ���ؼ�
                                ; ���� ���� �޸� ��巹���� ���
                                
    ; ����� ���ڿ��� ��巹��      
    mov esi, dword [ ebp + 16 ] ; �Ķ���� 3(����� ���ڿ��� ��巹��)

.MESSAGELOOP:               ; �޽����� ����ϴ� ����
    mov cl, byte [ esi ]    ; ESI �������Ͱ� ����Ű�� ���ڿ� ��ġ���� �� ���ڸ� 
                            ; CL �������Ϳ� ����
                            ; CL �������ʹ� ECX ���������� ���� 1����Ʈ�� �ǹ�
                            ; ���ڿ��� 1����Ʈ�� ����ϹǷ� ECX ���������� ���� 1����Ʈ�� ���

    cmp cl, 0               ; ����� ���ڿ� 0�� ��
    je .MESSAGEEND          ; ������ ������ ���� 0�̸� ���ڿ��� ����Ǿ�����
                            ; �ǹ��ϹǷ� .MESSAGEEND�� �̵��Ͽ� ���� ��� ����

    mov byte [ edi + 0xB8000 ], cl  ; 0�� �ƴ϶�� ���� �޸� ��巹�� 
                                    ; 0xB8000 + EDI �� ���ڸ� ���
    
    add esi, 1              ; ESI �������Ϳ� 1�� ���Ͽ� ���� ���ڿ��� �̵�
    add edi, 2              ; EDI �������Ϳ� 2�� ���Ͽ� ���� �޸��� ���� ���� ��ġ�� �̵�
                            ; ���� �޸𸮴� (����, �Ӽ�)�� ������ �����ǹǷ� ���ڸ� ����Ϸ���
                            ; 2�� ���ؾ� ��

    jmp .MESSAGELOOP        ; �޽��� ��� ������ �̵��Ͽ� ���� ���ڸ� ���

.MESSAGEEND:
    pop edx     ; �Լ����� ����� ���� EDX �������ͺ��� EBP �������ͱ����� ���ÿ�
    pop ecx     ; ���Ե� ���� �̿��ؼ� ����
    pop eax     ; ������ ���� �������� �� �����Ͱ� ���� ���� ������
    pop edi     ; �ڷᱸ��(Last-In, First-Out)�̹Ƿ� ����(push)�� ��������
    pop esi     ; ����(pop) �ؾ� ��
    pop ebp     ; ���̽� ������ ��������(BP) ����
    ret         ; �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   ������ ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; �Ʒ��� �����͵��� 8byte�� ���� �����ϱ� ���� �߰�
align 8, db 0

; GDTR�� ���� 8byte�� �����ϱ� ���� �߰�
dw 0x0000
; GDTR �ڷᱸ�� ����
GDTR:
    dw GDTEND - GDT - 1         ; �Ʒ��� ��ġ�ϴ� GDT ���̺��� ��ü ũ��
    dd ( GDT - $$ + 0x10000 )   ; �Ʒ��� ��ġ�ϴ� GDT ���̺��� ���� ��巹��

; GDT ���̺� ����
GDT:
    ; ��(NULL) ��ũ����, �ݵ�� 0���� �ʱ�ȭ�ؾ� ��
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    ; ��ȣ ��� Ŀ�ο� �ڵ� ���׸�Ʈ ��ũ����
    CODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    ; ��ȣ ��� Ŀ�ο� ������ ���׸�Ʈ ��ũ����
    DATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
GDTEND:

; ��ȣ ���� ��ȯ�Ǿ��ٴ� �޽���
SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success~!!', 0

times 512 - ( $ - $$ )  db  0x00    ; 512����Ʈ�� ���߱� ���� ���� �κ��� 0���� ä��
