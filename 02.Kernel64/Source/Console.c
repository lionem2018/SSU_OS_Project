/**
 *  file    Console.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ์ฝ์? ๊ด?? จ? ??ค ??ผ
 */

#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"

// ์ฝ์? ? ๋ณด๋?? ๊ด?๋ฆฌํ? ?๋ฃ๊ตฌ์ก?
CONSOLEMANAGER gs_stConsoleManager = { 0, };

/**
 *  ์ฝ์ ์ด๊ธฐ?
 */
void kInitializeConsole( int iX, int iY )
{
    // ?๋ฃ๊ตฌ์กฐ๋?? ๋ชจ๋ 0?ผ๋ก? ์ด๊ธฐ?
    kMemSet( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
    
    // ์ปค์ ?์น? ?ค? 
    kSetCursor( iX, iY );
}

/**
 *  ์ปค์? ?์น๋?? ?ค? 
 *      ๋ฌธ์๋ฅ? ์ถ๋ ฅ?  ?์น๋ ๊ฐ์ด ?ค? 
 */
void kSetCursor( int iX, int iY ) 
{
    int iLinearValue;

    // ์ปค์? ?์น๋?? ๊ณ์ฐ
    iLinearValue = iY * CONSOLE_WIDTH + iX;

    // CRTC ์ปจํธ๋ก? ?ด?? ?ค ? ์ง??ค?ฐ(?ฌ?ธ 0x3D4)? 0x0E๋ฅ? ? ?ก??ฌ
    // ?? ์ปค์ ?์น? ? ์ง??ค?ฐ๋ฅ? ? ?
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
    // CRTC ์ปจํธ๋ก? ?ฐ?ด?ฐ ? ์ง??ค?ฐ(?ฌ?ธ 0x3D5)? ์ปค์? ?? ๋ฐ์ด?ธ๋ฅ? ์ถ๋ ฅ
    kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );

    // CRTC ์ปจํธ๋ก? ?ด?? ?ค ? ์ง??ค?ฐ(?ฌ?ธ 0x3D4)? 0x0F๋ฅ? ? ?ก??ฌ
    // ?? ์ปค์ ?์น? ? ์ง??ค?ฐ๋ฅ? ? ?
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
    // CRTC ์ปจํธ๋ก? ?ฐ?ด?ฐ ? ์ง??ค?ฐ(?ฌ?ธ 0x3D5)? ์ปค์? ?? ๋ฐ์ด?ธ๋ฅ? ์ถ๋ ฅ
    kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );

    // ๋ฌธ์๋ฅ? ์ถ๋ ฅ?  ?์น? ??ฐ?ด?ธ
    gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

/**
 *  ??ฌ ์ปค์? ?์น๋?? ๋ฐํ
 */
