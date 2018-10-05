/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernal164Area( void );

/**
 *  �Ʒ� �Լ��� C ���? Ŀ���� ���� �κ���
 *      �ݵ��? �ٸ� �Լ��� ���� ���� ���ʿ� �����ؾ� ��
 */
void Main( void )
{
    DWORD i;

    kPrintString( 0, 4, "C Language Kernel Started~!!!" );

    // IA-32e Kernal area initialize
    kInitializeKernal164Area();
    kPrintString( 0, 5, "IA-32e Kernal Area Initialization Complete" );

    while( 1 ) ;
}

/**
 *  ���ڿ��� X, Y ��ġ�� ���?
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� �����? ��巹����? ���?
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL�� ���� ������ ���ڿ� ���?
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernal164Area( void )
{
    DWORD* pdwCurrentAddress;

    //set address 0x100000(1MB)
    pdwCurrentAddress = (DWORD *) 0x100000;

    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {
        *pdwCurrentAddress = 0x00;

        if( *pdwCurrentAddress != 0)
        {
            return FALSE;
        }

        pdwCurrentAddress++;
    }

    return TRUE;
}