
/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS?? ?¬?©?  ? ?Έλ¦¬ν° ?¨?? κ΄?? ¨? ??Ό
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>
/**
 *  λ©λͺ¨λ¦¬λ?? ?Ή?  κ°μΌλ‘? μ±μ??
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
 *  λ©λͺ¨λ¦? λ³΅μ¬
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
 *  λ©λͺ¨λ¦? λΉκ΅
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
 *  RFLAGS ? μ§??€?°? ?Έ?°?½?Έ ??κ·Έλ?? λ³?κ²½νκ³? ?΄?  ?Έ?°?½?Έ ??κ·Έμ ??λ₯? λ°ν
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // ?΄? ? RFLAGS ? μ§??€?° κ°μ ?½??? ?€? ?Έ?°?½?Έ κ°??₯/λΆκ?? μ²λ¦¬
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    // ?΄?  RFLAGS ? μ§??€?°? IF λΉνΈ(λΉνΈ 9)λ₯? ??Έ??¬ ?΄? ? ?Έ?°?½?Έ ??λ₯? λ°ν
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

// ?¨? μ΄? ?¬κΈ?(Mbyte ?¨?)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte ?΄?? ?μΉλ???° ?¨ ?¬κΈ°λ?? μ²΄ν¬
 *      μ΅μ΄ λΆ?? κ³Όμ ?? ?λ²λ§ ?ΈμΆν΄?Ό ?¨
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    // 64Mbyte(0x4000000)λΆ??° 4Mbyte?¨?λ‘? κ²??¬ ??
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // ?΄? ? λ©λͺ¨λ¦¬μ ?? κ°μ ????₯
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678? ?¨? ?½?? ? λ¬Έμ κ°? ?? κ³³κΉμ§?λ₯? ? ?¨? λ©λͺ¨λ¦? 
        // ??­?Όλ‘? ?Έ? 
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // ?΄?  λ©λͺ¨λ¦? κ°μΌλ‘? λ³΅μ
        *pdwCurrentAddress = dwPreviousValue;
        // ?€? 4Mbyte ?μΉλ‘ ?΄?
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // μ²΄ν¬κ°? ?±κ³΅ν ?΄?? ?€λ₯? 1Mbyteλ‘? ???΄ Mbyte ?¨?λ‘? κ³μ°
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  RAM ?¬κΈ°λ?? λ°ν
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() ?¨?? ?΄λΆ? κ΅¬ν
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch( iRadix )
    {
        // 16μ§μ
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;
        
        // 10μ§μ ?? κΈ°ν??
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16μ§μ λ¬Έμ?΄? QWORDλ‘? λ³?? 
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    // λ¬Έμ?΄? ?λ©΄μ μ°¨λ??λ‘? λ³??
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
 *  10μ§μ λ¬Έμ?΄? long?Όλ‘? λ³??
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // ???΄λ©? -λ₯? ? ?Έ?κ³? ?λ¨Έμ??λ₯? λ¨Όμ?? long?Όλ‘? λ³??
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // λ¬Έμ?΄? ?λ©΄μ μ°¨λ??λ‘? λ³??
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // ???΄λ©? - μΆκ??
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() ?¨?? ?΄λΆ? κ΅¬ν
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch( iRadix )
    {
        // 16μ§μ
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;
        
        // 10μ§μ ?? κΈ°ν??
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  16μ§μ κ°μ λ¬Έμ?΄λ‘? λ³??
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0?΄ ?€?΄?€λ©? λ°λ‘ μ²λ¦¬
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // λ²νΌ? 1? ?λ¦¬λ???° 16, 256, ...? ?λ¦? ??λ‘? ?«? ?½?
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
    
    // λ²νΌ? ?€?΄?? λ¬Έμ?΄? ?€μ§μ΄? ... 256, 16, 1? ?λ¦? ??λ‘? λ³?κ²?
    kReverseString( pcBuffer );
    return i;
}

/**
 *  10μ§μ κ°μ λ¬Έμ?΄λ‘? λ³??
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0?΄ ?€?΄?€λ©? λ°λ‘ μ²λ¦¬
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // λ§μ½ ???΄λ©? μΆλ ₯ λ²νΌ? '-'λ₯? μΆκ???κ³? ??λ‘? λ³??
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

    // λ²νΌ? 1? ?λ¦¬λ???° 10, 100, 1000 ...? ?λ¦? ??λ‘? ?«? ?½?
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // λ²νΌ? ?€?΄?? λ¬Έμ?΄? ?€μ§μ΄? ... 1000, 100, 10, 1? ?λ¦? ??λ‘? λ³?κ²?
    if( pcBuffer[ 0 ] == '-' )
    {
        // ???Έ κ²½μ°? λΆ??Έλ₯? ? ?Έ?κ³? λ¬Έμ?΄? ?€μ§μ
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }
    
    return i;
}

/**
 *  λ¬Έμ?΄? ??λ₯? ?€μ§μ
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // λ¬Έμ?΄? κ°??΄?°λ₯? μ€μ¬?Όλ‘? μ’?/?°λ₯? λ°κΏ? ??λ₯? ?€μ§μ
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sprintf() ?¨?? ?΄λΆ? κ΅¬ν
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    // κ°?λ³? ?Έ?λ₯? κΊΌλ΄? vsprintf() ?¨?? ?κ²¨μ€
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  vsprintf() ?¨?? ?΄λΆ? κ΅¬ν
 *      λ²νΌ? ?¬λ§? λ¬Έμ?΄? ?°?Ό ?°?΄?°λ₯? λ³΅μ¬
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    
    // ?¬λ§? λ¬Έμ?΄? κΈΈμ΄λ₯? ?½?΄? λ¬Έμ?΄? κΈΈμ΄λ§νΌ ?°?΄?°λ₯? μΆλ ₯ λ²νΌ? μΆλ ₯
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // %λ‘? ???λ©? ?°?΄?° ???? λ¬Έμλ‘? μ²λ¦¬
        if( pcFormatString[ i ] == '%' ) 
        {
            // % ?€?? λ¬Έμλ‘? ?΄?
            i++;
            switch( pcFormatString[ i ] ) 
            {
                // λ¬Έμ?΄ μΆλ ₯  
            case 's':
                // κ°?λ³? ?Έ?? ?€?΄?? ??Όλ―Έν°λ₯? λ¬Έμ?΄ ?????Όλ‘? λ³??
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // λ¬Έμ?΄? κΈΈμ΄λ§νΌ? μΆλ ₯ λ²νΌλ‘? λ³΅μ¬?κ³? μΆλ ₯? κΈΈμ΄λ§νΌ 
                // λ²νΌ? ?Έ?±?€λ₯? ?΄?
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // λ¬Έμ μΆλ ₯
            case 'c':
                // κ°?λ³? ?Έ?? ?€?΄?? ??Όλ―Έν°λ₯? λ¬Έμ ?????Όλ‘? λ³????¬ 
                // μΆλ ₯ λ²νΌ? λ³΅μ¬?κ³? λ²νΌ? ?Έ?±?€λ₯? 1λ§νΌ ?΄?
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // ? ? μΆλ ₯
            case 'd':
            case 'i':
                // κ°?λ³? ?Έ?? ?€?΄?? ??Όλ―Έν°λ₯? ? ? ?????Όλ‘? λ³????¬
                // μΆλ ₯ λ²νΌ? λ³΅μ¬?κ³? μΆλ ₯? κΈΈμ΄λ§νΌ λ²νΌ? ?Έ?±?€λ₯? ?΄?
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
                // 4λ°μ΄?Έ Hex μΆλ ₯
            case 'x':
            case 'X':
                // κ°?λ³? ?Έ?? ?€?΄?? ??Όλ―Έν°λ₯? DWORD ?????Όλ‘? λ³????¬
                // μΆλ ₯ λ²νΌ? λ³΅μ¬?κ³? μΆλ ₯? κΈΈμ΄λ§νΌ λ²νΌ? ?Έ?±?€λ₯? ?΄?
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8λ°μ΄?Έ Hex μΆλ ₯
            case 'q':
            case 'Q':
            case 'p':
                // κ°?λ³? ?Έ?? ?€?΄?? ??Όλ―Έν°λ₯? QWORD ?????Όλ‘? λ³????¬
                // μΆλ ₯ λ²νΌ? λ³΅μ¬?κ³? μΆλ ₯? κΈΈμ΄λ§νΌ λ²νΌ? ?Έ?±?€λ₯? ?΄?
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;
            
                // ?? ?΄?Ή?μ§? ??Όλ©? λ¬Έμλ₯? κ·Έλ??λ‘? μΆλ ₯?κ³? λ²νΌ? ?Έ?±?€λ₯?
                // 1λ§νΌ ?΄?
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // ?Όλ°? λ¬Έμ?΄ μ²λ¦¬
        else
        {
            // λ¬Έμλ₯? κ·Έλ??λ‘? μΆλ ₯?κ³? λ²νΌ? ?Έ?±?€λ₯? 1λ§νΌ ?΄?
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // NULL? μΆκ????¬ ?? ? λ¬Έμ?΄λ‘? λ§λ€κ³? μΆλ ₯? λ¬Έμ? κΈΈμ΄λ₯? λ°ν
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}
