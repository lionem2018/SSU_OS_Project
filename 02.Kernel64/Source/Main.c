/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ï¿½ï¿½ï¿½ï¿½ ï¿½Û¼ï¿½ï¿½ï¿½ Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿½ï¿½
 */

#include "Types.h"
#include "Keyboard.h"

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
    BYTE bFlags;
    BYTE bTemp;
    int i = 0;

    kPrintString( 0, 11, "Switch To IA-32e Mode Success~!!" );
    kPrintString( 0, 12, "IA-32e C Language Kernel Start..............[Pass]" );

    // print test on 0xAB8000 (another virtual video address)
    kPrintStringOn0xAB8000( 0, 13, "This message is printed through the video memory relocated to 0xAB8000" );
    
    // write test at 0x1FF000 (occur error)
    //kWriteTestAt0x1FF000();

    kPrintString( 0, 14, "Keybord Activate............................[    ]" );

    // ?¡Æ¨¬????? ?¡Æ¨«¨¬?¡©
    if( kActivateKeyboard() == TRUE )
    {
        kPrintString( 45, 14, "Pass" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kPrintString( 45, 14, "Fail" );
        while( 1 ) ;
    }
    
    while( 1 )
    {
        // ??¡¤? ©ö?¨¡?(¨¡¡À¨¡¢ç 0x60)¡Æ¢® ?¡À ?????? ©«¨¬?? ?????? ?¨¢?? ¨«? ???©«
        if( kIsOutputBufferFull() == TRUE )
        {
            // ??¡¤? ©ö?¨¡?(¨¡¡À¨¡¢ç 0x60)¢¯¢®¨«¡© ©«¨¬?? ?????? ?¨¢??¨«¡© ????
            bTemp = kGetKeyboardScanCode();
            
            // ©«¨¬?? ?????? ASCII ????¡¤? ¨¬??????? ??¨«??? ??????¢¯? ASCII ????¢¯?
            // ?¡©?©÷ ¢Ò??? ¢Ò©ø???? ?¢æ¨¬??? ©ö???
            if( kConvertScanCodeToASCIICode( bTemp, &( vcTemp[ 0 ] ), &bFlags ) == TRUE )
            {
                // ?¡Æ¡Æ¢® ?¡©¡¤??©ø???? ?¡Æ?? ASCII ???? ¡Æ¨£?? ?¡©??¢¯¢® ??¡¤?
                if( bFlags & KEY_FLAGS_DOWN )
                {
                    kPrintString( i++, 15, vcTemp );
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
       kPrintString(0, 14 , "Can write at 0x1ff000");
   }
}