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
#include "Synchronization.h"
#include "Utility.h"

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
        { "time", "Print time during processing a command", kPrintProcessingCommandTime },
        { "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask },
        { "changepriority", "Change Task Priority, ex)changepriority 1(ID) 2(Priority)",
                kChangeTaskPriority },
        { "tasklist", "Show Task List", kShowTaskList },
        { "killtask", "End Task, ex)killtask 1(ID) or 0xffffffff(All Task)", kKillTask },
        { "cpuload", "Show Processor Load", kCPULoad },
        { "testmutex", "Test Mutex Function", kTestMutex },
        { "testthread", "Test Thread And Process Function", kTestThread },
        { "showmatrix", "Show Matrix Screen", kShowMatrix },
        { "checktask", "Check task state", kCheckTask },
        { "showfairness", "Show fairness rate of two task", kShowFairness}
};                                     

//==============================================================================
//  ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½Úµï¿½
//==============================================================================
/**
 *  ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
 */
void kStartConsoleShell( void )
{
    char vcCommandHistoryList[ 10 ][ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ] = {0};
    int iHistoryCount = 0,iHistoryIndex = 0;
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    BYTE bLastSecond, bLastMinute, bCurrentSecond, bCurrentMinute, bHour;
    char cline[81] = {'=',};

    kPrintf( CONSOLESHELL_PROMPTMESSAGE );

    kMemSet(cline , '=' , 80);
    kPrintStringXY( 0, 23, cline);
    kPrintStringXY( 0, 24, CONSOLESHELL_RUNNINGTIME );
    kPrintTime(0, 0, 0, 0);
    kPrintStringXY( 58,24, CONSOLESHELL_CURRENTTIME );
    
    while( 1 )
    {
        
        // Å°ï¿½ï¿½ ï¿½ï¿½ï¿½Åµï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?
        bLastSecond = bLastMinute = bCurrentSecond = bCurrentMinute = bHour = 0;
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
                kSetCommandHistory(vcCommandHistoryList, vcCommandBuffer, &iHistoryCount);
                
                iHistoryIndex = iHistoryCount;
            }
            
            // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿? ï¿½ï¿½ Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE ); 
            kPrintStringXY( 0, 23, cline);
            kPrintTime(bLastMinute, bLastSecond, bCurrentMinute, bCurrentSecond);
            kPrintStringXY( 0, 24, CONSOLESHELL_RUNNINGTIME );
            kPrintStringXY( 58, 24,CONSOLESHELL_CURRENTTIME);         
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

void kSetCommandHistory( char (* historyList)[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT], const char* command, int * count )
{
    if (kMemCmp(historyList[(*count) - 1], command, kStrLen(command)))
    {
        if (*count < 10)
        {
            kMemCpy(historyList[(*count)], command, kStrLen(command));
            (*count)++;
        }
        else
        {
            for (int i = 0; i < 9; i++)
            {
                kMemSet( historyList[i], '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
                kMemCpy( historyList[i], historyList[i + 1], kStrLen(historyList[i + 1]) );
            }

            kMemSet( historyList[9], '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            kMemCpy(historyList[9], command, kStrLen(command));
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
// void kCreateTestTask( const char* pcParameterBuffer )
// {
//     KEYDATA stData;
//     int i = 0;
    
//     // ?ƒœ?Š¤?¬ ?„¤? •
//     kSetUpTask( &( gs_vstTask[ 1 ] ), 1, 0, ( QWORD ) kTestTask, &( gs_vstStack ), 
//                 sizeof( gs_vstStack ) );
    
//     // 'q' ?‚¤ê°? ?ž…? ¥?˜ì§? ?•Š?„ ?•Œê¹Œì?? ?ˆ˜?–‰
//     while( 1 )
//     {
//         // ë©”ì‹œì§?ë¥? ì¶œë ¥?•˜ê³? ?‚¤ ?ž…? ¥?„ ???ê¸?
//         kPrintf( "[%d] This message is from kConsoleShell. Press any key to "
//                  "switch TestTask~!!\n", i++ );
//         if( kGetCh() == 'q' )
//         {
//             break;
//         }
//         // ?œ„?—?„œ ?‚¤ê°? ?ž…? ¥?˜ë©? ?ƒœ?Š¤?¬ë¥? ? „?™˜
//         kSwitchContext( &( gs_vstTask[ 0 ].stContext ), &( gs_vstTask[ 1 ].stContext ) );
//     }
// }

void kPrintTime(BYTE bLastMinute, BYTE bLastSecond, BYTE bCurrentMinute, BYTE bCurrentSecond){

    char cRunningMinute[3] = {'\0'}, cRunningSecond[3] = {'\0'};
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

    if(bRunningMinute < 10){
        kIToA(bRunningMinute, cRunningMinute, 10);
        kMemCpy(cRunningMinute +1, cRunningMinute , 1);
        cRunningMinute[0] = '0';
        
    }

    else
        kIToA(bRunningMinute, cRunningMinute, 10);

    if(bRunningSecond < 10){
        kIToA(bRunningSecond, cRunningSecond, 10);
        kMemCpy(cRunningSecond + 1, cRunningSecond, 1);
        cRunningSecond[0] = '0';
    }

    else
        kIToA(bRunningSecond, cRunningSecond, 10);

    kPrintStringXY( 13, 24, cRunningMinute);
    kPrintStringXY( 15, 24, ":");
    kPrintStringXY( 16, 24, cRunningSecond);
    
   
}

void kPrintProcessingCommandTime( const char* pcParameterBuffer )
{
    QWORD lastTime, currentTime, resultTime;
    QWORD qwLastTSC, qwTotalTSC = 0; 

    if( kStrLen(pcParameterBuffer) == 0)
    {
        kPrintf( "ex)time <command>\n" );
        return ;
    }

    kDisableInterrupt();

    qwLastTSC = kReadTSC();
    kWaitUsingDirectPIT( MSTOCOUNT( 1 ) );
    qwTotalTSC += kReadTSC() - qwLastTSC;
    
    //kPrintf( "%d per 10ns\n", qwTotalTSC / 100000 );
    
    lastTime = kReadTSC();
    kEnableInterrupt();  

    kExecuteCommand(pcParameterBuffer); 

    kDisableInterrupt(); 
    currentTime = kReadTSC();
    kEnableInterrupt(); 

    //kPrintf( "lastTime(hex): %q\n", lastTime );
    //kPrintf( "currentTime(hex): %q\n", currentTime );
    resultTime = (currentTime - lastTime);
    
    //kPrintf( "currentTime - lastTime(hex): %q\n", resultTime );
   // kPrintf( "Running Time1(hex): %q\n", (resultTime * 10) / (qwTotalTSC / 100000) );

    resultTime = (resultTime * 10) / (qwTotalTSC / 100000);
    QWORD minute = resultTime / 60000000000;
    QWORD second = (resultTime % 60000000000) / 1000000000;
    QWORD msecond = (resultTime % 1000000000) / 1000000;
    QWORD usecond = (resultTime % 1000000) / 1000;
    QWORD nsecond = resultTime % 1000;

    kPrintf("real %d:%d:%d:%d:%d\n", minute, second, msecond, usecond, nsecond);

    kInitializePIT( MSTOCOUNT( 1 ), TRUE ); 

}

/**
 *  ÅÂ½ºÅ© 1
 *      È­¸é Å×µÎ¸®¸¦ µ¹¸é¼­ ¹®ÀÚ¸¦ Ãâ·Â
 */
static void kTestTask1( void )
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    
    // ÀÚ½ÅÀÇ ID¸¦ ¾ò¾î¼­ È­¸é ¿ÀÇÁ¼ÂÀ¸·Î »ç¿ë
    pstRunningTask = kGetRunningTask();
    iMargin = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) % 10;
    
    // È­¸é ³× ±ÍÅüÀÌ¸¦ µ¹¸é¼­ ¹®ÀÚ Ãâ·Â
    for( j = 0 ; j < 1000000 ; j++ )
    {
        switch( i )
        {
        case 0:
            iX++;
            if( iX >= ( CONSOLE_WIDTH - iMargin ) )
            {
                i = 1;
            }
            break;
            
        case 1:
            iY++;
            if( iY >= ( CONSOLE_HEIGHT - iMargin ) )
            {
                i = 2;
            }
            break;
            
        case 2:
            iX--;
            if( iX < iMargin )
            {
                i = 3;
            }
            break;
            
        case 3:
            iY--;
            if( iY < iMargin )
            {
                i = 0;
            }
            break;
        }
        
        // ¹®ÀÚ ¹× »ö±ò ÁöÁ¤
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bCharactor = bData;
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bAttribute = bData & 0x0F;
        bData++;
        
        // ´Ù¸¥ ÅÂ½ºÅ©·Î ÀüÈ¯
        //kSchedule();
    }

    //kExitTask();
}

/**
 *  ÅÂ½ºÅ© 2
 *      ÀÚ½ÅÀÇ ID¸¦ Âü°íÇÏ¿© Æ¯Á¤ À§Ä¡¿¡ È¸ÀüÇÏ´Â ¹Ù¶÷°³ºñ¸¦ Ãâ·Â
 */
static void kTestTask2( void )
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };
    
    // ÀÚ½ÅÀÇ ID¸¦ ¾ò¾î¼­ È­¸é ¿ÀÇÁ¼ÂÀ¸·Î »ç¿ë
    pstRunningTask = kGetRunningTask();
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - 
        ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    while(1)
    {
        // È¸ÀüÇÏ´Â ¹Ù¶÷°³ºñ¸¦ Ç¥½Ã
        pstScreen[ iOffset ].bCharactor = vcData[ i % 4 ];
        // »ö±ò ÁöÁ¤
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
        i++;
        
        // ´Ù¸¥ ÅÂ½ºÅ©·Î ÀüÈ¯
        //kSchedule();
    }
}

/**
 *  ÅÂ½ºÅ©¸¦ »ý¼ºÇØ¼­ ¸ÖÆ¼ ÅÂ½ºÅ· ¼öÇà
 */
static void kCreateTestTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcType[ 30 ];
    char vcCount[ 30 ];
    int i;
    
    // ÆÄ¶ó¹ÌÅÍ¸¦ ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcType );
    kGetNextParameter( &stList, vcCount );

    switch( kAToI( vcType, 10 ) )
    {
    // Å¸ÀÔ 1 ÅÂ½ºÅ© »ý¼º
    case 1:
        for( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {    
            if( kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask1 ) == NULL )
            {
                break;
            }
        }
        
        kPrintf( "Task1 %d Created\n", i );
        break;
        
    // Å¸ÀÔ 2 ÅÂ½ºÅ© »ý¼º
    case 2:
    default:
        for( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {    
            if( kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 ) == NULL )
            {
                break;
            }
        }
        kPrintf( "Task2 %d Created\n", i );

        break;
    }    
}   

/**
 *  ÅÂ½ºÅ©ÀÇ ¿ì¼± ¼øÀ§¸¦ º¯°æ
 */
static void kChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // ÆÄ¶ó¹ÌÅÍ¸¦ ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    kGetNextParameter( &stList, vcPriority );
    
    // ÅÂ½ºÅ©ÀÇ ¿ì¼± ¼øÀ§¸¦ º¯°æ
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }
    
    bPriority = kAToI( vcPriority, 10 );
    
    kPrintf( "Change Task Priority ID [0x%q] Priority[%d] ", qwID, bPriority );
    if( kChangePriority( qwID, bPriority ) == TRUE )
    {
        kPrintf( "Success\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
}

/**
 *  ÇöÀç »ý¼ºµÈ ¸ðµç ÅÂ½ºÅ©ÀÇ Á¤º¸¸¦ Ãâ·Â
 */
static void kShowTaskList( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;
    
    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        // TCB¸¦ ±¸ÇØ¼­ TCB°¡ »ç¿ë ÁßÀÌ¸é ID¸¦ Ãâ·Â
        pstTCB = kGetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // ÅÂ½ºÅ©°¡ 10°³ Ãâ·ÂµÉ ¶§¸¶´Ù, °è¼Ó ÅÂ½ºÅ© Á¤º¸¸¦ Ç¥½ÃÇÒÁö ¿©ºÎ¸¦ È®ÀÎ
            if( ( iCount != 0 ) && ( ( iCount % 10 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
            }
            
            kPrintf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q], Thread[%d]\n", 1 + iCount++,
                     pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ), 
                     pstTCB->qwFlags, kGetListCount( &( pstTCB->stChildThreadList ) ) );
            kPrintf( "    Parent PID[0x%Q], Memory Address[0x%Q], Size[0x%Q]\n",
                    pstTCB->qwParentProcessID, pstTCB->pvMemoryAddress, pstTCB->qwMemorySize );
            kPrintf( "    Ticket[%d], Schedule Time[%d]\n", pstTCB->ticket, pstTCB->execCount);
        }
    }
}

