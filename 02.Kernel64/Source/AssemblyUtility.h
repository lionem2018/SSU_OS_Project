/**
 *  file    Utility.h
 *  date    2009/01/07
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ������� ��ƿ��Ƽ �Լ����� ������ ��� ����
 */

#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
//
////////////////////////////////////////////////////////////////////////////////
BYTE kInPortByte( WORD wPort );
void kOutPortByte( WORD wPort, BYTE bData );

#endif /*__ASSEMBLYUTILITY_H__*/
