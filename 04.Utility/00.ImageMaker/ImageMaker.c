/**
 *  file    ImageMaker.c
 *  date    2008/12/16
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ? ?™?˜™?Š¸ ? ?‹¸?Œ?˜™? ?™?˜™ ì»¤å ?™?˜™ ? ?‹±ë±„ì˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?‹¹ê³¤ì˜™, ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?Œ?Œ?˜™ ImageMaker? ?™?˜™ 
 *          ? ????™?˜™ ? ?™?˜™? ?™?˜™
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifndef O_BINARY
#define O_BINARY 0x00
#endif

#define BYTESOFSECTOR  512

// ? ?Œ‰?‡½?˜™ ? ?™?˜™? ?™?˜™
int AdjustInSectorSize( int iFd, int iSourceSize );
void WriteKernelInformation( int iTargetFd, int iKernelSectorCount );
int CopyFile( int iSourceFd, int iTargetFd, int IsKernel );

/**
 *  Main ? ?Œ‰?‡½?˜™
*/
int main(int argc, char* argv[])
{
    int iSourceFd;
    int iTargetFd;
    int iBootLoaderSize;
    int iKernel32SectorCount;
    int iSourceSize;

    // ì»¤å ?‹¤?“¸?˜™ ? ?™?˜™? ?™?˜™ ? ?‹¬?‡½?˜™ ? ?‹¯?‚¼?˜™
    if( argc < 3 )
    {
        fprintf( stderr, "[ERROR] ImageMaker.exe BootLoader.bin Kernel32.bin\n" );
        exit( -1 );
    }
    
    // Disk.img ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™
    if( ( iTargetFd = open( "Disk.img", O_RDWR | O_CREAT |  O_TRUNC |
            O_BINARY, S_IREAD | S_IWRITE ) ) == -1 )
    {
        fprintf( stderr , "[ERROR] Disk.img open fail.\n" );
        exit( -1 );
    }

    //--------------------------------------------------------------------------
    // ? ?™?˜™?Š¸ ? ?‹¸?Œ?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?˜?„œ ? ?™?˜™?   ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™?¬ ? ?‹±ë±„ì˜™? ?™?˜™ ? ?™?˜™? ?‹¹ë¤„ì˜™ ? ?™?˜™? ?™?˜™
    //--------------------------------------------------------------------------
    printf( "[INFO] Copy boot loader to image file\n" );
    if( ( iSourceFd = open( argv[ 1 ], O_RDONLY | O_BINARY ) ) == -1 )
    {
        fprintf( stderr, "[ERROR] %s open fail\n", argv[ 1 ] );
        exit( -1 );
    }

    iSourceSize = CopyFile( iSourceFd, iTargetFd, 0 );
    close( iSourceFd );
    
    // ? ?™?˜™? ?™?˜™ ?¬? ?©ë¥? ? ?™?˜™? ?™?˜™ ?¬? ?™?˜™? ?™?˜™ 512? ?™?˜™? ?™?˜™?Š¸? ?™?˜™ ? ?™?˜™? ?Œ©ê¹ì˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?‹¸ë¸ì˜™? ?™?˜™ 0x00 ? ?™?˜™? ?™?˜™ ì±„å ?™?˜™
    iBootLoaderSize = AdjustInSectorSize( iTargetFd , iSourceSize );
    printf( "[INFO] %s size = [%d] and sector count = [%d]\n",
            argv[ 1 ], iSourceSize, iBootLoaderSize );

    //--------------------------------------------------------------------------
    // 32? ?™?˜™?Š¸ ì»¤å ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?˜?„œ ? ?™?˜™?   ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™?¬ ? ?‹±ë±„ì˜™? ?™?˜™ ? ?™?˜™? ?‹¹ë¤„ì˜™ ? ?™?˜™? ?™?˜™
    //--------------------------------------------------------------------------
    printf( "[INFO] Copy protected mode kernel to image file\n" );
    if( ( iSourceFd = open( argv[ 2 ], O_RDONLY | O_BINARY ) ) == -1 )
    {
        fprintf( stderr, "[ERROR] %s open fail\n", argv[ 2 ] );
        exit( -1 );
    }

    iSourceSize = CopyFile( iSourceFd, iTargetFd ,1);
    close( iSourceFd );
    
    // ? ?™?˜™? ?™?˜™ ?¬? ?©ë¥? ? ?™?˜™? ?™?˜™ ?¬? ?™?˜™? ?™?˜™ 512? ?™?˜™? ?™?˜™?Š¸? ?™?˜™ ? ?™?˜™? ?Œ©ê¹ì˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?‹¸ë¸ì˜™? ?™?˜™ 0x00 ? ?™?˜™? ?™?˜™ ì±„å ?™?˜™
    iKernel32SectorCount = AdjustInSectorSize( iTargetFd, iSourceSize );
    printf( "[INFO] %s size = [%d] and sector count = [%d]\n",
                argv[ 2 ], iSourceSize, iKernel32SectorCount );

    //--------------------------------------------------------------------------
    // ? ?™?˜™?¬ ? ?‹±ë±„ì˜™? ?™?˜™? ?™?˜™ ì»¤å ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™
    //--------------------------------------------------------------------------
    printf( "[INFO] Start to write kernel information\n" );    
    // ? ?™?˜™?Š¸? ?™?˜™? ?™?˜™? ?™?˜™ 5? ?™?˜™ì§? ? ?™?˜™? ?™?˜™?Š¸? ?™?˜™? ?™?˜™ ì»¤å ?‹¸?š¸?˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™
    WriteKernelInformation( iTargetFd, iKernel32SectorCount );
    printf( "[INFO] Image file create complete\n" );

    close( iTargetFd );
    return 0;
}