/**
 *  ÅÂ½ºÅ©¸¦ Á¾·á
 */
static void kKillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // ÆÄ¶ó¹ÌÅÍ¸¦ ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    
    // ÅÂ½ºÅ©¸¦ Á¾·á
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }
    
    // Æ¯Á¤ ID¸¸ Á¾·áÇÏ´Â °æ¿ì
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = kGetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // ½Ã½ºÅÛ Å×½ºÆ®´Â Á¦¿Ü
        if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
        {
            kPrintf( "Kill Task ID [0x%q] ", qwID );
            if( kEndTask( qwID ) == TRUE )
            {
                kPrintf( "Success\n" );
            }
            else
            {
                kPrintf( "Fail\n" );
            }
        }
        else
        {
            kPrintf( "Task does not exist or task is system task\n" );
        }
    }
    // ÄÜ¼Ö ¼Ð°ú À¯ÈÞ ÅÂ½ºÅ©¸¦ Á¦¿ÜÇÏ°í ¸ðµç ÅÂ½ºÅ© Á¾·á
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = kGetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // ½Ã½ºÅÛ Å×½ºÆ®´Â »èÁ¦ ¸ñ·Ï¿¡¼­ Á¦¿Ü
            if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
            {
                kPrintf( "Kill Task ID [0x%q] ", qwID );
                if( kEndTask( qwID ) == TRUE )
                {
                    kPrintf( "Success\n" );
                }
                else
                {
                    kPrintf( "Fail\n" );
                }
            }
        }
    }
}

