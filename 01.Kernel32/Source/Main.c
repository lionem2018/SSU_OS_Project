/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ?��?��?��?��?��?�� ?��?��?��?��?��?��?�� 커占?��?��?��?��?�� ?��?��?��?��?��?��?�� ?��?��?��?��?��?��?�� ?��?��?��?��?��?��
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte( void );

/**
 *  ?��?��뤄옙 ?��?��?��?��?��?��?�� C ?��?��?��?��? 커占?��?��?��?��?�� ?��?��?��?��?��?�� ?��?��븝옙?��?��?��
 *      ?��?��?��?��?��? ?��?��몌옙 ?��?��?��?��?��?��?�� ?��?��?��?��?��?�� ?��?��?��?��?��?�� ?��?��?��?��?��?��?�� ?��?��?��?��?��?��?��?��?��?�� ?��?��?��
 */
void Main( void )
{
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[ 13 ] = { 0, };

    kPrintString( 0, 4, "Protected Mode C Language Kernel Start......[Pass]" );

    // 최소 메모�? ?��기�?? 만족?��?�� �? �??��
    kPrintString( 0, 5, "Minimum Memory Size Check...................[    ]" );
    if( kIsMemoryEnough() == FALSE )
    {
        kPrintString( 45, 5, "Fail" );
        kPrintString( 0, 6, "Not Enough Memory~!! MINT64 OS Requires Over 64Mbyte Memory~!!" );
        while( 1 ) ;
    }
    else
    {
        kPrintString( 45, 5, "Pass" );
    }

    // IA-32e Kernal area initialize
    kPrintString( 0, 6, "IA-32e Kernel Area Initialize...............[    ]" );
    if( kInitializeKernel64Area() == FALSE )
    {
        kPrintString( 45, 6, "Fail" );
        kPrintString( 0, 7, "Kernel Area Initialization Fail~!!" );
        while( 1 ) ;
    }
    kPrintString( 45, 6, "Pass" );

    // IA-32e 모드 커널?�� ?��?�� ?��?���? ?��?���? ?��?��
    kPrintString( 0, 7, "IA-32e Page Tables Initialize...............[    ]" );
    kInitializePageTables();
    kPrintString( 45, 7, "Pass" );

    // ??·?ŒŒŒ­ ???¶?? ??��º? ?Ð±?
    kReadCPUID( 0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX );
    *( DWORD* ) vcVendorString = dwEBX;
    *( ( DWORD* ) vcVendorString + 1 ) = dwEDX;
    *( ( DWORD* ) vcVendorString + 2 ) = dwECX;
    kPrintString( 0, 8, "Processor Vendor String.....................[            ]" );
    kPrintString( 45, 8, vcVendorString );
    
    // 64º?Æ® ??¿ø ??¹? ?®??
    kReadCPUID( 0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX );
    kPrintString( 0, 9, "64bit Mode Support Check....................[    ]" );
    if( dwEDX & ( 1 << 29 ) )
    {
        kPrintString( 45, 9, "Pass" );
    }
    else
    {
        kPrintString( 45, 9, "Fail" );
        kPrintString( 0, 10, "This processor does not support 64bit mode~!!" );
        while( 1 ) ;
    }
    
    // IA-32e ?ð?? ?¿³??? 0x200000(2Mbyte) ????·¹œº·? ???¿
    kPrintString( 0, 10, "Copy IA-32e Kernel To 2M Address............[    ]" );
    kCopyKernel64ImageTo2Mbyte();
    kPrintString( 45, 10, "Pass" );
    
    // IA-32e ?ð??·? ????
    kPrintString( 0, 11, "Switch To IA-32e Mode" );
    kSwitchAndExecute64bitKernel();

    while( 1 ) ;
}

/**
 *  ?��?��?��?��?��?��?��?��?��?�� X, Y ?��?��?��치占?��?�� ?��?��?��?��?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ?��?��?��?��?��?��?�� ?��?��?��?��?��?��?��?�� ?��?��?��?��?��?��?��?��?��?�� ?��?��?��?��?��?��?��? ?��?��?��?��뱄옙?��?��?��?��? ?��?��?��?��?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL?��?��?�� ?��?��?��?��?��?�� ?��?��?��?��?��?��?��?��?�� ?��?��?��?��?��?��?�� ?��?��?��?��?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernel64Area( void )
{
     DWORD* pdwCurrentAddress;
    
    // 초기?���? ?��?��?�� ?��?��?��?��?�� 0x100000(1MB)?�� ?��?��
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 마�??�? ?��?��?��?��?�� 0x600000(6MB)까�?? 루프�? ?��면서 4바이?��?�� 0?���? 채�??
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {        
        *pdwCurrentAddress = 0x00;

        // 0?���? ????��?�� ?�� ?��?�� ?��?��?�� ?�� 0?�� ?��?���? ?��?���? ?��?�� ?��?��?��?���? 
        // ?��?��?��?��?�� 문제�? ?���? 것이�?�? ?��?��?�� 진행?���? ?���? 종료
        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }
        
        // ?��?�� ?��?��?��?���? ?��?��
        pdwCurrentAddress++;
    }
    
    return TRUE;
}

BOOL kIsMemoryEnough( void )
{
    DWORD* pdwCurrentAddress;
   
    // 0x100000(1MB)�??�� �??�� ?��?��
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 0x4000000(64MB)까�?? 루프�? ?��면서 ?��?��
    while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        *pdwCurrentAddress = 0x12345678;
        
        // 0x12345678�? ????��?�� ?�� ?��?�� ?��?��?�� ?�� 0x12345678?�� ?��?���? ?��?���? 
        // ?��?�� ?��?��?��?���? ?��?��?��?��?�� 문제�? ?���? 것이�?�? ?��?��?�� 진행?���? ?���? 종료
        if( *pdwCurrentAddress != 0x12345678 )
        {
           return FALSE;
        }
        
        // 1MB?�� ?��?��?��면서 ?��?��
        pdwCurrentAddress += ( 0x100000 / 4 );
    }
    return TRUE;
}

/**
 *  IA-32e ?ð?? ?¿³??? 0x200000(2Mbyte) ????·¹œº¿¡ º¹??
 */
void kCopyKernel64ImageTo2Mbyte( void )
{
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD* pdwSourceAddress,* pdwDestinationAddress;
    int i;
    
    // 0x7C05¿¡ ?? ?¿³? Œœ?? Œ?, 0x7C07¿¡ º??? ?ð?? ?¿³? Œœ?? Œ?°¡ ???? ???œ
    wTotalKernelSectorCount = *( ( WORD* ) 0x7C05 );
    wKernel32SectorCount = *( ( WORD* ) 0x7C07 );

    pdwSourceAddress = ( DWORD* ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );
    pdwDestinationAddress = ( DWORD* ) 0x200000;
    // IA-32e ?ð?? ?¿³? Œœ?? ??±????­ º¹??
    for( i = 0 ; i < 512 * ( wTotalKernelSectorCount - wKernel32SectorCount ) / 4;
        i++ )
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}