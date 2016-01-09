
/******************************************************************************/
/**
 *   Include File
**/
/******************************************************************************/
#include <common.h>
#include "../../include/lcdc.h"
#include "../../include/nboot.h"
#include "bmp_layout.h"

/******************************************************************************/
/**
 *   Define
**/
/******************************************************************************/
#define REGD(a)		            (*(volatile uint32_t *)(a))
#define BMP_TAG                 "INITBMP\n"
#define CFG_LOGO_MAX_SIZE       (2 << 20)
#define LOGO_UNZIP_ADDR         (CONFIG_LOAD_BASE_ADDR + KERN_SIZE)

/******************************************************************************/
/**
 *   Global Variable
**/
/******************************************************************************/


/******************************************************************************/
/**
 *   Extern Function
**/
/******************************************************************************/
extern int gunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp);

/******************************************************************************/
/**
 *   Backlight_Init Function
**/
/******************************************************************************/
void Backlight_Init(void)
{
    // Backlight turn on
    REGD(GPIO_PF_MODE) |= GPIO_PF10;
    REGD(GPIO_PF_DIR)  |= GPIO_PF10;
    REGD(GPIO_PF_DREG) |= GPIO_PF10;
}

/******************************************************************************/
/**
 *   Lcdc_Init Function
**/
/******************************************************************************/
void Lcdc_Init(void)
{
    REGD(LCDC_CTRL) = LCDC_MODE_ENABLE | LCDC_RESET | LCDC_DUMBDISABLE;
    REGD(LCDC_PFRR) = LCDC_PCFR;
    REGD(LCDC_TYPE) = LCDC_PANELTYPE | LCDC_CHIPSELECT | LCDC_PANELDATAWIDTH | LCDC_PANELCOLOR;
    REGD(LCDC_HDCFG) = LCDC_HDPS | LCDC_HT;
    REGD(LCDC_VDCFG) = LCDC_VDPS | LCDC_VT;
    REGD(LCDC_DISPCFG) = LCDC_BPP | LCDC_DMAC;
    REGD(LCDC_MWSA) = LCDC_MWSTARTADDR;
    REGD(LCDC_MWVW) = LCDC_MWVWIDTH;
    REGD(LCDC_MWSIZE) = LCDC_MWWIDTH | LCDC_MWHEIGHT;
    REGD(LCDC_CTRL) = LCDC_MODE_ENABLE;  /* bug TO1 - clear LCDC_CTL_DUMBDIS */
}