/**
 *  ÇÁ·Î¼¼¼­ÀÇ »ç¿ë·üÀ» Ç¥½Ã
 */
static void kCPULoad( const char* pcParameterBuffer )
{
    kPrintf( "Processor Load : %d%%\n", kGetProcessorLoad() );
    kPrintf( "Console Load : %d%%\n", kGetConsoleProcessorLoad() );
}
    
// ¹ÂÅØ½º Å×½ºÆ®¿ë ¹ÂÅØ½º¿Í º¯¼ö
static MUTEX gs_stMutex;
static volatile QWORD gs_qwAdder;

/**
 *  ¹ÂÅØ½º¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ©
 */
static void kPrintNumberTask( void )
{
    int i;
    int j;
    QWORD qwTickCount;

    // 50ms Á¤µµ ´ë±âÇÏ¿© ÄÜ¼Ö ¼ÐÀÌ Ãâ·ÂÇÏ´Â ¸Þ½ÃÁö¿Í °ãÄ¡Áö ¾Êµµ·Ï ÇÔ
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 50 )
    {
        kSchedule();
    }    
    
    // ·çÇÁ¸¦ µ¹¸é¼­ ¼ýÀÚ¸¦ Ãâ·Â
    for( i = 0 ; i < 5 ; i++ )
    {
        kLock( &( gs_stMutex ) );
        kPrintf( "Task ID [0x%Q] Value[%d]\n", kGetRunningTask()->stLink.qwID,
                gs_qwAdder );
        
        gs_qwAdder += 1;
        kUnlock( & ( gs_stMutex ) );
    
        // ÇÁ·Î¼¼¼­ ¼Ò¸ð¸¦ ´Ã¸®·Á°í Ãß°¡ÇÑ ÄÚµå
        for( j = 0 ; j < 30000 ; j++ ) ;
    }
    
    // ¸ðµç ÅÂ½ºÅ©°¡ Á¾·áÇÒ ¶§±îÁö 1ÃÊ(100ms) Á¤µµ ´ë±â
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 1000 )
    {
        kSchedule();
    }    
    
    // ÅÂ½ºÅ© Á¾·á
    //kExitTask();
}

