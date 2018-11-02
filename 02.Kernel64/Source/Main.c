/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"

// �Լ� ����
void kPrintStringOn0xAB8000( int iX, int iY, const char* pcString );
void kWriteTestAt0x1FF000();

/**
 *  �Ʒ� �Լ��� C ���? Ŀ���� ���� �κ���
 */
void Main( void )
{
    char vcTemp[ 2 ] = { 0, };
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;
    DWORD tmp;

    int iCursorX, iCursorY;

    kInitializeConsole( 0, 11 );    
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );
    
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
    
    kPrintf( "Keyboard Activate And Queue Initialize......[    ]" );

    // Ű���带 Ȱ��ȭ
    if( kInitializeKeyboard() == TRUE )
    {
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
    // PIC ??���硤?��? ??��??�� ���� ?��?? ????��?���� ?�ƨ���?��
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    // print test on 0xAB8000 (another virtual video address)
    kPrintStringOn0xAB8000( 0, 20, "This message is printed through the video memory relocated to 0xAB8000" );
    kPrintf( "\n" );

    kStartConsoleShell();
}

/**
 *  ���ڿ��� X, Y ��ġ�� ���?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ��ǥ �� �̿��ؼ� ���ڿ��� �����? ��巹����? ���?
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintStringOn0xAB8000(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL�� ���� ������ ���ڿ� ���
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