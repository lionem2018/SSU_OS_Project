/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ܼ� �п� ���õ� �ҽ� ����
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

// Ŀ�ǵ� ���̺� ����
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
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
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
        
        // Ű�� ���ŵ� ������ ���?
        bLastSecond = bLastMinute = bCurrentSecond = bCurrentMinute = bHour = 0;
        bKey = kGetCh();
        // Backspace Ű ó��
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // ���� Ŀ�� ��ġ�� ���? �� ���� ������ �̵��� ���� ������ ����ϰ�? 
                // Ŀ�ǵ� ���ۿ��� ������ ���� ����
                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        // ���� Ű ó��
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );
            
            if( iCommandBufferIndex > 0 )
            {
                // Ŀ�ǵ� ���ۿ� �ִ� �����? ����
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                kReadRTCTime( &bHour, &bLastMinute, &bLastSecond );
                kExecuteCommand( vcCommandBuffer );
                kReadRTCTime( &bHour, &bCurrentMinute, &bCurrentSecond );
                kSetCommandHistory(vcCommandHistoryList, vcCommandBuffer, &iHistoryCount);
                
                iHistoryIndex = iHistoryCount;
            }
            
            // ������Ʈ ���? �� Ŀ�ǵ� ���� �ʱ�ȭ
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE ); 
            kPrintStringXY( 0, 23, cline);
            kPrintTime(bLastMinute, bLastSecond, bCurrentMinute, bCurrentSecond);
            kPrintStringXY( 0, 24, CONSOLESHELL_RUNNINGTIME );
            kPrintStringXY( 58, 24,CONSOLESHELL_CURRENTTIME);         
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // ����Ʈ Ű, CAPS Lock, NUM Lock, Scroll Lock�� ����
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
            // TAB??? 공백?���? ?��?��
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // 버퍼?�� 공간?�� ?��?��?��?�� ?���? �??��
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}
/*
 *  Ŀ�ǵ� ���ۿ� �ִ� Ŀ�ǵ带 ���Ͽ� �ش� Ŀ�ǵ带 ó���ϴ� �Լ��� ����
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // �������� ���е� Ŀ�ǵ带 ����
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // Ŀ�ǵ� ���̺��� �˻��ؼ� ������ �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // Ŀ�ǵ��� ���̿� ������ ������ ��ġ�ϴ��� �˻�
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ����Ʈ���� ã�� �� ���ٸ� ���� ���?
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �Ķ����? �ڷᱸ���� �ʱ�ȭ
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �������� ���е� �Ķ������? �����? ���̸� ��ȯ
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �� �̻� �Ķ���Ͱ�? ������ ����
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �Ķ���͸�? �����ϰ� ���̸� ��ȯ
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �Ķ������? ��ġ ������Ʈ
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ���?
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

    // ���� �� Ŀ�ǵ��� ���̸� ���?
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // ���� ���?
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  ȭ���� ���� 
 */
void kCls( const char* pcParameterBuffer )
{
    // �� ������ �����? ������ ����ϹǷ�? ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  �� �޸� ũ�⸦ ���?
 */
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ���ڿ��� �� ���ڸ� ���ڷ� ��ȯ�Ͽ� ȭ�鿡 ���?
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    // �Ķ����? �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while( 1 )
    {
        // ���� �Ķ���͸�? ����, �Ķ������? ���̰� 0�̸� �Ķ���Ͱ�? ���� ���̹Ƿ�
        // ����
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // �Ķ���Ϳ�? ���� ������ ����ϰ�? 16�������� 10�������� �Ǵ��Ͽ� ��ȯ�� ��
        // �����? printf�� ���?
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, 
                 vcParameter, iLength );

        // 0x�� �����ϸ� 16����, �׿ܴ� 10������ �Ǵ�
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
 *  PC�� �����?(Reboot)
 */
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    // Ű���� ��Ʈ�ѷ��� ���� PC�� �����?
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
 *  PIT 컨트롤러?�� 카운?�� 0 ?��?��
 */
void kSetTimer( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    PARAMETERLIST stList;
    long lValue;
    BOOL bPeriodic;

    // ?��?��미터 초기?��
    kInitializeParameter( &stList, pcParameterBuffer );
    
    // milisecond 추출
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }
    lValue = kAToI( vcParameter, 10 );

    // Periodic 추출
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
 *  PIT 컨트롤러�? 직접 ?��?��?��?�� ms ?��?�� ???�?  
 */
void kWaitUsingPIT( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    long lMillisecond;
    int i;
    
    // ?��?��미터 초기?��
    kInitializeParameter( &stList, pcParameterBuffer );
    if( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)wait 100(ms)\n" );
        return ;
    }
    
    lMillisecond = kAToI( pcParameterBuffer, 10 );
    kPrintf( "%d ms Sleep Start...\n", lMillisecond );
    
    // ?��?��?��?���? 비활?��?��?���? PIT 컨트롤러�? ?��?�� 직접 ?��간을 측정
    kDisableInterrupt();
    for( i = 0 ; i < lMillisecond / 30 ; i++ )
    {
        kWaitUsingDirectPIT( MSTOCOUNT( 30 ) );
    }
    kWaitUsingDirectPIT( MSTOCOUNT( lMillisecond % 30 ) );   
    kEnableInterrupt();
    kPrintf( "%d ms Sleep Complete\n", lMillisecond );
    
    // ????���? 복원
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );
}

