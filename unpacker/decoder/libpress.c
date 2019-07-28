#include <SYS\TYPES.H>
#include <LIBETC.H>
#include <LIBPRESS.H>
#include "..\core.h"

extern void *DMACallback(int index, void (*callback)());

struct IQYCBR
{
	u_short type;
	u_char dummy;
	u_char size;
	u_char iq_y[64];
	u_char iq_c[64];
} iq_ycr=
{
	1,0,64,
	{
		 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26, 24, 26, 27,
		27, 27, 26, 26, 26, 26, 27, 27, 27, 29, 29, 29, 34, 34, 34, 29,
		29, 29, 27, 27, 29, 29, 32, 32, 34, 34, 37, 38, 37, 35, 35, 34,
		35, 38, 38, 40, 40, 40, 48, 48, 46, 46, 56, 56, 58, 69, 69, 83
	},
	{
		 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26, 24, 26, 27,
		27, 27, 26, 26, 26, 26, 27, 27, 27, 29, 29, 29, 34, 34, 34, 29,
		29, 29, 27, 27, 29, 29, 32, 32, 34, 34, 37, 38, 37, 35, 35, 34,
		35, 38, 38, 40, 40, 40, 48, 48, 46, 46, 56, 56, 58, 69, 69, 83
	}
};

struct DCT
{
	u_short type;
	u_char dummy;
	u_char size;
	short dct[64];
	u_long unk[2];
} dct=
{
	0,0,96,
	{
		23170, 23170, 23170, 23170, 23170, 23170, 23170, 23170,
		32138, 27245, 18204,  6392, -6393,-18205,-27246,-32139,
		30273, 12539,-12540,-30274,-30274,-12540, 12539, 30273,
		27245, -6393,-32139,-18205, 18204, 32138,  6392,-27246,
		23170,-23171,-23171, 23170, 23170,-23171,-23171, 23170,
		18204,-32139,  6392, 27245,-27246, -6393, 32138,-18205,
		12539,-30274, 30273,-12540,-12540, 30273,-30274, 12539,
		 6392,-18205, 27245,-32139, 32138,-27246, 18204, -6393,
	},
	{0xE7350,0x460000}
};

volatile u_long *MDEC_CMD=	(volatile u_long*)0x1F801820;
volatile u_long *MDEC_CTRL=	(volatile u_long*)0x1F801824;
volatile u_long *DPCR=		(volatile u_long*)0x1F8010F0;

volatile u_long *D0_MADR=	(volatile u_long*)0x1F801080;
volatile u_long *D0_BCR=	(volatile u_long*)0x1F801084;
volatile u_long *D0_CHCR=	(volatile u_long*)0x1F801088;
volatile u_long *D1_MADR=	(volatile u_long*)0x1F801090;
volatile u_long *D1_BCR=	(volatile u_long*)0x1F801094;
volatile u_long *D1_CHCR=	(volatile u_long*)0x1F801098;
volatile u_long *D2_MADR=	(volatile u_long*)0x1F8010A0;
volatile u_long *D2_BCR=	(volatile u_long*)0x1F8010A4;
volatile u_long *D2_CHCR=	(volatile u_long*)0x1F8010A8;
volatile u_long *D3_MADR=	(volatile u_long*)0x1F8010B0;
volatile u_long *D3_BCR=	(volatile u_long*)0x1F8010B4;
volatile u_long *D3_CHCR=	(volatile u_long*)0x1F8010B8;

static u_long timeout(
#ifdef DEBUG
	char *mes
#endif
	)
{
	u_long ret;

	//DEBUGPRINT(("%s timeout\n",mes));
	*MDEC_CTRL=0x80000000;
	*D0_CHCR=0;
	*D1_CHCR=0;

	ret=*D1_CHCR;
	*MDEC_CTRL=0x60000000;

	return ret;
}

