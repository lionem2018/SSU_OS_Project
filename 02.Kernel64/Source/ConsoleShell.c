/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ï¿½Ü¼ï¿½ ï¿½Ğ¿ï¿½ ï¿½ï¿½ï¿½Ãµï¿½ ï¿½Ò½ï¿½ ï¿½ï¿½ï¿½ï¿½
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
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "FileSystem.h"
#include "User.h"

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
        { "showfairness", "Show fairness rate of two task", kShowFairness},          
        { "dynamicmeminfo", "Show Dyanmic Memory Information", kShowDyanmicMemoryInformation },
        { "testseqalloc", "Test Sequential Allocation & Free", kTestSequentialAllocation },
        { "testranalloc", "Test Random Allocation & Free", kTestRandomAllocation },
        { "malloc", "Test Allocation & Free", kTestFreeList },
        { "hddinfo", "Show HDD Information", kShowHDDInformation },
        { "readsector", "Read HDD Sector, ex)readsector 0(LBA) 10(count)", 
                kReadSector },
        { "writesector", "Write HDD Sector, ex)writesector 0(LBA) 10(count)", 
                kWriteSector },
        { "mounthdd", "Mount HDD", kMountHDD },
        { "formathdd", "Format HDD", kFormatHDD },
        { "filesysteminfo", "Show File System Information", kShowFileSystemInformation },
        { "createfile", "Create File, ex)createfile a.txt", kCreateFileInRootDirectory },
        { "deletefile", "Delete File, ex)deletefile a.txt", kDeleteFileInRootDirectory },
        { "dir", "Show Directory", kShowRootDirectory },
        { "writefile", "Write Data To File, ex) writefile a.txt", kWriteDataToFile },
        { "readfile", "Read Data From File, ex) readfile a.txt", kReadDataFromFile },
        { "testfileio", "Test File I/O Function", kTestFileIO },
        { "testperformance", "Test File Read/WritePerformance", kTestPerformance },
        { "flush", "Flush File System Cache", kFlushCache },
        { "adduser", "Add User", kAddUser },
        { "chuser", "fChange User", kChangeUser },
        { "chmod", "Change File Permission", kChangePermission },
        { "chown", "Change File Owner root only use", kChangeOwner },
        { "chpass", "Change Password", kChangePasswd},
        { "deluser", "Delete User", kDeleteUser},
        { "showuser", "Show User Table", kShowUser},
};                                

char currentUserID [ 16 ];

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

    if(!firstUser()) selectUser();

    kPrintf( "%s", currentUserID );
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
            kPrintf( "%s", currentUserID );
            kPrintf( CONSOLESHELL_PROMPTMESSAGE ); 
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
            
            // ë²„í¼?— ê³µê°„?´ ?‚¨?•„?ˆ?„ ?•Œë§? ê°??Š¥
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

void selectUser(){
    char id[16]={0},password[16]={0};
    BOOL res;

    while(1){
        kPrintf("Input ID: ");
        getString(id);

        kPrintf("Input Password: ");
        getString(password);

        res = checkUserInfo(id,password);
        
        if(res==TRUE){
            kStrCpy(currentUserID, id);
            return;
        }
        else{
            kPrintf("Input Again\n");
        }
    }
}

void getString(char* input){
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;

    while( 1 )
    {
        bKey = kGetCh();
        // Backspace Å° 
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );
            
            // Ä¿ï¿½Çµï¿½ ï¿½ï¿½ï¿½Û¿ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½
            input[ iCommandBufferIndex ] = '\0';

            return;
        }
        // ï¿½ï¿½ï¿½ï¿½Æ® Å°, CAPS Lock, NUM Lock, Scroll Lockï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else
        {
            // TAB??? ê³µë°±?œ¼ë¡? ? „?™˜
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // ë²„í¼?— ê³µê°„?´ ?‚¨?•„?ˆ?„ ?•Œë§? ê°??Š¥
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                input[ iCommandBufferIndex++ ] = bKey;
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
    
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ğµï¿½ Ä¿ï¿½Çµå¸¦ ï¿½ï¿½ï¿½ï¿½
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
 *  ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ğµï¿½ ï¿½Ä¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½ï¿½ï¿? ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½È¯
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

    // µµ¿ò¸» Ãâ·Â
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // ¸ñ·ÏÀÌ ¸¹À» °æ¿ì ³ª´²¼­ º¸¿©ÁÜ
        if( ( i != 0 ) && ( ( i % 20 ) == 0 ) )
        {
            kPrintf( "Press any key to continue... ('q' is exit) : " );
            if( kGetCh() == 'q' )
            {
                kPrintf( "\n" );
                break;
            }        
            kPrintf( "\n" );
        }
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
 *  ï¿½ï¿½ ï¿½Ş¸ï¿½ Å©ï¿½â¸¦ ï¿½ï¿½ï¿?
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
 *  ????„ ?Š¤?ƒ¬?”„ ì¹´ìš´?„°ë¥? ?½?Œ  
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
    
    // 10ì´? ?™?•ˆ ë³??™”?•œ ????„ ?Š¤?ƒ¬?”„ ì¹´ìš´?„°ë¥? ?´?š©?•˜?—¬ ?”„ë¡œì„¸?„œ?˜ ?†?„ë¥? ê°„ì ‘? ?œ¼ë¡? ì¸¡ì •
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
 *  RTC ì»¨íŠ¸ë¡¤ëŸ¬?— ????¥?œ ?¼? ë°? ?‹œê°? ? •ë³´ë?? ?‘œ?‹œ
 */
void kShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC ì»¨íŠ¸ë¡¤ëŸ¬?—?„œ ?‹œê°? ë°? ?¼?ë¥? ?½?Œ
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    kPrintf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             kConvertDayOfWeekToString( bDayOfWeek ) );
    kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

// TCB ?ë£Œêµ¬ì¡°ì?? ?Š¤?ƒ ? •?˜
static TCB gs_vstTask[ 2 ] = { 0, };
static QWORD gs_vstStack[ 1024 ] = { 0, };

void kTestTask( void )
{
    int i = 0;
    
    while( 1 )
    {
        // ë©”ì‹œì§?ë¥? ì¶œë ¥?•˜ê³? ?‚¤ ?…? ¥?„ ???ê¸?
        kPrintf( "[%d] This message is from kTestTask. Press any key to switch "
                 "kConsoleShell~!!\n", i++ );
        kGetCh();
        
        // ?œ„?—?„œ ?‚¤ê°? ?…? ¥?˜ë©? ?ƒœ?Š¤?¬ë¥? ? „?™˜
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
    
//     // 'q' ?‚¤ê°? ?…? ¥?˜ì§? ?•Š?„ ?•Œê¹Œì?? ?ˆ˜?–‰
//     while( 1 )
//     {
//         // ë©”ì‹œì§?ë¥? ì¶œë ¥?•˜ê³? ?‚¤ ?…? ¥?„ ???ê¸?
//         kPrintf( "[%d] This message is from kConsoleShell. Press any key to "
//                  "switch TestTask~!!\n", i++ );
//         if( kGetCh() == 'q' )
//         {
//             break;
//         }
//         // ?œ„?—?„œ ?‚¤ê°? ?…? ¥?˜ë©? ?ƒœ?Š¤?¬ë¥? ? „?™˜
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
 *  ÅÂ½ºÅ©¸¦ »ı¼ºÇØ¼­ ¸ÖÆ¼ ÅÂ½ºÅ· ¼öÇà
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
    // Å¸ÀÔ 1 ÅÂ½ºÅ© »ı¼º
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
        
    // Å¸ÀÔ 2 ÅÂ½ºÅ© »ı¼º
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
 *  ÇöÀç »ı¼ºµÈ ¸ğµç ÅÂ½ºÅ©ÀÇ Á¤º¸¸¦ Ãâ·Â
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
            kPrintf( "    Ticket[%d], CPU Time[%d]\n", pstTCB->ticket, pstTCB->execCount);
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
    // ÄÜ¼Ö ¼Ğ°ú À¯ÈŞ ÅÂ½ºÅ©¸¦ Á¦¿ÜÇÏ°í ¸ğµç ÅÂ½ºÅ© Á¾·á
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

    // 50ms Á¤µµ ´ë±âÇÏ¿© ÄÜ¼Ö ¼ĞÀÌ Ãâ·ÂÇÏ´Â ¸Ş½ÃÁö¿Í °ãÄ¡Áö ¾Êµµ·Ï ÇÔ
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 50 )
    {
        kSchedule();
    }    
    
    // ·çÇÁ¸¦ µ¹¸é¼­ ¼ıÀÚ¸¦ Ãâ·Â
    for( i = 0 ; i < 5 ; i++ )
    {
        kLock( &( gs_stMutex ) );
        kPrintf( "Task ID [0x%Q] Value[%d]\n", kGetRunningTask()->stLink.qwID,
                gs_qwAdder );
        
        gs_qwAdder += 1;
        kUnlock( & ( gs_stMutex ) );
    
        // ÇÁ·Î¼¼¼­ ¼Ò¸ğ¸¦ ´Ã¸®·Á°í Ãß°¡ÇÑ ÄÚµå
        for( j = 0 ; j < 30000 ; j++ ) ;
    }
    
    // ¸ğµç ÅÂ½ºÅ©°¡ Á¾·áÇÒ ¶§±îÁö 1ÃÊ(100ms) Á¤µµ ´ë±â
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 1000 )
    {
        kSchedule();
    }    
    
    // ÅÂ½ºÅ© Á¾·á
    //kExitTask();
}

