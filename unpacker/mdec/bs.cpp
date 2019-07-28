/*
	(c)2000 by BERO bero@geocities.co.jp

	under GPL
	
	some changes by dbalster@psxdev.de
	
	- all globals now in a context (to use it as shlib)
	- removed debugging printfs
*/
#include <stdafx.h>
#include <stdlib.h>
#include "bs.h"
#include "common.h"
#include "idctfst.h"

typedef struct {
/* bit i/o */
	unsigned int bitbuf;
	int bitcount,bs_size,totalbit;
	unsigned short *bsbuf;
/* huffman */
	int last_dc[3];
	int _type;
	int rlsize;
	const unsigned char *iqtab;
} bs_context_t;

/* static const char *copyright = N_("Copyright (C) 2000 by Daniel Balster <dbalster@psxdev.de>"); */

enum {B,G,R};
typedef int BLOCK;

#define	DCTSIZE2	64
#define	RGB2Y(r,g,b)	(  0.299*(r) + 0.587*(g) + 0.114*(b) )
#define	RGB2Cb(r,g,b)	( -0.16874*(r) - 0.33126*(g) +0.5*(b) )
#define	RGB2Cr(r,g,b)	(  0.5*(r) - 0.41869*(g) - 0.08131*(b) )

/*
16x16 RGB -> 8x8 Cb,Cr,Y0,Y1,Y2,Y3

[Y0][Y1] [Cb] [Cr]
[Y2][Y3]
*/
#define	Cb	0
#define	Cr	DCTSIZE2

static void rgb2yuv (unsigned char image[][3], BLOCK *blk)
{
	int x,y,i;
	int tmpblk[16*16][3],(*yuv)[3];
	BLOCK *yblk;

	yuv=tmpblk;
	for(i=0;i<16*16;i++) {
		yuv[0][0] = (int)RGB2Y (image[0][R],image[0][G],image[0][B])-128;
		yuv[0][1] = (int)RGB2Cb(image[0][R],image[0][G],image[0][B]);
		yuv[0][2] = (int)RGB2Cr(image[0][R],image[0][G],image[0][B]);
		yuv++; image++;
	}

	yuv = tmpblk;
	yblk = blk+DCTSIZE2*2;
	for(y=0;y<16;y+=2,blk+=4,yblk+=8,yuv+=8+16) {
		if (y==8) yblk+=DCTSIZE2;
		for(x=0;x<4;x++,blk++,yblk+=2,yuv+=2) {
			blk[Cb] = (yuv[0][1]+yuv[1][1]+yuv[16][1]+yuv[17][1])/4;
			blk[Cr] = (yuv[0][2]+yuv[1][2]+yuv[16][2]+yuv[17][2])/4;
			yblk[0] = yuv[ 0][0];
			yblk[1] = yuv[ 1][0];
			yblk[8] = yuv[16][0];
			yblk[9] = yuv[17][0];

			blk[4+Cb] = (yuv[8+0][1]+yuv[8+1][1]+yuv[8+16][1]+yuv[8+17][1])/4;
			blk[4+Cr] = (yuv[8+0][2]+yuv[8+1][2]+yuv[8+16][2]+yuv[8+17][2])/4;
			yblk[DCTSIZE2+0] = yuv[8+ 0][0];
			yblk[DCTSIZE2+1] = yuv[8+ 1][0];
			yblk[DCTSIZE2+8] = yuv[8+16][0];
			yblk[DCTSIZE2+9] = yuv[8+17][0];
		}
	}
}

#undef Cb
#undef Cr

/* bit i/o */
#define	BITBUFSIZE	16
#define	WriteWord(x)	ctxt->bsbuf[ctxt->bs_size++]=(x)

static void putbits_init (bs_context_t *ctxt)
{
	ctxt->bitbuf = 0;
	ctxt->bitcount = BITBUFSIZE;
	ctxt->bs_size = 0;
	ctxt->totalbit = 0;
}

static void putbits_flush (bs_context_t *ctxt)
{
	WriteWord(ctxt->bitbuf);
}

