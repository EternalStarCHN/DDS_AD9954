#ifndef __DDS9954_H__
#define __DDS9954_H__
#include "stdint.h"


#define PS0 PEout(0)
#define PS1 PEout(1)
#define IOUPDATE PEout(5)
#define AD9954_RES PEout(6)
#define AD9954_CS PCout(0)
#define AD9954_SCLK PCout(1)
#define AD9954_SDIO PCout(2)
//OSK 不用时接地

#define CFR1 0x00
#define CFR2 0x01
#define ASF 0x02
//#define ARR 0x03
#define FTW0 0x04
#define POW0 0x05
#define FTW1 0x06
#define NLSCW 0x07
#define PLSCW 0x08
#define RSCW0 0x07
#define RSCW1 0x08
#define RSCW2 0x09
#define RSCW3 0x0A
#define RAM 0x0B
#define Dou14 0x74
#define Dou16 0x84
#define Dou17 0x8C
#define Dou18 0x94
#define Dou19 0x9C
#define Dou20 0xA4

void delay_us(unsigned int m);
void GPIO_AD9954_Init(void);
void AD9954_RESET(void);
void AD9954_SENDBYTE(uint8_t dat);
void UPDATE(void);
void AD9954_Init(void);
void AD9954_SETFRE(uint32_t f);
void LINEARSWEEP(uint32_t f1,uint32_t f2);
void DirectSwitchFSK(uint32_t f1,uint32_t f2,uint32_t f3,uint32_t f4);
void DirectSwitchPSK(uint32_t  f,float phase1,float phase2,float phase3,float phase4);
void Generate_PSK(uint32_t  f,uint16_t *phase);
void Generate_FM(uint32_t *fre);
void ASK_Init(void);
void AM_Init(void);
void Write_ASF(uint16_t factor);

#endif