/**
 *  ¹ÂÅØ½º¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ© »ı¼º
 */
static void kTestMutex( const char* pcParameterBuffer )
{
    int i;
    
    gs_qwAdder = 1;
    
    // ¹ÂÅØ½º ÃÊ±âÈ­
    kInitializeMutex( &gs_stMutex );
    
    for( i = 0 ; i < 3 ; i++ )
    {
        // ¹ÂÅØ½º¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ©¸¦ 3°³ »ı¼º
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kPrintNumberTask );
    }    
    kPrintf( "Wait Util %d Task End...\n", i );
    kGetCh();
}

/**
 *  ÅÂ½ºÅ© 2¸¦ ÀÚ½ÅÀÇ ½º·¹µå·Î »ı¼ºÇÏ´Â ÅÂ½ºÅ©
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
 *  ½º·¹µå¸¦ Å×½ºÆ®ÇÏ´Â ÅÂ½ºÅ© »ı¼º
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
 *  ½º·¹µå¸¦ »ı¼ºÇÏ¿© ¸ÅÆ®¸¯½º È­¸éÃ³·³ º¸¿©ÁÖ´Â ÇÁ·Î¼¼½º
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
                    kPrintf( "    Ticket[%d], CPU Time[%d]\n", pstTCB->ticket, pstTCB->execCount);

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

/**
 *  µ¿Àû ¸Ş¸ğ¸® Á¤º¸¸¦ Ç¥½Ã
 */
static void kShowDyanmicMemoryInformation( const char* pcParameterBuffer )
{
    QWORD qwStartAddress, qwTotalSize, qwMetaSize, qwUsedSize;
    
    kGetDynamicMemoryInformation( &qwStartAddress, &qwTotalSize, &qwMetaSize, 
            &qwUsedSize );

    kPrintf( "============ Dynamic Memory Information ============\n" );
    kPrintf( "Start Address: [0x%Q]\n", qwStartAddress );
    kPrintf( "Total Size:    [0x%Q]byte, [%d]MB\n", qwTotalSize, 
            qwTotalSize / 1024 / 1024 );
    kPrintf( "Meta Size:     [0x%Q]byte, [%d]KB\n", qwMetaSize, 
            qwMetaSize / 1024 );
    kPrintf( "Used Size:     [0x%Q]byte, [%d]KB\n", qwUsedSize, qwUsedSize / 1024 );
}

/**
 *  ¸ğµç ºí·Ï ¸®½ºÆ®ÀÇ ºí·ÏÀ» ¼øÂ÷ÀûÀ¸·Î ÇÒ´çÇÏ°í ÇØÁ¦ÇÏ´Â Å×½ºÆ®
 */
static void kTestSequentialAllocation( const char* pcParameterBuffer )
{
    DYNAMICMEMORY* pstMemory;
    long i, j, k;
    QWORD* pqwBuffer;
    
    kPrintf( "============ Dynamic Memory Test ============\n" );
    pstMemory = kGetDynamicMemoryManager();
    
    for( i = 0 ; i < pstMemory->iMaxLevelCount ; i++ )
    {
        kPrintf( "Block List [%d] Test Start\n", i );
        kPrintf( "Allocation And Compare: ");
        
        // ¸ğµç ºí·ÏÀ» ÇÒ´ç ¹Ş¾Æ¼­ °ªÀ» Ã¤¿î ÈÄ °Ë»ç
        for( j = 0 ; j < ( pstMemory->iBlockCountOfSmallestBlock >> i ) ; j++ )
        {
            pqwBuffer = kAllocateMemory( DYNAMICMEMORY_MIN_SIZE << i );
            if( pqwBuffer == NULL )
            {
                kPrintf( "\nAllocation Fail\n" );
                return ;
            }

            // °ªÀ» Ã¤¿î ÈÄ ´Ù½Ã °Ë»ç
            for( k = 0 ; k < ( DYNAMICMEMORY_MIN_SIZE << i ) / 8 ; k++ )
            {
                pqwBuffer[ k ] = k;
            }
            
            for( k = 0 ; k < ( DYNAMICMEMORY_MIN_SIZE << i ) / 8 ; k++ )
            {
                if( pqwBuffer[ k ] != k )
                {
                    kPrintf( "\nCompare Fail\n" );
                    return ;
                }
            }
            // ÁøÇà °úÁ¤À» . À¸·Î Ç¥½Ã
            kPrintf( "." );
        }
        
        kPrintf( "\nFree: ");
        // ÇÒ´ç ¹ŞÀº ºí·ÏÀ» ¸ğµÎ ¹İÈ¯
        for( j = 0 ; j < ( pstMemory->iBlockCountOfSmallestBlock >> i ) ; j++ )
        {
            if( kFreeMemory( ( void * ) ( pstMemory->qwStartAddress + 
                         ( DYNAMICMEMORY_MIN_SIZE << i ) * j ) ) == FALSE )
            {
                kPrintf( "\nFree Fail\n" );
                return ;
            }
            // ÁøÇà °úÁ¤À» . À¸·Î Ç¥½Ã
            kPrintf( "." );
        }
        kPrintf( "\n" );
    }
    kPrintf( "Test Complete~!!!\n" );
}

static void kTestFreeList( const char* pcParameterBuffer )
{
    DYNAMICMEMORY* pstMemory;
    long i, j, k;
    QWORD* bigMemoryChunk;
    QWORD* memoryChunk;
    
    
    pstMemory = kGetDynamicMemoryManager();

    bigMemoryChunk = kAllocateMemory( DYNAMICMEMORY_MIN_SIZE * 4);

    kInitializeFreeList(memoryChunk,DYNAMICMEMORY_MIN_SIZE * 4);

    kShowCurrentFreeList();

    kPrintf( "============ Dynamic Memory Test ============\n" );
    
    memoryChunk = kAllocateChunk(8*8);
    
    for( k = 0 ; k < 8 ; k++ )
    {
        memoryChunk[ k ] = k;
    }
            
    for( k = 0 ; k < 8 ; k++ )
    {
        if( memoryChunk[ k ] != k )
        {
            kPrintf( "\nCompare Fail\n" );
            return ;
        }
    }

    kShowCurrentFreeList();

    kPrintf( "============ Free Memory Test ============\n" );

    kAddFreeChunk(memoryChunk);

    kShowCurrentFreeList();
    

    // int wow;
    //  for(wow=0 ; wow<3 ; wow++){

        
    //     srand(kGetTickCount());
    //     QWORD testLen = rand()%10+1;
    //     //kPrintf("===== %d case======%d\n",i,testLen);

    // memoryChunk = kAllocateChunk(testLen*8);
    
    // for( k = 0 ; k < testLen ; k++ )
    // {
    //     memoryChunk[ k ] = k;
    // }
            
    // for( k = 0 ; k < testLen ; k++ )
    // {
    //     if( memoryChunk[ k ] != k )
    //     {
    //         kPrintf( "\nCompare Fail\n" );
    //         return ;
    //     }
    // }

    // kShowCurrentFreeList();

    // kPrintf( "============ Free Memory Test ============\n" );

    // kAddFreeChunk(memoryChunk);

    // kShowCurrentFreeList();
    // }



    //kPrintf( "Test Complete~!!!\n" );
}

/**
 *  ÀÓÀÇ·Î ¸Ş¸ğ¸®¸¦ ÇÒ´çÇÏ°í ÇØÁ¦ÇÏ´Â °ÍÀ» ¹İº¹ÇÏ´Â ÅÂ½ºÅ©
 */