/**
 *  ? ?™?˜™? ?™?˜™ ? ?™?˜™ì¹˜å ?™?˜™? ?™?˜™ 512? ?™?˜™? ?™?˜™?Š¸ ? ?™?˜™?   ? ?™?˜™ì¹˜å ?™?˜™? ?™?˜™ ? ?™?˜™? ?Œ©?–µ?˜™ 0x00? ?™?˜™? ?™?˜™ ì±„å ?™?˜™
*/
int AdjustInSectorSize( int iFd, int iSourceSize )
{
    int i;
    int iAdjustSizeToSector;
    char cCh;
    int iSectorCount;

    iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
    cCh = 0x00;
    
    if( iAdjustSizeToSector != 0 )
    {
        iAdjustSizeToSector = 512 - iAdjustSizeToSector;
        printf( "[INFO] File size [%lu] and fill [%u] byte\n", iSourceSize, 
            iAdjustSizeToSector );
        for( i = 0 ; i < iAdjustSizeToSector ; i++ )
        {
            write( iFd , &cCh , 1 );
        }
    }
    else
    {
        printf( "[INFO] File size is aligned 512 byte\n" );
    }
    
    // ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™ ? ?‹¤?“¸?˜™? ?™?˜™? ?™?˜™
    iSectorCount = ( iSourceSize + iAdjustSizeToSector ) / BYTESOFSECTOR;
    return iSectorCount;
}

/**
 *  ? ?™?˜™?Š¸ ? ?‹¸?Œ?˜™? ?™?˜™ ì»¤å ?‹¸?š¸?˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?™?˜™
*/
void WriteKernelInformation( int iTargetFd, int iKernelSectorCount )
{
    unsigned short usData;
    long lPosition;
    
    // ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™? ?Œœ?š¸?˜™? ?™?˜™ 5? ?™?˜™? ?™?˜™?Š¸ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™ì¹˜å ?™?˜™ ì»¤å ?™?˜™? ?™?˜™ ? ?™?˜™ ? ?™?˜™? ?™?˜™ ? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™???? ?™?˜™
    lPosition = lseek( iTargetFd, 5, SEEK_SET );
    if( lPosition == -1 )
    {
        fprintf( stderr, "lseek fail. Return value = %d, errno = %d, %d\n", 
            lPosition, errno, SEEK_SET );
        exit( -1 );
    }

    usData = ( unsigned short ) iKernelSectorCount;
    write( iTargetFd, &usData, 2 );

    printf( "[INFO] Total sector count except boot loader [%d]\n", 
        iKernelSectorCount );
}

/**
 *  ? ????™?˜™ ? ?™?˜™? ?™?˜™(Source FD)? ?™?˜™ ? ?™?˜™? ?™?˜™? ?™?˜™ ? ?™?˜™?‘œ ? ?™?˜™? ?™?˜™(Target FD)? ?™?˜™ ? ?™?˜™? ?™?˜™? ?‹¹ê³¤ì˜™ ? ?™?˜™ ?¬? ?©ë¥? ? ?‹¤?“¸?˜™? ?™?˜™? ?™?˜™
*/
int CopyFile( int iSourceFd, int iTargetFd , int IsKernel )
{
    int iSourceFileSize;
    int iRead;
    int iWrite;
    char vcBuffer[ BYTESOFSECTOR ];

    iSourceFileSize = 0;


    //int hash=0;
    char hashBuff[4]={0};
    char FbyteChar[4]={0};
    int FbyteInt=0;
    int check=0;
    int ReadByte;

    char kernelByte[1536]={0};
    char hash[4]={0};
    
    if(IsKernel==1){
      /*while(1){
            ReadByte = read(iSourceFd,FbyteChar,4);
            for(int i=0;i<4;i++){
                int tmp=FbyteChar[i]&0xFF;
                //printf(" %d ",tmp);
                FbyteInt |= tmp;
                if(i!=3) FbyteInt <<= 8;
            }
            //printf("\nInt:%d ",FbyteInt);
            if(check==0)
            hash=FbyteInt;
            else
                hash^=FbyteInt;

            if(ReadByte!=4) break;
        }*/
        read(iSourceFd,kernelByte,sizeof(kernelByte));

        lseek(iSourceFd,0,SEEK_SET);
        
        /*for(int i=0;i<4;i++){
            int tmp=hash&0xFF;
            hashBuff[i] |= tmp;
            if(i!=3) hash >>= 8;
        }*/
        
        for(int i=0;i<1536;i++){
            //printf("%d %01X %01X %01X\n",i,hash[i%4],kernelByte[i],hash[i%4]^kernelByte[i]);
            if(i<4)hash[i]=kernelByte[i];
            else hash[i%4]^=kernelByte[i];
        }

        int hI=0;
        for(int i=0;i<4;i++){
            int tmp=hash[i];
            hI |= tmp;
            if(i!=3) hI<<=8;
        }
        
        iSourceFileSize = write(iTargetFd, hash, sizeof(hash));
        printf("%d\n",iSourceFileSize);
    }


    while( 1 )
    {
        iRead   = read( iSourceFd, vcBuffer, sizeof( vcBuffer ) );
        iWrite  = write( iTargetFd, vcBuffer, iRead );

        
        printf("\n%d %d\n",iRead,sizeof(vcBuffer));


        if( iRead != iWrite )
        {
            fprintf( stderr, "[ERROR] iRead != iWrite.. \n" );
            exit(-1);
        }
        iSourceFileSize += iRead;
        
        if( iRead != sizeof( vcBuffer ) )
        {
            break;
        }
    }


    return iSourceFileSize;
} 
