extern volatile uint16_t DISPCNT;
extern volatile uint16_t DISPSTAT;
extern volatile uint16_t VCOUNT;
extern volatile uint16_t BG0CNT;
extern volatile uint16_t BG1CNT;
extern volatile uint16_t BG2CNT;
extern volatile uint16_t BG3CNT;
extern volatile uint16_t BG0HOFS;
extern volatile uint16_t BG0VOFS;
extern volatile uint16_t BG1HOFS;
extern volatile uint16_t BG1VOFS;
extern volatile uint16_t BG2HOFS;
extern volatile uint16_t BG2VOFS;
extern volatile uint16_t BG3HOFS;
extern volatile uint16_t BG3VOFS;

extern volatile uint16_t BG2PA;
extern volatile uint16_t BG2PB;
extern volatile uint16_t BG2PC;
extern volatile uint16_t BG2PD;
extern volatile uint32_t BG2X;
extern volatile uint16_t BG2X_L;
extern volatile uint16_t BG2X_H;
extern volatile uint32_t BG2Y;
extern volatile uint16_t BG2Y_L;
extern volatile uint16_t BG2Y_H;

extern volatile uint32_t BG3PA;
extern volatile uint32_t BG3PB;
extern volatile uint32_t BG3PC;
extern volatile uint32_t BG3PD;
extern volatile uint32_t BG3X;
extern volatile uint16_t BG3X_L;
extern volatile uint16_t BG3X_H;
extern volatile uint32_t BG3Y;
extern volatile uint16_t BG3Y_L;
extern volatile uint16_t BG3Y_H;

extern volatile uint16_t WIN0H;
extern volatile uint16_t WIN1H;
extern volatile uint16_t WIN0V;
extern volatile uint16_t WIN1V;
extern volatile uint16_t WININ;
extern volatile uint16_t WINOUT;
extern volatile uint32_t MOSAIC;

extern volatile uint16_t BLDCNT;
extern volatile uint16_t BLDALPHA;
extern volatile uint32_t BLDY;

extern volatile uint32_t SOUNDCNT;
extern volatile uint16_t SOUNDCNT_L;
extern volatile uint16_t SOUNDCNT_H;
extern volatile uint32_t SOUNDCNT_X;

extern volatile uint32_t FIFO_A;
extern volatile uint16_t FIFO_A_L;
extern volatile uint16_t FIFO_A_H;
extern volatile uint32_t FIFO_B;
extern volatile uint16_t FIFO_B_L;
extern volatile uint16_t FIFO_B_H;

extern volatile void *DMA0SAD;
extern volatile void *DMA0DAD;
extern volatile uint16_t DMA0CNT_L;
extern volatile uint16_t DMA0CNT_H;
extern volatile void *DMA1SAD;
extern volatile void *DMA1DAD;
extern volatile uint16_t DMA1CNT_L;
extern volatile uint16_t DMA1CNT_H;
extern volatile void *DMA2SAD;
extern volatile void *DMA2DAD;
extern volatile uint16_t DMA2CNT_L;
extern volatile uint16_t DMA2CNT_H;
extern volatile void *DMA3SAD;
extern volatile void *DMA3DAD;
extern volatile uint16_t DMA3CNT_L;
extern volatile uint16_t DMA3CNT_H;

extern volatile uint16_t TM0CNT_L;
extern volatile uint16_t TM0CNT_H;
extern volatile uint16_t TM1CNT_L;
extern volatile uint16_t TM1CNT_H;
extern volatile uint16_t TM2CNT_L;
extern volatile uint16_t TM2CNT_H;
extern volatile uint16_t TM3CNT_L;
extern volatile uint16_t TM3CNT_H;

extern volatile uint16_t KEYINPUT;
extern volatile uint16_t KEYCNT;

#define KEY_A 0x0001
#define KEY_B 0x0002
#define KEY_SELECT 0x0004
#define KEY_START 0x0008
#define KEY_RIGHT 0x0010
#define KEY_LEFT 0x0020
#define KEY_UP 0x0040
#define KEY_DOWN 0x0080
#define KEY_R 0x0100
#define KEY_L 0x0200

extern volatile uint16_t IE;
extern volatile uint16_t IF;
extern volatile uint16_t WAITCNT;
extern volatile uint32_t IME;
extern void (*volatile ISR_funct)(void);
extern volatile uint32_t ISR_flags;

extern uint16_t VPAL0[];
extern uint16_t VPAL1[];
extern uint16_t VRAM0D[];
extern uint16_t VRAM1D[];
extern uint32_t VRAM0D32[];
extern uint32_t VRAM1D32[];
extern uint64_t VRAM0D64[];
extern uint64_t VRAM1D64[];
extern uint16_t VRAMTB[];
extern uint16_t VRAMTO[];
extern uint16_t VRAMHO[];
extern uint32_t VRAMTB32[];
extern uint32_t VRAMTO32[];
extern uint32_t VRAMHO32[];
extern uint64_t VRAMTB64[];
extern uint64_t VRAMTO64[];
extern uint64_t VRAMHO64[];
extern uint16_t VOAM16[];

