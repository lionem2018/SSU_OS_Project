/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C 占쏙옙占쏙옙 占쌜쇽옙占쏙옙 커占쏙옙占쏙옙 占쏙옙트占쏙옙 占쏙옙占쏙옙트 占쏙옙占쏙옙
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte( void );

/**
 *  占싣뤄옙 占쌉쇽옙占쏙옙 C 占쏙옙占? 커占쏙옙占쏙옙 占쏙옙占쏙옙 占싸븝옙占쏙옙
 *      占쌥듸옙占? 占쌕몌옙 占쌉쇽옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占십울옙 占쏙옙占쏙옙占쌔억옙 占쏙옙
 */
void Main( void )
{
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[ 13 ] = { 0, };

    kPrintString( 0, 4, "Protected Mode C Language Kernel Start......[Pass]" );

    // 최소 메모리 크기를 만족하는 지 검사
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

    // IA-32e 모드 커널을 위한 페이지 테이블 생성
    kPrintString( 0, 7, "IA-32e Page Tables Initialize...............[    ]" );
    kInitializePageTables();
    kPrintString( 45, 7, "Pass" );

    // ??·?ŒŒŒ­ ???¶?? ?€º? ?Ð±?
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
 *  占쏙옙占쌘울옙占쏙옙 X, Y 占쏙옙치占쏙옙 占쏙옙占?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y 占쏙옙표占쏙옙 占싱울옙占쌔쇽옙 占쏙옙占쌘울옙占쏙옙 占쏙옙占쏙옙占? 占쏙옙藥뱄옙占쏙옙占? 占쏙옙占?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌘울옙 占쏙옙占?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernel64Area( void )
{
     DWORD* pdwCurrentAddress;
    
    // 초기화를 시작할 어드레스인 0x100000(1MB)을 설정
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 마지막 어드레스인 0x600000(6MB)까지 루프를 돌면서 4바이트씩 0으로 채움
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {        
        *pdwCurrentAddress = 0x00;

        // 0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를 
        // 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }
        
        // 다음 어드레스로 이동
        pdwCurrentAddress++;
    }
    
    return TRUE;
}

BOOL kIsMemoryEnough( void )
{DWORD* pdwCurrentAddress;
   
    // 0x100000(1MB)부터 검사 시작
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 0x4000000(64MB)까지 루프를 돌면서 확인
    while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        *pdwCurrentAddress = 0x12345678;
        
        // 0x12345678로 저장한 후 다시 읽었을 때 0x12345678이 나오지 않으면 
        // 해당 어드레스를 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
        if( *pdwCurrentAddress != 0x12345678 )
        {
           return FALSE;
        }
        
        // 1MB씩 이동하면서 확인
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