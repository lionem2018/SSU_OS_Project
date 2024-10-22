/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ? ??? ?? ? ??ฝ?? ?? ์ปคๅ ??? ?? ? ???ธ? ?? ? ??? ???ธ ? ??? ??
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte( void );

/**
 *  ? ?ฃ๋ค์ ? ??ฝ?? ?? C ? ??? ? ์ปคๅ ??? ?? ? ??? ?? ? ?ธ๋ธ์? ??
 *      ? ?ฅ?ธ?? ? ? ?๋ช์ ? ??ฝ?? ?? ? ??? ?? ? ??? ?? ? ??? ?ญ?ธ? ? ??? ??? ??ต? ? ??
 */
void Main( void )
{
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[ 13 ] = { 0, };

    kPrintString( 0, 4, "Protected Mode C Language Kernel Start......[Pass]" );

    // ์ต์ ๋ฉ๋ชจ๋ฆ? ?ฌ๊ธฐ๋?? ๋ง์กฑ?? ์ง? ๊ฒ??ฌ
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

    // IA-32e ๋ชจ๋ ์ปค๋? ?? ??ด์ง? ??ด๋ธ? ??ฑ
    kPrintString( 0, 7, "IA-32e Page Tables Initialize...............[    ]" );
    kInitializePageTables();
    kPrintString( 45, 7, "Pass" );

    // ??ยท?ลลลยญ ???ยถ?? ??ฌยบ? ?รยฑ?
    kReadCPUID( 0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX );
    *( DWORD* ) vcVendorString = dwEBX;
    *( ( DWORD* ) vcVendorString + 1 ) = dwEDX;
    *( ( DWORD* ) vcVendorString + 2 ) = dwECX;
    kPrintString( 0, 8, "Processor Vendor String.....................[            ]" );
    kPrintString( 45, 8, vcVendorString );
    
    // 64ยบ?รยฎ ??ยฟรธ ??ยน? ?ยฎ??
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
    
    // IA-32e ?รฐ?? ?ยฟยณ??? 0x200000(2Mbyte) ????ยทยนลยบยท? ???ยฟ
    kPrintString( 0, 10, "Copy IA-32e Kernel To 2M Address............[    ]" );
    kCopyKernel64ImageTo2Mbyte();
    kPrintString( 45, 10, "Pass" );
    
    // IA-32e ?รฐ??ยท? ????
    kPrintString( 0, 11, "Switch To IA-32e Mode" );
    kSwitchAndExecute64bitKernel();

    while( 1 ) ;
}

/**
 *  ? ??? ??ธ?? ?? X, Y ? ??์นๅ ?? ? ??? ?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ? ???? ?? ? ?ฑ?ธ?? ??ฝ? ? ??? ??ธ?? ?? ? ??? ??? ? ? ???ฅ๋ฑ์? ??? ? ? ??? ?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL? ?? ? ??? ?? ? ??? ??? ?? ? ??? ??ธ? ? ??? ?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernel64Area( void )
{
     DWORD* pdwCurrentAddress;
    
    // ์ด๊ธฐ?๋ฅ? ???  ?ด?? ?ค?ธ 0x100000(1MB)? ?ค? 
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // ๋ง์??๋ง? ?ด?? ?ค?ธ 0x600000(6MB)๊น์?? ๋ฃจํ๋ฅ? ?๋ฉด์ 4๋ฐ์ด?ธ?ฉ 0?ผ๋ก? ์ฑ์??
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {        
        *pdwCurrentAddress = 0x00;

        // 0?ผ๋ก? ????ฅ? ? ?ค? ?ฝ?? ? 0?ด ??ค์ง? ??ผ๋ฉ? ?ด?น ?ด?? ?ค๋ฅ? 
        // ?ฌ?ฉ???ฐ ๋ฌธ์ ๊ฐ? ?๊ธ? ๊ฒ์ด๋ฏ?๋ก? ??ด? ์งํ?์ง? ?๊ณ? ์ข๋ฃ
        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }
        
        // ?ค? ?ด?? ?ค๋ก? ?ด?
        pdwCurrentAddress++;
    }
    
    return TRUE;
}

BOOL kIsMemoryEnough( void )
{
    DWORD* pdwCurrentAddress;
   
    // 0x100000(1MB)๋ถ??ฐ ๊ฒ??ฌ ??
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 0x4000000(64MB)๊น์?? ๋ฃจํ๋ฅ? ?๋ฉด์ ??ธ
    while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        *pdwCurrentAddress = 0x12345678;
        
        // 0x12345678๋ก? ????ฅ? ? ?ค? ?ฝ?? ? 0x12345678?ด ??ค์ง? ??ผ๋ฉ? 
        // ?ด?น ?ด?? ?ค๋ฅ? ?ฌ?ฉ???ฐ ๋ฌธ์ ๊ฐ? ?๊ธ? ๊ฒ์ด๋ฏ?๋ก? ??ด? ์งํ?์ง? ?๊ณ? ์ข๋ฃ
        if( *pdwCurrentAddress != 0x12345678 )
        {
           return FALSE;
        }
        
        // 1MB?ฉ ?ด??๋ฉด์ ??ธ
        pdwCurrentAddress += ( 0x100000 / 4 );
    }
    return TRUE;
}

/**
 *  IA-32e ?รฐ?? ?ยฟยณ??? 0x200000(2Mbyte) ????ยทยนลยบยฟยก ยบยน??
 */
void kCopyKernel64ImageTo2Mbyte( void )
{
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD* pdwSourceAddress,* pdwDestinationAddress;
    int i;
    
    // 0x7C05ยฟยก ?? ?ยฟยณ? ลล?? ล?, 0x7C07ยฟยก ยบ??? ?รฐ?? ?ยฟยณ? ลล?? ล?ยฐยก ???? ???ล
    wTotalKernelSectorCount = *( ( WORD* ) 0x7C05 );
    wKernel32SectorCount = *( ( WORD* ) 0x7C07 );

    pdwSourceAddress = ( DWORD* ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );
    pdwDestinationAddress = ( DWORD* ) 0x200000;
    // IA-32e ?รฐ?? ?ยฟยณ? ลล?? ??ยฑ????ยญ ยบยน??
    for( i = 0 ; i < 512 * ( wTotalKernelSectorCount - wKernel32SectorCount ) / 4;
        i++ )
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}