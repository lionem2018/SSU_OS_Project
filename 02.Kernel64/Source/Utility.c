/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS?—?„œ ?‚¬?š©?•  ?œ ?‹¸ë¦¬í‹° ?•¨?ˆ˜?— ê´?? ¨?œ ?ŒŒ?¼
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>

// PIT ÄÁÆ®·Ñ·¯°¡ ¹ß»ıÇÑ È½¼ö¸¦ ÀúÀåÇÒ Ä«¿îÅÍ
volatile QWORD g_qwTickCount = 0;

/**
 *  ë©”ëª¨ë¦¬ë?? ?Š¹? • ê°’ìœ¼ë¡? ì±„ì??
 */
void kMemSet( void* pvDestination, BYTE bData, int iSize )
{
    int i;

    for( i = 0 ; i < iSize ; i++ )
    {
        ( ( char* ) pvDestination )[ i ] = bData;
    }
}

/**
 *  ë©”ëª¨ë¦? ë³µì‚¬
 */
int kMemCpy( void* pvDestination, const void* pvSource, int iSize )
{
    int i;

    for( i = 0 ; i < iSize ; i++ )
    {
        ( ( char* ) pvDestination )[ i ] = ( ( char* ) pvSource )[ i ];
    }

    return iSize;
}

/**
 *  ë©”ëª¨ë¦? ë¹„êµ
 */
int kMemCmp( const void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    char cTemp;

    for( i = 0 ; i < iSize ; i++ )
    {
        cTemp = ( ( char* ) pvDestination )[ i ] - ( ( char* ) pvSource )[ i ];
        if( cTemp != 0 )
        {
            return ( int ) cTemp;
        }
    }
    return 0;
}

