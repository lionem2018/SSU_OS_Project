/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS?? ?¬?©?  ? ?¸ë¦¬í° ?¨?? ê´?? ¨? ??¼
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>

// PIT ÄÁÆ®·Ñ·¯°¡ ¹ß»ýÇÑ È½¼ö¸¦ ÀúÀåÇÒ Ä«¿îÅÍ
volatile QWORD g_qwTickCount = 0;

/**
 *  ë©ëª¨ë¦¬ë?? ?¹?  ê°ì¼ë¡? ì±ì??
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
 *  ë©ëª¨ë¦? ë³µì¬
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
 *  ë©ëª¨ë¦? ë¹êµ
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
 *  RFLAGS ? ì§??¤?°? ?¸?°?½?¸ ??ê·¸ë?? ë³?ê²½íê³? ?´?  ?¸?°?½?¸ ??ê·¸ì ??ë¥? ë°í
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // ?´? ? RFLAGS ? ì§??¤?° ê°ì ?½??? ?¤? ?¸?°?½?¸ ê°??¥/ë¶ê?? ì²ë¦¬
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    // ?´?  RFLAGS ? ì§??¤?°? IF ë¹í¸(ë¹í¸ 9)ë¥? ??¸??¬ ?´? ? ?¸?°?½?¸ ??ë¥? ë°í
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

// ?¨? ì´? ?¬ê¸?(Mbyte ?¨?)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte ?´?? ?ì¹ë???° ?¨ ?¬ê¸°ë?? ì²´í¬
 *      ìµì´ ë¶?? ê³¼ì ?? ?ë²ë§ ?¸ì¶í´?¼ ?¨
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    // 64Mbyte(0x4000000)ë¶??° 4Mbyte?¨?ë¡? ê²??¬ ??
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // ?´? ? ë©ëª¨ë¦¬ì ?? ê°ì ????¥
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678? ?¨? ?½?? ? ë¬¸ì ê°? ?? ê³³ê¹ì§?ë¥? ? ?¨? ë©ëª¨ë¦? 
        // ??­?¼ë¡? ?¸? 
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // ?´?  ë©ëª¨ë¦? ê°ì¼ë¡? ë³µì
        *pdwCurrentAddress = dwPreviousValue;
        // ?¤? 4Mbyte ?ì¹ë¡ ?´?
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // ì²´í¬ê°? ?±ê³µí ?´?? ?¤ë¥? 1Mbyteë¡? ???´ Mbyte ?¨?ë¡? ê³ì°
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  RAM ?¬ê¸°ë?? ë°í
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() ?¨?? ?´ë¶? êµ¬í
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch( iRadix )
    {
        // 16ì§ì
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;
        
        // 10ì§ì ?? ê¸°í??
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16ì§ì ë¬¸ì?´? QWORDë¡? ë³?? 
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    // ë¬¸ì?´? ?ë©´ì ì°¨ë??ë¡? ë³??
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
 *  10ì§ì ë¬¸ì?´? long?¼ë¡? ë³??
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // ???´ë©? -ë¥? ? ?¸?ê³? ?ë¨¸ì??ë¥? ë¨¼ì?? long?¼ë¡? ë³??
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // ë¬¸ì?´? ?ë©´ì ì°¨ë??ë¡? ë³??
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // ???´ë©? - ì¶ê??
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() ?¨?? ?´ë¶? êµ¬í
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch( iRadix )
    {
        // 16ì§ì
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;
        
        // 10ì§ì ?? ê¸°í??
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  16ì§ì ê°ì ë¬¸ì?´ë¡? ë³??
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0?´ ?¤?´?¤ë©? ë°ë¡ ì²ë¦¬
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // ë²í¼? 1? ?ë¦¬ë???° 16, 256, ...? ?ë¦? ??ë¡? ?«? ?½?
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
    
    // ë²í¼? ?¤?´?? ë¬¸ì?´? ?¤ì§ì´? ... 256, 16, 1? ?ë¦? ??ë¡? ë³?ê²?
    kReverseString( pcBuffer );
    return i;
}

/**
 *  10ì§ì ê°ì ë¬¸ì?´ë¡? ë³??
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0?´ ?¤?´?¤ë©? ë°ë¡ ì²ë¦¬
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // ë§ì½ ???´ë©? ì¶ë ¥ ë²í¼? '-'ë¥? ì¶ê???ê³? ??ë¡? ë³??
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

    // ë²í¼? 1? ?ë¦¬ë???° 10, 100, 1000 ...? ?ë¦? ??ë¡? ?«? ?½?
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // ë²í¼? ?¤?´?? ë¬¸ì?´? ?¤ì§ì´? ... 1000, 100, 10, 1? ?ë¦? ??ë¡? ë³?ê²?
    if( pcBuffer[ 0 ] == '-' )
    {
        // ???¸ ê²½ì°? ë¶??¸ë¥? ? ?¸?ê³? ë¬¸ì?´? ?¤ì§ì
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }
    
    return i;
}

/**
 *  ë¬¸ì?´? ??ë¥? ?¤ì§ì
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // ë¬¸ì?´? ê°??´?°ë¥? ì¤ì¬?¼ë¡? ì¢?/?°ë¥? ë°ê¿? ??ë¥? ?¤ì§ì
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sprintf() ?¨?? ?´ë¶? êµ¬í
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    // ê°?ë³? ?¸?ë¥? êº¼ë´? vsprintf() ?¨?? ?ê²¨ì¤
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  vsprintf() ?¨?? ?´ë¶? êµ¬í
 *      ë²í¼? ?¬ë§? ë¬¸ì?´? ?°?¼ ?°?´?°ë¥? ë³µì¬
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    
    // ?¬ë§? ë¬¸ì?´? ê¸¸ì´ë¥? ?½?´? ë¬¸ì?´? ê¸¸ì´ë§í¼ ?°?´?°ë¥? ì¶ë ¥ ë²í¼? ì¶ë ¥
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // %ë¡? ???ë©? ?°?´?° ???? ë¬¸ìë¡? ì²ë¦¬
        if( pcFormatString[ i ] == '%' ) 
        {
            // % ?¤?? ë¬¸ìë¡? ?´?
            i++;
            switch( pcFormatString[ i ] ) 
            {
                // ë¬¸ì?´ ì¶ë ¥  
            case 's':
                // ê°?ë³? ?¸?? ?¤?´?? ??¼ë¯¸í°ë¥? ë¬¸ì?´ ?????¼ë¡? ë³??
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // ë¬¸ì?´? ê¸¸ì´ë§í¼? ì¶ë ¥ ë²í¼ë¡? ë³µì¬?ê³? ì¶ë ¥? ê¸¸ì´ë§í¼ 
                // ë²í¼? ?¸?±?¤ë¥? ?´?
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // ë¬¸ì ì¶ë ¥
            case 'c':
                // ê°?ë³? ?¸?? ?¤?´?? ??¼ë¯¸í°ë¥? ë¬¸ì ?????¼ë¡? ë³????¬ 
                // ì¶ë ¥ ë²í¼? ë³µì¬?ê³? ë²í¼? ?¸?±?¤ë¥? 1ë§í¼ ?´?
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // ? ? ì¶ë ¥
            case 'd':
            case 'i':
                // ê°?ë³? ?¸?? ?¤?´?? ??¼ë¯¸í°ë¥? ? ? ?????¼ë¡? ë³????¬
                // ì¶ë ¥ ë²í¼? ë³µì¬?ê³? ì¶ë ¥? ê¸¸ì´ë§í¼ ë²í¼? ?¸?±?¤ë¥? ?´?
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
                // 4ë°ì´?¸ Hex ì¶ë ¥
            case 'x':
            case 'X':
                // ê°?ë³? ?¸?? ?¤?´?? ??¼ë¯¸í°ë¥? DWORD ?????¼ë¡? ë³????¬
                // ì¶ë ¥ ë²í¼? ë³µì¬?ê³? ì¶ë ¥? ê¸¸ì´ë§í¼ ë²í¼? ?¸?±?¤ë¥? ?´?
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8ë°ì´?¸ Hex ì¶ë ¥
            case 'q':
            case 'Q':
            case 'p':
                // ê°?ë³? ?¸?? ?¤?´?? ??¼ë¯¸í°ë¥? QWORD ?????¼ë¡? ë³????¬
                // ì¶ë ¥ ë²í¼? ë³µì¬?ê³? ì¶ë ¥? ê¸¸ì´ë§í¼ ë²í¼? ?¸?±?¤ë¥? ?´?
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;
            
                // ?? ?´?¹?ì§? ??¼ë©? ë¬¸ìë¥? ê·¸ë??ë¡? ì¶ë ¥?ê³? ë²í¼? ?¸?±?¤ë¥?
                // 1ë§í¼ ?´?
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // ?¼ë°? ë¬¸ì?´ ì²ë¦¬
        else
        {
            // ë¬¸ìë¥? ê·¸ë??ë¡? ì¶ë ¥?ê³? ë²í¼? ?¸?±?¤ë¥? 1ë§í¼ ?´?
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // NULL? ì¶ê????¬ ?? ? ë¬¸ì?´ë¡? ë§ë¤ê³? ì¶ë ¥? ë¬¸ì? ê¸¸ì´ë¥? ë°í
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

/**
 *  Tick Count¸¦ ¹ÝÈ¯
 */
QWORD kGetTickCount( void )
{
    return g_qwTickCount;
}

/**
 *  ¹Ð¸®¼¼ÄÁµå(milisecond) µ¿¾È ´ë±â
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