static void kRandomAllocationTask( void )
{
    TCB* pstTask;
    QWORD qwMemorySize;
    char vcBuffer[ 200 ];
    BYTE* pbAllocationBuffer;
    int i, j;
    int iY;
    
    pstTask = kGetRunningTask();
    iY = ( pstTask->stLink.qwID ) % 15 + 9;

    srand(kGetTickCount());

    for( j = 0 ; j < 10 ; j++ )
    {
        // 1KB ~ 32M±îÁö ÇÒ´çÇÏµµ·Ï ÇÔ
        do
        {
            qwMemorySize = ( ( rand() % ( 32 * 1024 ) ) + 1 ) * 1024;
            pbAllocationBuffer = kAllocateMemory( qwMemorySize );

            // ¸¸ÀÏ ¹öÆÛ¸¦ ÇÒ´ç ¹ŞÁö ¸øÇÏ¸é ´Ù¸¥ ÅÂ½ºÅ©°¡ ¸Ş¸ğ¸®¸¦ »ç¿ëÇÏ°í 
            // ÀÖÀ» ¼ö ÀÖÀ¸¹Ç·Î Àá½Ã ´ë±âÇÑ ÈÄ ´Ù½Ã ½Ãµµ
            if( pbAllocationBuffer == 0 )
            {
                kSleep( 1 );
            }
        } while( pbAllocationBuffer == 0 );
            
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Allocation Success", 
                  pbAllocationBuffer, qwMemorySize );
        // ÀÚ½ÅÀÇ ID¸¦ Y ÁÂÇ¥·Î ÇÏ¿© µ¥ÀÌÅÍ¸¦ Ãâ·Â
        kPrintStringXY( 20, iY, vcBuffer );
        kSleep( 200 );
        
        // ¹öÆÛ¸¦ ¹İÀ¸·Î ³ª´²¼­ ·£´ıÇÑ µ¥ÀÌÅÍ¸¦ ¶È°°ÀÌ Ã¤¿ò 
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Write...     ", 
                  pbAllocationBuffer, qwMemorySize );
        kPrintStringXY( 20, iY, vcBuffer );
        for( i = 0 ; i < qwMemorySize / 2 ; i++ )
        {
            pbAllocationBuffer[ i ] = rand() & 0xFF;
            pbAllocationBuffer[ i + ( qwMemorySize / 2 ) ] = pbAllocationBuffer[ i ];
        }
        kSleep( 200 );
        
        // Ã¤¿î µ¥ÀÌÅÍ°¡ Á¤»óÀûÀÎÁö ´Ù½Ã È®ÀÎ
        kSPrintf( vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Verify...   ", 
                  pbAllocationBuffer, qwMemorySize );
        kPrintStringXY( 20, iY, vcBuffer );
        for( i = 0 ; i < qwMemorySize / 2 ; i++ )
        {
            if( pbAllocationBuffer[ i ] != pbAllocationBuffer[ i + ( qwMemorySize / 2 ) ] )
            {
                kPrintf( "Task ID[0x%Q] Verify Fail\n", pstTask->stLink.qwID );
                kExitTask();
            }
        }
        kFreeMemory( pbAllocationBuffer );
        kSleep( 200 );
    }
    
    kExitTask();
}

/**
 *  ÅÂ½ºÅ©¸¦ ¿©·¯ °³ »ı¼ºÇÏ¿© ÀÓÀÇÀÇ ¸Ş¸ğ¸®¸¦ ÇÒ´çÇÏ°í ÇØÁ¦ÇÏ´Â °ÍÀ» ¹İº¹ÇÏ´Â Å×½ºÆ®
 */
static void kTestRandomAllocation( const char* pcParameterBuffer )
{
    int i;
    
    for( i = 0 ; i < 1000 ; i++ )
    {
        kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kRandomAllocationTask );
    }
}


/**
 *  ÇÏµå µğ½ºÅ©ÀÇ Á¤º¸¸¦ Ç¥½Ã
 */
static void kShowHDDInformation( const char* pcParameterBuffer )
{
    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // ÇÏµå µğ½ºÅ©ÀÇ Á¤º¸¸¦ ÀĞÀ½
    if( kGetHDDInformation( &stHDD ) == FALSE )
    {
        kPrintf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    kPrintf( "============ Primary Master HDD Information ============\n" );
    
    // ¸ğµ¨ ¹øÈ£ Ãâ·Â
    kMemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    kPrintf( "Model Number:\t %s\n", vcBuffer );
    
    // ½Ã¸®¾ó ¹øÈ£ Ãâ·Â
    kMemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    kPrintf( "Serial Number:\t %s\n", vcBuffer );

    // Çìµå, ½Ç¸°´õ, ½Ç¸°´õ ´ç ¼½ÅÍ ¼ö¸¦ Ãâ·Â
    kPrintf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    kPrintf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    kPrintf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // ÃÑ ¼½ÅÍ ¼ö Ãâ·Â
    kPrintf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );
}

/**
 *  ÇÏµå µğ½ºÅ©¿¡ ÆÄ¶ó¹ÌÅÍ·Î ³Ñ¾î¿Â LBA ¾îµå·¹½º¿¡¼­ ¼½ÅÍ ¼ö ¸¸Å­ ÀĞÀ½
 */
static void kReadSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BYTE bData;
    BOOL bExit = FALSE;
    
    // ÆÄ¶ó¹ÌÅÍ ¸®½ºÆ®¸¦ ÃÊ±âÈ­ÇÏ¿© LBA ¾îµå·¹½º¿Í ¼½ÅÍ ¼ö ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    if( ( kGetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( kGetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        kPrintf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = kAToI( vcLBA, 10 );
    iSectorCount = kAToI( vcSectorCount, 10 );
    
    // ¼½ÅÍ ¼ö¸¸Å­ ¸Ş¸ğ¸®¸¦ ÇÒ´ç ¹Ş¾Æ ÀĞ±â ¼öÇà
    pcBuffer = kAllocateMemory( iSectorCount * 512 );
    if( kReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        kPrintf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // µ¥ÀÌÅÍ ¹öÆÛÀÇ ³»¿ëÀ» Ãâ·Â
        for( j = 0 ; j < iSectorCount ; j++ )
        {
            for( i = 0 ; i < 512 ; i++ )
            {
                if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
                {
                    kPrintf( "\nPress any key to continue... ('q' is exit) : " );
                    if( kGetCh() == 'q' )
                    {
                        bExit = TRUE;
                        break;
                    }
                }                

                if( ( i % 16 ) == 0 )
                {
                    kPrintf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i ); 
                }

                // ¸ğµÎ µÎ ÀÚ¸®·Î Ç¥½ÃÇÏ·Á°í 16º¸´Ù ÀÛÀº °æ¿ì 0À» Ãß°¡ÇØÁÜ
                bData = pcBuffer[ j * 512 + i ] & 0xFF;
                if( bData < 16 )
                {
                    kPrintf( "0" );
                }
                kPrintf( "%X ", bData );
            }
            
            if( bExit == TRUE )
            {
                break;
            }
        }
        kPrintf( "\n" );
    }
    else
    {
        kPrintf( "Read Fail\n" );
    }
    
    kFreeMemory( pcBuffer );
}

/**
 *  ÇÏµå µğ½ºÅ©¿¡ ÆÄ¶ó¹ÌÅÍ·Î ³Ñ¾î¿Â LBA ¾îµå·¹½º¿¡¼­ ¼½ÅÍ ¼ö ¸¸Å­ ¾¸
 */
static void kWriteSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BOOL bExit = FALSE;
    BYTE bData;
    static DWORD s_dwWriteCount = 0;

    // ÆÄ¶ó¹ÌÅÍ ¸®½ºÆ®¸¦ ÃÊ±âÈ­ÇÏ¿© LBA ¾îµå·¹½º¿Í ¼½ÅÍ ¼ö ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    if( ( kGetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( kGetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        kPrintf( "ex) writesector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = kAToI( vcLBA, 10 );
    iSectorCount = kAToI( vcSectorCount, 10 );

    s_dwWriteCount++;
    
    // ¹öÆÛ¸¦ ÇÒ´ç ¹Ş¾Æ µ¥ÀÌÅÍ¸¦ Ã¤¿ò. 
    // ÆĞÅÏÀº 4 ¹ÙÀÌÆ®ÀÇ LBA ¾îµå·¹½º¿Í 4 ¹ÙÀÌÆ®ÀÇ ¾²±â°¡ ¼öÇàµÈ È½¼ö·Î »ı¼º
    pcBuffer = kAllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // ¾²±â ¼öÇà
    if( kWriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        kPrintf( "Write Fail\n" );
        return ;
    }
    kPrintf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // µ¥ÀÌÅÍ ¹öÆÛÀÇ ³»¿ëÀ» Ãâ·Â
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i++ )
        {
            if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
            {
                kPrintf( "\nPress any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    bExit = TRUE;
                    break;
                }
            }                

            if( ( i % 16 ) == 0 )
            {
                kPrintf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i ); 
            }

            // ¸ğµÎ µÎ ÀÚ¸®·Î Ç¥½ÃÇÏ·Á°í 16º¸´Ù ÀÛÀº °æ¿ì 0À» Ãß°¡ÇØÁÜ
            bData = pcBuffer[ j * 512 + i ] & 0xFF;
            if( bData < 16 )
            {
                kPrintf( "0" );
            }
            kPrintf( "%X ", bData );
        }
        
        if( bExit == TRUE )
        {
            break;
        }
    }
    kPrintf( "\n" );    
    kFreeMemory( pcBuffer );    
}

