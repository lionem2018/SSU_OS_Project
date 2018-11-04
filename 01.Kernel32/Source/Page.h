/**
 *  file    Types.h
 *  date    2008/12/28
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ����¡�� ���õ� ���� ������ ������ ����
 */

#ifndef __PAGE_H__
#define __PAGE_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ���� 32��Ʈ�� �Ӽ� �ʵ�
#define PAGE_FLAGS_P        0x00000001  // Present
#define PAGE_FLAGS_RW       0x00000002  // Read/Write
#define PAGE_FLAGS_US       0x00000004  // User/Supervisor(�÷��� ���� �� ���� ����)
#define PAGE_FLAGS_PWT      0x00000008  // Page Level Write-through
#define PAGE_FLAGS_PCD      0x00000010  // Page Level Cache Disable
#define PAGE_FLAGS_A        0x00000020  // Accessed
#define PAGE_FLAGS_D        0x00000040  // Dirty
#define PAGE_FLAGS_PS       0x00000080  // Page Size
#define PAGE_FLAGS_G        0x00000100  // Global
#define PAGE_FLAGS_PAT      0x00001000  // Page Attribute Table Index
// ���� 32��Ʈ�� �Ӽ� �ʵ�
#define PAGE_FLAGS_EXB      0x80000000  // Execute Disable ��Ʈ  
// �Ʒ��� ���Ǹ� ���� ������ �÷���
#define PAGE_FLAGS_DEFAULT  ( PAGE_FLAGS_P | PAGE_FLAGS_RW )

// ��Ÿ ����¡ ����
#define PAGE_TABLESIZE      0x1000
#define PAGE_MAXENTRYCOUNT  512
#define PAGE_DEFAULTSIZE    0x200000
#define PAGE_SMALLSIZE      0x1000

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

// ������ ��Ʈ���� ���� �ڷᱸ��
typedef struct kPageTableEntryStruct
{
    // PML4T�� PDPTE�� ���?
    // 1 ��Ʈ P, RW, US, PWT, PCD, A, 3 ��Ʈ Reserved, 3 ��Ʈ Avail, 
    // 20 ��Ʈ Base Address
    // PDE�� ���?
    // 1 ��Ʈ P, RW, US, PWT, PCD, A, D, 1, G, 3 ��Ʈ Avail, 1 ��Ʈ PAT, 8 ��Ʈ Avail, 
    // 11 ��Ʈ Base Address
    DWORD dwAttributeAndLowerBaseAddress;
    // 8 ��Ʈ Upper BaseAddress, 12 ��Ʈ Reserved, 11 ��Ʈ Avail, 1 ��Ʈ EXB 
    DWORD dwUpperBaseAddressAndEXB;
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kInitializePageTables( void );
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
        DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags );

#endif /*__PAGE_H__*/
