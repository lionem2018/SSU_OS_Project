/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS?��?�� ?��?��?�� ?��?��리티 ?��?��?�� �??��?�� ?��?��
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>

// PIT ��Ʈ�ѷ��� �߻��� Ƚ���� ������ ī����
volatile QWORD g_qwTickCount = 0;

/**
 *  메모리�?? ?��?�� 값으�? 채�??
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
 *  메모�? 복사
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
 *  메모�? 비교
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
 *  RFLAGS ?���??��?��?�� ?��?��?��?�� ?��?��그�?? �?경하�? ?��?�� ?��?��?��?�� ?��?��그의 ?��?���? 반환
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;

    // ?��?��?�� RFLAGS ?���??��?�� 값을 ?��??? ?��?�� ?��?��?��?�� �??��/불�?? 처리
    qwRFLAGS = kReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        kEnableInterrupt();
    }
    else
    {
        kDisableInterrupt();
    }

    // ?��?�� RFLAGS ?���??��?��?�� IF 비트(비트 9)�? ?��?��?��?�� ?��?��?�� ?��?��?��?�� ?��?���? 반환
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

// ?��?�� �? ?���?(Mbyte ?��?��)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte ?��?��?�� ?��치�???�� ?�� ?��기�?? 체크
 *      최초 �??�� 과정?��?�� ?��번만 ?��출해?�� ?��
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    // 64Mbyte(0x4000000)�??�� 4Mbyte?��?���? �??�� ?��?��
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // ?��?��?�� 메모리에 ?��?�� 값을 ????��
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678?�� ?��?�� ?��?��?�� ?�� 문제�? ?��?�� 곳까�?�? ?��?��?�� 메모�? 
        // ?��?��?���? ?��?��
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // ?��?�� 메모�? 값으�? 복원
        *pdwCurrentAddress = dwPreviousValue;
        // ?��?�� 4Mbyte ?��치로 ?��?��
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // 체크�? ?��공한 ?��?��?��?���? 1Mbyte�? ?��?��?�� Mbyte ?��?���? 계산
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  RAM ?��기�?? 반환
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() ?��?��?�� ?���? 구현
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch( iRadix )
    {
        // 16진수
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;
        
        // 10진수 ?��?�� 기�??
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16진수 문자?��?�� QWORD�? �??�� 
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    // 문자?��?�� ?��면서 차�??�? �??��
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
 *  10진수 문자?��?�� long?���? �??��
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // ?��?��?���? -�? ?��?��?���? ?��머�??�? 먼�?? long?���? �??��
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // 문자?��?�� ?��면서 차�??�? �??��
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // ?��?��?���? - 추�??
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() ?��?��?�� ?���? 구현
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch( iRadix )
    {
        // 16진수
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;
        
        // 10진수 ?��?�� 기�??
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  16진수 값을 문자?���? �??��
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0?�� ?��?��?���? 바로 처리
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // 버퍼?�� 1?�� ?��리�???�� 16, 256, ...?�� ?���? ?��?���? ?��?�� ?��?��
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
    
    // 버퍼?�� ?��?��?��?�� 문자?��?�� ?��집어?�� ... 256, 16, 1?�� ?���? ?��?���? �?�?
    kReverseString( pcBuffer );
    return i;
}

/**
 *  10진수 값을 문자?���? �??��
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0?�� ?��?��?���? 바로 처리
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // 만약 ?��?��?���? 출력 버퍼?�� '-'�? 추�???���? ?��?���? �??��
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

    // 버퍼?�� 1?�� ?��리�???�� 10, 100, 1000 ...?�� ?���? ?��?���? ?��?�� ?��?��
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // 버퍼?�� ?��?��?��?�� 문자?��?�� ?��집어?�� ... 1000, 100, 10, 1?�� ?���? ?��?���? �?�?
    if( pcBuffer[ 0 ] == '-' )
    {
        // ?��?��?�� 경우?�� �??���? ?��?��?���? 문자?��?�� ?��집음
        kReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        kReverseString( pcBuffer );
    }
    
    return i;
}

/**
 *  문자?��?�� ?��?���? ?��집음
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // 문자?��?�� �??��?���? 중심?���? �?/?���? 바꿔?�� ?��?���? ?��집음
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sprintf() ?��?��?�� ?���? 구현
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    // �?�? ?��?���? 꺼내?�� vsprintf() ?��?��?�� ?��겨줌
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  vsprintf() ?��?��?�� ?���? 구현
 *      버퍼?�� ?���? 문자?��?�� ?��?�� ?��?��?���? 복사
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    
    // ?���? 문자?��?�� 길이�? ?��?��?�� 문자?��?�� 길이만큼 ?��?��?���? 출력 버퍼?�� 출력
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // %�? ?��?��?���? ?��?��?�� ????�� 문자�? 처리
        if( pcFormatString[ i ] == '%' ) 
        {
            // % ?��?��?�� 문자�? ?��?��
            i++;
            switch( pcFormatString[ i ] ) 
            {
                // 문자?�� 출력  
            case 's':
                // �?�? ?��?��?�� ?��?��?��?�� ?��?��미터�? 문자?�� ????��?���? �??��
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // 문자?��?�� 길이만큼?�� 출력 버퍼�? 복사?���? 출력?�� 길이만큼 
                // 버퍼?�� ?��?��?���? ?��?��
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // 문자 출력
            case 'c':
                // �?�? ?��?��?�� ?��?��?��?�� ?��?��미터�? 문자 ????��?���? �??��?��?�� 
                // 출력 버퍼?�� 복사?���? 버퍼?�� ?��?��?���? 1만큼 ?��?��
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // ?��?�� 출력
            case 'd':
            case 'i':
                // �?�? ?��?��?�� ?��?��?��?�� ?��?��미터�? ?��?�� ????��?���? �??��?��?��
                // 출력 버퍼?�� 복사?���? 출력?�� 길이만큼 버퍼?�� ?��?��?���? ?��?��
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
                // 4바이?�� Hex 출력
            case 'x':
            case 'X':
                // �?�? ?��?��?�� ?��?��?��?�� ?��?��미터�? DWORD ????��?���? �??��?��?��
                // 출력 버퍼?�� 복사?���? 출력?�� 길이만큼 버퍼?�� ?��?��?���? ?��?��
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8바이?�� Hex 출력
            case 'q':
            case 'Q':
            case 'p':
                // �?�? ?��?��?�� ?��?��?��?�� ?��?��미터�? QWORD ????��?���? �??��?��?��
                // 출력 버퍼?�� 복사?���? 출력?�� 길이만큼 버퍼?�� ?��?��?���? ?��?��
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;
            
                // ?��?�� ?��?��?���? ?��?���? 문자�? 그�??�? 출력?���? 버퍼?�� ?��?��?���?
                // 1만큼 ?��?��
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // ?���? 문자?�� 처리
        else
        {
            // 문자�? 그�??�? 출력?���? 버퍼?�� ?��?��?���? 1만큼 ?��?��
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // NULL?�� 추�???��?�� ?��?��?�� 문자?���? 만들�? 출력?�� 문자?�� 길이�? 반환
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

/**
 *  Tick Count�� ��ȯ
 */
QWORD kGetTickCount( void )
{
    return g_qwTickCount;
}

/**
 *  �и�������(milisecond) ���� ���
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

