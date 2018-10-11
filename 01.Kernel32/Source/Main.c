/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ? ?™?˜™? ?™?˜™ ? ?Œœ?‡½?˜™? ?™?˜™ ì»¤å ?™?˜™? ?™?˜™ ? ?™?˜™?Š¸? ?™?˜™ ? ?™?˜™? ?™?˜™?Š¸ ? ?™?˜™? ?™?˜™
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte( void );

/**
 *  ? ?‹£ë¤„ì˜™ ? ?Œ‰?‡½?˜™? ?™?˜™ C ? ?™?˜™? ? ì»¤å ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™ ? ?‹¸ë¸ì˜™? ?™?˜™
 *      ? ?Œ¥?“¸?˜™? ? ? ?Œ•ëªŒì˜™ ? ?Œ‰?‡½?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?‹­?š¸?˜™ ? ?™?˜™? ?™?˜™? ?Œ”?–µ?˜™ ? ?™?˜™
 */
void Main( void )
{
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[ 13 ] = { 0, };

    kPrintString( 0, 4, "Protected Mode C Language Kernel Start......[Pass]" );

    // ìµœì†Œ ë©”ëª¨ë¦? ?¬ê¸°ë?? ë§Œì¡±?•˜?Š” ì§? ê²??‚¬
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

    // IA-32e ëª¨ë“œ ì»¤ë„?„ ?œ„?•œ ?˜?´ì§? ?…Œ?´ë¸? ?ƒ?„±
    kPrintString( 0, 7, "IA-32e Page Tables Initialize...............[    ]" );
    kInitializePageTables();
    kPrintString( 45, 7, "Pass" );

    // ??Â·?Å’Å’Å’Â­ ???Â¶?? ??‚¬Âº? ?ÃÂ±?
    kReadCPUID( 0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX );
    *( DWORD* ) vcVendorString = dwEBX;
    *( ( DWORD* ) vcVendorString + 1 ) = dwEDX;
    *( ( DWORD* ) vcVendorString + 2 ) = dwECX;
    kPrintString( 0, 8, "Processor Vendor String.....................[            ]" );
    kPrintString( 45, 8, vcVendorString );
    
    // 64Âº?Ã†Â® ??Â¿Ã¸ ??Â¹? ?Â®??
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
    
    // IA-32e ?Ã°?? ?Â¿Â³??? 0x200000(2Mbyte) ????Â·Â¹Å“ÂºÂ·? ???Â¿
    kPrintString( 0, 10, "Copy IA-32e Kernel To 2M Address............[    ]" );
    kCopyKernel64ImageTo2Mbyte();
    kPrintString( 45, 10, "Pass" );
    
    // IA-32e ?Ã°??Â·? ????
    kPrintString( 0, 11, "Switch To IA-32e Mode" );
    kSwitchAndExecute64bitKernel();

    while( 1 ) ;
}

/**
 *  ? ?™?˜™? ?Œ˜?š¸?˜™? ?™?˜™ X, Y ? ?™?˜™ì¹˜å ?™?˜™ ? ?™?˜™? ?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ? ?™?˜™?‘œ? ?™?˜™ ? ?‹±?š¸?˜™? ?Œ”?‡½?˜™ ? ?™?˜™? ?Œ˜?š¸?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ? ? ?™?˜™?—¥ë±„ì˜™? ?™?˜™? ? ? ?™?˜™? ?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL? ?™?˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?Œ˜?š¸?˜™ ? ?™?˜™? ?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernel64Area( void )
{
     DWORD* pdwCurrentAddress;
    
    // ì´ˆê¸°?™”ë¥? ?‹œ?‘?•  ?–´?“œ? ˆ?Š¤?¸ 0x100000(1MB)?„ ?„¤? •
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // ë§ˆì??ë§? ?–´?“œ? ˆ?Š¤?¸ 0x600000(6MB)ê¹Œì?? ë£¨í”„ë¥? ?Œë©´ì„œ 4ë°”ì´?Š¸?”© 0?œ¼ë¡? ì±„ì??
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {        
        *pdwCurrentAddress = 0x00;

        // 0?œ¼ë¡? ????¥?•œ ?›„ ?‹¤?‹œ ?½?—ˆ?„ ?•Œ 0?´ ?‚˜?˜¤ì§? ?•Š?œ¼ë©? ?•´?‹¹ ?–´?“œ? ˆ?Š¤ë¥? 
        // ?‚¬?š©?•˜?Š”?° ë¬¸ì œê°? ?ƒê¸? ê²ƒì´ë¯?ë¡? ?”?´?ƒ ì§„í–‰?•˜ì§? ?•Šê³? ì¢…ë£Œ
        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }
        
        // ?‹¤?Œ ?–´?“œ? ˆ?Š¤ë¡? ?´?™
        pdwCurrentAddress++;
    }
    
    return TRUE;
}

BOOL kIsMemoryEnough( void )
{
    DWORD* pdwCurrentAddress;
   
    // 0x100000(1MB)ë¶??„° ê²??‚¬ ?‹œ?‘
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 0x4000000(64MB)ê¹Œì?? ë£¨í”„ë¥? ?Œë©´ì„œ ?™•?¸
    while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        *pdwCurrentAddress = 0x12345678;
        
        // 0x12345678ë¡? ????¥?•œ ?›„ ?‹¤?‹œ ?½?—ˆ?„ ?•Œ 0x12345678?´ ?‚˜?˜¤ì§? ?•Š?œ¼ë©? 
        // ?•´?‹¹ ?–´?“œ? ˆ?Š¤ë¥? ?‚¬?š©?•˜?Š”?° ë¬¸ì œê°? ?ƒê¸? ê²ƒì´ë¯?ë¡? ?”?´?ƒ ì§„í–‰?•˜ì§? ?•Šê³? ì¢…ë£Œ
        if( *pdwCurrentAddress != 0x12345678 )
        {
           return FALSE;
        }
        
        // 1MB?”© ?´?™?•˜ë©´ì„œ ?™•?¸
        pdwCurrentAddress += ( 0x100000 / 4 );
    }
    return TRUE;
}

/**
 *  IA-32e ?Ã°?? ?Â¿Â³??? 0x200000(2Mbyte) ????Â·Â¹Å“ÂºÂ¿Â¡ ÂºÂ¹??
 */
void kCopyKernel64ImageTo2Mbyte( void )
{
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD* pdwSourceAddress,* pdwDestinationAddress;
    int i;
    
    // 0x7C05Â¿Â¡ ?? ?Â¿Â³? Å’Å“?? Å’?, 0x7C07Â¿Â¡ Âº??? ?Ã°?? ?Â¿Â³? Å’Å“?? Å’?Â°Â¡ ???? ???Å“
    wTotalKernelSectorCount = *( ( WORD* ) 0x7C05 );
    wKernel32SectorCount = *( ( WORD* ) 0x7C07 );

    pdwSourceAddress = ( DWORD* ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );
    pdwDestinationAddress = ( DWORD* ) 0x200000;
    // IA-32e ?Ã°?? ?Â¿Â³? Å’Å“?? ??Â±????Â­ ÂºÂ¹??
    for( i = 0 ; i < 512 * ( wTotalKernelSectorCount - wKernel32SectorCount ) / 4;
        i++ )
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}