/******************************************************************************/
/**
 *   Show_BmpLogo Function
**/
/******************************************************************************/
void *Check_BmpLogo(uint32_t logo_addr)
{
    uint32_t       i, j;
    bmp_header_t   *bmp_header = (bmp_header_t *)(logo_addr + TAG_SIZE);
	ulong          len;
#if defined BPP_32
    /* 32 bpp */
    uint32_t       *pLogoPtr = (uint32_t *)(logo_addr + TAG_SIZE + bmp_header->data_offset);
    uint32_t       bitmap[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    uint8_t        *pARGB;
#elif defined BPP_16
    /* 16 bpp */
    uint16_t       *pLogoPtr = (uint16_t *)(logo_addr + TAG_SIZE + bmp_header->data_offset);
    uint16_t       bitmap[DISPLAY_HEIGHT][DISPLAY_WIDTH];
#endif

    if (memcmp((void *)logo_addr, BMP_TAG, strlen(BMP_TAG)) != 0)
    {
        printf("Device have not BMP Logo()\n");
        return NULL;
    }

    if (!((bmp_header->signature[0]=='B') && (bmp_header->signature[1]=='M')))
	{
	    if (bmp_header->compression != 0)   // 0 is None (BI_RGB)
        {
            /*
        	 * Decompress bmp image
        	 */
        	len = CFG_LOGO_MAX_SIZE;
        	if (gunzip((void *)LOGO_UNZIP_ADDR, CFG_LOGO_MAX_SIZE, (uchar *)logo_addr+TAG_SIZE, &len) != 0) {
        		printf("There is no valid bmp file at the given address\n");
        		return;
        	}
        	if (len == CFG_LOGO_MAX_SIZE) {
        		printf("Image could be truncated (increase CFG_LOGO_MAX_SIZE)!\n");
                return;
        	}
            bmp_header = (bmp_header_t *)LOGO_UNZIP_ADDR;
            pLogoPtr = (uint16_t *)(LOGO_UNZIP_ADDR + bmp_header->data_offset);
            printf("Anderson : Check_BmpLogo : pLogoPtr = %x\n", pLogoPtr);
            return (void *)pLogoPtr;
        }
    }

    return NULL;
}

/******************************************************************************/
/**
 *   Show_BmpLogo Function
**/
/******************************************************************************/
#if defined BPP_32
void Show_BmpLogo(uint32_t *pLogoPtr)
#elif defined BPP_16
void Show_BmpLogo(uint16_t *pLogoPtr)
#endif
{
    uint32_t       i, j;
	ulong          len;
#if defined BPP_32
    /* 32 bpp */
    uint32_t       bitmap[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    uint8_t        *pARGB;
#elif defined BPP_16
    /* 16 bpp */
    uint16_t       bitmap[DISPLAY_HEIGHT][DISPLAY_WIDTH];
#endif

    for (i = 0; i < DISPLAY_HEIGHT; i++)
    {
        for (j = 0; j < DISPLAY_WIDTH; j++)
        {
            bitmap[DISPLAY_HEIGHT-1-i][j] = pLogoPtr[(DISPLAY_WIDTH * i) + j];
#ifdef BPP_32
            /* 32 bpp */
            pARGB = (uint8_t *)&bitmap[DISPLAY_HEIGHT-1-i][j];
            // RGBA => ABGR
            bitmap[DISPLAY_HEIGHT-1-i][j] = *pARGB | (bitmap[DISPLAY_HEIGHT-1-i][j] >> 8);
#endif
        }
    }

    memcpy((void *)LCDC_MWSTARTADDR, bitmap, DISPLAY_WIDTH*DISPLAY_HEIGHT*PIXEL_BYTE);
}

/******************************************************************************/
/**
 *   Panel_SetReg Function
**/
/******************************************************************************/
void Panel_SetReg(int reg, int data)
{
 	int bitNum;

	reg |= 0x700000;
	data |= 0x720000;
	REGD(GPIO_PD_DREG) &= ~GPIO_PANEL_CS;
	for(bitNum=23;bitNum>=0;bitNum--)
	{
		if ((reg&(0x1<<bitNum)) != 0)
            REGD(GPIO_PD_DREG) |= GPIO_PANEL_MOSI;
        else
            REGD(GPIO_PD_DREG) &= ~GPIO_PANEL_MOSI;
        REGD(GPIO_PD_DREG) &= ~GPIO_PANEL_CLK;
        REGD(GPIO_PD_DREG) |= GPIO_PANEL_CLK;
	}
    REGD(GPIO_PD_DREG) |= GPIO_PANEL_CS;
    REGD(GPIO_PD_DREG) &= ~GPIO_PANEL_CS;
	for(bitNum=23;bitNum>=0;bitNum--)
	{
        if ((data&(0x1<<bitNum)) != 0)
            REGD(GPIO_PD_DREG) |= GPIO_PANEL_MOSI;
        else
            REGD(GPIO_PD_DREG) &= ~GPIO_PANEL_MOSI;
        REGD(GPIO_PD_DREG) &= ~GPIO_PANEL_CLK;
        REGD(GPIO_PD_DREG) |= GPIO_PANEL_CLK;
	}
    REGD(GPIO_PD_DREG) |= GPIO_PANEL_CS;
}

/******************************************************************************/
/**
 *   Panel_Init Function
**/
/******************************************************************************/
void Panel_Init(void)
{
    REGD(GPIO_PD_MODE) |= GPIO_PANEL_REST;
    REGD(GPIO_PD_MODE) |= GPIO_PANEL_CS;
    REGD(GPIO_PD_MODE) |= GPIO_PANEL_CLK;
    REGD(GPIO_PD_MODE) |= GPIO_PANEL_MOSI;
    REGD(GPIO_PD_DIR) |= GPIO_PANEL_REST;
    REGD(GPIO_PD_DIR) |= GPIO_PANEL_CS;
    REGD(GPIO_PD_DIR) |= GPIO_PANEL_CLK;
    REGD(GPIO_PD_DIR) |= GPIO_PANEL_MOSI;
    REGD(GPIO_PD_DREG) |= GPIO_PANEL_REST;
    REGD(GPIO_PD_DREG) |= GPIO_PANEL_CS;
    REGD(GPIO_PD_DREG) |= GPIO_PANEL_CLK;
    REGD(GPIO_PD_DREG) |= GPIO_PANEL_MOSI;

    Panel_SetReg(0x0001, 0x6300);
	Panel_SetReg(0x0004, 0x07c7);
	Panel_SetReg(0x0005, 0xbc84);
}

