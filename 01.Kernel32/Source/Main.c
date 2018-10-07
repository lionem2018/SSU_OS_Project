/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ï¿½ï¿½ï¿½ï¿½ ï¿½Û¼ï¿½ï¿½ï¿½ Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿½ï¿½
 */

#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );

/**
 *  ï¿½Æ·ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ C ï¿½ï¿½ï¿? Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Îºï¿½ï¿½ï¿½
 *      ï¿½Ýµï¿½ï¿? ï¿½Ù¸ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ê¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ø¾ï¿½ ï¿½ï¿½
 */
void Main( void )
{
    DWORD i;

    kPrintString( 0, 4, "C Language Kernel Start...................[Pass]" );

    // ÃÖ¼Ò ¸Þ¸ð¸® Å©±â¸¦ ¸¸Á·ÇÏ´Â Áö °Ë»ç
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

    while( 1 ) ;
}

/**
 *  ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ X, Y ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½ï¿½ï¿?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ï¿½ï¿½Ç¥ï¿½ï¿½ ï¿½Ì¿ï¿½ï¿½Ø¼ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULLï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ ï¿½ï¿½ï¿?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernel64Area( void )
{
     DWORD* pdwCurrentAddress;
    
    // ÃÊ±âÈ­¸¦ ½ÃÀÛÇÒ ¾îµå·¹½ºÀÎ 0x100000(1MB)À» ¼³Á¤
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // ¸¶Áö¸· ¾îµå·¹½ºÀÎ 0x600000(6MB)±îÁö ·çÇÁ¸¦ µ¹¸é¼­ 4¹ÙÀÌÆ®¾¿ 0À¸·Î Ã¤¿ò
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {        
        *pdwCurrentAddress = 0x00;

        // 0À¸·Î ÀúÀåÇÑ ÈÄ ´Ù½Ã ÀÐ¾úÀ» ¶§ 0ÀÌ ³ª¿ÀÁö ¾ÊÀ¸¸é ÇØ´ç ¾îµå·¹½º¸¦ 
        // »ç¿ëÇÏ´Âµ¥ ¹®Á¦°¡ »ý±ä °ÍÀÌ¹Ç·Î ´õÀÌ»ó ÁøÇàÇÏÁö ¾Ê°í Á¾·á
        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }
        
        // ´ÙÀ½ ¾îµå·¹½º·Î ÀÌµ¿
        pdwCurrentAddress++;
    }
    
    return TRUE;
}

BOOL kIsMemoryEnough( void )
{DWORD* pdwCurrentAddress;
   
    // 0x100000(1MB)ºÎÅÍ °Ë»ç ½ÃÀÛ
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 0x4000000(64MB)±îÁö ·çÇÁ¸¦ µ¹¸é¼­ È®ÀÎ
    while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        *pdwCurrentAddress = 0x12345678;
        
        // 0x12345678·Î ÀúÀåÇÑ ÈÄ ´Ù½Ã ÀÐ¾úÀ» ¶§ 0x12345678ÀÌ ³ª¿ÀÁö ¾ÊÀ¸¸é 
        // ÇØ´ç ¾îµå·¹½º¸¦ »ç¿ëÇÏ´Âµ¥ ¹®Á¦°¡ »ý±ä °ÍÀÌ¹Ç·Î ´õÀÌ»ó ÁøÇàÇÏÁö ¾Ê°í Á¾·á
        if( *pdwCurrentAddress != 0x12345678 )
        {
           return FALSE;
        }
        
        // 1MB¾¿ ÀÌµ¿ÇÏ¸é¼­ È®ÀÎ
        pdwCurrentAddress += ( 0x100000 / 4 );
    }
    return TRUE;
}