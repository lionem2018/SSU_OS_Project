/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ï¿½Ü¼ï¿½ ï¿½Ð¿ï¿½ ï¿½ï¿½ï¿½Ãµï¿½ ï¿½Ò½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"

// Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½Ìºï¿½ ï¿½ï¿½ï¿½ï¿½
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
        { "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", 
                kSetTimer },
        { "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
        { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
        { "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
        { "date", "Show Date And Time", kShowDateAndTime },
        { "createtask", "Create Task", kCreateTestTask },
        { "time", "Print time during processing a command", kPrintProcessingCommandTime }
};                      

char vcCommandHistoryList[ 10 ][ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
int iHistoryCount = 0;

//==============================================================================
//  ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Úµï¿½
//==============================================================================
/**
 *  ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
void kStartConsoleShell( void )
{
    int iHistoryIndex = 0;
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    BYTE bLastSecond, bLastMinute, bCurrentSecond, bCurrentMinute, bHour;

    
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
    kPrintTime(0, 0, 0, 0);
    while( 1 )
    {
        // Å°ï¿½ï¿½ ï¿½ï¿½ï¿½Åµï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?
        bKey = kGetCh();
        // Backspace Å° Ã³ï¿½ï¿½
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // ï¿½ï¿½ï¿½ï¿½ Ä¿ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½ï¿½î¼? ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï°ï¿? 
                // Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½Û¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        // ï¿½ï¿½ï¿½ï¿½ Å° Ã³ï¿½ï¿½
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );
            
            if( iCommandBufferIndex > 0 )
            {
                // Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½Û¿ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';

                kReadRTCTime( &bHour, &bLastMinute, &bLastSecond );

                kExecuteCommand( vcCommandBuffer );

                kReadRTCTime( &bHour, &bCurrentMinute, &bCurrentSecond );

                kSetCommandHistory(vcCommandBuffer);
                
                iHistoryIndex = iHistoryCount;
            }
            
            // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿? ï¿½ï¿½ Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE ); 
            kPrintTime(bLastMinute, bLastSecond, bCurrentMinute, bCurrentSecond);          
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // ï¿½ï¿½ï¿½ï¿½Æ® Å°, CAPS Lock, NUM Lock, Scroll Lockï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
         else if ( bKey == KEY_UP || bKey == KEY_DOWN)
        {
            kGetCursor( &iCursorX, &iCursorY );

            for(int i = 0; i < iCommandBufferIndex; i++)
            {
                iCursorX--;
                kPrintStringXY( iCursorX, iCursorY, " " );
            }

            kSetCursor(iCursorX, iCursorY);

            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;

            if( bKey == KEY_UP && iHistoryIndex > 0 )
                iHistoryIndex--;
            else if( bKey == KEY_DOWN && iHistoryIndex < iHistoryCount -1 )
                iHistoryIndex++;

            kPrintStringXY( iCursorX, iCursorY, vcCommandHistoryList[iHistoryIndex]);
            kSetCursor( iCursorX + kStrLen(vcCommandHistoryList[iHistoryIndex]), iCursorY );
            
            kMemCpy(vcCommandBuffer, vcCommandHistoryList[iHistoryIndex], kStrLen(vcCommandHistoryList[iHistoryIndex]));
            iCommandBufferIndex = kStrLen(vcCommandHistoryList[iHistoryIndex]);
        }
        else
        {
            // TAB??? ê³µë°±?œ¼ë¡? ? „?™˜
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // ë²„í¼?— ê³µê°„?´ ?‚¨?•„?žˆ?„ ?•Œë§? ê°??Š¥
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}
/*
 *  Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½Û¿ï¿½ ï¿½Ö´ï¿½ Ä¿ï¿½Çµå¸¦ ï¿½ï¿½ï¿½Ï¿ï¿½ ï¿½Ø´ï¿½ Ä¿ï¿½Çµå¸¦ Ã³ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ðµï¿½ Ä¿ï¿½Çµå¸¦ ï¿½ï¿½ï¿½ï¿½
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½Ë»ï¿½ï¿½Ø¼ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ì¸ï¿½ï¿½ï¿½ Ä¿ï¿½Çµå°¡ ï¿½Ö´ï¿½ï¿½ï¿½ È®ï¿½ï¿½
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // Ä¿ï¿½Çµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½Ï´ï¿½ï¿½ï¿½ ï¿½Ë»ï¿½
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ Ã£ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? ï¿½Ú·á±¸ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ðµï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½È¯
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // ï¿½ï¿½ ï¿½Ì»ï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½Í°ï¿? ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¸ï¿½Å­ ï¿½Ìµï¿½ï¿½Ï¸é¼­ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ë»ï¿½
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // ï¿½Ä¶ï¿½ï¿½ï¿½Í¸ï¿? ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½È¯
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // ï¿½Ä¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½Ä¡ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  Ä¿ï¿½Çµå¸¦ Ã³ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Úµï¿½
//==============================================================================
/**
 *  ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?
 */
void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;
    
    
    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ Ä¿ï¿½Çµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿?
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  È­ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 
 */
void kCls( const char* pcParameterBuffer )
{
    // ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï¹Ç·ï¿? È­ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ 1ï¿½ï¿½ Ä¿ï¿½ï¿½ ï¿½Ìµï¿½
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  ï¿½ï¿½ ï¿½Þ¸ï¿½ Å©ï¿½â¸¦ ï¿½ï¿½ï¿?
 */
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ï¿½ï¿½ï¿½Ú¿ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ú¸ï¿½ ï¿½ï¿½ï¿½Ú·ï¿½ ï¿½ï¿½È¯ï¿½Ï¿ï¿½ È­ï¿½é¿¡ ï¿½ï¿½ï¿?
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    // ï¿½Ä¶ï¿½ï¿½ï¿½ï¿? ï¿½Ê±ï¿½È­
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while( 1 )
    {
        // ï¿½ï¿½ï¿½ï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½Í¸ï¿? ï¿½ï¿½ï¿½ï¿½, ï¿½Ä¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½Ì°ï¿½ 0ï¿½Ì¸ï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½Í°ï¿? ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¹Ç·ï¿½
        // ï¿½ï¿½ï¿½ï¿½
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // ï¿½Ä¶ï¿½ï¿½ï¿½Í¿ï¿? ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï°ï¿? 16ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 10ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ç´ï¿½ï¿½Ï¿ï¿½ ï¿½ï¿½È¯ï¿½ï¿½ ï¿½ï¿½
        // ï¿½ï¿½ï¿½ï¿½ï¿? printfï¿½ï¿½ ï¿½ï¿½ï¿?
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, 
                 vcParameter, iLength );

        // 0xï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¸ï¿½ 16ï¿½ï¿½ï¿½ï¿½, ï¿½×¿Ü´ï¿½ 10ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ç´ï¿½
        if( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }
        
        iCount++;
    }
}

/**
 *  PCï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?(Reboot)
 */
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    // Å°ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Æ®ï¿½Ñ·ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ PCï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}

void kSetCommandHistory(const char* command)
{
    if (kMemCmp(vcCommandHistoryList[iHistoryCount - 1], command, kStrLen(command)))
    {
        if (iHistoryCount < 10)
        {
            kMemSet( vcCommandHistoryList[iHistoryCount], '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            kMemCpy( vcCommandHistoryList[iHistoryCount], command, kStrLen(command) );
            iHistoryCount++;
        }
        else
        {
            for (int i = 0; i < 9; i++)
            {
                kMemSet( vcCommandHistoryList[i], '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
                kMemCpy( vcCommandHistoryList[i], vcCommandHistoryList[i + 1], kStrLen(vcCommandHistoryList[i+ 1]) );
            }

            kMemSet( vcCommandHistoryList[9], '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            kMemCpy( vcCommandHistoryList[9], command, kStrLen(command) );
        }
    }
}
/**
 *  PIT ì»¨íŠ¸ë¡¤ëŸ¬?˜ ì¹´ìš´?„° 0 ?„¤? •
 */
void kSetTimer( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    PARAMETERLIST stList;
    long lValue;
    BOOL bPeriodic;

    // ?ŒŒ?¼ë¯¸í„° ì´ˆê¸°?™”
    kInitializeParameter( &stList, pcParameterBuffer );
    
    // milisecond ì¶”ì¶œ
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }
    lValue = kAToI( vcParameter, 10 );

    // Periodic ì¶”ì¶œ
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }    
    bPeriodic = kAToI( vcParameter, 10 );
    
    kInitializePIT( MSTOCOUNT( lValue ), bPeriodic );
    kPrintf( "Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic );
}

/**
 *  PIT ì»¨íŠ¸ë¡¤ëŸ¬ë¥? ì§ì ‘ ?‚¬?š©?•˜?—¬ ms ?™?•ˆ ???ê¸?  
 */
void kWaitUsingPIT( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    long lMillisecond;
    int i;
    
    // ?ŒŒ?¼ë¯¸í„° ì´ˆê¸°?™”
    kInitializeParameter( &stList, pcParameterBuffer );
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)wait 100(ms)\n" );
        return ;
    }
    
    lMillisecond = kAToI( pcParameterBuffer, 10 );
    kPrintf( "%d ms Sleep Start...\n", lMillisecond );
    
    // ?¸?„°?Ÿ½?Š¸ë¥? ë¹„í™œ?„±?™”?•˜ê³? PIT ì»¨íŠ¸ë¡¤ëŸ¬ë¥? ?†µ?•´ ì§ì ‘ ?‹œê°„ì„ ì¸¡ì •
    kDisableInterrupt();
    for( i = 0 ; i < lMillisecond / 30 ; i++ )
    {
        kWaitUsingDirectPIT( MSTOCOUNT( 30 ) );
    }
    kWaitUsingDirectPIT( MSTOCOUNT( lMillisecond % 30 ) );   
    kEnableInterrupt();
    kPrintf( "%d ms Sleep Complete\n", lMillisecond );
    
    // ????´ë¨? ë³µì›
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );
}

/**
 *  ????ž„ ?Š¤?ƒ¬?”„ ì¹´ìš´?„°ë¥? ?½?Œ  
 */
void kReadTimeStampCounter( const char* pcParameterBuffer )
{
    QWORD qwTSC;
    
    qwTSC = kReadTSC();
    kPrintf( "Time Stamp Counter = %q\n", qwTSC );
}

/**
 *  ?”„ë¡œì„¸?„œ?˜ ?†?„ë¥? ì¸¡ì •
 */
void kMeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    kPrintf( "Now Measuring." );
    
    // 10ì´? ?™?•ˆ ë³??™”?•œ ????ž„ ?Š¤?ƒ¬?”„ ì¹´ìš´?„°ë¥? ?´?š©?•˜?—¬ ?”„ë¡œì„¸?„œ?˜ ?†?„ë¥? ê°„ì ‘? ?œ¼ë¡? ì¸¡ì •
    kDisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf( "." );
    }
    // ????´ë¨? ë³µì›
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );    
    kEnableInterrupt();
    
    kPrintf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC ì»¨íŠ¸ë¡¤ëŸ¬?— ????ž¥?œ ?¼?ž ë°? ?‹œê°? ? •ë³´ë?? ?‘œ?‹œ
 */
void kShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC ì»¨íŠ¸ë¡¤ëŸ¬?—?„œ ?‹œê°? ë°? ?¼?žë¥? ?½?Œ
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    kPrintf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             kConvertDayOfWeekToString( bDayOfWeek ) );
    kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

// TCB ?žë£Œêµ¬ì¡°ì?? ?Š¤?ƒ ? •?˜
static TCB gs_vstTask[ 2 ] = { 0, };
static QWORD gs_vstStack[ 1024 ] = { 0, };

void kTestTask( void )
{
    int i = 0;
    
    while( 1 )
    {
        // ë©”ì‹œì§?ë¥? ì¶œë ¥?•˜ê³? ?‚¤ ?ž…? ¥?„ ???ê¸?
        kPrintf( "[%d] This message is from kTestTask. Press any key to switch "
                 "kConsoleShell~!!\n", i++ );
        kGetCh();
        
        // ?œ„?—?„œ ?‚¤ê°? ?ž…? ¥?˜ë©? ?ƒœ?Š¤?¬ë¥? ? „?™˜
        kSwitchContext( &( gs_vstTask[ 1 ].stContext ), &( gs_vstTask[ 0 ].stContext ) );
    }
}
/**
 *  ?ƒœ?Š¤?¬ë¥? ?ƒ?„±?•´?„œ ë©??‹° ?ƒœ?Š¤?‚¹ ?ˆ˜?–‰
 */
