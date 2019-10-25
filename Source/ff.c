//ff.c
#include "rtc.h"
#include "ff_config.h"

static pFileSystem g_pfs;
static tFlashOpt g_FlashOpt;

FRESULT f_open(FIL *fp, const unsigned long fn, eMODE mode)
{
    unsigned long i;//, LastIndex = MAX_FILE_NUM(), IdleIndex = MAX_FILE_NUM();
    tFileHead fh[MAX_FILE_NUM()];
    tFileHead LastFile;
    FRESULT res = FR_NO_FILE;
    
    for(i = 0; i < MAX_FILE_NUM(); i++)
    {
        g_FlashOpt.read((unsigned char *)&fh[i], g_pfs->FileAddr[i], sizeof(fh[0]));
    }
    
    for(i = 0; i < MAX_FILE_NUM(); i++)
    {
        if(fh[i].FileName == fn)
        {
            fp -> FileName = fh[i].FileName;
            fp -> StartAddr = g_pfs->FileAddr[i];
            fp -> mode = mode;
            fp -> flag = 0xFACE;
            res = FR_OK;
            break;
        }
    }
    
    if(mode == FA_READ)
    {
        if(res == FR_NO_FILE)
        {
            fp -> FileName = 0xFFFFFFFF;
            fp -> StartAddr = 0xFFFFFFFF;
            fp -> flag = 0xFFFF;
            fp -> mode = FA_ERROR;
            fp -> pointer = 0xFFFFFFFF;
            
            return FR_NO_FILE;
        }
        else
        {
            fp -> pointer = sizeof(fh[0]);
        }
    }
    else if(mode == FA_WRITE)
    {
        fp -> FileName = fn;
        fp -> pointer = sizeof(fh[0]);
        fp -> mode = mode;
        fp -> flag = 0xFACE;
        if(res == FR_NO_FILE)
        {
            for(i = 0; i < MAX_FILE_NUM(); i++)
            {
                if(fh[i].FileName == 0xFFFFFFFF)
                {
                    fp -> StartAddr = g_pfs->FileAddr[i];
                    g_FlashOpt.erase(fp -> StartAddr);
                    g_FlashOpt.write((unsigned char *)&(fp->FileName), fp -> StartAddr, sizeof(fp -> FileName));
                    break;;
                }
            }
            if(i == MAX_FILE_NUM())//not find idle file,the find lated file
            {
                LastFile.FileName = 0xFFFFFFFF;
                for(i = 0; i < MAX_FILE_NUM(); i++)
                {
                    if(LastFile.FileName < fh[i].FileName)
                    {
                        LastFile.FileName = fh[i].FileName;
                        fp -> StartAddr = g_pfs->FileAddr[i];
                    }
                }
                g_FlashOpt.erase(fp -> StartAddr);
                g_FlashOpt.write((unsigned char *)&(fp->FileName), fp -> StartAddr, sizeof(fp -> FileName));
            }
        }
    }
    else
    {
    }
    
    return FR_OK;
}

FRESULT f_close(FIL *fp)
{
    fp->FileName = 0xFFFFFFFF;
    fp->flag = 0xFF;
    return FR_OK;
}

FRESULT f_read(FIL *fp, const unsigned char *path, unsigned char *pBuf, unsigned long br)
{
    return FR_OK;
}

FRESULT f_write(FIL *fp, const unsigned char *path, unsigned char *pBuf, unsigned long bw)
{
    return FR_OK;
}

FRESULT f_mount(FileSystem * pfs, tFlashOpt *pOpt)
{
    g_FlashOpt.init = pOpt -> init;
    g_FlashOpt.erase = pOpt -> erase;
    g_FlashOpt.chip_erase = pOpt -> chip_erase;
    g_FlashOpt.read  = pOpt -> read;
    g_FlashOpt.write = pOpt -> write;
    g_FlashOpt.init();
    g_FlashOpt.gettime = pOpt -> gettime;
    
    pfs -> fs_type = 0xFF;
    
    g_FlashOpt.read((unsigned char *)pfs, FLASH_PHYS_ADDR(), sizeof(FileSystem));
    
    if(pfs -> fs_type == 'H')
    {
        g_pfs = pfs;
        return FR_OK;
    }
    else
    {
        return FR_ERROR;
    }
}

FRESULT f_mkfs(FileSystem * pfs)
{
    unsigned short i;
    tRTC rtc;
    unsigned long AvailableBlocks, FileBlocks;
    
    pfs -> fs_type = 'H';
    g_FlashOpt.gettime(&rtc);
    
    AvailableBlocks = (unsigned long)FLASH_PHYS_SZ() / (unsigned long)FLASH_PHYS_BLOCK_SZ() - 1;
    FileBlocks = AvailableBlocks / MAX_FILE_NUM();
    pfs -> FileBlocks = AvailableBlocks / MAX_FILE_NUM();
    
    for(i = 0; i < MAX_FILE_NUM(); i++)
    {
        pfs -> FileAddr[i] = 1 + i * FileBlocks;
    }
    
    g_FlashOpt.chip_erase();
    g_FlashOpt.write((unsigned char *)pfs, FLASH_PHYS_ADDR(), sizeof(FileSystem));
    
    return FR_OK;
}

FRESULT f_mkfs_lite(FileSystem * pfs)
{
    g_FlashOpt.erase(FLASH_PHYS_ADDR());
    g_FlashOpt.write((unsigned char *)pfs, FLASH_PHYS_ADDR(), sizeof(FileSystem));
    return FR_OK;
}