/**
 *  ÇÏµå µğ½ºÅ©¸¦ ¿¬°á
 */
static void kMountHDD( const char* pcParameterBuffer )
{
    if( kMount() == FALSE )
    {
        kPrintf( "HDD Mount Fail\n" );
        return ;
    }

    kMemCpy( currentUserID, "root", 5 );

    // ÆÄÀÏ »ı¼º
    FILE* fp = fopen( ".userinfo", "w", currentUserID);
    if( fp == NULL )
    {
        kPrintf( "User Info Reset Fail\n" );
        return ;
    }
      
    if( fwrite( "root/", 1, 5, fp ) != 5 )
    {
        kPrintf( "File Wirte Fail\n" );
        fclose( fp );
        return ;
    }

    fclose( fp );
    kFlushFileSystemCache();
    kPrintf( "HDD Mount Success\n" );
}

/**
 *  ÇÏµå µğ½ºÅ©¿¡ ÆÄÀÏ ½Ã½ºÅÛÀ» »ı¼º(Æ÷¸Ë)
 */
static void kFormatHDD( const char* pcParameterBuffer )
{
    if( kFormat() == FALSE )
    {
        kPrintf( "HDD Format Fail\n" );
        return ;
    }
    kPrintf( "HDD Format Success\n" );
}

/**
 *  ÆÄÀÏ ½Ã½ºÅÛ Á¤º¸¸¦ Ç¥½Ã
 */
static void kShowFileSystemInformation( const char* pcParameterBuffer )
{
    FILESYSTEMMANAGER stManager;
    
    kGetFileSystemInformation( &stManager );
    
    kPrintf( "================== File System Information ==================\n" );
    kPrintf( "Mouted:\t\t\t\t\t %d\n", stManager.bMounted );
    kPrintf( "Reserved Sector Count:\t\t\t %d Sector\n", stManager.dwReservedSectorCount );
    kPrintf( "Cluster Link Table Start Address:\t %d Sector\n", 
            stManager.dwClusterLinkAreaStartAddress );
    kPrintf( "Cluster Link Table Size:\t\t %d Sector\n", stManager.dwClusterLinkAreaSize );
    kPrintf( "Data Area Start Address:\t\t %d Sector\n", stManager.dwDataAreaStartAddress );
    kPrintf( "Total Cluster Count:\t\t\t %d Cluster\n", stManager.dwTotalClusterCount );
}

/**
 *  ·çÆ® µğ·ºÅÍ¸®¿¡ ºó ÆÄÀÏÀ» »ı¼º
 */
static void kCreateFileInRootDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    DWORD dwCluster;
    int i;
    FILE* pstFile;
    
    // ÆÄ¶ó¹ÌÅÍ ¸®½ºÆ®¸¦ ÃÊ±âÈ­ÇÏ¿© ÆÄÀÏ ÀÌ¸§À» ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    pstFile = fopen( vcFileName, "w", currentUserID);
    if( pstFile == NULL )
    {
        kPrintf( "File Create Fail\n" );
        return;
    }
    fclose( pstFile );
    kPrintf( "File Create Success\n" );
}

/**
 *  ·çÆ® µğ·ºÅÍ¸®¿¡¼­ ÆÄÀÏÀ» »èÁ¦
 */
static void kDeleteFileInRootDirectory( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    const char pcFileName[50];
    int iLength;
    int iDirectoryoffset;
    DIRECTORYENTRY stEntry;
    int icheckUser;
    
    // ÆÄ¶ó¹ÌÅÍ ¸®½ºÆ®¸¦ ÃÊ±âÈ­ÇÏ¿© ÆÄÀÏ ÀÌ¸§À» ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    iDirectoryoffset = kFindDirectoryEntry(pcFileName, &stEntry);
    iLength = kStrLen(currentUserID);
    icheckUser =kMemCmp(stEntry.ownUserID,currentUserID,iLength);

    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    if((icheckUser  && !(stEntry.bPermission &= 0x02)) || 
    (!(stEntry.bPermission &= 0x10) && !icheckUser)){
        kPrintf("Permission denied\n");
    }

    else{
        if( remove( vcFileName ) != 0 )
        {
            kPrintf( "File Not Found or File Opened\n" );
            return ;
        }
    
        kPrintf( "File Delete Success\n" );
    }
}

/**
 *  ·çÆ® µğ·ºÅÍ¸®ÀÇ ÆÄÀÏ ¸ñ·ÏÀ» Ç¥½Ã
 */
