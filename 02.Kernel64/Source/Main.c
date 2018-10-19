/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ï¿½ï¿½ï¿½ï¿½ ï¿½Û¼ï¿½ï¿½ï¿½ Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿½ï¿½
 */

#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"

// ï¿½Ô¼ï¿½ ï¿½ï¿½ï¿½ï¿½
void kPrintString( int iX, int iY, const char* pcString );
void kPrintStringOn0xAB8000( int iX, int iY, const char* pcString );
void kWriteTestAt0x1FF000();

/**
 *  ï¿½Æ·ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ C ï¿½ï¿½ï¿? Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Îºï¿½ï¿½ï¿½
 */
void Main( void )
{
    char vcTemp[ 2 ] = { 0, };
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;
    DWORD tmp;
    kPrintString( 0, 11, "Switch To IA-32e Mode Success~!!" );
    kPrintString( 0, 12, "IA-32e C Language Kernel Start..............[Pass]" );
    
    // print test on 0xAB8000 (another virtual video address)
    kPrintStringOn0xAB8000( 0, 18, "This message is printed through the video memory relocated to 0xAB8000" );
    
    // write test at 0x1FF000 (occur error)
    //kWriteTestAt0x1FF000();
    kPrintString( 0, 12, "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kPrintString( 45, 12, "Pass" );

    kPrintString( 0, 13, "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kPrintString( 45, 13, "Pass" );

    kPrintString( 0, 14, "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();
    kLoadIDTR( IDTR_STARTADDRESS );
    kPrintString( 45, 14, "Pass" );

    kPrintString( 0, 15, "Keyboard Activate And Queue Initialize......[    ]" );
    // Å°º¸µå¸¦ È°¼ºÈ­
    if( kInitializeKeyboard() == TRUE )
    {
        kPrintString( 45, 15, "Pass" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kPrintString( 45, 15, "Fail" );
        while( 1 ) ;
    }

    kPrintString( 0, 16, "PIC Controller And Interrupt Initialize.....[    ]" );
    // PIC ÄÁÆ®·Ñ·¯ ÃÊ±âÈ­ ¹× ¸ðµç ÀÎÅÍ·´Æ® È°¼ºÈ­
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kPrintString( 45, 16, "Pass" );

    while( 1 )
    {
        // Å° Å¥¿¡ µ¥ÀÌÅÍ°¡ ÀÖÀ¸¸é Å°¸¦ Ã³¸®ÇÔ
        if( kGetKeyFromKeyQueue( &stData ) == TRUE )
        {
            // Å°°¡ ´­·¯Á³À¸¸é Å°ÀÇ ASCII ÄÚµå °ªÀ» È­¸é¿¡ Ãâ·Â
            if( stData.bFlags & KEY_FLAGS_DOWN )
            {
                // Å° µ¥ÀÌÅÍÀÇ ACII ÄÚµå °ªÀ» ÀúÀå
                vcTemp[ 0 ] = stData.bASCIICode;
                kPrintString( i++, 17, vcTemp );

                // 0ÀÌ ÀÔ·ÂµÇ¸é º¯¼ö¸¦ 0À¸·Î ³ª´©¾î Divide Error ¿¹¿Ü(º¤ÅÍ 0¹ø)À»
                // ¹ß»ý½ÃÅ´
                if( vcTemp[ 0 ] == '0' )
                {
                    // ¾Æ·¡ ÄÚµå¸¦ ¼öÇàÇÏ¸é Divide Error ¿¹¿Ü°¡ ¹ß»ýÇÏ¿©
                    // Ä¿³ÎÀÇ ÀÓ½Ã ÇÚµé·¯°¡ ¼öÇàµÊ
                    //bTemp = bTemp / 0;
                    kWriteTestAt0x1FF000();
                }
            }
        }
    }
}

/**
 *  ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ X, Y ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½ï¿½ï¿?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ï¿½ï¿½Ç¥ ï¿½ï¿½ ï¿½Ì¿ï¿½ï¿½Ø¼ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿?
    pstScreen += ( iY * 80 ) + iX;

    // NULLï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ ï¿½ï¿½ï¿?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintStringOn0xAB8000(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ÁÂÇ¥¸¦ ÀÌ¿ëÇØ¼­ ¹®ÀÚ¿­À» Ãâ·ÂÇÒ ¾îµå·¹½º¸¦ °è»ê
    pstScreen += ( iY * 80 ) + iX;
    
    // NULLÀÌ ³ª¿Ã ¶§±îÁö ¹®ÀÚ¿­ Ãâ·Â
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kWriteTestAt0x1FF000(){
   DWORD* AcessReadOnly = ( DWORD* ) 0x1ff000;

   *AcessReadOnly = 0x12345678;

   if( *AcessReadOnly == 0x12345678 ){
       kPrintString(0, 19 , "Can write at 0x1ff000");
   }
}