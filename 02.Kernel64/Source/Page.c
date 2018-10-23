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
void kModifyPageTableEntryFlags( void )
{
	PDENTRY* tmpPTEntry = (PDENTRY *) 0x142000;

	kSetPageEntryData( &( tmpPTEntry[ PAGE_MAXENTRYCOUNT-1 ] ), 0, 0x1ff000, PAGE_FLAGS_DEFAULT, 0 );

	//DWORD * pt = (DWORD *)0x1ff000;
	invlpg(0x1ff000);
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

static inline void invlpg(void* m)
{
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}