void kCreateTestTask( const char* pcParameterBuffer )
{
    KEYDATA stData;
    int i = 0;
    
    // ?ƒœ?Š¤?¬ ?„¤? •
    kSetUpTask( &( gs_vstTask[ 1 ] ), 1, 0, ( QWORD ) kTestTask, &( gs_vstStack ), 
                sizeof( gs_vstStack ) );
    
    // 'q' ?‚¤ê°? ?ž…? ¥?˜ì§? ?•Š?„ ?•Œê¹Œì?? ?ˆ˜?–‰
    while( 1 )
    {
        // ë©”ì‹œì§?ë¥? ì¶œë ¥?•˜ê³? ?‚¤ ?ž…? ¥?„ ???ê¸?
        kPrintf( "[%d] This message is from kConsoleShell. Press any key to "
                 "switch TestTask~!!\n", i++ );
        if( kGetCh() == 'q' )
        {
            break;
        }
        // ?œ„?—?„œ ?‚¤ê°? ?ž…? ¥?˜ë©? ?ƒœ?Š¤?¬ë¥? ? „?™˜
        kSwitchContext( &( gs_vstTask[ 0 ].stContext ), &( gs_vstTask[ 1 ].stContext ) );
    }
}

void kPrintTime(BYTE bLastMinute, BYTE bLastSecond, BYTE bCurrentMinute, BYTE bCurrentSecond){
    char cline[81] = {'=',};
    char cSecond[3] = {'\0'}, cMinute[3] = {'\0'}, cHour[3] = {'\0'};
    char cRunningMinute[3] = {'\0'}, cRunningSecond[3] = {'\0'};
    BYTE bSecond, bMinute, bHour;
    BYTE bRunningMinute, bRunningSecond;

    if(bLastMinute > bCurrentMinute)
        bRunningMinute = 60 + bCurrentMinute - bLastMinute;
    else
        bRunningMinute = bCurrentMinute - bLastMinute;

    if(bLastSecond > bCurrentSecond)
    {
        bRunningMinute--;
        bRunningSecond = 60 + bCurrentSecond - bLastSecond;

        if(bRunningSecond > 60)
            bRunningSecond -= 60;
    }
    else
        bRunningSecond = bCurrentSecond - bLastSecond;

    kMemSet(cline , '=' , 80);
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kIToA(bSecond,cSecond, 10);
    kIToA(bMinute,cMinute, 10);
    kIToA(bHour,cHour, 10);
    kIToA(bRunningMinute, cRunningMinute, 10);
    kIToA(bRunningSecond, cRunningSecond, 10);
    kPrintStringXY( 0, 23, cline);
    kPrintStringXY( 0, 24, CONSOLESHELL_RUNNINGTIME );
    kPrintStringXY( 13, 24, cRunningMinute);
    kPrintStringXY( 15, 24, ":");
    kPrintStringXY( 16, 24, cRunningSecond);
    kPrintStringXY( 57, 24,CONSOLESHELL_CURRENTTIME);
    kPrintStringXY( 71, 24, cHour);
    kPrintStringXY( 73, 24, ":");
    kPrintStringXY( 74, 24, cMinute);
    kPrintStringXY( 76, 24, ":");
    kPrintStringXY( 77, 24, cSecond);
}