static void putbits (bs_context_t *ctxt, unsigned int x, unsigned int n)
{
	ctxt->totalbit+=n;

	if (n<(unsigned)ctxt->bitcount) {
		ctxt->bitcount-=n;
		ctxt->bitbuf |= x << ctxt->bitcount;
	} else {
		n-=ctxt->bitcount;
		WriteWord(ctxt->bitbuf | (x>>n) );
		if (n<BITBUFSIZE) {
			ctxt->bitcount = BITBUFSIZE-n;
		} else {
			WriteWord( x>>(n-BITBUFSIZE) );
			ctxt->bitcount = BITBUFSIZE*2-n;
		}
		ctxt->bitbuf = x << ctxt->bitcount;
	}
}

typedef struct {
	unsigned int code,nbits;
} huff_t;

const static huff_t dc_y_table[] = {
	{4,3},{0,2},{1,2},{5,3},{6,3},{14,4},{30,5},{62,6},{126,7},{254,8}
};

const static huff_t dc_c_table[] = {
	{0,2},{1,2},{2,2},{6,3},{14,4},{30,5},{62,6},{126,7},{254,8},{510,9}
};

#include "table.h"

static void encode_init (bs_context_t *ctxt, void *outbuf, int type, int q_scale)
{
	ctxt->_type = type;
	ctxt->last_dc[0] = 0;
	ctxt->last_dc[1] = 0;
	ctxt->last_dc[2] = 0;
	ctxt->rlsize = 0;
	putbits_init(ctxt);

	ctxt->bsbuf = (unsigned short*)outbuf;
	ctxt->bsbuf[1] = 0x3800;
	ctxt->bsbuf[2] = q_scale;
	ctxt->bsbuf[3] = type;
	ctxt->bs_size+=4;
}

static void encode_finish (bs_context_t *ctxt)
{
        putbits(ctxt, 32704, 16);
	putbits_flush(ctxt);
	ctxt->bsbuf[0] = (((ctxt->rlsize+1)/2)+31)&~31;
}

static void encode_dc (bs_context_t *ctxt, int n, int level)
{
	if (ctxt->_type==2) {
		putbits(ctxt,level&0x3ff,10);
	} else {
		const huff_t *table;
		int prev,cnt;

		level = level/4;
		if (n<2) {
			table = dc_c_table;
			prev = ctxt->last_dc[n];
			ctxt->last_dc[n] = level;
		} else {
			table = dc_y_table;
			prev = ctxt->last_dc[2];
			ctxt->last_dc[2] = level;
		}
		level -= prev;
		if (level==0) cnt=0;
		else {
			int alevel = level;
			if (alevel<0) alevel=-alevel;
			for(cnt=8;(alevel>>cnt)==0;cnt--);
			cnt++;
			if (level<0) level--;
		}
		putbits(ctxt,table[cnt].code,table[cnt].nbits);
		if (cnt) putbits(ctxt,level&((1<<cnt)-1),cnt);
	}
	ctxt->rlsize++;
}

static void encode_ac (bs_context_t *ctxt, int run, int level)
{
	int abslevel,sign;
	if (level>0) {
		abslevel = level;
		sign = 0;
	} else {
		abslevel = -level;
		sign = 1;
	}
	if (run<=31 && abslevel<=maxlevel[run]) {
		putbits(ctxt,huff_table[run][abslevel-1].code+sign,huff_table[run][abslevel-1].nbits);
	} else {
		/* ESCAPE */
		putbits(ctxt,1,6);
		putbits(ctxt,(run<<10)+(level&0x3ff),16);
	}
	ctxt->rlsize++;
}

static void encode_eob (bs_context_t *ctxt)
{
	putbits(ctxt, 2,2);
	ctxt->rlsize++;
}

extern void DCT(BLOCK *blk);

unsigned char zscan[DCTSIZE2] = {
	0 ,1 ,8 ,16,9 ,2 ,3 ,10,
	17,24,32,25,18,11,4 ,5 ,
	12,19,26,33,40,48,41,34,
	27,20,13,6 ,7 ,14,21,28,
	35,42,49,56,57,50,43,36,
	29,22,15,23,30,37,44,51,
	58,59,52,45,38,31,39,46,
	53,60,61,54,47,55,62,63
};

