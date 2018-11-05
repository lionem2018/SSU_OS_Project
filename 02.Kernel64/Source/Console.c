/**
 *  file    Console.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ì½˜ì†”?— ê´?? ¨?œ ?†Œ?Š¤ ?ŒŒ?¼
 */

#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"

// ì½˜ì†”?˜ ? •ë³´ë?? ê´?ë¦¬í•˜?Š” ?ë£Œêµ¬ì¡?
CONSOLEMANAGER gs_stConsoleManager = { 0, };

/**
 *  ì½˜ì†” ì´ˆê¸°?™”
 */
void kInitializeConsole( int iX, int iY )
{
    // ?ë£Œêµ¬ì¡°ë?? ëª¨ë‘ 0?œ¼ë¡? ì´ˆê¸°?™”
    kMemSet( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
    
    // ì»¤ì„œ ?œ„ì¹? ?„¤? •
    kSetCursor( iX, iY );
}

/**
 *  ì»¤ì„œ?˜ ?œ„ì¹˜ë?? ?„¤? •
 *      ë¬¸ìë¥? ì¶œë ¥?•  ?œ„ì¹˜ë„ ê°™ì´ ?„¤? •
 */
void kSetCursor( int iX, int iY ) 
{
    int iLinearValue;

    // ì»¤ì„œ?˜ ?œ„ì¹˜ë?? ê³„ì‚°
    iLinearValue = iY * CONSOLE_WIDTH + iX;

    // CRTC ì»¨íŠ¸ë¡? ?–´?“œ? ˆ?Š¤ ? ˆì§??Š¤?„°(?¬?Š¸ 0x3D4)?— 0x0Eë¥? ? „?†¡?•˜?—¬
    // ?ƒ?œ„ ì»¤ì„œ ?œ„ì¹? ? ˆì§??Š¤?„°ë¥? ?„ ?ƒ
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
    // CRTC ì»¨íŠ¸ë¡? ?°?´?„° ? ˆì§??Š¤?„°(?¬?Š¸ 0x3D5)?— ì»¤ì„œ?˜ ?ƒ?œ„ ë°”ì´?Š¸ë¥? ì¶œë ¥
    kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );

    // CRTC ì»¨íŠ¸ë¡? ?–´?“œ? ˆ?Š¤ ? ˆì§??Š¤?„°(?¬?Š¸ 0x3D4)?— 0x0Fë¥? ? „?†¡?•˜?—¬
    // ?•˜?œ„ ì»¤ì„œ ?œ„ì¹? ? ˆì§??Š¤?„°ë¥? ?„ ?ƒ
    kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
    // CRTC ì»¨íŠ¸ë¡? ?°?´?„° ? ˆì§??Š¤?„°(?¬?Š¸ 0x3D5)?— ì»¤ì„œ?˜ ?•˜?œ„ ë°”ì´?Š¸ë¥? ì¶œë ¥
    kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );

    // ë¬¸ìë¥? ì¶œë ¥?•  ?œ„ì¹? ?—…?°?´?Š¸
    gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

/**
 *  ?˜„?¬ ì»¤ì„œ?˜ ?œ„ì¹˜ë?? ë°˜í™˜
 */
