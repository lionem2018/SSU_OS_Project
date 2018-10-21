/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"
#include "Keyboard.h"

// �Լ� ����
void kPrintString( int iX, int iY, const char* pcString );
void kPrintStringOn0xAB8000( int iX, int iY, const char* pcString );
void kWriteTestAt0x1FF000();

/**
 *  �Ʒ� �Լ��� C ���? Ŀ���� ���� �κ���
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

    // ?�ƨ�????? ?�ƨ���?��
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
        // ??��? ��?��?(�������� 0x60)�Ƣ� ?�� ?????? ����?? ?????? ?��?? ��? ???��
        if( kIsOutputBufferFull() == TRUE )
        {
            // ??��? ��?��?(�������� 0x60)�������� ����?? ?????? ?��??���� ????
            bTemp = kGetKeyboardScanCode();
            
            // ����?? ?????? ASCII ????��? ��??????? ??��??? ??????��? ASCII ????��?
            // ?��?�� ��??? �ҩ�???? ?�樬??? ��???
            if( kConvertScanCodeToASCIICode( bTemp, &( vcTemp[ 0 ] ), &bFlags ) == TRUE )
            {
                // ?�ơƢ� ?����??��???? ?��?? ASCII ???? �ƨ�?? ?��??���� ??��?
                if( bFlags & KEY_FLAGS_DOWN )
                {
                    kPrintString( i++, 15, vcTemp );
                }
            }
        }
    }
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
       kPrintString(0, 14 , "Can write at 0x1ff000");
   }
}