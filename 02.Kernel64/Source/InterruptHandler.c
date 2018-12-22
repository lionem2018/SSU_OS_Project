/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ?¸?°?½?¸ ë°? ??¸ ?¸?¤?¬? ê´?? ¨? ??¤ ??¼
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Page.h"
#include "Console.h"
#include "RTC.h"
#include "Utility.h"
#include "Task.h"
#include "Descriptor.h"
#include "HardDisk.h"
/**
 *  ê³µíµ?¼ë¡? ?¬?©?? ??¸ ?¸?¤?¬
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    char hexadecimal[33]={0, };
    char faultAddress[33]={'0','x'};

    getFaultAddress();
    
    register long decimal asm("rax");
 
    int position = 0;
    while (1)
    {
        int mod = decimal % 16;    // 16?¼ë¡? ???? ? ?ë¨¸ì??ë¥? êµ¬í¨
        if (mod < 10) // ?ë¨¸ì??ê°? 10ë³´ë¤ ??¼ë©?
        {
            // ?«? 0? ASCII ì½ë ê°? 48 + ?ë¨¸ì??
            hexadecimal[position] = 48 + mod;
        }
        else    // ?ë¨¸ì??ê°? 10ë³´ë¤ ?¬ê±°ë ê°ì¼ë©?
        {
            // ?ë¨¸ì???? 10? ëº? ê°ê³¼ ?ë¬? ???ë¬¸ì A? ASCII ì½ë ê°? 65ë¥? ??¨
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16?¼ë¡? ?? ëª«ì ????¥

        position++;    // ?ë¦¿ì ë³?ê²?

        if (decimal == 0)    // ëª«ì´ 0?´?ë©? ë°ë³µ? ??
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // ?¸?°?½?¸ ë²¡í°ë¥? ?ë©? ?¤ë¥¸ìª½ ?? 2?ë¦? ? ?ë¡? ì¶ë ¥
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;
/*
    kPrintString( 0, 0, "====================================================" );
    kPrintString( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintString( 0, 2, "                    Address:                        " );
    kPrintString( 27, 2, faultAddress );
    kPrintString( 0, 3, "====================================================" );
*/
    kPrintStringXY( 0, 0, "====================================================" );
    kPrintStringXY( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintStringXY( 0, 2, "                    Vector:                         " );
    kPrintStringXY( 27, 2, vcBuffer );
    kPrintStringXY( 0, 3, "====================================================" );

    while( 1 ) ;
}

