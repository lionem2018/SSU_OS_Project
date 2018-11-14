/**
 *  file    ModeSwitch.h
 *  date    2009/01/01
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ��� ��ȯ�� ���õ� �Լ����� ������ ����
 */
 
#ifndef __MODESWITCH_H__
#define __MODESWITCH_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kReadCPUID( DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, 
        DWORD* pdwEDX );
void kSwitchAndExecute64bitKernel( void );

#endif /*__MODESWITCH_H__*/
