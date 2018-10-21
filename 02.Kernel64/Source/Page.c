/**
 *  file    Types.h
 *  date    2008/12/28
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ����¡�� ���õ� ���� ������ ������ ����
 */

#include "Page.h"

/**
 *	IA-32e ���? Ŀ���� ���� ������ ���̺� ����
 */
void kSetProtectionPageTableEntry( void )
{
	PDENTRY* tmpPTEntry = (PDENTRY *) 0x142000;
	DWORD flags = (tmpPTEntry[PAGE_MAXENTRYCOUNT].dwAttributeAndLowerBaseAddress & 0x1FFF) | PAGE_FLAGS_P;

	kSetPageEntryData( &( tmpPTEntry[ PAGE_MAXENTRYCOUNT-1 ] ), 0, 0x1ff000, PAGE_FLAGS_DEFAULT, 0 );
}

void kSetRWPageTableEntry( void )
{
	PDENTRY* tmpPTEntry = (PDENTRY *) 0x142000;
	DWORD flags = (tmpPTEntry[PAGE_MAXENTRYCOUNT].dwAttributeAndLowerBaseAddress & 0x1FFF) | PAGE_FLAGS_RW;

	kSetPageEntryData( &( tmpPTEntry[ PAGE_MAXENTRYCOUNT-1 ] ), 0, 0x1ff000, flags, 0 );
}

/**
 *	������ ��Ʈ���� ���� �ּҿ� �Ӽ� �÷��׸� ����
 */
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
		DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = ( dwUpperBaseAddress & 0xFF ) | 
		dwUpperFlags;
}
