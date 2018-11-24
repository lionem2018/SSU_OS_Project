/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C О©╫О©╫О©╫О©╫ О©╫ш╪О©╫О©╫О©╫ д©О©╫О©╫О©╫О©╫ О©╫О©╫ф╝О©╫О©╫ О©╫О©╫О©╫О©╫ф╝ О©╫О©╫О©╫О©╫
 */

#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Task.h"
#include "PIT.h"

// О©╫т╪О©╫ О©╫О©╫О©╫О©╫
void kPrintString( int iX, int iY, const char* pcString );
void kPrintStringOn0xAB8000( int iX, int iY, const char* pcString );
void kWriteTestAt0x1FF000();

/**
 *  О©╫ф╥О©╫ О©╫т╪О©╫О©╫О©╫ C О©╫О©╫О©?? д©О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫ О©╫н╨О©╫О©╫О©╫
 */
void Main( void )
{
    char vcTemp[ 2 ] = { 0, };
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;
    DWORD tmp;

    int iCursorX, iCursorY;

    // Л╫≤Л├■?²└ К╗╪Л?? Л╢┬Й╦╟?≥■?∙° ?⌡└, ?▀╓?²▄ ?·▒?≈┘?²└ ?┬≤?√┴
    kInitializeConsole( 0, 11 );    
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );

    // write test at 0x1FF000 (occur error)
    //kWriteTestAt0x1FF000();
    // К╤??▄┘ ?┐│?≥╘?²└ ?≥■К╘╢Л≈░ Л╤°К═╔
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
    // 1ms╢Г гя╧Ь╬© юнем╥╢ф╝╟║ ╧ъ╩Щго╣╣╥о ╪Ёа╓
    kInitializePIT( MSTOCOUNT( 1 ), 1 );

    kPrintf("Keyboard Activate And Queue Initialize......[    ]" );
    // е╟О©╫О©╫О©╫Е╦╕ х╟О©╫О©╫х╜
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
    // PIC О©╫О©╫ф╝О©╫я╥О©╫ О©╫й╠О©╫х╜ О©╫О©╫ О©╫О©╫О©? О©╫О©╫О©╫м╥О©╫ф╝ х╟О©╫О©╫х╜
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    // ю╞хч еб╫╨е╘╦╕ ╫ц╫╨еш ╫╨╥╧╣Е╥н ╩Щ╪╨го╟М ╪пю╩ ╫цюш
    kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE, 0, 0, 
            ( QWORD ) kIdleTask );
    kStartConsoleShell();

    // print test on 0xAB8000 (another virtual video address)
    kPrintStringOn0xAB8000( 0, 20, "This message is printed through the video memory relocated to 0xAB8000" );
    
    kPrintf( "\n" );
/*
    while( 1 )
    {
        // е╟ е╔О©╫О©╫ О©╫О©╫О©╫О©╫О©╫м╟О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ е╟О©╫О©╫ цЁО©╫О©╫О©╫О©╫
        if( kGetKeyFromKeyQueue( &stData ) == TRUE )
        {
            // е╟О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ е╟О©╫О©╫ ASCII О©╫з╣О©╫ О©╫О©╫О©╫О©╫ х╜О©╫И©║ О©╫О©╫О©?
            if( stData.bFlags & KEY_FLAGS_DOWN )
            {
                // е╟ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ ACII О©╫з╣О©╫ О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫
                vcTemp[ 0 ] = stData.bASCIICode;
                kPrintString( i++, 17, vcTemp );

                // 0О©╫О©╫ О©╫т╥б╣г╦О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ 0О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ Divide Error О©╫О©╫О©╫О©╫(О©╫О©╫О©╫О©╫ 0О©╫О©╫)О©╫О©╫
                // О©╫ъ╩О©╫О©╫О©╫е╢
                if( vcTemp[ 0 ] == '0' )
                {
                    // О©╫ф╥О©╫ О©╫з╣Е╦╕ О©╫О©╫О©╫О©╫О©╫о╦О©╫ Divide Error О©╫О©╫О©╫э╟О©╫ О©╫ъ╩О©╫О©╫о©О©╫
                    // д©О©╫О©╫О©╫О©╫ О©╫с╫О©╫ О©╫з╣И╥╞О©╫О©╫ О©╫О©╫О©╫О©╫О©?
                    //bTemp = bTemp / 0;
                    kWriteTestAt0x1FF000();
                }
            }
        }
    }*/
    kStartConsoleShell();
}

/**
 *  О©╫О©╫О©╫з©О©╫О©╫О©╫ X, Y О©╫О©╫д║О©╫О©╫ О©╫О©╫О©??
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y О©╫О©╫г╔ О©╫О©╫ О©╫л©О©╫О©╫ь╪О©╫ О©╫О©╫О©╫з©О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©?? О©╫О©╫Е╥╧О©╫О©╫О©╫О©?? О©╫О©╫О©??
    pstScreen += ( iY * 80 ) + iX;

    // NULLО©╫О©╫ О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫з©О©╫ О©╫О©╫О©??
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintStringOn0xAB8000(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y О©╫О©╫г╔О©╫О©╫ О©╫л©О©╫О©╫ь╪О©╫ О©╫О©╫О©╫з©О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©? О©╫О©╫Е╥╧О©╫О©╫О©╫О©? О©╫О©╫О©?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULLО©╫О©╫ О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫з©О©╫ О©╫О©╫О©?
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