/**
 *  ????�� ?��?��?�� 카운?���? ?��?��  
 */
void kReadTimeStampCounter( const char* pcParameterBuffer )
{
    QWORD qwTSC;
    
    qwTSC = kReadTSC();
    kPrintf( "Time Stamp Counter = %q\n", qwTSC );
}

/**
 *  ?��로세?��?�� ?��?���? 측정
 */
void kMeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    kPrintf( "Now Measuring." );
    
    // 10�? ?��?�� �??��?�� ????�� ?��?��?�� 카운?���? ?��?��?��?�� ?��로세?��?�� ?��?���? 간접?��?���? 측정
    kDisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf( "." );
    }
    // ????���? 복원
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );    
    kEnableInterrupt();
    
    kPrintf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC 컨트롤러?�� ????��?�� ?��?�� �? ?���? ?��보�?? ?��?��
 */
void kShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC 컨트롤러?��?�� ?���? �? ?��?���? ?��?��
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    kPrintf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             kConvertDayOfWeekToString( bDayOfWeek ) );
    kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

// TCB ?��료구조�?? ?��?�� ?��?��
static TCB gs_vstTask[ 2 ] = { 0, };
static QWORD gs_vstStack[ 1024 ] = { 0, };

void kTestTask( void )
{
    int i = 0;
    
    while( 1 )
    {
        // 메시�?�? 출력?���? ?�� ?��?��?�� ???�?
        kPrintf( "[%d] This message is from kTestTask. Press any key to switch "
                 "kConsoleShell~!!\n", i++ );
        kGetCh();
        
        // ?��?��?�� ?���? ?��?��?���? ?��?��?���? ?��?��
        kSwitchContext( &( gs_vstTask[ 1 ].stContext ), &( gs_vstTask[ 0 ].stContext ) );
    }
}
/**
 *  ?��?��?���? ?��?��?��?�� �??�� ?��?��?�� ?��?��
 */
// void kCreateTestTask( const char* pcParameterBuffer )
// {
//     KEYDATA stData;
//     int i = 0;
    
//     // ?��?��?�� ?��?��
//     kSetUpTask( &( gs_vstTask[ 1 ] ), 1, 0, ( QWORD ) kTestTask, &( gs_vstStack ), 
//                 sizeof( gs_vstStack ) );
    
//     // 'q' ?���? ?��?��?���? ?��?�� ?��까�?? ?��?��
//     while( 1 )
//     {
//         // 메시�?�? 출력?���? ?�� ?��?��?�� ???�?
//         kPrintf( "[%d] This message is from kConsoleShell. Press any key to "
//                  "switch TestTask~!!\n", i++ );
//         if( kGetCh() == 'q' )
//         {
//             break;
//         }
//         // ?��?��?�� ?���? ?��?��?���? ?��?��?���? ?��?��
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
 *  �½�ũ 1
 *      ȭ�� �׵θ��� ���鼭 ���ڸ� ���
 */
static void kTestTask1( void )
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    
    // �ڽ��� ID�� �� ȭ�� ���������� ���
    pstRunningTask = kGetRunningTask();
    iMargin = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) % 10;
    
    // ȭ�� �� �����̸� ���鼭 ���� ���
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
        
        // ���� �� ���� ����
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bCharactor = bData;
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bAttribute = bData & 0x0F;
        bData++;
        
        // �ٸ� �½�ũ�� ��ȯ
        //kSchedule();
    }

    //kExitTask();
}

/**
 *  �½�ũ 2
 *      �ڽ��� ID�� �����Ͽ� Ư�� ��ġ�� ȸ���ϴ� �ٶ����� ���
 */
static void kTestTask2( void )
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };
    
    // �ڽ��� ID�� �� ȭ�� ���������� ���
    pstRunningTask = kGetRunningTask();
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - 
        ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    while(1)
    {
        // ȸ���ϴ� �ٶ����� ǥ��
        pstScreen[ iOffset ].bCharactor = vcData[ i % 4 ];
        // ���� ����
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
        i++;
        
        // �ٸ� �½�ũ�� ��ȯ
        //kSchedule();
    }
}

/**
 *  �½�ũ�� �����ؼ� ��Ƽ �½�ŷ ����
 */
