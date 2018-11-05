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
        { "createtask", "Create Task", kCreateTestTask },
};                                     

//==============================================================================
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
 */
void kStartConsoleShell( void )
{
    char vcCommandHistoryList[ 10 ][ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iHistoryCount = 0,iHistoryIndex = 0;
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    BYTE bSecond, bMinute, bHour;

    
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
    kPrintTime();
    while( 1 )
    {
        // Ű�� ���ŵ� ������ ���?
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
                kExecuteCommand( vcCommandBuffer );

                kSetCommandHistory(vcCommandHistoryList, vcCommandBuffer, &iHistoryCount);
                
                iHistoryIndex = iHistoryCount;
            }
            
            // ������Ʈ ���? �� Ŀ�ǵ� ���� �ʱ�ȭ
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE ); 
            kPrintTime();          
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
void kCreateTestTask( const char* pcParameterBuffer )
{
    KEYDATA stData;
    int i = 0;
    
    // ?��?��?�� ?��?��
    kSetUpTask( &( gs_vstTask[ 1 ] ), 1, 0, ( QWORD ) kTestTask, &( gs_vstStack ), 
                sizeof( gs_vstStack ) );
    
    // 'q' ?���? ?��?��?���? ?��?�� ?��까�?? ?��?��
    while( 1 )
    {
        // 메시�?�? 출력?���? ?�� ?��?��?�� ???�?
        kPrintf( "[%d] This message is from kConsoleShell. Press any key to "
                 "switch TestTask~!!\n", i++ );
        if( kGetCh() == 'q' )
        {
            break;
        }
        // ?��?��?�� ?���? ?��?��?���? ?��?��?���? ?��?��
        kSwitchContext( &( gs_vstTask[ 0 ].stContext ), &( gs_vstTask[ 1 ].stContext ) );
    }
}

void kPrintTime(){
    char cline[81] = {'=',};
    char cSecond[3] = {'\0'}, cMinute[3] = {'\0'}, cHour[3] = {'\0'};
    BYTE bSecond, bMinute, bHour;

    kMemSet(cline , '=' , 80);
    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kIToA(bSecond,cSecond, 10);
    kIToA(bMinute,cMinute, 10);
    kIToA(bHour,cHour, 10);
    kPrintStringXY( 0, 23, cline);
    kPrintStringXY( 0, 24, CONSOLESHELL_RUNNINGTIME );
    kPrintStringXY( 57, 24,CONSOLESHELL_CURRENTTIME);
    kPrintStringXY( 71, 24, cHour);
    kPrintStringXY( 73, 24, ":");
    kPrintStringXY( 74, 24, cMinute);
    kPrintStringXY( 76, 24, ":");
    kPrintStringXY( 77, 24, cSecond);
}