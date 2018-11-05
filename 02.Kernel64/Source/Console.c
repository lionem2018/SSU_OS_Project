/**
 *  file    Console.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   콘솔?�� �??��?�� ?��?�� ?��?��
 */

#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"

// 콘솔?�� ?��보�?? �?리하?�� ?��료구�?
CONSOLEMANAGER gs_stConsoleManager = { 0, };

/**
 *  콘솔 초기?��
 */
void kInitializeConsole( int iX, int iY )
{
    // ?��료구조�?? 모두 0?���? 초기?��
    kMemSet( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
    
    // 커서 ?���? ?��?��
    kSetCursor( iX, iY );
}

/**
 *  커서?�� ?��치�?? ?��?��
 *      문자�? 출력?�� ?��치도 같이 ?��?��
 */
void kSetCursor( int iX, int iY ) 
{
    int iLinearValue;

    // 커서?�� ?��치�?? 계산
    iLinearValue = iY * CONSOLE_WIDTH + iX;

    // CRTC 컨트�? ?��?��?��?�� ?���??��?��(?��?�� 0x3D4)?�� 0x0E�? ?��?��?��?��
    // ?��?�� 커서 ?���? ?���??��?���? ?��?��
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
    // CRTC 컨트�? ?��?��?�� ?���??��?��(?��?�� 0x3D5)?�� 커서?�� ?��?�� 바이?���? 출력
    kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );

    // CRTC 컨트�? ?��?��?��?�� ?���??��?��(?��?�� 0x3D4)?�� 0x0F�? ?��?��?��?��
    // ?��?�� 커서 ?���? ?���??��?���? ?��?��
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
    // CRTC 컨트�? ?��?��?�� ?���??��?��(?��?�� 0x3D5)?�� 커서?�� ?��?�� 바이?���? 출력
    kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );

    // 문자�? 출력?�� ?���? ?��?��?��?��
    gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

/**
 *  ?��?�� 커서?�� ?��치�?? 반환
 */
void kGetCursor( int *piX, int *piY )
{
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

/**
 *  printf ?��?��?�� ?���? 구현
 */
void kPrintf( const char* pcFormatString, ... )
{
    va_list ap;
    char vcBuffer[ 1024 ];
    int iNextPrintOffset;

    // �?�? ?��?�� 리스?���? ?��?��?��?�� vsprintf()�? 처리
    va_start( ap, pcFormatString );
    kVSPrintf( vcBuffer, pcFormatString, ap );
    va_end( ap );
    
    // ?���? 문자?��?�� ?��면에 출력
    iNextPrintOffset = kConsolePrintString( vcBuffer );
    
    // 커서?�� ?��치�?? ?��?��?��?��
    kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

/**
 *  \n, \t??? 같�?? 문자�? ?��?��?�� 문자?��?�� 출력?�� ?��, ?��면상?�� ?��?�� 출력?�� ?��치�?? 
 *  반환
 */
int kConsolePrintString( const char* pcBuffer )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i, j;
    int iLength;
    int iPrintOffset;
    
    // 문자?��?�� 출력?�� ?��치�?? ????��
    iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;

    // 문자?��?�� 길이만큼 ?��면에 출력
    iLength = kStrLen( pcBuffer );    
    for( i = 0 ; i < iLength ; i++ )
    {
        // 개행 처리
        if( pcBuffer[ i ] == '\n' )
        {
            // 출력?�� ?��치�?? 80?�� 배수 컬럼?���? ?���?
            // ?��?�� ?��?��?�� ?��??? 문자?��?�� ?��만큼 ?��?��?�� ?��?�� ?��?��?���? ?��치시?��
            iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
        }
        // ?�� 처리
        else if( pcBuffer[ i ] == '\t' )
        {
            // 출력?�� ?��치�?? 8?�� 배수 컬럼?���? ?���?
            iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
        }
        // ?���? 문자?�� 출력
        else
        {
            // 비디?�� 메모리에 문자??? ?��?��?�� ?��?��?��?�� 문자�? 출력?���?
            // 출력?�� ?��치�?? ?��?��?���? ?��?��
            pstScreen[ iPrintOffset ].bCharactor = pcBuffer[ i ];
            pstScreen[ iPrintOffset ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            iPrintOffset++;
        }
        
        // 출력?�� ?��치�?? ?��면의 최댓�?(80 * 25)?�� 벗어?��?���? ?��?���? 처리
        if( iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
        {
            // �??�� ?��줄을 ?��?��?�� ?��머�??�? ?���? ?���? 복사
            kMemCpy( CONSOLE_VIDEOMEMORYADDRESS, 
                     CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof( CHARACTER ),
                     ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof( CHARACTER ) );

            // �??�� 마�??�? ?��?��??? 공백?���? 채�??
            for( j = ( CONSOLE_HEIGHT - 1 ) * ( CONSOLE_WIDTH ) ; 
                 j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ; j++ )
            {
                // 공백 출력
                pstScreen[ j ].bCharactor = ' ';
                pstScreen[ j ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            }
            
            // 출력?�� ?��치�?? �??�� ?��?���? ?��?��?�� 처음?���? ?��?��
            iPrintOffset = ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH;
        }
    }
    return iPrintOffset;
}

/**
 *  ?���? ?��면을 ?��?��
 */
void kClearScreen( void )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i;
    
    // ?���? ?��체�?? 공백?���? 채우�?, 커서?�� ?��치�?? 0, 0?���? ?���?
    for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
    {
        pstScreen[ i ].bCharactor = ' ';
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
    
    // 커서�? ?���? ?��?��?���? ?��?��
    kSetCursor( 0, 0 );
}

/**
 *  getch() ?��?��?�� 구현
 */
BYTE kGetCh( void )
{
    KEYDATA stData;
    
    // ?���? ?��?��질때까�?? ???기함
    while( 1 )
    {
        // ?�� ?��?�� ?��?��?���? ?��?��?�� ?��까�?? ???�?
        while( kGetKeyFromKeyQueue( &stData ) == FALSE )
        {
            ;
        }
        
        // ?���? ?��?��?��?�� ?��?��?���? ?��?��?���? ASCII 코드�? 반환
        if( stData.bFlags & KEY_FLAGS_DOWN )
        {
            return stData.bASCIICode;
        }
    }
}

/**
 *  문자?��?�� X, Y ?��치에 출력
 */
void kPrintStringXY( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i;
    
    // 비디?�� 메모�? ?��?��?��?��?��?�� ?��?�� 출력?�� ?��치�?? 계산
    pstScreen += ( iY * CONSOLE_WIDTH ) + iX;
    // 문자?��?�� 길이만큼 루프�? ?��면서 문자??? ?��?��?�� ????��
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
}