static void kCreateTestTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcType[ 30 ];
    char vcCount[ 30 ];
    int i;
    
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcType );
    kGetNextParameter( &stList, vcCount );

    switch( kAToI( vcType, 10 ) )
    {
    // Ÿ�� 1 �½�ũ ����
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
        
    // Ÿ�� 2 �½�ũ ����
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
 *  �½�ũ�� �켱 ������ ����
 */
static void kChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    kGetNextParameter( &stList, vcPriority );
    
    // �½�ũ�� �켱 ������ ����
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
 *  ���� ������ ��� �½�ũ�� ������ ���
 */
static void kShowTaskList( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;
    
    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        // TCB�� ���ؼ� TCB�� ��� ���̸� ID�� ���
        pstTCB = kGetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �½�ũ�� 10�� ��µ� ������, ��� �½�ũ ������ ǥ������ ���θ� Ȯ��
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
 *  �½�ũ�� ����
 */
static void kKillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // �Ķ���͸� ����
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    
    // �½�ũ�� ����
    if( kMemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = kAToI( vcID + 2, 16 );
    }
    else
    {
        qwID = kAToI( vcID, 10 );
    }
    
    // Ư�� ID�� �����ϴ� ���
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = kGetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �ý��� �׽�Ʈ�� ����
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
    // �ܼ� �а� ���� �½�ũ�� �����ϰ� ��� �½�ũ ����
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = kGetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �ý��� �׽�Ʈ�� ���� ��Ͽ��� ����
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
 *  ���μ����� ������ ǥ��
 */
static void kCPULoad( const char* pcParameterBuffer )
{
    kPrintf( "Processor Load : %d%%\n", kGetProcessorLoad() );
    kPrintf( "Console Load : %d%%\n", kGetConsoleProcessorLoad() );
}
    
// ���ؽ� �׽�Ʈ�� ���ؽ��� ����
static MUTEX gs_stMutex;
static volatile QWORD gs_qwAdder;

/**
 *  ���ؽ��� �׽�Ʈ�ϴ� �½�ũ
 */
static void kPrintNumberTask( void )
{
    int i;
    int j;
    QWORD qwTickCount;

    // 50ms ���� ����Ͽ� �ܼ� ���� ����ϴ� �޽����� ��ġ�� �ʵ��� ��
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 50 )
    {
        kSchedule();
    }    
    
    // ������ ���鼭 ���ڸ� ���
    for( i = 0 ; i < 5 ; i++ )
    {
        kLock( &( gs_stMutex ) );
        kPrintf( "Task ID [0x%Q] Value[%d]\n", kGetRunningTask()->stLink.qwID,
                gs_qwAdder );
        
        gs_qwAdder += 1;
        kUnlock( & ( gs_stMutex ) );
    
        // ���μ��� �Ҹ� �ø����� �߰��� �ڵ�
        for( j = 0 ; j < 30000 ; j++ ) ;
    }
    
    // ��� �½�ũ�� ������ ������ 1��(100ms) ���� ���
    qwTickCount = kGetTickCount();
    while( ( kGetTickCount() - qwTickCount ) < 1000 )
    {
        kSchedule();
    }    
    
    // �½�ũ ����
    //kExitTask();
}

/**
 *  ���ؽ��� �׽�Ʈ�ϴ� �½�ũ ����
 */
static void kTestMutex( const char* pcParameterBuffer )
{
    int i;
    
    gs_qwAdder = 1;
    
    // ���ؽ� �ʱ�ȭ
    kInitializeMutex( &gs_stMutex );
    
    for( i = 0 ; i < 3 ; i++ )
    {
        // ���ؽ��� �׽�Ʈ�ϴ� �½�ũ�� 3�� ����
        kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, ( QWORD ) kPrintNumberTask );
    }    
    kPrintf( "Wait Util %d Task End...\n", i );
    kGetCh();
}

/**
 *  �½�ũ 2�� �ڽ��� ������� �����ϴ� �½�ũ
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
 *  �����带 �׽�Ʈ�ϴ� �½�ũ ����
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
 *  ö�ڸ� �귯������ �ϴ� ������
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
        // ��� �����
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
 *  �����带 �����Ͽ� ��Ʈ���� ȭ��ó�� �����ִ� ���μ���
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

    // Ű�� �ԷµǸ� ���μ��� ����
    kGetCh();
}

/**
 *  ��Ʈ���� ȭ���� ������
 */
static void kShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = kCreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) kMatrixProcess );
    if( pstProcess != NULL )
    {
        kPrintf( "Matrix Process [0x%Q] Create Success\n" );

        // �½�ũ�� ���� �� ������ ���
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
                // TCB�� ���ؼ� TCB�� ��� ���̸� ID�� ���
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