void kGetCursor( int *piX, int *piY )
{
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

/**
 *  printf ?•¨?ˆ˜?˜ ?‚´ë¶? êµ¬í˜„
 */
void kPrintf( const char* pcFormatString, ... )
{
    va_list ap;
    char vcBuffer[ 1024 ];
    int iNextPrintOffset;

    // ê°?ë³? ?¸? ë¦¬ìŠ¤?Š¸ë¥? ?‚¬?š©?•´?„œ vsprintf()ë¡? ì²˜ë¦¬
    va_start( ap, pcFormatString );
    kVSPrintf( vcBuffer, pcFormatString, ap );
    va_end( ap );
    
    // ?¬ë§? ë¬¸ì?—´?„ ?™”ë©´ì— ì¶œë ¥
    iNextPrintOffset = kConsolePrintString( vcBuffer );
    
    // ì»¤ì„œ?˜ ?œ„ì¹˜ë?? ?—…?°?´?Š¸
    kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

/**
 *  \n, \t??? ê°™ì?? ë¬¸ìê°? ?¬?•¨?œ ë¬¸ì?—´?„ ì¶œë ¥?•œ ?›„, ?™”ë©´ìƒ?˜ ?‹¤?Œ ì¶œë ¥?•  ?œ„ì¹˜ë?? 
 *  ë°˜í™˜
 */
int kConsolePrintString( const char* pcBuffer )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i, j;
    int iLength;
    int iPrintOffset;
    
    // ë¬¸ì?—´?„ ì¶œë ¥?•  ?œ„ì¹˜ë?? ????¥
    iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;

    // ë¬¸ì?—´?˜ ê¸¸ì´ë§Œí¼ ?™”ë©´ì— ì¶œë ¥
    iLength = kStrLen( pcBuffer );    
    for( i = 0 ; i < iLength ; i++ )
    {
        // ê°œí–‰ ì²˜ë¦¬
        if( pcBuffer[ i ] == '\n' )
        {
            // ì¶œë ¥?•  ?œ„ì¹˜ë?? 80?˜ ë°°ìˆ˜ ì»¬ëŸ¼?œ¼ë¡? ?˜®ê¹?
            // ?˜„?¬ ?¼?¸?˜ ?‚¨??? ë¬¸ì?—´?˜ ?ˆ˜ë§Œí¼ ?”?•´?„œ ?‹¤?Œ ?¼?¸?œ¼ë¡? ?œ„ì¹˜ì‹œ?‚´
            iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
        }
        // ?ƒ­ ì²˜ë¦¬
        else if( pcBuffer[ i ] == '\t' )
        {
            // ì¶œë ¥?•  ?œ„ì¹˜ë?? 8?˜ ë°°ìˆ˜ ì»¬ëŸ¼?œ¼ë¡? ?˜®ê¹?
            iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
        }
        // ?¼ë°? ë¬¸ì?—´ ì¶œë ¥
        else
        {
            // ë¹„ë””?˜¤ ë©”ëª¨ë¦¬ì— ë¬¸ì??? ?†?„±?„ ?„¤? •?•˜?—¬ ë¬¸ìë¥? ì¶œë ¥?•˜ê³?
            // ì¶œë ¥?•  ?œ„ì¹˜ë?? ?‹¤?Œ?œ¼ë¡? ?´?™
            pstScreen[ iPrintOffset ].bCharactor = pcBuffer[ i ];
            pstScreen[ iPrintOffset ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            iPrintOffset++;
        }
        
        // ì¶œë ¥?•  ?œ„ì¹˜ê?? ?™”ë©´ì˜ ìµœëŒ“ê°?(80 * 25)?„ ë²—ì–´?‚¬?œ¼ë©? ?Š¤?¬ë¡? ì²˜ë¦¬
        if( iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
        {
            // ê°??¥ ?œ—ì¤„ì„ ? œ?™¸?•œ ?‚˜ë¨¸ì??ë¥? ?•œì¤? ?œ„ë¡? ë³µì‚¬
            kMemCpy( CONSOLE_VIDEOMEMORYADDRESS, 
                     CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof( CHARACTER ),
                     ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof( CHARACTER ) );

            // ê°??¥ ë§ˆì??ë§? ?¼?¸??? ê³µë°±?œ¼ë¡? ì±„ì??
            for( j = ( CONSOLE_HEIGHT - 1 ) * ( CONSOLE_WIDTH ) ; 
                 j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ; j++ )
            {
                // ê³µë°± ì¶œë ¥
                pstScreen[ j ].bCharactor = ' ';
                pstScreen[ j ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            }
            
            // ì¶œë ¥?•  ?œ„ì¹˜ë?? ê°??¥ ?•„?˜ìª? ?¼?¸?˜ ì²˜ìŒ?œ¼ë¡? ?„¤? •
            iPrintOffset = ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH;
        }
    }
    return iPrintOffset;
}

/**
 *  ? „ì²? ?™”ë©´ì„ ?‚­? œ
 */
void kClearScreen( void )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i;
    
    // ?™”ë©? ? „ì²´ë?? ê³µë°±?œ¼ë¡? ì±„ìš°ê³?, ì»¤ì„œ?˜ ?œ„ì¹˜ë?? 0, 0?œ¼ë¡? ?˜®ê¹?
    for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
    {
        pstScreen[ i ].bCharactor = ' ';
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
    
    // ì»¤ì„œë¥? ?™”ë©? ?ƒ?‹¨?œ¼ë¡? ?´?™
    kSetCursor( 0, 0 );
}

/**
 *  getch() ?•¨?ˆ˜?˜ êµ¬í˜„
 */
BYTE kGetCh( void )
{
    KEYDATA stData;
    
    // ?‚¤ê°? ?ˆŒ?Ÿ¬ì§ˆë•Œê¹Œì?? ???ê¸°í•¨
    while( 1 )
    {
        // ?‚¤ ??— ?°?´?„°ê°? ?ˆ˜?‹ ?  ?•Œê¹Œì?? ???ê¸?
        while( kGetKeyFromKeyQueue( &stData ) == FALSE )
        {
            ;
        }
        
        // ?‚¤ê°? ?ˆŒ? ¸?‹¤?Š” ?°?´?„°ê°? ?ˆ˜?‹ ?˜ë©? ASCII ì½”ë“œë¥? ë°˜í™˜
        if( stData.bFlags & KEY_FLAGS_DOWN )
        {
            return stData.bASCIICode;
        }
    }
}

/**
 *  ë¬¸ì?—´?„ X, Y ?œ„ì¹˜ì— ì¶œë ¥
 */
void kPrintStringXY( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    int i;
    
    // ë¹„ë””?˜¤ ë©”ëª¨ë¦? ?–´?“œ? ˆ?Š¤?—?„œ ?˜„?¬ ì¶œë ¥?•  ?œ„ì¹˜ë?? ê³„ì‚°
    pstScreen += ( iY * CONSOLE_WIDTH ) + iX;
    // ë¬¸ì?—´?˜ ê¸¸ì´ë§Œí¼ ë£¨í”„ë¥? ?Œë©´ì„œ ë¬¸ì??? ?†?„±?„ ????¥
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
}