/**
 *  ê³µíµ?¼ë¡? ?¬?©?? ?¸?°?½?¸ ?¸?¤?¬
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // ?¸?°?½?¸ê°? ë°ì??? ?ë¦¬ë ¤ê³? ë©ìì§?ë¥? ì¶ë ¥?? ë¶?ë¶?
    // ?¸?°?½?¸ ë²¡í°ë¥? ?ë©? ?¤ë¥¸ìª½ ?? 2?ë¦? ? ?ë¡? ì¶ë ¥
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // ë°ì? ?? ì¶ë ¥
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================

    // EOI ? ?¡
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  ?¤ë³´ë ?¸?°?½?¸? ?¸?¤?¬
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ?¸?°?½?¸ê°? ë°ì??? ?ë¦¬ë ¤ê³? ë©ìì§?ë¥? ì¶ë ¥?? ë¶?ë¶?
    // ?¸?°?½?¸ ë²¡í°ë¥? ?ë©? ?¼ìª? ?? 2?ë¦? ? ?ë¡? ì¶ë ¥
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // ë°ì? ?? ì¶ë ¥
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // ?¤ë³´ë ì»¨í¸ë¡¤ë¬?? ?°?´?°ë¥? ?½?´? ASCIIë¡? ë³????¬ ?? ?½?
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI ? ?¡
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

void kPageFaultExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    char hexadecimal[33]={0, };
    char faultAddress[33]={'0','x'};

    QWORD decimal = getFaultAddress();
    
    //register long decimal asm("rdx");

    DWORD * ErrorCode = (DWORD * ) 0x7FFFD0;
    _Bool isProtection = (*ErrorCode) & 0x1;
 
    int position = 0;
    /*
    while (1)
    {
        int mod = decimal % 16;    // 16?¼ë¡? ???? ? ?ë¨¸ì??ë¥? êµ¬í¨
        if (mod < 10) // ?ë¨¸ì??ê°? 10ë³´ë¤ ??¼ë©?
        {
            // ?«? 0? ASCII ì½ë ê°? 48 + ?ë¨¸ì??
            hexadecimal[position] = 48 + mod;
        }
        else    // ?ë¨¸ì??ê°? 10ë³´ë¤ ?¬ê±°ë ê°ì¼ë©?
        {
            // ?ë¨¸ì???? 10? ëº? ê°ê³¼ ?ë¬? ???ë¬¸ì A? ASCII ì½ë ê°? 65ë¥? ??¨
            hexadecimal[position] = 97 + (mod - 10);
        }

        decimal = decimal / 16;    // 16?¼ë¡? ?? ëª«ì ????¥

        position++;    // ?ë¦¿ì ë³?ê²?

        if (decimal == 0)    // ëª«ì´ 0?´?ë©? ë°ë³µ? ??
            break;
    }

    int idx=2;
    int p=position-1;
    while(p>=0){
        faultAddress[idx++]=hexadecimal[p--];
    }

    // ?¸?°?½?¸ ë²¡í°ë¥? ?ë©? ?¤ë¥¸ìª½ ?? 2?ë¦? ? ?ë¡? ì¶ë ¥
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;

    kPrintString( 0, 0, "====================================================" );
    if(isProtection)
        kPrintString( 0, 1, "          Protection Fault Occur~!!!!         " );
    else
    {
        kPrintString( 0, 1, "             Page Fault Occur~!!!!            " );
    }
    
    kPrintString( 0, 2, "                Address:                        " );
    kPrintString( 25, 2, faultAddress );
    kPrintString( 0, 3, "====================================================" );


    kModifyPageTableEntryFlags(decimal);
    */ 
}
void kTimerHandler(int iVectorNumber){
    BYTE bHour, bMinute, bSecond; 
    char cHour[3] = {'\0'},cMinute[3] = {'\0'}, cSecond[3] = {'\0'};
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ?¸?°?½?¸ê°? ë°ì??? ?ë¦¬ë ¤ê³? ë©ìì§?ë¥? ì¶ë ¥?? ë¶?ë¶?
    // ?¸?°?½?¸ ë²¡í°ë¥? ?ë©? ?¼ìª? ?? 2?ë¦? ? ?ë¡? ì¶ë ¥
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // ë°ì? ?? ì¶ë ¥
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================

    kReadRTCTime( &bHour, &bMinute, &bSecond );
    if(bHour < 10){
        kIToA(bHour,cHour, 10);
        kMemCpy(cHour +1 , cHour,1);
        cHour[0] = '0';
    }
    else
        kIToA(bHour,cHour, 10);

    if(bMinute < 10){
        kIToA(bMinute,cMinute, 10);
        kMemCpy(cMinute +1 , cMinute,1);
        cMinute[0] = '0';
    }

    else
        kIToA(bMinute,cMinute, 10);
        
    if(bSecond < 10){
        kIToA(bSecond,cSecond, 10);
        kMemCpy(cSecond +1 , cSecond,1);
        cSecond[0] = '0';
    }

    else
        kIToA(bSecond,cSecond, 10);
        
    kPrintStringXY( 71, 24, cHour);
    kPrintStringXY( 73, 24, ":");
    kPrintStringXY( 74, 24, cMinute);
    kPrintStringXY( 76, 24, ":");
    kPrintStringXY( 77, 24, cSecond);

    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );

    // EOI Àü¼Û
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );

    // Å¸ÀÌ¸Ó ¹ß»ý È½¼ö¸¦ Áõ°¡
    g_qwTickCount++;

    // ÅÂ½ºÅ©°¡ »ç¿ëÇÑ ÇÁ·Î¼¼¼­ÀÇ ½Ã°£À» ÁÙÀÓ
    kDecreaseProcessorTime();
    // ÇÁ·Î¼¼¼­°¡ »ç¿ëÇÒ ¼ö ÀÖ´Â ½Ã°£À» ´Ù ½è´Ù¸é ÅÂ½ºÅ© ÀüÈ¯ ¼öÇà
    if( kIsProcessorTimeExpired() == TRUE )
    {
        kScheduleInInterrupt();
    }
    
}

/**
 *  ÇÏµå µð½ºÅ©¿¡¼­ ¹ß»ýÇÏ´Â ÀÎÅÍ·´Æ®ÀÇ ÇÚµé·¯
 */
void kHDDHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iHDDInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ÀÎÅÍ·´Æ®°¡ ¹ß»ýÇßÀ½À» ¾Ë¸®·Á°í ¸Þ½ÃÁö¸¦ Ãâ·ÂÇÏ´Â ºÎºÐ
    // ÀÎÅÍ·´Æ® º¤ÅÍ¸¦ È­¸é ¿ÞÂÊ À§¿¡ 2ÀÚ¸® Á¤¼ö·Î Ãâ·Â
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // ¹ß»ýÇÑ È½¼ö Ãâ·Â
    vcBuffer[ 8 ] = '0' + g_iHDDInterruptCount;
    g_iHDDInterruptCount = ( g_iHDDInterruptCount + 1 ) % 10;
    // ¿ÞÂÊ À§¿¡ ÀÖ´Â ¸Þ½ÃÁö¿Í °ãÄ¡Áö ¾Êµµ·Ï (10, 0)¿¡ Ãâ·Â
    kPrintStringXY( 10, 0, vcBuffer );
    //=========================================================================

    // Ã¹ ¹øÂ° PATA Æ÷Æ®ÀÇ ÀÎÅÍ·´Æ® ¹ß»ý ¿©ºÎ¸¦ TRUE·Î ¼³Á¤
    kSetHDDInterruptFlag( TRUE, TRUE );
    // EOI Àü¼Û
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