static void kShowRootDirectory( const char* pcParameterBuffer )
{
    DIR* pstDirectory;
    int i, iCount, iTotalCount;
    struct dirent* pstEntry;
    char vcBuffer[ 400 ];
    char vcTempValue[ 50 ];
    DWORD dwTotalByte;
    DWORD dwUsedClusterCount;
    FILESYSTEMMANAGER stManager;
    
    // ÆÄÀÏ ½Ã½ºÅÛ Á¤º¸¸¦ ¾òÀ½
    kGetFileSystemInformation( &stManager );
     
    // ·çÆ® µğ·ºÅÍ¸®¸¦ ¿®
    pstDirectory = opendir( "/" );
    if( pstDirectory == NULL )
    {
        kPrintf( "Root Directory Open Fail\n" );
        return ;
    }
    
    // ¸ÕÀú ·çÇÁ¸¦ µ¹¸é¼­ µğ·ºÅÍ¸®¿¡ ÀÖ´Â ÆÄÀÏÀÇ °³¼ö¿Í ÀüÃ¼ ÆÄÀÏÀÌ »ç¿ëÇÑ Å©±â¸¦ °è»ê
    iTotalCount = 0;
    dwTotalByte = 0;
    dwUsedClusterCount = 0;
    while( 1 )
    {
        // µğ·ºÅÍ¸®¿¡¼­ ¿£Æ®¸® ÇÏ³ª¸¦ ÀĞÀ½
        pstEntry = readdir( pstDirectory );
        // ´õÀÌ»ó ÆÄÀÏÀÌ ¾øÀ¸¸é ³ª°¨
        if( pstEntry == NULL )
        {
            break;
        }
        iTotalCount++;
        dwTotalByte += pstEntry->dwFileSize;

        // ½ÇÁ¦·Î »ç¿ëµÈ Å¬·¯½ºÅÍÀÇ °³¼ö¸¦ °è»ê
        if( pstEntry->dwFileSize == 0 )
        {
            // Å©±â°¡ 0ÀÌ¶óµµ Å¬·¯½ºÅÍ 1°³´Â ÇÒ´çµÇ¾î ÀÖÀ½
            dwUsedClusterCount++;
        }
        else
        {
            // Å¬·¯½ºÅÍ °³¼ö¸¦ ¿Ã¸²ÇÏ¿© ´õÇÔ
            dwUsedClusterCount += ( pstEntry->dwFileSize + 
                ( FILESYSTEM_CLUSTERSIZE - 1 ) ) / FILESYSTEM_CLUSTERSIZE;
        }
    }
    
    // ½ÇÁ¦ ÆÄÀÏÀÇ ³»¿ëÀ» Ç¥½ÃÇÏ´Â ·çÇÁ
    rewinddir( pstDirectory );
    iCount = 0;
    while( 1 )
    {
        // µğ·ºÅÍ¸®¿¡¼­ ¿£Æ®¸® ÇÏ³ª¸¦ ÀĞÀ½
        pstEntry = readdir( pstDirectory );
        // ´õÀÌ»ó ÆÄÀÏÀÌ ¾øÀ¸¸é ³ª°¨
        if( pstEntry == NULL )
        {
            break;
        }
        
        // ÀüºÎ °ø¹éÀ¸·Î ÃÊ±âÈ­ ÇÑ ÈÄ °¢ À§Ä¡¿¡ °ªÀ» ´ëÀÔ
        kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
        vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';

        //////////////////////////////////////////////////////////////////////////////////////////////
        if ( pstEntry->bPermission & 0x20 )
            kSPrintf( vcTempValue, "r" );
        else
            kSPrintf( vcTempValue, "-" );

        if ( pstEntry->bPermission & 0x10 )
            kSPrintf( vcTempValue+1, "w" );
        else
            kSPrintf( vcTempValue+1, "-" );

        if ( pstEntry->bPermission & 0x08 )
            kSPrintf( vcTempValue+2, "x" );
        else
            kSPrintf( vcTempValue+2, "-" );

        if ( pstEntry->bPermission & 0x04 )
            kSPrintf( vcTempValue+3, "r" );
        else
            kSPrintf( vcTempValue+3, "-" );

        if ( pstEntry->bPermission & 0x02 )
            kSPrintf( vcTempValue+4, "w" );
        else
            kSPrintf( vcTempValue+4, "-" );

        if ( pstEntry->bPermission & 0x01 )
            kSPrintf( vcTempValue+5, "x" );
        else
            kSPrintf( vcTempValue+5, "-" );

        kMemCpy(vcBuffer, vcTempValue, kStrLen( vcTempValue ) );

        kMemCpy(vcBuffer+10, pstEntry->ownUserID, kStrLen(pstEntry->ownUserID)+1 );
        kPrintf( "%s\n", vcBuffer );

        kMemSet( vcBuffer, ' ', sizeof( vcBuffer ) - 1 );
        vcBuffer[ sizeof( vcBuffer ) - 1 ] = '\0';

        /////////////////////////////////////////////////////////////////////////////////////////////
        
        // ÆÄÀÏ ÀÌ¸§ »ğÀÔ
        kMemCpy( vcBuffer, pstEntry->d_name, 
                 kStrLen( pstEntry->d_name ) );

        // ÆÄÀÏ ±æÀÌ »ğÀÔ
        kSPrintf( vcTempValue, "%d Byte", pstEntry->dwFileSize );
        kMemCpy( vcBuffer + 30, vcTempValue, kStrLen( vcTempValue ) );

        // ÆÄÀÏÀÇ ½ÃÀÛ Å¬·¯½ºÅÍ »ğÀÔ
        kSPrintf( vcTempValue, "0x%X Cluster", pstEntry->dwStartClusterIndex );
        kMemCpy( vcBuffer + 55, vcTempValue, kStrLen( vcTempValue ) + 1 );
        kPrintf( "   %s\n", vcBuffer );

        if( ( iCount != 0 ) && ( ( iCount % 20 ) == 0 ) )
        {
            kPrintf( "Press any key to continue... ('q' is exit) : " );
            if( kGetCh() == 'q' )
            {
                kPrintf( "\n" );
                break;
            }        
        }
        iCount++;
    }
    
    // ÃÑ ÆÄÀÏÀÇ °³¼ö¿Í ÆÄÀÏÀÇ ÃÑ Å©±â¸¦ Ãâ·Â
    kPrintf( "\t\tTotal File Count: %d\n", iTotalCount );
    kPrintf( "\t\tTotal File Size: %d KByte (%d Cluster)\n", dwTotalByte, 
             dwUsedClusterCount );
    
    // ³²Àº Å¬·¯½ºÅÍ ¼ö¸¦ ÀÌ¿ëÇØ¼­ ¿©À¯ °ø°£À» Ãâ·Â
    kPrintf( "\t\tFree Space: %d KByte (%d Cluster)\n", 
             ( stManager.dwTotalClusterCount - dwUsedClusterCount ) * 
             FILESYSTEM_CLUSTERSIZE / 1024, stManager.dwTotalClusterCount - 
             dwUsedClusterCount );
    
    // µğ·ºÅÍ¸®¸¦ ´İÀ½
    closedir( pstDirectory );
}

/**
 *  ÆÄÀÏÀ» »ı¼ºÇÏ¿© Å°º¸µå·Î ÀÔ·ÂµÈ µ¥ÀÌÅÍ¸¦ ¾¸
 */