void kPrintProcessingCommandTime( const char* pcParameterBuffer )
{
    QWORD lastTime, currentTime, resultTime1, resultTime2;
    QWORD qwLastTSC, qwTotalTSC = 0;
    double baseTSC;

    // kDisableInterrupt();

    // qwLastTSC = kReadTSC();
    // kWaitUsingDirectPIT( MSTOCOUNT( 1 ) );
    // qwTotalTSC += kReadTSC() - qwLastTSC;
    
    // //qwTotalTSC /= 1000000;
    // kPrintf( "USecond  = %d\n", qwTotalTSC ); 
    
    // preTime = kReadTSC();
    // kEnableInterrupt();  

    // kExecuteCommand(pcParameterBuffer); 

    // kDisableInterrupt(); 
    // postTime = kReadTSC();
    // kEnableInterrupt(); 

    // kPrintf( "postTime  = %d\n", postTime );
    // resultTime = postTime - preTime;
    // //resultTime /= 1000 * 1000;
    // kPrintf( "postTime - preTime = %d\n", resultTime );

    // kPrintf( "Time  = %d\n", resultTime / qwTotalTSC  );

    // kInitializePIT( MSTOCOUNT( 1 ), TRUE ); 

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    kDisableInterrupt();

        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 1 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;
    
    kPrintf( "%d per 10ns\n", qwTotalTSC / 100000 );
    
    lastTime = kReadTSC();
    kEnableInterrupt();  

    kExecuteCommand(pcParameterBuffer); 

    kDisableInterrupt(); 
    currentTime = kReadTSC();
    kEnableInterrupt(); 

    kPrintf( "currentTime(hex): %q\n", currentTime );
    resultTime1 = (currentTime - lastTime);
    //resultTime2 = (QWORD) (resultTime2 / ((double)qwTotalTSC / 1000000));
    
    kPrintf( "currentTime - lastTime(hex): %q\n", resultTime1 );
    kPrintf( "Running Time1(hex): %q\n", (resultTime1 * 10) / (qwTotalTSC / 100000) );

    resultTime1 = (resultTime1 * 10) / (qwTotalTSC / 100000);
    int minute = resultTime1 / 60000000000;
    int second = (resultTime1 % 60000000000) / 1000000000;
    int msecond = (resultTime1 % 1000000000) / 1000000;
    int usecond = (resultTime1 % 1000000) / 1000;
    int nsecond = resultTime1 % 1000;

    kPrintf("real %d:%d:%d:%d:%d\n", minute, second, msecond, usecond, nsecond);
    //kPrintf( "Running Time2(hex): %q\n", resultTime2 );

    kInitializePIT( MSTOCOUNT( 1 ), TRUE ); 

    /////////////////////////////////////////////////////////////////////////////////////////

    // WORD wLastCounter0;
    // WORD wCurrentCounter0;
    
    // // PIT ÄÁÆ®·Ñ·¯¸¦ 0~0xFFFF±îÁö ¹Ýº¹ÇØ¼­ Ä«¿îÆÃÇÏµµ·Ï ¼³Á¤
    // kInitializePIT( 0, TRUE );
    
    // // Áö±ÝºÎÅÍ wCount ÀÌ»ó Áõ°¡ÇÒ ¶§±îÁö ´ë±â
    // wLastCounter0 = kReadCounter0();

    // kExecuteCommand(pcParameterBuffer);

    // wCurrentCounter0 = kReadCounter0();
    
    // kPrintf( "Time  = %q\n", wLastCounter0 - wCurrentCounter0  );

    // kInitializePIT( MSTOCOUNT( 1 ), TRUE ); 
}