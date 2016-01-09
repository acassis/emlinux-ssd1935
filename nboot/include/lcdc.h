
/******************************************************************************/
/**
 *   Include
**/
/******************************************************************************/


/******************************************************************************/
/**
 *   Define
**/
/******************************************************************************/
#define SSD1933_ABB1_BASE                       0x08000000
#define SSD1933_ABB2_BASE                       0x08100000
#define SSD1933_LCDC_BASE                       (SSD1933_ABB1_BASE + 0x00005000)
#define SSD1933_GPIO_BASE                       (SSD1933_ABB2_BASE + 0x0000F000)

#define GPIO_PD_MODE  			                (SSD1933_GPIO_BASE + 0x0318)
#define GPIO_PD_DIR  			                (SSD1933_GPIO_BASE + 0x0320)
#define GPIO_PD_DREG  			                (SSD1933_GPIO_BASE + 0x033C)
#define GPIO_PF_MODE  			                (SSD1933_GPIO_BASE + 0x0518)
#define GPIO_PF_DIR  			                (SSD1933_GPIO_BASE + 0x0520)
#define GPIO_PF_DREG  			                (SSD1933_GPIO_BASE + 0x053C)

#define LCDC_CTRL  			                    (SSD1933_LCDC_BASE + 0x0008)
#define LCDC_PFRR  			                    (SSD1933_LCDC_BASE + 0x0014)
#define LCDC_TYPE  			                    (SSD1933_LCDC_BASE + 0x0018)
#define LCDC_HDCFG  		                    (SSD1933_LCDC_BASE + 0x0020)
#define LCDC_VDCFG  		                    (SSD1933_LCDC_BASE + 0x0024)
#define LCDC_DISPCFG  		                    (SSD1933_LCDC_BASE + 0x0050)
#define LCDC_MWSA   		                    (SSD1933_LCDC_BASE + 0x005C)
#define LCDC_MWVW   		                    (SSD1933_LCDC_BASE + 0x0060)
#define LCDC_MWSIZE  		                    (SSD1933_LCDC_BASE + 0x0064)

#define GPIO_PD19           (1 << 19)
#define GPIO_PF10           (1 << 10)

#define GPIO_PANEL_REST		(1 << 23)
#define GPIO_PANEL_CS		(1 << 19)
#define GPIO_PANEL_CLK		(1 << 20)
#define GPIO_PANEL_MOSI		(1 << 21)

/* define 32 or 16 bpp */
#if 0
/* 32 bpp */
#define BPP_32
#else
/* 16 bpp */
#define BPP_16
#endif

#define DISPLAY_WIDTH       320
#define DISPLAY_HEIGHT      240
#define LCDC_DUMBDISABLE    (1<<2)
#define LCDC_RESET          (1<<1)
#define LCDC_MODE_ENABLE    (1<<0)
#define LCDC_PCFR           0x0000B723                  // Pixel Clock Frequency Ratio
#define LCDC_PANELTYPE      (1<<0)                      // Panel Type
#define LCDC_CHIPSELECT     (0<<3)                      // Chip Select ID = 0
#define LCDC_PANELDATAWIDTH (3<<4)                      // Panel Data Width = 24-bit
#define LCDC_PANELCOLOR     (1<<6)                      // Panel Color Select
#define LCDC_HDPS           (68<<0)                     // Horizontal Display Position
#define LCDC_HT             ((408-1)<<16)               // Horizontal Display Length
#define LCDC_VDPS           (18<<0)                     // Vertical Display Position
#define LCDC_VT             ((262-1)<<16)               // Vertical Display Length
#if defined BPP_32
/* 32 bpp */
#define LCDC_BPP            6                           // Bit Per Pixel
#elif defined BPP_16
/* 16 bpp */
#define LCDC_BPP            4                           // Bit Per Pixel
#endif
#define LCDC_DMAC           (6<<16)                     // DMA Configuration
#define LCDC_MWSTARTADDR    0x51FDA000                  // Main Window Display Start Address
#if defined BPP_32
/* 32 bpp */
#define LCDC_MWVWIDTH       (DISPLAY_WIDTH*4)           // Main Window Virtual Width
#elif defined BPP_16
/* 16 bpp */
#define LCDC_MWVWIDTH       (DISPLAY_WIDTH*2)           // Main Window Virtual Width
#endif
#define LCDC_MWWIDTH        (DISPLAY_WIDTH-1)           // Main Window Display Width
#define LCDC_MWHEIGHT       ((DISPLAY_HEIGHT-1)<<16)    // Main Window Display Height

#if defined BPP_32
/* 32 bpp */
#define PIXEL_BYTE          4
#elif defined BPP_16
/* 16 bpp */
#define PIXEL_BYTE          2
#endif

#define TAG_SIZE            8
#define BITMAP_OFFSET       (DISPLAY_WIDTH*DISPLAY_HEIGHT*PIXEL_BYTE + 70)

/******************************************************************************/
/**
 *   Exported Function
**/
/******************************************************************************/
void Backlight_Init(void);
void Lcdc_Init(void);
//void Show_BmpLogo(uint32_t logo_addr);
#if defined BPP_32
void Show_BmpLogo(uint32_t *pLogoPtr);
#elif defined BPP_16
void Show_BmpLogo(uint16_t *pLogoPtr);
#endif
void Panel_Init(void);