static int wait_in()
{
	/*static*/ int loop;

	loop=0x100000;
	while(*MDEC_CTRL & 0x20000000)
	{
		loop--;
		if(loop==-1)
		{
#ifdef DEBUG
			timeout("MDEC_in_sync");
#else
			timeout();
#endif
			return -1;
		}
	}

	return 0;
}

static int wait_out()
{
	/*static*/ int loop;
	
	loop=0x100000;
	while(*D1_CHCR & 0x10000000)
	{
		loop--;
		if(loop==-1)
		{
#ifdef DEBUG
			timeout("MDEC_out_sync");
#else
			timeout();
#endif
			return -1;
		}
	}

	return 0;
}

static u_long get_mdec_ctrl()
{
	return *MDEC_CTRL;
}

static void dma_send(u_long *p, u_long size)
{
	wait_in();

	*DPCR|=0x88;
	*D0_MADR=(u_long)&p[1];
	*D0_BCR=((size >> 5) <<16) | 0x20;
	*MDEC_CMD=*p;
	*D0_CHCR =0x01000201;
}

static void dma_get(u_long *buf, u_long size)
{
	wait_out();

	*DPCR|=0x88;
	*D1_CHCR=0;
	*D1_MADR=(u_long)buf;
	*D1_BCR=((size >> 5) << 16) | 0x20;
	*D1_CHCR=0x1000200;
}

static u_long reset(int mode)
{
	u_long ret;

	switch(mode)
	{
	case 0:
		*MDEC_CTRL=0x80000000;
		*D0_CHCR=0;
		*D1_CHCR=0;
		*MDEC_CTRL=0x60000000;
		dma_send((u_long*)&iq_ycr,64/2);
		dma_send((u_long*)&dct,   64/2);
		break;
	case 1:
		*MDEC_CTRL=0x80000000;
		*D0_CHCR=0;
		*D1_CHCR=0;
		// wait
		ret=*D1_CHCR;
		*MDEC_CTRL=0x60000000;
		break;
	default:
		DEBUGPRINT(("MDEC_rest: bad option"));
		ret=0;
	}

	return ret;
}

void DecDCTReset(int mode)
{
	if(!mode) ResetCallback();
	reset(mode);
}

DECDCTENV *DecDCTGetEnv(DECDCTENV *env)
{
	zmemcpy(env->iq_c,iq_ycr.iq_c,64);
	zmemcpy(env->iq_y,iq_ycr.iq_y,64);
	zmemcpy(env->dct, dct.dct,    sizeof(env->dct));

	return env;
}

DECDCTENV *DecDCTPutEnv(DECDCTENV *env)
{
	zmemcpy(iq_ycr.iq_c,env->iq_c,64);
	zmemcpy(iq_ycr.iq_y,env->iq_y,64);

	dma_send((u_long*)&iq_ycr,64/2);
	dma_send((u_long*)&dct,   64/2);

	return env;
}

//int DecDCTBufSize(u_long *bs);
//int DecDCTvlc(u_long *bs, u_long *buf);
//int DecDCTvlc2(u_long *bs, u_long *buf, DECDCTTAB table);
//int DecDCTvlcSize(int size);
//int DecDCTvlcSize2(int size);
//void DecDCTvlcBuild(u_short *table);
void DecDCTin(u_long *buf, int mode)
{
	*buf= mode & 1 ? *buf & ~0x8000000 : *buf | 0x8000000;
	*buf= mode & 2 ? *buf & ~0x2000000 : *buf | 0x2000000;

	dma_send(buf,(u_short)*buf);
}

void DecDCTout(u_long *buf, int size)
{
	dma_get(buf,size);
}

int DecDCTinSync(int mode)
{
	if(!mode) return wait_in();
	return (get_mdec_ctrl()>>29) & 1;
}

int DecDCToutSync(int mode)
{
	if(!mode) return wait_out();
	return (*MDEC_CTRL >> 24) & 1;
}

int DecDCTinCallback(void (*func)())
{
	return (int)DMACallback(0,func);
}

int DecDCToutCallback(void (*func)())
{
	return (int)DMACallback(1,func);
}
