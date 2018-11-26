/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ?��?��?��?�� �? ?��?�� ?��?��?��?�� �??��?�� ?��?�� ?��?��
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Page.h"
#include "Console.h"
#include "RTC.h"
#include "Utility.h"
#include "Task.h"
#include "Descriptor.h"
/**
 *  공통?���? ?��?��?��?�� ?��?�� ?��?��?��
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
        int mod = decimal % 16;    // 16?���? ?��?��?��?�� ?�� ?��머�??�? 구함
        if (mod < 10) // ?��머�??�? 10보다 ?��?���?
        {
            // ?��?�� 0?�� ASCII 코드 �? 48 + ?��머�??
            hexadecimal[position] = 48 + mod;
        }
        else    // ?��머�??�? 10보다 ?��거나 같으�?
        {
            // ?��머�???��?�� 10?�� �? 값과 ?���? ???문자 A?�� ASCII 코드 �? 65�? ?��?��
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16?���? ?��?�� 몫을 ????��

        position++;    // ?��릿수 �?�?

        if (decimal == 0)    // 몫이 0?��?���? 반복?�� ?��?��
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // ?��?��?��?�� 벡터�? ?���? ?��른쪽 ?��?�� 2?���? ?��?���? 출력
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
 *  공통?���? ?��?��?��?�� ?��?��?��?�� ?��?��?��
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // ?��?��?��?���? 발생?��?��?�� ?��리려�? 메시�?�? 출력?��?�� �?�?
    // ?��?��?��?�� 벡터�? ?���? ?��른쪽 ?��?�� 2?���? ?��?���? 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생?�� ?��?�� 출력
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================

    // EOI ?��?��
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  ?��보드 ?��?��?��?��?�� ?��?��?��
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ?��?��?��?���? 발생?��?��?�� ?��리려�? 메시�?�? 출력?��?�� �?�?
    // ?��?��?��?�� 벡터�? ?���? ?���? ?��?�� 2?���? ?��?���? 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생?�� ?��?�� 출력
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // ?��보드 컨트롤러?��?�� ?��?��?���? ?��?��?�� ASCII�? �??��?��?�� ?��?�� ?��?��
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI ?��?��
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
    /*
    while (1)
    {
        int mod = decimal % 16;    // 16?���? ?��?��?��?�� ?�� ?��머�??�? 구함
        if (mod < 10) // ?��머�??�? 10보다 ?��?���?
        {
            // ?��?�� 0?�� ASCII 코드 �? 48 + ?��머�??
            hexadecimal[position] = 48 + mod;
        }
        else    // ?��머�??�? 10보다 ?��거나 같으�?
        {
            // ?��머�???��?�� 10?�� �? 값과 ?���? ???문자 A?�� ASCII 코드 �? 65�? ?��?��
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16?���? ?��?�� 몫을 ????��

        position++;    // ?��릿수 �?�?

        if (decimal == 0)    // 몫이 0?��?���? 반복?�� ?��?��
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // ?��?��?��?�� 벡터�? ?���? ?��른쪽 ?��?�� 2?���? ?��?���? 출력
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
    */ 
}
void kTimerHandler(int iVectorNumber){
    BYTE bHour, bMinute, bSecond; 
    char cHour[3] = {'\0'},cMinute[3] = {'\0'}, cSecond[3] = {'\0'};
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ?��?��?��?���? 발생?��?��?�� ?��리려�? 메시�?�? 출력?��?�� �?�?
    // ?��?��?��?�� 벡터�? ?���? ?���? ?��?�� 2?���? ?��?���? 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생?�� ?��?�� 출력
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================

    kReadRTCTime( &bHour, &bMinute, &bSecond );
    if(bHour < 10){
        kIToA(bHour,cHour, 10);
        kMemCpy(cHour +1 , cHour,1);
        cHour[0] = '0';
    }
    else
        kIToA(bHour,cHour, 10);

    if(bMinute < 10){
        kIToA(bMinute,cMinute, 10);
        kMemCpy(cMinute +1 , cMinute,1);
        cMinute[0] = '0';
    }

    else
        kIToA(bMinute,cMinute, 10);
        
    if(bSecond < 10){
        kIToA(bSecond,cSecond, 10);
        kMemCpy(cSecond +1 , cSecond,1);
        cSecond[0] = '0';
    }

    else
        kIToA(bSecond,cSecond, 10);
        
    kPrintStringXY( 71, 24, cHour);
    kPrintStringXY( 73, 24, ":");
    kPrintStringXY( 74, 24, cMinute);
    kPrintStringXY( 76, 24, ":");
    kPrintStringXY( 77, 24, cSecond);

    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );

    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );

    // Ÿ�̸� �߻� Ƚ���� ����
    g_qwTickCount++;

    // �½�ũ�� ����� ���μ����� �ð��� ����
    kDecreaseProcessorTime();
    // ���μ����� ����� �� �ִ� �ð��� �� ��ٸ� �½�ũ ��ȯ ����
    if( kIsProcessorTimeExpired() == TRUE )
    {
        kScheduleInInterrupt();
    }
    
}
