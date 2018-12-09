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
#include "Console.h"
#include "ConsoleShell.h"
#include "Task.h"
#include "PIT.h"
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "FileSystem.h"

// ï¿½Ô¼ï¿½ ï¿½ï¿½ï¿½ï¿½
void kPrintString( int iX, int iY, const char* pcString );
void kPrintStringOn0xAB8000( int iX, int iY, const char* pcString );
void kWriteTestAt0x1FF000();

/**
 *  ï¿½Æ·ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ C ï¿½ï¿½ï¿?? Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Îºï¿½ï¿½ï¿½
 */
void Main( void )
{
    char vcTemp[ 2 ] = { 0, };
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;
    DWORD tmp;

    int iCursorX, iCursorY;

    // ì½˜ì†”?„ ë¨¼ì?? ì´ˆê¸°?™”?•œ ?›„, ?‹¤?Œ ?‘?—…?„ ?ˆ˜?–‰
    kInitializeConsole( 0, 11 );    
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );

    // write test at 0x1FF000 (occur error)
    //kWriteTestAt0x1FF000();
    // ë¶??Œ… ?ƒ?™©?„ ?™”ë©´ì— ì¶œë ¥
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    kPrintf( "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    kPrintf( "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();    
    kLoadIDTR( IDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    kPrintf( "Total RAM Size Check........................[    ]" );
    kCheckTotalRAMSize();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );

    kPrintf( "TCB Pool And Scheduler Initialize...........[Pass]\n" );
    iCursorY++;
    kInitializeScheduler();

    // µ¿Àû ¸Ş¸ğ¸® ÃÊ±âÈ­
    kPrintf( "Dynamic Memory Initialize...................[Pass]\n" );
    iCursorY++;
    kInitializeDynamicMemory();

    // 1ms´ç ÇÑ¹ø¾¿ ÀÎÅÍ·´Æ®°¡ ¹ß»ıÇÏµµ·Ï ¼³Á¤
    kInitializePIT( MSTOCOUNT( 1 ), 1 );

    kPrintf("Keyboard Activate And Queue Initialize......[    ]" );
    // Å°ï¿½ï¿½ï¿½å¸¦ È°ï¿½ï¿½È­
    if( kInitializeKeyboard() == TRUE )
    {
        //iCursorY--;
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
        while( 1 ) ;
    }

    kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
    // PIC ï¿½ï¿½Æ®ï¿½Ñ·ï¿½ ï¿½Ê±ï¿½È­ ï¿½ï¿½ ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½Í·ï¿½Æ® È°ï¿½ï¿½È­
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

// ÆÄÀÏ ½Ã½ºÅÛÀ» ÃÊ±âÈ­
    kPrintf( "File System Initialize......................[    ]" );
    if( kInitializeFileSystem() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
    }
    

    // À¯ÈŞ ÅÂ½ºÅ©¸¦ ½Ã½ºÅÛ ½º·¹µå·Î »ı¼ºÇÏ°í ¼ĞÀ» ½ÃÀÛ
    kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE, 0, 0, 
            ( QWORD ) kIdleTask );
    kStartConsoleShell();

    // print test on 0xAB8000 (another virtual video address)
    kPrintStringOn0xAB8000( 0, 20, "This message is printed through the video memory relocated to 0xAB8000" );
    
    kPrintf( "\n" );
/*
    while( 1 )
    {
        // Å° Å¥ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Í°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Å°ï¿½ï¿½ Ã³ï¿½ï¿½ï¿½ï¿½
        if( kGetKeyFromKeyQueue( &stData ) == TRUE )
        {
            // Å°ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Å°ï¿½ï¿½ ASCII ï¿½Úµï¿½ ï¿½ï¿½ï¿½ï¿½ È­ï¿½é¿¡ ï¿½ï¿½ï¿?
            if( stData.bFlags & KEY_FLAGS_DOWN )
            {
                // Å° ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ACII ï¿½Úµï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
                vcTemp[ 0 ] = stData.bASCIICode;
                kPrintString( i++, 17, vcTemp );

                // 0ï¿½ï¿½ ï¿½Ô·ÂµÇ¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 0ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Divide Error ï¿½ï¿½ï¿½ï¿½(ï¿½ï¿½ï¿½ï¿½ 0ï¿½ï¿½)ï¿½ï¿½
                // ï¿½ß»ï¿½ï¿½ï¿½Å´
                if( vcTemp[ 0 ] == '0' )
                {
                    // ï¿½Æ·ï¿½ ï¿½Úµå¸¦ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¸ï¿½ Divide Error ï¿½ï¿½ï¿½Ü°ï¿½ ï¿½ß»ï¿½ï¿½Ï¿ï¿½
                    // Ä¿ï¿½ï¿½ï¿½ï¿½ ï¿½Ó½ï¿½ ï¿½Úµé·¯ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?
                    //bTemp = bTemp / 0;
                    kWriteTestAt0x1FF000();
                }
            }
        }
    }*/
    kStartConsoleShell();
}

/**
 *  ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ X, Y ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½ï¿½ï¿??
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ï¿½ï¿½Ç¥ ï¿½ï¿½ ï¿½Ì¿ï¿½ï¿½Ø¼ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?? ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿?? ï¿½ï¿½ï¿??
    pstScreen += ( iY * 80 ) + iX;

    // NULLï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ ï¿½ï¿½ï¿??
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintStringOn0xAB8000(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ï¿½ï¿½Ç¥ï¿½ï¿½ ï¿½Ì¿ï¿½ï¿½Ø¼ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½å·¹ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULLï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ú¿ï¿½ ï¿½ï¿½ï¿?
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