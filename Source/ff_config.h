//ff_config.h
#ifndef __FF_CONFIG_H__
#define __FF_CONFIG_H__

#ifndef FLASH_PHYS_SZ
#define FLASH_PHYS_SZ(ignore)       (1024UL*1024*16)
#endif
#ifndef FLASH_PHYS_ADDR
#define FLASH_PHYS_ADDR(ignore)     (0)
#endif
#ifndef FLASH_PHYS_PAGE_SZ
#define FLASH_PHYS_PAGE_SZ(ignore)  (256)
#endif
#ifndef FLASH_PHYS_BLOCK_SZ
#define FLASH_PHYS_BLOCK_SZ(ignore) (4096)
#endif
#ifndef FLASH_PHYS_ERASE_SZ
#define FLASH_PHYS_ERASE_SZ(ignore) (4096)
#endif
#ifndef MAX_FILE_NUM
#define MAX_FILE_NUM(ignore)        (10)
#endif

typedef void (* pinit)(void);
typedef void (* perase)(unsigned long sector);
typedef void (* pchiperase)(void);
typedef void (* pread)(unsigned char *buffer, unsigned long address, unsigned long len);
typedef void (* pwrite)(unsigned char const *buffer, unsigned long address, unsigned long len);
typedef unsigned char (* pgettime)(tRTC * rtc);

typedef enum
{
    FR_OK = 0,
    FR_ERROR,
    FR_NO_FILE,
}FRESULT;

typedef enum
{
    FA_READ = 0,
    FA_WRITE,
    FA_APPEND,
    FA_ERROR,
}eMODE;

typedef struct
{
    unsigned long FileName;
    unsigned long pointer;
    unsigned long StartAddr;
    unsigned short flag;
    eMODE mode;
}FIL, *pFIL;

typedef struct
{
    unsigned long FileName;
}tFileHead, *pFileHead;

typedef struct
{
    unsigned long FileBlocks;
    unsigned long FileAddr[MAX_FILE_NUM()];
    unsigned char fs_type;
    unsigned char parameter[FLASH_PHYS_PAGE_SZ()];
}FileSystem, *pFileSystem;

typedef struct
{
    pinit init;
    perase erase;
    pread read;
    pwrite write;
    pchiperase chip_erase;
    pgettime gettime;
}tFlashOpt;

FRESULT f_open(FIL *fp, const unsigned long fn, eMODE mode);
FRESULT f_close(FIL *fp);
FRESULT f_read(FIL *fp, const unsigned char *path, unsigned char *pBuf, unsigned long br);
FRESULT f_write(FIL *fp, const unsigned char *path, unsigned char *pBuf, unsigned long bw);
FRESULT f_mount(FileSystem * pfs, tFlashOpt *pOpt);
FRESULT f_mkfs(FileSystem * pfs);
FRESULT f_mkfs_lite(FileSystem * pfs);

#endif//__FF_CONFIG_H__
