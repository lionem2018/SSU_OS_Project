/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���ͷ�Ʈ �� ���� �ڵ鷯�� ���õ� �ҽ� ����
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Page.h"
#include "Console.h"

/**
 *  �������� ����ϴ� ���� �ڵ鷯
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    char hexadecimal[33]={0, };
    char faultAddress[33]={'0','x'};

    getFaultAddress();
    
    register long decimal asm("rax");
 
    int position = 0;
    while (1)
    {
        int mod = decimal % 16;    // 16���� �������� �� �������� ����
        if (mod < 10) // �������� 10���� ������
        {
            // ���� 0�� ASCII �ڵ� �� 48 + ������
            hexadecimal[position] = 48 + mod;
        }
        else    // �������� 10���� ũ�ų� ������
        {
            // ���������� 10�� �� ���� ���� �빮�� A�� ASCII �ڵ� �� 65�� ����
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16���� ���� ���� ����

        position++;    // �ڸ��� ����

        if (decimal == 0)    // ���� 0�̵Ǹ� �ݺ��� ����
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;
/*
    kPrintString( 0, 0, "====================================================" );
    kPrintString( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintString( 0, 2, "                    Address:                        " );
    kPrintString( 27, 2, faultAddress );
    kPrintString( 0, 3, "====================================================" );
*/
    kPrintStringXY( 0, 0, "====================================================" );
    kPrintStringXY( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintStringXY( 0, 2, "                    Vector:                         " );
    kPrintStringXY( 27, 2, vcBuffer );
    kPrintStringXY( 0, 3, "====================================================" );

    while( 1 ) ;
}

/**
 *  �������� ����ϴ� ���ͷ�Ʈ �ڵ鷯
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    //kPrintString( 70, 0, vcBuffer );
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================

    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  Ű���� ���ͷ�Ʈ�� �ڵ鷯
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ���� ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintString( 0, 0, vcBuffer );
    //=========================================================================

    // Ű���� ��Ʈ�ѷ����� �����͸� �о ASCII�� ��ȯ�Ͽ� ť�� ����
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

void kPageFaultExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    char hexadecimal[33]={0, };
    char faultAddress[33]={'0','x'};

    QWORD decimal = getFaultAddress();
    
    //register long decimal asm("rdx");

    DWORD * ErrorCode = (DWORD * ) 0x7FFFD0;
    _Bool isProtection = (*ErrorCode) & 0x1;
 
    int position = 0;
    while (1)
    {
        int mod = decimal % 16;    // 16���� �������� �� �������� ����
        if (mod < 10) // �������� 10���� ������
        {
            // ���� 0�� ASCII �ڵ� �� 48 + ������
            hexadecimal[position] = 48 + mod;
        }
        else    // �������� 10���� ũ�ų� ������
        {
            // ���������� 10�� �� ���� ���� �빮�� A�� ASCII �ڵ� �� 65�� ����
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16���� ���� ���� ����

        position++;    // �ڸ��� ����

        if (decimal == 0)    // ���� 0�̵Ǹ� �ݺ��� ����
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;

    kPrintString( 0, 0, "====================================================" );
    if(isProtection)
        kPrintString( 0, 1, "          Protection Fault Occur~!!!!         " );
    else
    {
        kPrintString( 0, 1, "             Page Fault Occur~!!!!            " );
    }
    
    kPrintString( 0, 2, "                Address:                        " );
    kPrintString( 25, 2, faultAddress );
    kPrintString( 0, 3, "====================================================" );


    kModifyPageTableEntryFlags(decimal);
    
    
}