/**
 *  ¹ÂÅØ½º¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ© »ý¼º
 */
static void kTestMutex( const char* pcParameterBuffer )
{
    int i;
    
    gs_qwAdder = 1;
    
    // ¹ÂÅØ½º ÃÊ±âÈ­
    kInitializeMutex( &gs_stMutex );
    
    for( i = 0 ; i < 3 ; i++ )
    {
        // ¹ÂÅØ½º¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ©¸¦ 3°³ »ý¼º
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kPrintNumberTask );
    }    
    kPrintf( "Wait Util %d Task End...\n", i );
    kGetCh();
}

/**
 *  ÅÂ½ºÅ© 2¸¦ ÀÚ½ÅÀÇ ½º·¹µå·Î »ý¼ºÇÏ´Â ÅÂ½ºÅ©
 */
static void kCreateThreadTask( void )
{
    int i;
    
    for( i = 0 ; i < 3 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask2 );
    }
    
    while( 1 )
    {
        kSleep( 1 );
    }
}

/**
 *  ½º·¹µå¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ© »ý¼º
 */
static void kTestThread( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_PROCESS, ( void * )0xEEEEEEEE, 0x1000, 
                              ( QWORD ) kCreateThreadTask );
    if( pstProcess != NULL )
    {
        kPrintf( "Process [0x%Q] Create Success\n", pstProcess->stLink.qwID ); 
    }
    else
    {
        kPrintf( "Process Create Fail\n" );
    }
}