static unsigned char xxx_iqtab[DCTSIZE2] =
{
	 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26, 24, 26, 27,
	27, 27, 26, 26, 26, 26, 27, 27, 27, 29, 29, 29, 34, 34, 34, 29,
	29, 29, 27, 27, 29, 29, 32, 32, 34, 34, 37, 38, 37, 35, 35, 34,
	35, 38, 38, 40, 40, 40, 48, 48, 46, 46, 56, 56, 58, 69, 69, 83
};

const unsigned char *bs_iqtab (void) { return xxx_iqtab; }

static void blk2huff (bs_context_t *ctxt,BLOCK *blk,int q_scale)
{
	int i,k,run,level;
	for(i=0;i<6;i++)
	{
		DCT(blk);
		for(k=0;k<DCTSIZE2;k++) blk[k]>>=3;
		level = blk[0]/ctxt->iqtab[0];
		encode_dc(ctxt,i,level);
		run = 0;
		for(k=1;k<64;k++)
		{
			level = blk[zscan[k]]*8/(ctxt->iqtab[zscan[k]]*q_scale);
			if (level==0) run++;
			else
			{
				encode_ac(ctxt,run,level);
				run=0;
			}
		}
		encode_eob(ctxt);
		blk+=DCTSIZE2;
	}
}

Uint8 bs_roundtbl[256*3];

void bs_init (void)
{
	int i;
	for(i=0;i<256;i++) {
		bs_roundtbl [i]=0;
		bs_roundtbl [i+256]=i;
		bs_roundtbl [i+512]=255;
	}
}

int bs_encode (bs_header_t *outbuf,bs_input_image_t *img,int type,int q_scale,
        const unsigned char *myiqtab)
{
	unsigned char image[16][16][3];
	BLOCK blk[6][DCTSIZE2];
	bs_context_t *ctxt = (bs_context_t *)malloc(sizeof(bs_context_t));

	int x,y,xw,yw,rl;
	
	ctxt->iqtab = myiqtab ? myiqtab : bs_iqtab();

	encode_init(ctxt,outbuf,type,q_scale);

	for(x=0;x<img->width;x+=16)
	{
		xw = img->width-x; if (xw>16) xw = 16;
		for(y=0;y<img->height;y+=16)
		{
			unsigned char *p0 = img->top + x*(img->bit)/8 + y*img->nextline;
			int i,j=0;
			yw = img->height-y; if (yw>16) yw = 16;

			/* get 16x16 image */
			for(i=0;i<yw;i++)
			{
				unsigned char *p = p0;
				p0+=img->nextline;
				switch(img->bit)
				{
				case 16:
					for(j=0;j<xw;j++)
					{
						int c = *(unsigned short*)p;
						image[i][j][B] = ((c>>10)&31)*8;
						image[i][j][G] = ((c>>5)&31)*8;
						image[i][j][R] = ((c&31))*8;
						p+=2;
					}
					break;
				case 24:
					for(j=0;j<xw;j++)
					{
						image[i][j][R] = p[R];
						image[i][j][G] = p[G];
						image[i][j][B] = p[B];
						p+=3;
					}
					break;
				}
				for(;j<16;j++)
				{
					image[i][j][R] = image[i][xw-1][R];
					image[i][j][G] = image[i][xw-1][G];
					image[i][j][B] = image[i][xw-1][B];
				}
			}

			for(;i<16;i++)
			{
				for(j=0;j<16;j++)
				{
					image[i][j][R] = image[yw-1][j][R];
					image[i][j][G] = image[yw-1][j][G];
					image[i][j][B] = image[yw-1][j][B];
				}
			}

			rgb2yuv(image[0],blk[0]);
			blk2huff(ctxt,blk[0],q_scale);
		}
	}

	encode_finish(ctxt);
	
	rl = (ctxt->bs_size * 2);
	free (ctxt);
	
	return rl;
}