static void kWriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    char userName[ 16 ];
    const char pcFileName[50];
    int iLength;
    FILE* fp;
    int iEnterCount;
    BYTE bKey;
    int iDirectoryoffset;
    DIRECTORYENTRY stEntry;
    int icheckUser;
    int icheckroot;
    BOOL bcheckPermission = TRUE;

    // ÆÄ¶ó¹ÌÅÍ ¸®½ºÆ®¸¦ ÃÊ±âÈ­ÇÏ¿© ÆÄÀÏ ÀÌ¸§À» ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    kMemCpy(pcFileName,vcFileName, iLength + 1);

    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    kMemCpy(userName, currentUserID, kStrLen( currentUserID ));
    userName[ kStrLen(currentUserID) ] = '\0';
    iDirectoryoffset = kFindDirectoryEntry(pcFileName, &stEntry);
    iLength = kStrLen(currentUserID);
    icheckUser =kMemCmp(stEntry.ownUserID,currentUserID,iLength);
    icheckroot = kMemCmp(currentUserID,"root",4);

    // ÆÄÀÏ »ı¼º
    fp = fopen( vcFileName, "w", userName);
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }

    // not root user 
    if(icheckroot != 0 && iDirectoryoffset != -1){
        //file owner not correct
        if(icheckUser != 0 && !(stEntry.bPermission &= 0x02)){
            kPrintf("Permission denied\n");
            bcheckPermission = FALSE;
        }
        else if(!icheckUser && !(stEntry.bPermission &= 0x10) ){
            kPrintf("Permission denied\n");
            bcheckPermission = FALSE;
        }
    }
    
    if(bcheckPermission){
    // ¿£ÅÍ Å°°¡ ¿¬¼ÓÀ¸·Î 3¹ø ´­·¯Áú ¶§±îÁö ³»¿ëÀ» ÆÄÀÏ¿¡ ¾¸
    iEnterCount = 0;
    while( 1 )
    {
        bKey = kGetCh();
        // ¿£ÅÍ Å°ÀÌ¸é ¿¬¼Ó 3¹ø ´­·¯Á³´Â°¡ È®ÀÎÇÏ¿© ·çÇÁ¸¦ ºüÁ® ³ª°¨
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // ¿£ÅÍ Å°°¡ ¾Æ´Ï¶ó¸é ¿£ÅÍ Å° ÀÔ·Â È½¼ö¸¦ ÃÊ±âÈ­
        else
        {
            iEnterCount = 0;
        }
        
        kPrintf( "%c", bKey );
        if( fwrite( &bKey, 1, 1, fp ) != 1 )
        {
            kPrintf( "File Wirte Fail\n" );
            break;
        }
    }
    
    kPrintf( "File Create Success\n" );
    fclose( fp );
    }

     if( kFlushFileSystemCache() == TRUE )
    {
        kPrintf( "Pass\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
}

/**
 *  ÆÄÀÏÀ» ¿­¾î¼­ µ¥ÀÌÅÍ¸¦ ÀĞÀ½
 */
static void kReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    char userName [ 16 ];
    const char pcFileName[50];
    int iLength;
    FILE* fp;
    int iEnterCount;
    BYTE bKey;
    int iDirectoryoffset;
    DIRECTORYENTRY stEntry;
    int icheckUser;
    int icheckroot;
    BOOL bcheckPermission = TRUE;
    
    // ÆÄ¶ó¹ÌÅÍ ¸®½ºÆ®¸¦ ÃÊ±âÈ­ÇÏ¿© ÆÄÀÏ ÀÌ¸§À» ÃßÃâ
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    kMemCpy(pcFileName,vcFileName, iLength + 1);

    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        kPrintf( "Too Long or Too Short File Name\n" );
        return ;
    }

    iDirectoryoffset = kFindDirectoryEntry(pcFileName, &stEntry);
    iLength = kStrLen(currentUserID);
    icheckUser =kMemCmp(stEntry.ownUserID,currentUserID,iLength);
    icheckroot = kMemCmp(currentUserID,"root",4);
    
    fp = fopen( vcFileName, "r", currentUserID);
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }
   
    if(icheckroot != 0 ){
        //file owner not correct
        if(icheckUser != 0 && !(stEntry.bPermission &= 0x04)){
            kPrintf("Permission denied\n");
            bcheckPermission = FALSE;
        }
        else if(!icheckUser && !(stEntry.bPermission &= 0x20) ){
            kPrintf("Permission denied\n");
            bcheckPermission = FALSE;
        }
    }
    // ÆÄÀÏÀÇ ³¡±îÁö Ãâ·ÂÇÏ´Â °ÍÀ» ¹İº¹
    if(bcheckPermission){
        iEnterCount = 0;
        while( 1 )
    {
        if( fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        kPrintf( "%c", bKey );
        
        // ¸¸¾à ¿£ÅÍ Å°ÀÌ¸é ¿£ÅÍ Å° È½¼ö¸¦ Áõ°¡½ÃÅ°°í 20¶óÀÎ±îÁö Ãâ·ÂÇß´Ù¸é 
        // ´õ Ãâ·ÂÇÒÁö ¿©ºÎ¸¦ ¹°¾îº½
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            
            if( ( iEnterCount != 0 ) && ( ( iEnterCount % 20 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
                iEnterCount = 0;
            }
        }
    }
    fclose( fp );
    }
}

/**
 *  ÆÄÀÏ I/O¿¡ °ü·ÃµÈ ±â´ÉÀ» Å×½ºÆ®
 */
static void kTestFileIO( const char* pcParameterBuffer )
{
    FILE* pstFile;
    BYTE* pbBuffer;
    int i;
    int j;
    DWORD dwRandomOffset;
    DWORD dwByteCount;
    BYTE vbTempBuffer[ 1024 ];
    DWORD dwMaxFileSize;
    
    kPrintf( "================== File I/O Function Test ==================\n" );
    
    // 4MbyteÀÇ ¹öÆÛ ÇÒ´ç
    dwMaxFileSize = 4 * 1024 * 1024;
    pbBuffer = kAllocateMemory( dwMaxFileSize );
    if( pbBuffer == NULL )
    {
        kPrintf( "Memory Allocation Fail\n" );
        return ;
    }
    // Å×½ºÆ®¿ë ÆÄÀÏÀ» »èÁ¦
    remove( "testfileio.bin" );

    //==========================================================================
    // ÆÄÀÏ ¿­±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "1. File Open Fail Test..." );
    // r ¿É¼ÇÀº ÆÄÀÏÀ» »ı¼ºÇÏÁö ¾ÊÀ¸¹Ç·Î, Å×½ºÆ® ÆÄÀÏÀÌ ¾ø´Â °æ¿ì NULLÀÌ µÇ¾î¾ß ÇÔ
    pstFile = fopen( "testfileio.bin", "r", currentUserID);
    if( pstFile == NULL )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
        fclose( pstFile );
    }
    
    //==========================================================================
    // ÆÄÀÏ »ı¼º Å×½ºÆ®
    //==========================================================================
    kPrintf( "2. File Create Test..." );
    // w ¿É¼ÇÀº ÆÄÀÏÀ» »ı¼ºÇÏ¹Ç·Î, Á¤»óÀûÀ¸·Î ÇÚµéÀÌ ¹İÈ¯µÇ¾î¾ßÇÔ
    pstFile = fopen( "testfileio.bin", "w", currentUserID);
    if( pstFile != NULL )
    {
        kPrintf( "[Pass]\n" );
        kPrintf( "    File Handle [0x%Q]\n", pstFile );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }
    
    //==========================================================================
    // ¼øÂ÷ÀûÀÎ ¿µ¿ª ¾²±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "3. Sequential Write Test(Cluster Size)..." );
    // ¿­¸° ÇÚµéÀ» °¡Áö°í ¾²±â ¼öÇà
    for( i = 0 ; i < 100 ; i++ )
    {
        kMemSet( pbBuffer, i, FILESYSTEM_CLUSTERSIZE );
        if( fwrite( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    %d Cluster Error\n", i );
            break;
        }
    }
    if( i >= 100 )
    {
        kPrintf( "[Pass]\n" );
    }
    
    //==========================================================================
    // ¼øÂ÷ÀûÀÎ ¿µ¿ª ÀĞ±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "4. Sequential Read And Verify Test(Cluster Size)..." );
    // ÆÄÀÏÀÇ Ã³À½À¸·Î ÀÌµ¿
    fseek( pstFile, -100 * FILESYSTEM_CLUSTERSIZE, SEEK_END );
    
    // ¿­¸° ÇÚµéÀ» °¡Áö°í ÀĞ±â ¼öÇà ÈÄ, µ¥ÀÌÅÍ °ËÁõ
    for( i = 0 ; i < 100 ; i++ )
    {
        // ÆÄÀÏÀ» ÀĞÀ½
        if( fread( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) !=
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        
        // µ¥ÀÌÅÍ °Ë»ç
        for( j = 0 ; j < FILESYSTEM_CLUSTERSIZE ; j++ )
        {
            if( pbBuffer[ j ] != ( BYTE ) i )
            {
                kPrintf( "[Fail]\n" );
                kPrintf( "    %d Cluster Error. [%X] != [%X]\n", i, pbBuffer[ j ], 
                         ( BYTE ) i );
                break;
            }
        }
    }
    if( i >= 100 )
    {
        kPrintf( "[Pass]\n" );
    }

    //==========================================================================
    // ÀÓÀÇÀÇ ¿µ¿ª ¾²±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "5. Random Write Test...\n" );
    
    // ¹öÆÛ¸¦ ¸ğµÎ 0À¸·Î Ã¤¿ò
    kMemSet( pbBuffer, 0, dwMaxFileSize );
    // ¿©±â Àú±â¿¡ ¿Å°Ü´Ù´Ï¸é¼­ µ¥ÀÌÅÍ¸¦ ¾²°í °ËÁõ
    // ÆÄÀÏÀÇ ³»¿ëÀ» ÀĞ¾î¼­ ¹öÆÛ·Î º¹»ç
    fseek( pstFile, -100 * FILESYSTEM_CLUSTERSIZE, SEEK_CUR );
    fread( pbBuffer, 1, dwMaxFileSize, pstFile );
    srand(kGetTickCount());
    
    // ÀÓÀÇÀÇ À§Ä¡·Î ¿Å±â¸é¼­ µ¥ÀÌÅÍ¸¦ ÆÄÀÏ°ú ¹öÆÛ¿¡ µ¿½Ã¿¡ ¾¸
    for( i = 0 ; i < 100 ; i++ )
    {
        dwByteCount = ( rand() % ( sizeof( vbTempBuffer ) - 1 ) ) + 1;
        dwRandomOffset = rand() % ( dwMaxFileSize - dwByteCount );
        
        kPrintf( "    [%d] Offset [%d] Byte [%d]...", i, dwRandomOffset, 
                dwByteCount );

        // ÆÄÀÏ Æ÷ÀÎÅÍ¸¦ ÀÌµ¿
        fseek( pstFile, dwRandomOffset, SEEK_SET );
        kMemSet( vbTempBuffer, i, dwByteCount );
              
        // µ¥ÀÌÅÍ¸¦ ¾¸
        if( fwrite( vbTempBuffer, 1, dwByteCount, pstFile ) != dwByteCount )
        {
            kPrintf( "[Fail]\n" );
            break;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
        
        kMemSet( pbBuffer + dwRandomOffset, i, dwByteCount );
    }
    
    // ¸Ç ¸¶Áö¸·À¸·Î ÀÌµ¿ÇÏ¿© 1¹ÙÀÌÆ®¸¦ ½á¼­ ÆÄÀÏÀÇ Å©±â¸¦ 4Mbyte·Î ¸¸µê
    fseek( pstFile, dwMaxFileSize - 1, SEEK_SET );
    fwrite( &i, 1, 1, pstFile );
    pbBuffer[ dwMaxFileSize - 1 ] = ( BYTE ) i;

    //==========================================================================
    // ÀÓÀÇÀÇ ¿µ¿ª ÀĞ±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "6. Random Read And Verify Test...\n" );
    // ÀÓÀÇÀÇ À§Ä¡·Î ¿Å±â¸é¼­ ÆÄÀÏ¿¡¼­ µ¥ÀÌÅÍ¸¦ ÀĞ¾î ¹öÆÛÀÇ ³»¿ë°ú ºñ±³
    for( i = 0 ; i < 100 ; i++ )
    {
        dwByteCount = ( rand() % ( sizeof( vbTempBuffer ) - 1 ) ) + 1;
        dwRandomOffset = rand() % ( ( dwMaxFileSize ) - dwByteCount );

        kPrintf( "    [%d] Offset [%d] Byte [%d]...", i, dwRandomOffset, 
                dwByteCount );
        
        // ÆÄÀÏ Æ÷ÀÎÅÍ¸¦ ÀÌµ¿
        fseek( pstFile, dwRandomOffset, SEEK_SET );
        
        // µ¥ÀÌÅÍ ÀĞÀ½
        if( fread( vbTempBuffer, 1, dwByteCount, pstFile ) != dwByteCount )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    Read Fail\n", dwRandomOffset ); 
            break;
        }
        
        // ¹öÆÛ¿Í ºñ±³
        if( kMemCmp( pbBuffer + dwRandomOffset, vbTempBuffer, dwByteCount ) 
                != 0 )
        {
            kPrintf( "[Fail]\n" );
            kPrintf( "    Compare Fail\n", dwRandomOffset ); 
            break;
        }
        
        kPrintf( "[Pass]\n" );
    }
    
    //==========================================================================
    // ´Ù½Ã ¼øÂ÷ÀûÀÎ ¿µ¿ª ÀĞ±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "7. Sequential Write, Read And Verify Test(1024 Byte)...\n" );
    // ÆÄÀÏÀÇ Ã³À½À¸·Î ÀÌµ¿
    fseek( pstFile, -dwMaxFileSize, SEEK_CUR );
    
    // ¿­¸° ÇÚµéÀ» °¡Áö°í ¾²±â ¼öÇà. ¾ÕºÎºĞ¿¡¼­ 2Mbyte¸¸ ¾¸
    for( i = 0 ; i < ( 2 * 1024 * 1024 / 1024 ) ; i++ )
    {
        kPrintf( "    [%d] Offset [%d] Byte [%d] Write...", i, i * 1024, 1024 );

        // 1024 ¹ÙÀÌÆ®¾¿ ÆÄÀÏÀ» ¾¸
        if( fwrite( pbBuffer + ( i * 1024 ), 1, 1024, pstFile ) != 1024 )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
    }

    // ÆÄÀÏÀÇ Ã³À½À¸·Î ÀÌµ¿
    fseek( pstFile, -dwMaxFileSize, SEEK_SET );
    
    // ¿­¸° ÇÚµéÀ» °¡Áö°í ÀĞ±â ¼öÇà ÈÄ µ¥ÀÌÅÍ °ËÁõ. Random Write·Î µ¥ÀÌÅÍ°¡ Àß¸ø 
    // ÀúÀåµÉ ¼ö ÀÖÀ¸¹Ç·Î °ËÁõÀº 4Mbyte ÀüÃ¼¸¦ ´ë»óÀ¸·Î ÇÔ
    for( i = 0 ; i < ( dwMaxFileSize / 1024 )  ; i++ )
    {
        // µ¥ÀÌÅÍ °Ë»ç
        kPrintf( "    [%d] Offset [%d] Byte [%d] Read And Verify...", i, 
                i * 1024, 1024 );
        
        // 1024 ¹ÙÀÌÆ®¾¿ ÆÄÀÏÀ» ÀĞÀ½
        if( fread( vbTempBuffer, 1, 1024, pstFile ) != 1024 )
        {
            kPrintf( "[Fail]\n" );
            return ;
        }
        
        if( kMemCmp( pbBuffer + ( i * 1024 ), vbTempBuffer, 1024 ) != 0 )
        {
            kPrintf( "[Fail]\n" );
            break;
        }
        else
        {
            kPrintf( "[Pass]\n" );
        }
    }
        
    //==========================================================================
    // ÆÄÀÏ »èÁ¦ ½ÇÆĞ Å×½ºÆ®
    //==========================================================================
    kPrintf( "8. File Delete Fail Test..." );
    // ÆÄÀÏÀÌ ¿­·ÁÀÖ´Â »óÅÂÀÌ¹Ç·Î ÆÄÀÏÀ» Áö¿ì·Á°í ÇÏ¸é ½ÇÆĞÇØ¾ß ÇÔ
    if( remove( "testfileio.bin" ) != 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }
    
    //==========================================================================
    // ÆÄÀÏ ´İ±â Å×½ºÆ®
    //==========================================================================
    kPrintf( "9. File Close Test..." );
    // ÆÄÀÏÀÌ Á¤»óÀûÀ¸·Î ´İÇô¾ß ÇÔ
    if( fclose( pstFile ) == 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }

    //==========================================================================
    // ÆÄÀÏ »èÁ¦ Å×½ºÆ®
    //==========================================================================
    kPrintf( "10. File Delete Test..." );
    // ÆÄÀÏÀÌ ´İÇûÀ¸¹Ç·Î Á¤»óÀûÀ¸·Î Áö¿öÁ®¾ß ÇÔ 
    if( remove( "testfileio.bin" ) == 0 )
    {
        kPrintf( "[Pass]\n" );
    }
    else
    {
        kPrintf( "[Fail]\n" );
    }
    
    // ¸Ş¸ğ¸®¸¦ ÇØÁ¦
    kFreeMemory( pbBuffer );    
}

/**
 *  ÆÄÀÏÀ» ÀĞ°í ¾²´Â ¼Óµµ¸¦ ÃøÁ¤
 */
static void kTestPerformance( const char* pcParameterBuffer )
{
    FILE* pstFile;
    DWORD dwClusterTestFileSize;
    DWORD dwOneByteTestFileSize;
    QWORD qwLastTickCount;
    DWORD i;
    BYTE* pbBuffer;
    
    // Å¬·¯½ºÅÍ´Â 1Mbyte±îÁö ÆÄÀÏÀ» Å×½ºÆ®
    dwClusterTestFileSize = 1024 * 1024;
    // 1¹ÙÀÌÆ®¾¿ ÀĞ°í ¾²´Â Å×½ºÆ®´Â ½Ã°£ÀÌ ¸¹ÀÌ °É¸®¹Ç·Î 16Kbyte¸¸ Å×½ºÆ®
    dwOneByteTestFileSize = 16 * 1024;
    
    // Å×½ºÆ®¿ë ¹öÆÛ ¸Ş¸ğ¸® ÇÒ´ç
    pbBuffer = kAllocateMemory( dwClusterTestFileSize );
    if( pbBuffer == NULL )
    {
        kPrintf( "Memory Allocate Fail\n" );
        return ;
    }
    
    // ¹öÆÛ¸¦ ÃÊ±âÈ­
    kMemSet( pbBuffer, 0, FILESYSTEM_CLUSTERSIZE );
    
    kPrintf( "================== File I/O Performance Test ==================\n" );

    //==========================================================================
    // Å¬·¯½ºÅÍ ´ÜÀ§·Î ÆÄÀÏÀ» ¼øÂ÷ÀûÀ¸·Î ¾²´Â Å×½ºÆ®
    //==========================================================================
    kPrintf( "1.Sequential Read/Write Test(Cluster Size)\n" );

    // ±âÁ¸ÀÇ Å×½ºÆ® ÆÄÀÏÀ» Á¦°ÅÇÏ°í »õ·Î ¸¸µê
    remove( "performance.txt" );
    pstFile = fopen( "performance.txt", "w", currentUserID);
    if( pstFile == NULL )
    {
        kPrintf( "File Open Fail\n" );
        kFreeMemory( pbBuffer );
        return ;
    }
    
    qwLastTickCount = kGetTickCount();
    // Å¬·¯½ºÅÍ ´ÜÀ§·Î ¾²´Â Å×½ºÆ®
    for( i = 0 ; i < ( dwClusterTestFileSize / FILESYSTEM_CLUSTERSIZE ) ; i++ )
    {
        if( fwrite( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) != 
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "Write Fail\n" );
            // ÆÄÀÏÀ» ´İ°í ¸Ş¸ğ¸®¸¦ ÇØÁ¦ÇÔ
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // ½Ã°£ Ãâ·Â
    kPrintf( "   Sequential Write(Cluster Size): %d ms\n", kGetTickCount() - 
             qwLastTickCount );

    //==========================================================================
    // Å¬·¯½ºÅÍ ´ÜÀ§·Î ÆÄÀÏÀ» ¼øÂ÷ÀûÀ¸·Î ÀĞ´Â Å×½ºÆ®
    //==========================================================================
    // ÆÄÀÏÀÇ Ã³À½À¸·Î ÀÌµ¿
    fseek( pstFile, 0, SEEK_SET );
    
    qwLastTickCount = kGetTickCount();
    // Å¬·¯½ºÅÍ ´ÜÀ§·Î ÀĞ´Â Å×½ºÆ®
    for( i = 0 ; i < ( dwClusterTestFileSize / FILESYSTEM_CLUSTERSIZE ) ; i++ )
    {
        if( fread( pbBuffer, 1, FILESYSTEM_CLUSTERSIZE, pstFile ) != 
            FILESYSTEM_CLUSTERSIZE )
        {
            kPrintf( "Read Fail\n" );
            // ÆÄÀÏÀ» ´İ°í ¸Ş¸ğ¸®¸¦ ÇØÁ¦ÇÔ
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // ½Ã°£ Ãâ·Â
    kPrintf( "   Sequential Read(Cluster Size): %d ms\n", kGetTickCount() - 
             qwLastTickCount );
    
    //==========================================================================
    // 1 ¹ÙÀÌÆ® ´ÜÀ§·Î ÆÄÀÏÀ» ¼øÂ÷ÀûÀ¸·Î ¾²´Â Å×½ºÆ®
    //==========================================================================
    kPrintf( "2.Sequential Read/Write Test(1 Byte)\n" );
    
    // ±âÁ¸ÀÇ Å×½ºÆ® ÆÄÀÏÀ» Á¦°ÅÇÏ°í »õ·Î ¸¸µê
    remove( "performance.txt" );
    pstFile = fopen( "performance.txt", "w", currentUserID);
    if( pstFile == NULL )
    {
        kPrintf( "File Open Fail\n" );
        kFreeMemory( pbBuffer );
        return ;
    }
    
    qwLastTickCount = kGetTickCount();
    // 1 ¹ÙÀÌÆ® ´ÜÀ§·Î ¾²´Â Å×½ºÆ®
    for( i = 0 ; i < dwOneByteTestFileSize ; i++ )
    {
        if( fwrite( pbBuffer, 1, 1, pstFile ) != 1 )
        {
            kPrintf( "Write Fail\n" );
            // ÆÄÀÏÀ» ´İ°í ¸Ş¸ğ¸®¸¦ ÇØÁ¦ÇÔ
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // ½Ã°£ Ãâ·Â
    kPrintf( "   Sequential Write(1 Byte): %d ms\n", kGetTickCount() - 
             qwLastTickCount );

    //==========================================================================
    // 1 ¹ÙÀÌÆ® ´ÜÀ§·Î ÆÄÀÏÀ» ¼øÂ÷ÀûÀ¸·Î ÀĞ´Â Å×½ºÆ®
    //==========================================================================
    // ÆÄÀÏÀÇ Ã³À½À¸·Î ÀÌµ¿
    fseek( pstFile, 0, SEEK_SET );
    
    qwLastTickCount = kGetTickCount();
    // 1 ¹ÙÀÌÆ® ´ÜÀ§·Î ÀĞ´Â Å×½ºÆ®
    for( i = 0 ; i < dwOneByteTestFileSize ; i++ )
    {
        if( fread( pbBuffer, 1, 1, pstFile ) != 1 )
        {
            kPrintf( "Read Fail\n" );
            // ÆÄÀÏÀ» ´İ°í ¸Ş¸ğ¸®¸¦ ÇØÁ¦ÇÔ
            fclose( pstFile );
            kFreeMemory( pbBuffer );
            return ;
        }
    }
    // ½Ã°£ Ãâ·Â
    kPrintf( "   Sequential Read(1 Byte): %d ms\n", kGetTickCount() - 
             qwLastTickCount );
    
    // ÆÄÀÏÀ» ´İ°í ¸Ş¸ğ¸®¸¦ ÇØÁ¦ÇÔ
    fclose( pstFile );
    kFreeMemory( pbBuffer );
}

/**
 *  ÆÄÀÏ ½Ã½ºÅÛÀÇ Ä³½Ã ¹öÆÛ¿¡ ÀÖ´Â µ¥ÀÌÅÍ¸¦ ¸ğµÎ ÇÏµå µğ½ºÅ©¿¡ ¾¸ 
 */
static void kFlushCache( const char* pcParameterBuffer )
{
    QWORD qwTickCount;
    
    qwTickCount = kGetTickCount();
    kPrintf( "Cache Flush... ");
    if( kFlushFileSystemCache() == TRUE )
    {
        kPrintf( "Pass\n" );
    }
    else
    {
        kPrintf( "Fail\n" );
    }
    kPrintf( "Total Time = %d ms\n", kGetTickCount() - qwTickCount );
}

static void kAddUser(){
    char id[16]={0},password[16]={0};
    BOOL res;
    int icheckroot;

    icheckroot = kMemCmp(currentUserID,"root",4);

    if(icheckroot != 0){
        kPrintf("Permission denied\n");
    }

    else{

        kPrintf("Input New ID: ");
        getString(id);

        kPrintf("Input New Password: ");
        getString(password);

        res = addUser(id,password);
        
        if(res==TRUE) kPrintf("Success!\n");
        else{
            kPrintf("Same user name already\n");
        }
        kFlushFileSystemCache();
    }
}

static void kChangeUser(){
    char id[16]={0},password[16]={0};
    BOOL res;

        kPrintf("Input ID: ");
        getString(id);

        kPrintf("Input Password: ");
        getString(password);

        res = checkUserInfo(id,password);
        
        if(res==TRUE) {
            kPrintf("Success!\n");
            kStrCpy(currentUserID,id);
            kPrintf("now user %s\n",currentUserID);
        }
        else{
            kPrintf("Failed\n");
        }
}

static void kChangePasswd(){
    char password[16]={0}, newPasswd[16]={0}, newPasswd2[16]={0};;
    BOOL res;
    if(isPrePasswdExist(currentUserID)){
        kPrintf("Input password: ");
        getString(password);
        res=checkUserInfo(currentUserID,password);
        if(res==FALSE){
            kPrintf("Wrong password\n");
            return;
        }
    }
    
    kPrintf("Input new password: ");
    getString(newPasswd);
    kPrintf("Input Again: ");
    getString(newPasswd2);

    if(!kStrCmp(newPasswd,newPasswd2)){
        kPrintf("Not Same\n");
        return;
    }

    setPasswd(currentUserID, newPasswd);

    kFlushFileSystemCache();

    kPrintf("Success!\n");
}

static void kDeleteUser(){
    char id[16]={0},password[16]={0};
    BOOL res;
    int icheckroot;

    icheckroot = kMemCmp(currentUserID,"root",4);

    if(icheckroot != 0){
        kPrintf("Permission denied\n");
    }

    else{
        kPrintf("Input ID: ");
        getString(id);

        if(kStrCmp(id,currentUserID)){
            kPrintf("You can't erase current user\n");
            return;
        }

        if(kStrCmp(id,"root")){
            kPrintf("You can't erase root user\n");
            return;
        }

        kPrintf("Input Password: ");
        getString(password);

        res = checkUserInfo(id,password);

        if(res==FALSE){
            kPrintf("Wrong ID orPassword\n");
        }

        deleteUser(id);

        kFlushFileSystemCache();

        kPrintf("Sucess!\n");
    }
}

static void kChangePermission( const char* pcParameterBuffer ){
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    char userName [ 16 ];
    char FilePermission[3];
    int iLength;
    FILE* fp;
    int iEnterCount;
    int iFilePermission = 0;
    BOOL bCheckSucess;
    
    kInitializeParameter( &stList, pcParameterBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';

    iLength = kGetNextParameter( &stList, FilePermission);
    FilePermission[iLength] = '\0';
    iFilePermission = kAToI( FilePermission , 10 );

    bCheckSucess = kChangeFilePermission(vcFileName, iFilePermission);
    if(bCheckSucess)
        kPrintf("Permission change Sucess\n");

    else
        kPrintf("Permission change Fail\n");    

}

void kChangeOwner(const char * pcParamegerBuffer ){

    PARAMETERLIST stList;
    int iLength;
    char vcFileName[50];
    char OwnUser[50];
    int iDirectoryoffset;
    DIRECTORYENTRY stEntry;
    int icheckUser;
    BOOL bCheckOwner;
    int icheckUserExist;

    kInitializeParameter( &stList, pcParamegerBuffer );
    iLength = kGetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';

    iLength = kGetNextParameter( &stList, OwnUser);
    OwnUser[ iLength ] = '\0';
    icheckUser = kMemCmp("root",currentUserID,4);
    icheckUserExist = getUserTableIndex(OwnUser);
    
    if(icheckUserExist == -1){
        kPrintf("User not Exist\n");
        return;
    }
    if(!icheckUser && icheckUserExist != -1){
        bCheckOwner = kChangeFileOwner(vcFileName, OwnUser);
        if(bCheckOwner)
            kPrintf("File Owner change Success\n");

        else
            kPrintf("File Owner change Fail\n");
    }
        
    else{
        kPrintf("Permission denied\n");
    }

}
    
static void kShowUser(){
    showTable();
}