/**
 *  Ã¶ÀÚ¸¦ Èê·¯³»¸®°Ô ÇÏ´Â ½º·¹µå
 */
static void kDropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    srand(kGetTickCount());

    iX = rand() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // Àá½Ã ´ë±âÇÔ
        kSleep( rand() % 20 );
        
        if( ( rand() % 20 ) < 16 )
        {
            vcText[ 0 ] = ' ';
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                kPrintStringXY( iX, i , vcText );
                kSleep( 50 );
            }
        }        
        else
        {
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                vcText[ 0 ] = i + rand();
                kPrintStringXY( iX, i, vcText );
                kSleep( 50 );
            }
        }
    }
}

/**
 *  ½º·¹µå¸¦ »ý¼ºÇÏ¿© ¸ÅÆ®¸¯½º È­¸éÃ³·³ º¸¿©ÁÖ´Â ÇÁ·Î¼¼½º
 */
static void kMatrixProcess( void )
{
    int i;
    
    for( i = 0 ; i < 300 ; i++ )
    {
        if( kCreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0, 
                         ( QWORD ) kDropCharactorThread ) == NULL )
        {
            break;
        }
        
        kSleep( rand() % 5 + 5 );
    }
    
    kPrintf( "%d Thread is created\n", i );

    // Å°°¡ ÀÔ·ÂµÇ¸é ÇÁ·Î¼¼½º Á¾·á
    kGetCh();
}

/**
 *  ¸ÅÆ®¸¯½º È­¸éÀ» º¸¿©ÁÜ
 */
static void kShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = kCreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) kMatrixProcess );
    if( pstProcess != NULL )
    {
        kPrintf( "Matrix Process [0x%Q] Create Success\n" );

        // ÅÂ½ºÅ©°¡ Á¾·á µÉ ¶§±îÁö ´ë±â
        while( ( pstProcess->stLink.qwID >> 32 ) != 0 )
        {
            kSleep( 100 );
        }
    }
    else
    {
        kPrintf( "Matrix Process Create Fail\n" );
    }
}

static void kCheckTask( const char* pcParameterBuffer )
{
    TCB* pstTCB;
    int lastTimeCount = 0, currentTimeCount = 0;

    kCreateTestTask("2 5");
    kChangeTaskPriority("0x300000002 0");
    kChangeTaskPriority("0x400000003 1");
    kChangeTaskPriority("0x500000004 2");
    kChangeTaskPriority("0x600000005 3");
    kChangeTaskPriority("0x700000006 4");

    while(1)
    {
        kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
        kPrintf( " ReadyListCount: %d", getSchedulerReadyListCount() );
        kPrintf( " whole Tick Count: %d\n", kGetTickCount() );

            for( int i = 0 ; i < TASK_MAXCOUNT ; i++ )
            {
                // TCB¸¦ ±¸ÇØ¼­ TCB°¡ »ç¿ë ÁßÀÌ¸é ID¸¦ Ãâ·Â
                pstTCB = kGetTCBInTCBPool( i );
                if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
                {
                    kPrintf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q], Thread[%d]\n", i,
                            pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ), 
                            pstTCB->qwFlags, kGetListCount( &( pstTCB->stChildThreadList ) ) );
                    kPrintf( "    Parent PID[0x%Q], Memory Address[0x%Q], Size[0x%Q]\n",
                            pstTCB->qwParentProcessID, pstTCB->pvMemoryAddress, pstTCB->qwMemorySize );
                    kPrintf( "    Ticket[%d], Schedule Time[%d]\n", pstTCB->ticket, pstTCB->execCount);

                    //pstTCB->execCount = 0;
                }
            }

        kSleep( 2000 );
    }
}

void kShowFairness(){
    int i=0;
    for( i = 0 ; i < 2 ; i++ )
        {    
             if( kCreateFairnessTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kTestTask1,i)== NULL )
            {
                break;
            }
        }
    kPrintf( "Task1 %d Created\n", i );
}