void kGetCursor( int *piX, int *piY )
{
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

/**
 *  printf ?จ?? ?ด๋ถ? ๊ตฌํ
 */
void kPrintf( const char* pcFormatString, ... )
{
    va_list ap;
    char vcBuffer[ 1024 ];
    int iNextPrintOffset;

    // ๊ฐ?๋ณ? ?ธ? ๋ฆฌ์ค?ธ๋ฅ? ?ฌ?ฉ?ด? vsprintf()๋ก? ์ฒ๋ฆฌ
    va_start( ap, pcFormatString );
    kVSPrintf( vcBuffer, pcFormatString, ap );
    va_end( ap );
    
    // ?ฌ๋ง? ๋ฌธ์?ด? ?๋ฉด์ ์ถ๋ ฅ
    iNextPrintOffset = kConsolePrintString( vcBuffer );
    
    // ์ปค์? ?์น๋?? ??ฐ?ด?ธ
    kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

/**
 *  \n, \t??? ๊ฐ์?? ๋ฌธ์๊ฐ? ?ฌ?จ? ๋ฌธ์?ด? ์ถ๋ ฅ? ?, ?๋ฉด์? ?ค? ์ถ๋ ฅ?  ?์น๋?? 
 *  ๋ฐํ
 */
int kConsolePrintString( const char* pcBuffer )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i, j;
    int iLength;
    int iPrintOffset;
    
    // ๋ฌธ์?ด? ์ถ๋ ฅ?  ?์น๋?? ????ฅ
    iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;

    // ๋ฌธ์?ด? ๊ธธ์ด๋งํผ ?๋ฉด์ ์ถ๋ ฅ
    iLength = kStrLen( pcBuffer );    
    for( i = 0 ; i < iLength ; i++ )
    {
        // ๊ฐํ ์ฒ๋ฆฌ
        if( pcBuffer[ i ] == '\n' )
        {
            // ์ถ๋ ฅ?  ?์น๋?? 80? ๋ฐฐ์ ์ปฌ๋ผ?ผ๋ก? ?ฎ๊น?
            // ??ฌ ?ผ?ธ? ?จ??? ๋ฌธ์?ด? ?๋งํผ ??ด? ?ค? ?ผ?ธ?ผ๋ก? ?์น์?ด
            iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
        }
        // ?ญ ์ฒ๋ฆฌ
        else if( pcBuffer[ i ] == '\t' )
        {
            // ์ถ๋ ฅ?  ?์น๋?? 8? ๋ฐฐ์ ์ปฌ๋ผ?ผ๋ก? ?ฎ๊น?
            iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
        }
        // ?ผ๋ฐ? ๋ฌธ์?ด ์ถ๋ ฅ
        else
        {
            // ๋น๋?ค ๋ฉ๋ชจ๋ฆฌ์ ๋ฌธ์??? ??ฑ? ?ค? ??ฌ ๋ฌธ์๋ฅ? ์ถ๋ ฅ?๊ณ?
            // ์ถ๋ ฅ?  ?์น๋?? ?ค??ผ๋ก? ?ด?
            pstScreen[ iPrintOffset ].bCharactor = pcBuffer[ i ];
            pstScreen[ iPrintOffset ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            iPrintOffset++;
        }
        
        // ์ถ๋ ฅ?  ?์น๊?? ?๋ฉด์ ์ต๋๊ฐ?(80 * 25)? ๋ฒ์ด?ฌ?ผ๋ฉ? ?ค?ฌ๋ก? ์ฒ๋ฆฌ
        if( iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
        {
            // ๊ฐ??ฅ ?์ค์ ? ?ธ? ?๋จธ์??๋ฅ? ?์ค? ?๋ก? ๋ณต์ฌ
            kMemCpy( CONSOLE_VIDEOMEMORYADDRESS, 
                     CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof( CHARACTER ),
                     ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof( CHARACTER ) );

            // ๊ฐ??ฅ ๋ง์??๋ง? ?ผ?ธ??? ๊ณต๋ฐฑ?ผ๋ก? ์ฑ์??
            for( j = ( CONSOLE_HEIGHT - 1 ) * ( CONSOLE_WIDTH ) ; 
                 j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ; j++ )
            {
                // ๊ณต๋ฐฑ ์ถ๋ ฅ
                pstScreen[ j ].bCharactor = ' ';
                pstScreen[ j ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            }
            
            // ์ถ๋ ฅ?  ?์น๋?? ๊ฐ??ฅ ??์ช? ?ผ?ธ? ์ฒ์?ผ๋ก? ?ค? 
            iPrintOffset = ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH;
        }
    }
    return iPrintOffset;
}

/**
 *  ? ์ฒ? ?๋ฉด์ ?ญ? 
 */
void kClearScreen( void )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i;
    
    // ?๋ฉ? ? ์ฒด๋?? ๊ณต๋ฐฑ?ผ๋ก? ์ฑ์ฐ๊ณ?, ์ปค์? ?์น๋?? 0, 0?ผ๋ก? ?ฎ๊น?
    for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
    {
        pstScreen[ i ].bCharactor = ' ';
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
    
    // ์ปค์๋ฅ? ?๋ฉ? ??จ?ผ๋ก? ?ด?
    kSetCursor( 0, 0 );
}

/**
 *  getch() ?จ?? ๊ตฌํ
 */
BYTE kGetCh( void )
{
    KEYDATA stData;
    
    // ?ค๊ฐ? ??ฌ์ง๋๊น์?? ???๊ธฐํจ
    while( 1 )
    {
        // ?ค ?? ?ฐ?ด?ฐ๊ฐ? ?? ?  ?๊น์?? ???๊ธ?
        while( kGetKeyFromKeyQueue( &stData ) == FALSE )
        {
            ;
        }
        
        // ?ค๊ฐ? ?? ธ?ค? ?ฐ?ด?ฐ๊ฐ? ?? ?๋ฉ? ASCII ์ฝ๋๋ฅ? ๋ฐํ
        if( stData.bFlags & KEY_FLAGS_DOWN )
        {
            return stData.bASCIICode;
        }
    }
}

/**
 *  ๋ฌธ์?ด? X, Y ?์น์ ์ถ๋ ฅ
 */
void kPrintStringXY( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i;
    
    // ๋น๋?ค ๋ฉ๋ชจ๋ฆ? ?ด?? ?ค?? ??ฌ ์ถ๋ ฅ?  ?์น๋?? ๊ณ์ฐ
    pstScreen += ( iY * CONSOLE_WIDTH ) + iX;
    // ๋ฌธ์?ด? ๊ธธ์ด๋งํผ ๋ฃจํ๋ฅ? ?๋ฉด์ ๋ฌธ์??? ??ฑ? ????ฅ
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
}
