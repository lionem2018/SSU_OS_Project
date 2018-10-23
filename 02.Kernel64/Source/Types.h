/**
 *  file    Types.h
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   Ŀ�ο��� ����ϴ�? ���� Ÿ���� ������ ����
 */

#ifndef __TYPES_H__
#define __TYPES_H__

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// �⺻ Ÿ�� ���� ��ũ��
#define BYTE    unsigned char
#define WORD    unsigned short
#define DWORD   unsigned int
#define QWORD   unsigned long
#define BOOL    unsigned char

#define TRUE    1
#define FALSE   0
#define NULL    0

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

// ���� ���? �� �ؽ�Ʈ ���? ȭ���� �����ϴ� �ڷᱸ��
typedef struct kCharactorStruct
{
    BYTE bCharactor;
    BYTE bAttribute;
} CHARACTER;

typedef struct kPageTableEntryStruct
{
    DWORD dwAttributeAndLowerBaseAddress; 
    DWORD dwUpperBaseAddressAndEXB;
} PDENTRY, PML4TENTRY, PDPTENTRY, PTENTRY;

#pragma pack( pop )

#endif /*__TYPES_H__*/