/**
 *  RFLAGS ? ˆì§??Š¤?„°?˜ ?¸?„°?Ÿ½?Š¸ ?”Œ?˜ê·¸ë?? ë³?ê²½í•˜ê³? ?´? „ ?¸?„°?Ÿ½?Š¸ ?”Œ?˜ê·¸ì˜ ?ƒ?ƒœë¥? ë°˜í™˜
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // ?´? „?˜ RFLAGS ? ˆì§??Š¤?„° ê°’ì„ ?½??? ?’¤?— ?¸?„°?Ÿ½?Š¸ ê°??Š¥/ë¶ˆê?? ì²˜ë¦¬
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    // ?´? „ RFLAGS ? ˆì§??Š¤?„°?˜ IF ë¹„íŠ¸(ë¹„íŠ¸ 9)ë¥? ?™•?¸?•˜?—¬ ?´? „?˜ ?¸?„°?Ÿ½?Š¸ ?ƒ?ƒœë¥? ë°˜í™˜
    if( qwRFLAGS & 0x0200 )
    {
        return TRUE;
    }
    return FALSE;
}

int kStrLen( const char* pcBuffer )
{
    int i;
    
    for( i = 0 ; ; i++ )
    {
        if( pcBuffer[ i ] == '\0' )
        {
            break;
        }
    }
    return i;
}

// ?¨?˜ ì´? ?¬ê¸?(Mbyte ?‹¨?œ„)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte ?´?ƒ?˜ ?œ„ì¹˜ë???„° ?¨ ?¬ê¸°ë?? ì²´í¬
 *      ìµœì´ˆ ë¶??Œ… ê³¼ì •?—?„œ ?•œë²ˆë§Œ ?˜¸ì¶œí•´?•¼ ?•¨
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    // 64Mbyte(0x4000000)ë¶??„° 4Mbyte?‹¨?œ„ë¡? ê²??‚¬ ?‹œ?‘
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // ?´? „?— ë©”ëª¨ë¦¬ì— ?ˆ?˜ ê°’ì„ ????¥
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678?„ ?¨?„œ ?½?—ˆ?„ ?•Œ ë¬¸ì œê°? ?—†?Š” ê³³ê¹Œì§?ë¥? ?œ ?š¨?•œ ë©”ëª¨ë¦? 
        // ?˜?—­?œ¼ë¡? ?¸? •
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // ?´? „ ë©”ëª¨ë¦? ê°’ìœ¼ë¡? ë³µì›
        *pdwCurrentAddress = dwPreviousValue;
        // ?‹¤?Œ 4Mbyte ?œ„ì¹˜ë¡œ ?´?™
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // ì²´í¬ê°? ?„±ê³µí•œ ?–´?“œ? ˆ?Š¤ë¥? 1Mbyteë¡? ?‚˜?ˆ„?–´ Mbyte ?‹¨?œ„ë¡? ê³„ì‚°
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  RAM ?¬ê¸°ë?? ë°˜í™˜
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() ?•¨?ˆ˜?˜ ?‚´ë¶? êµ¬í˜„
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch( iRadix )
    {
        // 16ì§„ìˆ˜
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;
        
        // 10ì§„ìˆ˜ ?˜?Š” ê¸°í??
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16ì§„ìˆ˜ ë¬¸ì?—´?„ QWORDë¡? ë³??™˜ 
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    // ë¬¸ì?—´?„ ?Œë©´ì„œ ì°¨ë??ë¡? ë³??™˜
    for( i = 0 ; pcBuffer[ i ] != '\0' ; i++ )
    {
        qwValue *= 16;
        if( ( 'A' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'Z' ) )
        {
            qwValue += ( pcBuffer[ i ] - 'A' ) + 10;
        }
        else if( ( 'a' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'z' ) )
        {
            qwValue += ( pcBuffer[ i ] - 'a' ) + 10;
        }
        else 
        {
            qwValue += pcBuffer[ i ] - '0';
        }
    }
    return qwValue;
}

/**
 *  10ì§„ìˆ˜ ë¬¸ì?—´?„ long?œ¼ë¡? ë³??™˜
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // ?Œ?ˆ˜?´ë©? -ë¥? ? œ?™¸?•˜ê³? ?‚˜ë¨¸ì??ë¥? ë¨¼ì?? long?œ¼ë¡? ë³??™˜
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // ë¬¸ì?—´?„ ?Œë©´ì„œ ì°¨ë??ë¡? ë³??™˜
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // ?Œ?ˆ˜?´ë©? - ì¶”ê??
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() ?•¨?ˆ˜?˜ ?‚´ë¶? êµ¬í˜„
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch( iRadix )
    {
        // 16ì§„ìˆ˜
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;
        
        // 10ì§„ìˆ˜ ?˜?Š” ê¸°í??
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  16ì§„ìˆ˜ ê°’ì„ ë¬¸ì?—´ë¡? ë³??™˜
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0?´ ?“¤?–´?˜¤ë©? ë°”ë¡œ ì²˜ë¦¬
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // ë²„í¼?— 1?˜ ?ë¦¬ë???„° 16, 256, ...?˜ ?ë¦? ?ˆœ?„œë¡? ?ˆ«? ?‚½?…
    for( i = 0 ; qwValue > 0 ; i++ )
    {
        qwCurrentValue = qwValue % 16;
        if( qwCurrentValue >= 10 )
        {
            pcBuffer[ i ] = 'A' + ( qwCurrentValue - 10 );
        }
        else
        {
            pcBuffer[ i ] = '0' + qwCurrentValue;
        }
        
        qwValue = qwValue / 16;
    }
    pcBuffer[ i ] = '\0';
    
    // ë²„í¼?— ?“¤?–´?ˆ?Š” ë¬¸ì?—´?„ ?’¤ì§‘ì–´?„œ ... 256, 16, 1?˜ ?ë¦? ?ˆœ?„œë¡? ë³?ê²?
    kReverseString( pcBuffer );
    return i;
}

/**
 *  10ì§„ìˆ˜ ê°’ì„ ë¬¸ì?—´ë¡? ë³??™˜
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0?´ ?“¤?–´?˜¤ë©? ë°”ë¡œ ì²˜ë¦¬
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // ë§Œì•½ ?Œ?ˆ˜?´ë©? ì¶œë ¥ ë²„í¼?— '-'ë¥? ì¶”ê???•˜ê³? ?–‘?ˆ˜ë¡? ë³??™˜
    if( lValue < 0 )
    {
        i = 1;
        pcBuffer[ 0 ] = '-';
        lValue = -lValue;
    }
    else
    {
        i = 0;
    }

    // ë²„í¼?— 1?˜ ?ë¦¬ë???„° 10, 100, 1000 ...?˜ ?ë¦? ?ˆœ?„œë¡? ?ˆ«? ?‚½?…
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // ë²„í¼?— ?“¤?–´?ˆ?Š” ë¬¸ì?—´?„ ?’¤ì§‘ì–´?„œ ... 1000, 100, 10, 1?˜ ?ë¦? ?ˆœ?„œë¡? ë³?ê²?
    if( pcBuffer[ 0 ] == '-' )
    {
        // ?Œ?ˆ˜?¸ ê²½ìš°?Š” ë¶??˜¸ë¥? ? œ?™¸?•˜ê³? ë¬¸ì?—´?„ ?’¤ì§‘ìŒ
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }
    
    return i;
}

/**
 *  ë¬¸ì?—´?˜ ?ˆœ?„œë¥? ?’¤ì§‘ìŒ
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // ë¬¸ì?—´?˜ ê°??š´?°ë¥? ì¤‘ì‹¬?œ¼ë¡? ì¢?/?š°ë¥? ë°”ê¿”?„œ ?ˆœ?„œë¥? ?’¤ì§‘ìŒ
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sprintf() ?•¨?ˆ˜?˜ ?‚´ë¶? êµ¬í˜„
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    // ê°?ë³? ?¸?ë¥? êº¼ë‚´?„œ vsprintf() ?•¨?ˆ˜?— ?„˜ê²¨ì¤Œ
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  vsprintf() ?•¨?ˆ˜?˜ ?‚´ë¶? êµ¬í˜„
 *      ë²„í¼?— ?¬ë§? ë¬¸ì?—´?— ?”°?¼ ?°?´?„°ë¥? ë³µì‚¬
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    
    // ?¬ë§? ë¬¸ì?—´?˜ ê¸¸ì´ë¥? ?½?–´?„œ ë¬¸ì?—´?˜ ê¸¸ì´ë§Œí¼ ?°?´?„°ë¥? ì¶œë ¥ ë²„í¼?— ì¶œë ¥
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // %ë¡? ?‹œ?‘?•˜ë©? ?°?´?„° ????… ë¬¸ìë¡? ì²˜ë¦¬
        if( pcFormatString[ i ] == '%' ) 
        {
            // % ?‹¤?Œ?˜ ë¬¸ìë¡? ?´?™
            i++;
            switch( pcFormatString[ i ] ) 
            {
                // ë¬¸ì?—´ ì¶œë ¥  
            case 's':
                // ê°?ë³? ?¸??— ?“¤?–´?ˆ?Š” ?ŒŒ?¼ë¯¸í„°ë¥? ë¬¸ì?—´ ????…?œ¼ë¡? ë³??™˜
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // ë¬¸ì?—´?˜ ê¸¸ì´ë§Œí¼?„ ì¶œë ¥ ë²„í¼ë¡? ë³µì‚¬?•˜ê³? ì¶œë ¥?•œ ê¸¸ì´ë§Œí¼ 
                // ë²„í¼?˜ ?¸?±?Š¤ë¥? ?´?™
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // ë¬¸ì ì¶œë ¥
            case 'c':
                // ê°?ë³? ?¸??— ?“¤?–´?ˆ?Š” ?ŒŒ?¼ë¯¸í„°ë¥? ë¬¸ì ????…?œ¼ë¡? ë³??™˜?•˜?—¬ 
                // ì¶œë ¥ ë²„í¼?— ë³µì‚¬?•˜ê³? ë²„í¼?˜ ?¸?±?Š¤ë¥? 1ë§Œí¼ ?´?™
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // ? •?ˆ˜ ì¶œë ¥
            case 'd':
            case 'i':
                // ê°?ë³? ?¸??— ?“¤?–´?ˆ?Š” ?ŒŒ?¼ë¯¸í„°ë¥? ? •?ˆ˜ ????…?œ¼ë¡? ë³??™˜?•˜?—¬
                // ì¶œë ¥ ë²„í¼?— ë³µì‚¬?•˜ê³? ì¶œë ¥?•œ ê¸¸ì´ë§Œí¼ ë²„í¼?˜ ?¸?±?Š¤ë¥? ?´?™
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
                // 4ë°”ì´?Š¸ Hex ì¶œë ¥
            case 'x':
            case 'X':
                // ê°?ë³? ?¸??— ?“¤?–´?ˆ?Š” ?ŒŒ?¼ë¯¸í„°ë¥? DWORD ????…?œ¼ë¡? ë³??™˜?•˜?—¬
                // ì¶œë ¥ ë²„í¼?— ë³µì‚¬?•˜ê³? ì¶œë ¥?•œ ê¸¸ì´ë§Œí¼ ë²„í¼?˜ ?¸?±?Š¤ë¥? ?´?™
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8ë°”ì´?Š¸ Hex ì¶œë ¥
            case 'q':
            case 'Q':
            case 'p':
                // ê°?ë³? ?¸??— ?“¤?–´?ˆ?Š” ?ŒŒ?¼ë¯¸í„°ë¥? QWORD ????…?œ¼ë¡? ë³??™˜?•˜?—¬
                // ì¶œë ¥ ë²„í¼?— ë³µì‚¬?•˜ê³? ì¶œë ¥?•œ ê¸¸ì´ë§Œí¼ ë²„í¼?˜ ?¸?±?Š¤ë¥? ?´?™
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;
            
                // ?œ„?— ?•´?‹¹?•˜ì§? ?•Š?œ¼ë©? ë¬¸ìë¥? ê·¸ë??ë¡? ì¶œë ¥?•˜ê³? ë²„í¼?˜ ?¸?±?Š¤ë¥?
                // 1ë§Œí¼ ?´?™
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // ?¼ë°? ë¬¸ì?—´ ì²˜ë¦¬
        else
        {
            // ë¬¸ìë¥? ê·¸ë??ë¡? ì¶œë ¥?•˜ê³? ë²„í¼?˜ ?¸?±?Š¤ë¥? 1ë§Œí¼ ?´?™
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // NULL?„ ì¶”ê???•˜?—¬ ?™„? „?•œ ë¬¸ì?—´ë¡? ë§Œë“¤ê³? ì¶œë ¥?•œ ë¬¸ì?˜ ê¸¸ì´ë¥? ë°˜í™˜
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

/**
 *  Tick Count¸¦ ¹İÈ¯
 */
QWORD kGetTickCount( void )
{
    return g_qwTickCount;
}

/**
 *  ¹Ğ¸®¼¼ÄÁµå(milisecond) µ¿¾È ´ë±â
 */
void kSleep( QWORD qwMillisecond )
{
    QWORD qwLastTickCount;
    
    qwLastTickCount = g_qwTickCount;
    
    while( ( g_qwTickCount - qwLastTickCount ) <= qwMillisecond )
    {
        kSchedule();
    }
}

volatile QWORD gs_qwRandomValue = 0;

void srand(QWORD qwSeed){
    gs_qwRandomValue = qwSeed;
}

QWORD rand(void){
    gs_qwRandomValue = (gs_qwRandomValue * 412153 + 5571031) >> 16;
    return gs_qwRandomValue;
}

