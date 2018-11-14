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
#include "Task.h"
#include "PIT.h"

// �Լ� ����
void kPrintString( int iX, int iY, const char* pcString );
void kPrintStringOn0xAB8000( int iX, int iY, const char* pcString );
void kWriteTestAt0x1FF000();

/**
 *  �Ʒ� �Լ��� C ���?? Ŀ���� ���� �κ���
 */
void Main( void )
{
    char vcTemp[ 2 ] = { 0, };
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;
    DWORD tmp;

    int iCursorX, iCursorY;

    // 콘솔?�� 먼�?? 초기?��?�� ?��, ?��?�� ?��?��?�� ?��?��
    kInitializeConsole( 0, 11 );    
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );

    // write test at 0x1FF000 (occur error)
    //kWriteTestAt0x1FF000();
    // �??�� ?��?��?�� ?��면에 출력
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
    // 1ms�� �ѹ��� ���ͷ�Ʈ�� �߻��ϵ��� ����
    kInitializePIT( MSTOCOUNT( 1 ), 1 );

    kPrintf("Keyboard Activate And Queue Initialize......[    ]" );
    // Ű���带 Ȱ��ȭ
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
    // PIC ��Ʈ�ѷ� �ʱ�ȭ �� ���? ���ͷ�Ʈ Ȱ��ȭ
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    // ���� �½�ũ�� �ý��� ������� �����ϰ� ���� ����
    kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE, 0, 0, 
            ( QWORD ) kIdleTask );
    kStartConsoleShell();

    // print test on 0xAB8000 (another virtual video address)
    kPrintStringOn0xAB8000( 0, 20, "This message is printed through the video memory relocated to 0xAB8000" );
    
    kPrintf( "\n" );
/*
    while( 1 )
    {
        // Ű ť�� �����Ͱ� ������ Ű�� ó����
        if( kGetKeyFromKeyQueue( &stData ) == TRUE )
        {
            // Ű�� ���������� Ű�� ASCII �ڵ� ���� ȭ�鿡 ���?
            if( stData.bFlags & KEY_FLAGS_DOWN )
            {
                // Ű �������� ACII �ڵ� ���� ����
                vcTemp[ 0 ] = stData.bASCIICode;
                kPrintString( i++, 17, vcTemp );

                // 0�� �ԷµǸ� ������ 0���� ������ Divide Error ����(���� 0��)��
                // �߻���Ŵ
                if( vcTemp[ 0 ] == '0' )
                {
                    // �Ʒ� �ڵ带 �����ϸ� Divide Error ���ܰ� �߻��Ͽ�
                    // Ŀ���� �ӽ� �ڵ鷯�� �����?
                    //bTemp = bTemp / 0;
                    kWriteTestAt0x1FF000();
                }
            }
        }
    }*/
    kStartConsoleShell();
}

/**
 *  ���ڿ��� X, Y ��ġ�� ���??
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ��ǥ �� �̿��ؼ� ���ڿ��� �����?? ��巹����?? ���??
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���??
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintStringOn0xAB8000(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� �����? ��巹����? ���?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL�� ���� ������ ���ڿ� ���?
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