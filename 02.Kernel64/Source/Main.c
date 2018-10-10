/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"

// �Լ� ����
void kPrintString( int iX, int iY, const char* pcString );
void kPrintString2();

/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
 */
void Main( void )
{
    PDENTRY* AcessReadOnly = (PDENTRY*) 0x1fc000;
    DWORD tmp;
    kPrintString( 0, 11, "Switch To IA-32e Mode Success~!!" );
    kPrintString( 0, 12, "IA-32e C Language Kernel Start..............[Pass]" );
    kPrintString2();
    //tmp = AcessReadOnly -> dwAttributeAndLowerBaseAddress;
    //kPrintString2( 0, 14);
    //AcessReadOnly -> dwAttributeAndLowerBaseAddress = AcessReadOnly -> dwAttributeAndLowerBaseAddress | 0x2;
    //kPrintString2(0 , 13);
    /*
    tmp = x & 0xff00;
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    pstScreen += (16 * 80);
    for(int i = 0; i < 32; i++){
       if ( i % 4 == 0)
         pstScreen += 1;
    
    pstScreen [ i ].bCharactor = ((int) tmp & 0x1 ) + '0';
    tmp = (int) tmp >> 1;
   }*/
}

/**
 *  ���ڿ��� X, Y ��ġ�� ���
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintString2(){
   DWORD* AcessReadOnly = ( DWORD* ) 0x1ff000;

   *AcessReadOnly = 0x12345678;

   if( *AcessReadOnly == 0x12345678 ){
       kPrintString(0, 13 , "test");
   }

}