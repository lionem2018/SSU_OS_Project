/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   인터럽트 및 예외 핸들러에 관련된 소스 파일
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Page.h"

/**
 *  공통으로 사용하는 예외 핸들러
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    char hexadecimal[33]={0, };
    char faultAddress[33]={'0','x'};

    getFaultAddress();
    
    register long decimal asm("rdx");
 
    int position = 0;
    while (1)
    {
        int mod = decimal % 16;    // 16으로 나누었을 때 나머지를 구함
        if (mod < 10) // 나머지가 10보다 작으면
        {
            // 숫자 0의 ASCII 코드 값 48 + 나머지
            hexadecimal[position] = 48 + mod;
        }
        else    // 나머지가 10보다 크거나 같으면
        {
            // 나머지에서 10을 뺀 값과 영문 대문자 A의 ASCII 코드 값 65를 더함
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16으로 나눈 몫을 저장

        position++;    // 자릿수 변경

        if (decimal == 0)    // 몫이 0이되면 반복을 끝냄
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;

    kPrintString( 0, 0, "====================================================" );
    kPrintString( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintString( 0, 2, "                    Address:                        " );
    kPrintString( 27, 2, faultAddress );
    kPrintString( 0, 3, "====================================================" );

    while( 1 ) ;
}

/**
 *  공통으로 사용하는 인터럽트 핸들러
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // 인터럽트가 발생했음을 알리려고 메시지를 출력하는 부분
    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생한 횟수 출력
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintString( 70, 0, vcBuffer );
    //=========================================================================

    // EOI 전송
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  키보드 인터럽트의 핸들러
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // 인터럽트가 발생했음을 알리려고 메시지를 출력하는 부분
    // 인터럽트 벡터를 화면 왼쪽 위에 2자리 정수로 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생한 횟수 출력
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintString( 0, 0, vcBuffer );
    //=========================================================================

    // 키보드 컨트롤러에서 데이터를 읽어서 ASCII로 변환하여 큐에 삽입
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI 전송
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

void kPageFaultExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    char hexadecimal[33]={0, };
    char faultAddress[33]={'0','x'};

    getFaultAddress();
    
    register long decimal asm("rdx");
 
    int position = 0;
    while (1)
    {
        int mod = decimal % 16;    // 16으로 나누었을 때 나머지를 구함
        if (mod < 10) // 나머지가 10보다 작으면
        {
            // 숫자 0의 ASCII 코드 값 48 + 나머지
            hexadecimal[position] = 48 + mod;
        }
        else    // 나머지가 10보다 크거나 같으면
        {
            // 나머지에서 10을 뺀 값과 영문 대문자 A의 ASCII 코드 값 65를 더함
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16으로 나눈 몫을 저장

        position++;    // 자릿수 변경

        if (decimal == 0)    // 몫이 0이되면 반복을 끝냄
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;

    kPrintString( 0, 0, "====================================================" );
    kPrintString( 0, 1, "           Page Fault Exception Occur~!!!!          " );
    kPrintString( 0, 2, "                    Address:                        " );
    kPrintString( 27, 2, faultAddress );
    kPrintString( 0, 3, "====================================================" );

    DWORD * ErrorCode = (DWORD * ) 0x7FFFD0;
    _Bool isProtection = (*ErrorCode) & 0x1;

    if(isProtection)
        kPrintString(19, 0, "PROTECTION");
    else
    {
        kPrintString(19, 0, "NON-PROTECTION");
    }

    kSetProtectionPageTableEntry();
    //kSetRWPageTableEntry();
}
