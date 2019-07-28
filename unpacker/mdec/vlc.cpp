#include <stdafx.h>

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "bs.h"
#include "idctfst.h"

typedef struct tagRgbPic
{
	Uint8 b, g, r;
} RGB_pic;

#define	SOFT

#define	CODE1(a,b,c)	(((a)<<10)|((b)&0x3ff)|((c)<<16))
/* run, level, bit */
#define	CODE(a,b,c)		CODE1(a,b,c+1),CODE1(a,-b,c+1)
#define	CODE0(a,b,c)	CODE1(a,b,c),CODE1(a,b,c)
#define	CODE2(a,b,c)	CODE1(a,b,c+1),CODE1(a,b,c+1)
#define	RUNOF(a)	((a)>>10)
#define	VALOF(a)	((short)((a)<<6)>>6)
#define	BITOF(a)	((a)>>16)
#define	EOB	0xfe00
#define	ESCAPE_CODE	CODE1(63,0,6)
#define	EOB_CODE	CODE1(63,512,2)

#undef MULR
#undef MULB
#undef MULG2

#define MAKERGB15(r,g,b)		(r)|((g)<<5)|((b)<<10)
#define	MULR(a)					((1434 * (a))) 
#define	MULB(a)					((1807 * (a))) 
#define	MULG2(a, b)				((-351 * (a) - 728 * (b)))
#define MULY(a)					((a) << 10)
#define MULY(a)					((a) << 10)

#define SCALE(x, n)				((x) >> (n))
#define SCALER(x, n)			(((x) + ((1 << (n)) >> 1)) >> (n))
#define SCALE5(c)				SCALER(c, 23)
#define CLAMP5(c)				( ((c) < -16) ? 0 : (((c) > (31 - 16)) ? 31 : ((c) + 16)) )
#define CLAMP_SCALE5(a)			(CLAMP5(SCALE5(a)))

#define AAN_CONST_BITS			12
#define AAN_PRESCALE_BITS		16

#define AAN_CONST_SIZE			24
#define AAN_CONST_SCALE			(AAN_CONST_SIZE - AAN_CONST_BITS)

#define AAN_PRESCALE_SIZE		20
#define AAN_PRESCALE_SCALE		(AAN_PRESCALE_SIZE-AAN_PRESCALE_BITS)
#define AAN_EXTRA				12

#define FIX_1_082392200			SCALER(18159528, AAN_CONST_SCALE) // B6
#define FIX_1_414213562			SCALER(23726566, AAN_CONST_SCALE) // A4
#define FIX_1_847759065			SCALER(31000253, AAN_CONST_SCALE) // A2
#define FIX_2_613125930			SCALER(43840978, AAN_CONST_SCALE) // B2

#define MULS(var, const)		(SCALE((var) * (const), AAN_CONST_BITS))

#define	RLE_RUN(a)				((a) >> 10)
#define	RLE_VAL(a)				(((int)(a) << (sizeof(int) * 8 - 10)) >> (sizeof(int) * 8 - 10))

/*
	DC code
	Y			U,V
0	100			00	 		0
1	00x			01x			-1,1
2	01xx		10xx		-3,-2,2,3
3	101xxx		110xxx		-7..-4,4..7
4	110xxxx		1110		-15..-8,8..15
5	1110xxxxx	11110		-31..-16,16..31
6	11110xxxxxx	111110		-63..-32,32..63
7	111110		1111110		-127..-64,64..127
8	1111110		11111110	-255..-128,128..255
	7+8			8+8
*/

/*
	This table based on MPEG2DEC by MPEG Software Simulation Group
*/

/* Table B-14, DCT coefficients	table zero,
* codes	0100 ... 1xxx (used	for	all	other coefficients)
*/
static const Uint32 VLCtabnext[12*2] =	{
	CODE(0,2,4),  CODE(2,1,4),	CODE2(1,1,3),  CODE2(1,-1,3),
	CODE0(63,512,2), CODE0(63,512,2), CODE0(63,512,2), CODE0(63,512,2),	/*EOB*/
	CODE2(0,1,2),  CODE2(0,1,2),	CODE2(0,-1,2),  CODE2(0,-1,2)
};

/* Table B-14, DCT coefficients	table zero,
* codes	000001xx ... 00111xxx
*/
static const Uint32 VLCtab0[60*2] = {
	CODE0(63,0,6), CODE0(63,0,6),CODE0(63,0,6), CODE0(63,0,6), /* ESCAPE */
	CODE2(2,2,7), CODE2(2,-2,7), CODE2(9,1,7), CODE2(9,-1,7),
	CODE2(0,4,7), CODE2(0,-4,7), CODE2(8,1,7), CODE2(8,-1,7),
	CODE2(7,1,6), CODE2(7,1,6), CODE2(7,-1,6), CODE2(7,-1,6),
	CODE2(6,1,6), CODE2(6,1,6), CODE2(6,-1,6), CODE2(6,-1,6),
	CODE2(1,2,6), CODE2(1,2,6), CODE2(1,-2,6), CODE2(1,-2,6),
	CODE2(5,1,6), CODE2(5,1,6), CODE2(5,-1,6), CODE2(5,-1,6),
	CODE(13,1,8), CODE(0,6,8), CODE(12,1,8), CODE(11,1,8),
	CODE(3,2,8), CODE(1,3,8), CODE(0,5,8), CODE(10,1,8),
	CODE2(0,3,5), CODE2(0,3,5), CODE2(0,3,5), CODE2(0,3,5),
	CODE2(0,-3,5), CODE2(0,-3,5), CODE2(0,-3,5), CODE2(0,-3,5),
	CODE2(4,1,5), CODE2(4,1,5), CODE2(4,1,5), CODE2(4,1,5),
	CODE2(4,-1,5), CODE2(4,-1,5), CODE2(4,-1,5), CODE2(4,-1,5),
	CODE2(3,1,5), CODE2(3,1,5), CODE2(3,1,5), CODE2(3,1,5),
	CODE2(3,-1,5), CODE2(3,-1,5), CODE2(3,-1,5), CODE2(3,-1,5)
};

/* Table B-14, DCT coefficients	table zero,
* codes	0000001000 ... 0000001111
*/
static const Uint32 VLCtab1[8*2] =	{
	CODE(16,1,10), CODE(5,2,10), CODE(0,7,10), CODE(2,3,10),
	CODE(1,4,10), CODE(15,1,10), CODE(14,1,10),	CODE(4,2,10)
};

/* Table B-14/15, DCT coefficients table zero /	one,
* codes	000000010000 ... 000000011111
*/
static const Uint32 VLCtab2[16*2] = {
	CODE(0,11,12), CODE(8,2,12), CODE(4,3,12), CODE(0,10,12),
	CODE(2,4,12), CODE(7,2,12),	CODE(21,1,12), CODE(20,1,12),
	CODE(0,9,12), CODE(19,1,12), CODE(18,1,12),	CODE(1,5,12),
	CODE(3,3,12), CODE(0,8,12),	CODE(6,2,12), CODE(17,1,12)
};

/* Table B-14/15, DCT coefficients table zero /	one,
* codes	0000000010000 ... 0000000011111
*/
static const Uint32 VLCtab3[16*2] = {
	CODE(10,2,13), CODE(9,2,13), CODE(5,3,13), CODE(3,4,13),
	CODE(2,5,13), CODE(1,7,13),	CODE(1,6,13), CODE(0,15,13),
	CODE(0,14,13), CODE(0,13,13), CODE(0,12,13), CODE(26,1,13),
	CODE(25,1,13), CODE(24,1,13), CODE(23,1,13), CODE(22,1,13)
};

/* Table B-14/15, DCT coefficients table zero /	one,
* codes	00000000010000 ... 00000000011111
*/
static const Uint32 VLCtab4[16*2] = {
	CODE(0,31,14), CODE(0,30,14), CODE(0,29,14), CODE(0,28,14),
	CODE(0,27,14), CODE(0,26,14), CODE(0,25,14), CODE(0,24,14),
	CODE(0,23,14), CODE(0,22,14), CODE(0,21,14), CODE(0,20,14),
	CODE(0,19,14), CODE(0,18,14), CODE(0,17,14), CODE(0,16,14)
};

/* Table B-14/15, DCT coefficients table zero /	one,
* codes	000000000010000	...	000000000011111
*/
static const Uint32 VLCtab5[16*2] = {
	CODE(0,40,15), CODE(0,39,15), CODE(0,38,15), CODE(0,37,15),
	CODE(0,36,15), CODE(0,35,15), CODE(0,34,15), CODE(0,33,15),
	CODE(0,32,15), CODE(1,14,15), CODE(1,13,15), CODE(1,12,15),
	CODE(1,11,15), CODE(1,10,15), CODE(1,9,15),	CODE(1,8,15)
};

/* Table B-14/15, DCT coefficients table zero /	one,
* codes	0000000000010000 ... 0000000000011111
*/
static const Uint32 VLCtab6[16*2] = {
	CODE(1,18,16), CODE(1,17,16), CODE(1,16,16), CODE(1,15,16),
	CODE(6,3,16), CODE(16,2,16), CODE(15,2,16),	CODE(14,2,16),
	CODE(13,2,16), CODE(12,2,16), CODE(11,2,16), CODE(31,1,16),
	CODE(30,1,16), CODE(29,1,16), CODE(28,1,16), CODE(27,1,16)
};

/*
	DC code
	Y				U,V
0	100				00	 				0
1	00x				01x					-1,1
2	01xx			10xx				-3,-2,2,3
3	101xxx			110xxx				-7..-4,4..7
4	110xxxx			1110xxxx			-15..-8,8..15
5	1110xxxxx		11110xxxxx			-31..-16,16..31
6	11110xxxxxx		111110xxxxxx		-63..-32,32..63
7	111110xxxxxxx	1111110xxxxxxx		-127..-64,64..127
8	1111110xxxxxxxx	11111110xxxxxxxx	-255..-128,128..255
*/

static const Uint32 DC_Ytab0[48] = {
	CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),
	CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),
	CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),
	CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),

	CODE1(0,-3,4),CODE1(0,-3,4),CODE1(0,-3,4),CODE1(0,-3,4),
	CODE1(0,-2,4),CODE1(0,-2,4),CODE1(0,-2,4),CODE1(0,-2,4),
	CODE1(0,2,4),CODE1(0,2,4),CODE1(0,2,4),CODE1(0,2,4),
	CODE1(0,3,4),CODE1(0,3,4),CODE1(0,3,4),CODE1(0,3,4),

	CODE1(0,0,3),CODE1(0,0,3),CODE1(0,0,3),CODE1(0,0,3),
	CODE1(0,0,3),CODE1(0,0,3),CODE1(0,0,3),CODE1(0,0,3),
	CODE1(0,-7,6),CODE1(0,-6,6),CODE1(0,-5,6),CODE1(0,-4,6),
	CODE1(0,4,6),CODE1(0,5,6),CODE1(0,6,6),CODE1(0,7,6),

};

static const Uint32 DC_UVtab0[56] = {
	CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),
	CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),
	CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),
	CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),CODE1(0,0,2),

	CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),
	CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),CODE1(0,-1,3),
	CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),
	CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),CODE1(0,1,3),

	CODE1(0,-3,4),CODE1(0,-3,4),CODE1(0,-3,4),CODE1(0,-3,4),
	CODE1(0,-2,4),CODE1(0,-2,4),CODE1(0,-2,4),CODE1(0,-2,4),
	CODE1(0,2,4),CODE1(0,2,4),CODE1(0,2,4),CODE1(0,2,4),
	CODE1(0,3,4),CODE1(0,3,4),CODE1(0,3,4),CODE1(0,3,4),

	CODE1(0,-7,6),CODE1(0,-6,6),CODE1(0,-5,6),CODE1(0,-4,6),
	CODE1(0,4,6),CODE1(0,5,6),CODE1(0,6,6),CODE1(0,7,6),
};

#define	DCTSIZE2	64

/* decode one intra	coded MPEG-1 block */

#define	Show_Bits(N)	(bitbuf>>(32-(N)))
/* ç≈è¨óLå¯bit 17 bit*/

#define	Flush_Buffer(N) {bitbuf <<=(N);incnt +=(N);while(incnt>=0) {bitbuf |= Get_Word()<<incnt;incnt-=16;}}

#define	Init_Buffer() {bitbuf = (mdec_bs[0]<<16)|(mdec_bs[1]);mdec_bs+=2;incnt = -16;}

#define	Get_Word()	(*mdec_bs++)
#define	Printf	printf


int DecDCTvlc(Uint16 *mdec_bs,Uint16 *mdec_rl)
{
/*	Uint16 *mdec_bs = mdecbs,*mdec_rl = mdecrl */
	Uint16 *rl_end;
	Uint32 bitbuf;
	int incnt; /* 16-óLå¯bitêî x86=char risc = long */
	int q_code;
	int type,n;
	int last_dc[3];

/* BS_HDR  Uint16 rlsize,magic,ver,q_scale */

	/* printf("%04x,%04x,",mdec_bs[0],mdec_bs[1]); */
	*(long*)mdec_rl=*(long*)mdec_bs;
	mdec_rl+=2;
	rl_end = mdec_rl+(int)mdec_bs[0]*2;
	q_code = (mdec_bs[2]<<10); /* code = q */
	type = mdec_bs[3];
	mdec_bs+=4;

	Init_Buffer();

	n = 0;
	last_dc[0]=last_dc[1]=last_dc[2] = 0;
	while(mdec_rl<rl_end) {
	  Uint32 code2;
		/* DC */
	  if (type==2) {
		code2 = Show_Bits(10)|(10<<16); /* DC code */
	  } else {
		code2 = Show_Bits(6);
		if (n>=2) {
			/* Y */
			if (code2<48) {
				code2 = DC_Ytab0[code2];
				code2 = (code2&0xffff0000)|((last_dc[2]+=VALOF(code2)*4)&0x3ff);
			} else {
				int nbit,val;
				int bit = 3;
				while(Show_Bits(bit)&1) { bit++;}
				bit++;
				nbit = bit*2-1;
				val = Show_Bits(nbit)&((1<<bit)-1);
				if ((val&(1<<(bit-1)))==0)
					val -= (1<<bit)-1;
				val = (last_dc[2]+=val*4);
				code2 = (nbit<<16) | (val&0x3ff);
			}
			/* printf("%d ",last_dc[2]); */
		} else {
			/* U,V */
			if (code2<56) {
				code2 = DC_UVtab0[code2];
				code2 = (code2&0xffff0000)|((last_dc[n]+=VALOF(code2)*4)&0x3ff);
			} else {
				int nbit,val;
				int bit = 4;
				while(Show_Bits(bit)&1) { bit++;}
				nbit = bit*2;
				val = Show_Bits(nbit)&((1<<bit)-1);
				if ((val&(1<<(bit-1)))==0)
					val -= (1<<bit)-1;
				val = (last_dc[n]+=val*4);
				code2 = (nbit<<16) | (val&0x3ff);
			}
			/* printf("%d ",last_dc[n]); */
		}
		if (++n==6) n=0;
	  }
	/*	printf("%d ",VALOF(code2)); */
	  code2 |= q_code;

		/* AC */
	  while(mdec_rl<rl_end){
/*		Uint32 code; */
#define	code code2
#define	SBIT	17
		*mdec_rl++=(Uint16)code2;
		Flush_Buffer(BITOF(code2));
		code = Show_Bits(SBIT);
		if      (code>=1<<(SBIT- 2)) {
			code2 = VLCtabnext[(code>>12)-8];
			if (code2==EOB_CODE) break;
		}
		else if (code>=1<<(SBIT- 6)) {
			code2 = VLCtab0[(code>>8)-8];
			if (code2==ESCAPE_CODE) {
				Flush_Buffer(6); /* ESCAPE len */
				code2 = Show_Bits(16)| (16<<16);
			}
		}
		else if (code>=1<<(SBIT- 7)) code2 = VLCtab1[(code>>6)-16];
		else if (code>=1<<(SBIT- 8)) code2 = VLCtab2[(code>>4)-32];
		else if (code>=1<<(SBIT- 9)) code2 = VLCtab3[(code>>3)-32];
		else if (code>=1<<(SBIT-10)) code2 = VLCtab4[(code>>2)-32];
		else if (code>=1<<(SBIT-11)) code2 = VLCtab5[(code>>1)-32];
		else if (code>=1<<(SBIT-12)) code2 = VLCtab6[(code>>0)-32];
		else {
			do {
				*mdec_rl++=EOB;
			} while(mdec_rl<rl_end);
			return 0;
		}
	  }
	  *mdec_rl++=(Uint16)code2; /* EOB code */
	  Flush_Buffer(2); /* EOB bitlen */
	}
	return 0;
}



/* this table is based on djpeg by Independent Jpeg Group */

static const int aanscales[DCTSIZE2] =
{
	1048576, 1454417, 1370031, 1232995, 1048576,  823861, 567485, 289301,
	1454417, 2017334, 1900287, 1710213, 1454417, 1142728, 787125, 401273,
	1370031, 1900287, 1790031, 1610986, 1370031, 1076426, 741455, 377991,
	1232995, 1710213, 1610986, 1449849, 1232995,  968758, 667292, 340183,
	1048576, 1454417, 1370031, 1232995, 1048576,  823861, 567485, 289301,
	823861,  1142728, 1076426, 968758,  823861,  647303, 445870, 227303,
	567485,  787125,  741455,  667292,  567485,  445870, 307121, 156569,
	289301,  401273,  377991,  340183,  289301,  227303, 156569,  79818
};

extern unsigned char zscan[DCTSIZE2];

typedef struct {
	int iqtab[DCTSIZE2 * DCTSIZE2];
	const unsigned char *iq_y;
	Uint16 *mdec_rl,*rl_end;
	int mdec_mode;
} bs_context_t;

void iqtab_init(bs_context_t *ctxt)
{
#define CONST_BITS 14
#define	IFAST_SCALE_BITS 2
	int i;
	for (i = 0; i < DCTSIZE2; i++) {
		ctxt->iqtab[i] = ctxt->iq_y[i] * aanscales[i] >> (CONST_BITS - IFAST_SCALE_BITS);
	}
}

extern void IDCT(BLOCK *blk,int k);

#define DSIZE			8
#define DSIZE2			(DSIZE * DSIZE)

static inline void fillcol(int *blk, int val) {
	blk[0 * DSIZE] = blk[1 * DSIZE] = blk[2 * DSIZE] = blk[3 * DSIZE]
		= blk[4 * DSIZE] = blk[5 * DSIZE] = blk[6 * DSIZE] = blk[7 * DSIZE] = val;
}

static inline void fillrow(int *blk, int val) {
	blk[0] = blk[1] = blk[2] = blk[3]
		= blk[4] = blk[5] = blk[6] = blk[7] = val;
}

void idct(int *block, int used_col)
{
	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	int z5, z10, z11, z12, z13;
	int *ptr;
	int i;

	// the block has only the DC coefficient
	if (used_col == -1) {
		int v = block[0];
		for (i = 0; i < DSIZE2; i++) block[i] = v;
		return;
	}

	// last_col keeps track of the highest column with non zero coefficients
	ptr = block;
	for (i = 0; i < DSIZE; i++, ptr++) {
		if ((used_col & (1 << i)) == 0) {
			// the column is empty or has only the DC coefficient
			if (ptr[DSIZE * 0]) {
				fillcol(ptr, ptr[0]);
				used_col |= (1 << i);
			}
			continue;
		}

		// further optimization could be made by keeping track of 
		// last_row in rl2blk
		z10 = ptr[DSIZE * 0] + ptr[DSIZE * 4]; // s04
		z11 = ptr[DSIZE * 0] - ptr[DSIZE * 4]; // d04
		z13 = ptr[DSIZE * 2] + ptr[DSIZE * 6]; // s26
		z12 = MULS(ptr[DSIZE * 2] - ptr[DSIZE * 6], FIX_1_414213562) - z13;
		//^^^^  d26=d26*2*A4-s26

		tmp0 = z10 + z13; // os07 = s04 + s26
		tmp3 = z10 - z13; // os34 = s04 - s26
		tmp1 = z11 + z12; // os16 = d04 + d26
		tmp2 = z11 - z12; // os25 = d04 - d26

		z13 = ptr[DSIZE * 3] + ptr[DSIZE * 5]; //s53
		z10 = ptr[DSIZE * 3] - ptr[DSIZE * 5]; //-d53 
		z11 = ptr[DSIZE * 1] + ptr[DSIZE * 7]; //s17
		z12 = ptr[DSIZE * 1] - ptr[DSIZE * 7]; //d17

		tmp7 = z11 + z13; // od07 = s17 + s53

		z5 = (z12 - z10) * (FIX_1_847759065);
		tmp6 = SCALE(z10*(FIX_2_613125930)+z5, AAN_CONST_BITS) - tmp7;
		tmp5 = MULS(z11 - z13, FIX_1_414213562) - tmp6;
		tmp4 = SCALE(z12*(FIX_1_082392200)-z5, AAN_CONST_BITS) + tmp5;

		// path #1
		//z5 = (z12 - z10)* FIX_1_847759065; 
		// tmp0 = (d17 + d53) * 2*A2

		//tmp6 = DESCALE(z10*FIX_2_613125930 + z5, CONST_BITS) - tmp7; 
		// od16 = (d53*-2*B2 + tmp0) - od07

		//tmp4 = DESCALE(z12*FIX_1_082392200 - z5, CONST_BITS) + tmp5; 
		// od34 = (d17*2*B6 - tmp0) + od25

		// path #2

		// od34 = d17*2*(B6-A2) - d53*2*A2
		// od16 = d53*2*(A2-B2) + d17*2*A2

		// end

		//    tmp5 = MULS(z11 - z13, FIX_1_414213562) - tmp6;
		// od25 = (s17 - s53)*2*A4 - od16

		ptr[DSIZE * 0] = (tmp0 + tmp7); // os07 + od07
		ptr[DSIZE * 7] = (tmp0 - tmp7); // os07 - od07
		ptr[DSIZE * 1] = (tmp1 + tmp6); // os16 + od16
		ptr[DSIZE * 6] = (tmp1 - tmp6); // os16 - od16
		ptr[DSIZE * 2] = (tmp2 + tmp5); // os25 + od25
		ptr[DSIZE * 5] = (tmp2 - tmp5); // os25 - od25
		ptr[DSIZE * 4] = (tmp3 + tmp4); // os34 + od34
		ptr[DSIZE * 3] = (tmp3 - tmp4); // os34 - od34
	}

	ptr = block;
	if (used_col == 1) {
		for (i = 0; i < DSIZE; i++)
			fillrow(block + DSIZE * i, block[DSIZE * i]);
	}
	else {
		for (i = 0; i < DSIZE; i++, ptr += DSIZE) {
			z10 = ptr[0] + ptr[4];
			z11 = ptr[0] - ptr[4];
			z13 = ptr[2] + ptr[6];
			z12 = MULS(ptr[2] - ptr[6], FIX_1_414213562) - z13;

			tmp0 = z10 + z13;
			tmp3 = z10 - z13;
			tmp1 = z11 + z12;
			tmp2 = z11 - z12;

			z13 = ptr[3] + ptr[5];
			z10 = ptr[3] - ptr[5];
			z11 = ptr[1] + ptr[7];
			z12 = ptr[1] - ptr[7];

			tmp7 = z11 + z13;
			z5 = (z12 - z10) * FIX_1_847759065;
			tmp6 = SCALE(z10 * FIX_2_613125930 + z5, AAN_CONST_BITS) - tmp7;
			tmp5 = MULS(z11 - z13, FIX_1_414213562) - tmp6;
			tmp4 = SCALE(z12 * FIX_1_082392200 - z5, AAN_CONST_BITS) + tmp5;

			ptr[0] = tmp0 + tmp7;

			ptr[7] = tmp0 - tmp7;
			ptr[1] = tmp1 + tmp6;
			ptr[6] = tmp1 - tmp6;
			ptr[2] = tmp2 + tmp5;
			ptr[5] = tmp2 - tmp5;
			ptr[4] = tmp3 + tmp4;
			ptr[3] = tmp3 - tmp4;
		}
	}
}

unsigned short get_rldata(unsigned short *&rldata, unsigned short *rlend)
{
	if(rldata < rlend)
		return *rldata++;

	return 0xfe00;
}

Uint16* rl2blk(bs_context_t *ctxt, BLOCK *blk,Uint16 *mdec_rl)
{
#if 0
	int i,k,q_scale,rl;
	memset(blk,0,6*DCTSIZE2*sizeof(BLOCK));
	for(i=0;i<6;i++) {
		rl = *mdec_rl++;
		q_scale = RUNOF(rl);
		blk[0] = ctxt->iqtab[0]*VALOF(rl);
		k = 0;
		for(;;) {
			rl = *mdec_rl++;
			if (rl==EOB) break;
			k += RUNOF(rl)+1;
			blk[zscan[k]] = ctxt->iqtab[zscan[k]]*q_scale*VALOF(rl)/8;
		}

		IDCT(blk,k+1);
		
		blk+=DCTSIZE2;
	}
	return mdec_rl;
#else
	int i, k, q_scale, rl, used_col;
	int *iqtab;

	memset(blk, 0, 6 * DSIZE2 * sizeof(int));
	iqtab = ctxt->iqtab;
	for (i = 0; i < 6; i++)
	{
		// decode blocks (Cr,Cb,Y1,Y2,Y3,Y4)
		if (i == 2) iqtab = (int*)ctxt->iq_y;//iq_y;

		rl = get_rldata(mdec_rl, ctxt->rl_end);//*mdec_rl++;
		if (rl == 0xFE00) break;
		q_scale = RLE_RUN(rl);
		blk[0] = SCALER(iqtab[0] * RLE_VAL(rl), AAN_EXTRA - 3);
		for (k = 0, used_col = 0;;)
		{
			rl = get_rldata(mdec_rl, ctxt->rl_end);
			if (rl == 0xFE00) break;
			k += RLE_RUN(rl) + 1;	// skip zero-coefficients

			if (k > 63)
			{
				// printf("run lenght exceeded 64 enties\n");
				break;
			}

			// zigzag transformation
			blk[zscan[k]] = SCALER(RLE_VAL(rl) * iqtab[k] * q_scale, AAN_EXTRA);
			// keep track of used columns to speed up the idtc
			used_col |= (zscan[k] > 7) ? 1 << (zscan[k] & 7) : 0;
		}

		if (k == 0) used_col = -1;
		// used_col is -1 for blocks with only the DC coefficient
		// any other value is a bitmask of the columns that have 
		// at least one non zero cofficient in the rows 1-7
		// single coefficients in row 0 are treted specially 
		// in the idtc function
		idct(blk, used_col);
		blk += DSIZE2;
	}
	return mdec_rl;
#endif
}

#define	RGB15(r,g,b)	( (((b)&0xf8)<<7)|(((g)&0xf8)<<2)|((r)>>3) )

#define	ROUND(r)	bs_roundtbl[(r)+256]
#if 1
#define	SHIFT	12
#define	toFIX(a)	(int)((a)*(1<<SHIFT))
#define	toINT(a)	((a)>>SHIFT)
#define	FIX_1	toFIX(1)
//#define	MULR(a)	toINT((a)*toFIX(1.402))
//#define	MULG(a)	toINT((a)*toFIX(-0.3437))
//#define	MULG2(a)	toINT((a)*toFIX(-0.7143))
//#define	MULB(a)	toINT((a)*toFIX(1.772))
#else
#define	MULR(a)	0
#define	MULG(a)	0
#define	MULG2(a)	0
#define	MULB(a)	0
#endif


/*
int ROUND(int r)
{
	if (r<0) return 0;
	else if (r>255) return 255;
	else return r;
}
*/

extern Uint8 bs_roundtbl[256*3];

static inline void putquadrgb15(unsigned short *image, int x, int *Yblk, int Cr, int Cb, unsigned short stp)
{
	int Y, R, G, B;
	R = MULR(Cb);
	G = MULG2(Cr, Cb);
	B = MULB(Cr);

	// added transparency
	Y = MULY(Yblk[0]);
	image[0] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
	Y = MULY(Yblk[1]);
	image[1] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
	Y = MULY(Yblk[8]);
	image[16] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
	Y = MULY(Yblk[9]);
	image[17] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
}

static void yuv2rgb15(BLOCK *blk, Uint16 *image)
{
#if 0
	int x, yy;
	BLOCK *yblk = blk + DCTSIZE2 * 2;
	for (yy = 0; yy < 16; yy += 2, blk += 4, yblk += 8, image += 8 + 16) {
		if (yy == 8) yblk += DCTSIZE2;
		for (x = 0; x < 4; x++, blk++, yblk += 2, image += 2) {
			int r0, b0, g0, y;
			r0 = MULR(blk[DCTSIZE2]); /* cr */
			g0 = MULG(blk[0]) + MULG2(blk[DCTSIZE2]);
			b0 = MULB(blk[0]); /* cb */
			y = yblk[0] + 128;
			image[0] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			y = yblk[1] + 128 + 4;
			image[1] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			y = yblk[8] + 128 + 6;
			image[16] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			y = yblk[9] + 128 + 2;
			image[17] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			r0 = MULR(blk[4 + DCTSIZE2]);
			g0 = MULG(blk[4]) + MULG2(blk[4 + DCTSIZE2]);
			b0 = MULB(blk[4]);
			y = yblk[DCTSIZE2 + 0] + 128;
			image[8 + 0] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			y = yblk[DCTSIZE2 + 1] + 128 + 4;
			image[8 + 1] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			y = yblk[DCTSIZE2 + 8] + 128 + 6;
			image[8 + 16] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
			y = yblk[DCTSIZE2 + 9] + 128 + 2;
			image[8 + 17] = RGB15(ROUND(r0 + y), ROUND(g0 + y), ROUND(b0 + y));
		}
	}
#else
	int *yblk = blk + 128;
	int stp = 1;
	unsigned short *dp = image;

	for (int yy = 0, yi = 0; yy < 16; yy += 2, blk += 4, yblk += 8, dp += 8 + 16, yi++)
	{
		if (yy == 8) yblk += 64;
		for (int x = 0; x < 4; x++, blk++, yblk += 2, dp += 2)
		{
			putquadrgb15(&dp[0], x, &yblk[0], blk[64], blk[0], stp);
			putquadrgb15(&dp[8], x, &yblk[64], blk[68], blk[4], stp);
		}
	}
#endif
}

enum {B, G, R};

#define	SCALE8(c)			SCALER(c, 20) 
#define CLAMP8(c)			( ((c) < -128) ? 0 : (((c) > (255 - 128)) ? 255 : ((c) + 128)) )
#define CLAMP_SCALE8(a)		(CLAMP8(SCALE8(a)))

static inline void putquadrgb24(unsigned char * image, int *Yblk, int Cr, int Cb)
{
	int Y, R, G, B;

	R = MULR(Cr);
	G = MULG2(Cb, Cr);
	B = MULB(Cb);

	Y = MULY(Yblk[0]);
	image[0 * 3 + 0] = CLAMP_SCALE8(Y + R);
	image[0 * 3 + 1] = CLAMP_SCALE8(Y + G);
	image[0 * 3 + 2] = CLAMP_SCALE8(Y + B);
	Y = MULY(Yblk[1]);
	image[1 * 3 + 0] = CLAMP_SCALE8(Y + R);
	image[1 * 3 + 1] = CLAMP_SCALE8(Y + G);
	image[1 * 3 + 2] = CLAMP_SCALE8(Y + B);
	Y = MULY(Yblk[8]);
	image[16 * 3 + 0] = CLAMP_SCALE8(Y + R);
	image[16 * 3 + 1] = CLAMP_SCALE8(Y + G);
	image[16 * 3 + 2] = CLAMP_SCALE8(Y + B);
	Y = MULY(Yblk[9]);
	image[17 * 3 + 0] = CLAMP_SCALE8(Y + R);
	image[17 * 3 + 1] = CLAMP_SCALE8(Y + G);
	image[17 * 3 + 2] = CLAMP_SCALE8(Y + B);
}

static void yuv2rgb24(BLOCK *blk, unsigned char *dp)
{
#if 0
	int x,yy;
	BLOCK *yblk = blk+DCTSIZE2*2;
	for(yy=0;yy<16;yy+=2,blk+=4,yblk+=8,image+=8+16) {
		if (yy==8) yblk+=DCTSIZE2;
		for(x=0;x<4;x++,blk++,yblk+=2,image+=2) {
			int r0,b0,g0,y;
			r0 = MULR(blk[DCTSIZE2]); /* cr */
			g0 = MULG(blk[0])+MULG2(blk[DCTSIZE2]);
			b0 = MULB(blk[0]); /* cb */
			y = yblk[0]+128;
			image[0].r = ROUND(r0+y);
			image[0].g = ROUND(g0+y);
			image[0].b = ROUND(b0+y);
			y = yblk[1]+128;
			image[1].r = ROUND(r0+y);
			image[1].g = ROUND(g0+y);
			image[1].b = ROUND(b0+y);
			y = yblk[8]+128;
			image[16].r = ROUND(r0+y);
			image[16].g = ROUND(g0+y);
			image[16].b = ROUND(b0+y);
			y = yblk[9]+128;
			image[17].r = ROUND(r0+y);
			image[17].g = ROUND(g0+y);
			image[17].b = ROUND(b0+y);

			r0 = MULR(blk[4+DCTSIZE2]);
			g0 = MULG(blk[4])+MULG2(blk[4+DCTSIZE2]);
			b0 = MULB(blk[4]);
			y = yblk[DCTSIZE2+0]+128;
			image[8+0].r = ROUND(r0+y);
			image[8+0].g = ROUND(g0+y);
			image[8+0].b = ROUND(b0+y);
			y = yblk[DCTSIZE2+1]+128;
			image[8+1].r = ROUND(r0+y);
			image[8+1].g = ROUND(g0+y);
			image[8+1].b = ROUND(b0+y);
			y = yblk[DCTSIZE2+8]+128;
			image[8+16].r = ROUND(r0+y);
			image[8+16].g = ROUND(g0+y);
			image[8+16].b = ROUND(b0+y);
			y = yblk[DCTSIZE2+9]+128;
			image[8+17].r = ROUND(r0+y);
			image[8+17].g = ROUND(g0+y);
			image[8+17].b = ROUND(b0+y);
		}
	}
#else
	int *yblk = blk + 128;
	for (int yy = 0; yy < 16; yy += 2, blk += 4, yblk += 8, dp += (8 + 16) * 3)
	{
		if (yy == 8) yblk += 64;
		for (int x = 0; x < 4; x++, blk++, yblk += 2, dp += 2 * 3)
		{
			putquadrgb24(&dp[0],  &yblk[0],  blk[0], blk[64]);
			putquadrgb24(&dp[24], &yblk[64], blk[4], blk[68]);
		}
	}
#endif
}

static void DecDCTReset(bs_context_t *ctxt, int mode)
{
	iqtab_init(ctxt);
}

static void DecDCTin(bs_context_t *ctxt, Uint16 *mdecrl,int mode)
{
	mdecrl+=2;
	ctxt->mdec_rl = mdecrl;
	ctxt->rl_end = mdecrl+mdecrl[-2]*2;
	ctxt->mdec_mode = mode;
}

static void DecDCTout(bs_context_t *ctxt, Uint16 *image,int size)
{
	BLOCK blk[DCTSIZE2*6];
	int blocksize;
	if (ctxt->mdec_mode) blocksize = 24 * 16;
	else blocksize = 16 * 16;
	for (; size > 0; size -= blocksize / 2, image += blocksize)
	{
		ctxt->mdec_rl = rl2blk(ctxt, blk, ctxt->mdec_rl);
		if (ctxt->mdec_mode == 0) yuv2rgb15(blk, image);
		else yuv2rgb24(blk, (unsigned char*)image);
	}
}

void bs_decode_rgb24 (
	unsigned char *outbuf,	/* output RGB bytes (width*height*3) */
	bs_header_t *img,		/* input BS image */
	int width, int height,	/* dimension of BS image */
	const unsigned char *myiqtab
	)
{
	unsigned short *buf2 = (unsigned short *) outbuf;
	unsigned short *bufp = (unsigned short *) img;
	bs_context_t ctxt;
	unsigned short *rl,*image;
	int slice;
	/* int rlsize; */
	int mode;
	int x,y;
	int height2 = (height+15)&~15;
	int w;

	ctxt.iq_y = myiqtab ? myiqtab : bs_iqtab();
	mode=1;
	w=24;
	width = width*3/2;

	image = (unsigned short *) malloc (height2*w*sizeof(short));
	rl = (unsigned short *) malloc ((bufp[0]+2)*sizeof(long));

	DecDCTReset(&ctxt,0);
	DecDCTvlc(bufp,rl);
	DecDCTin(&ctxt,rl,mode);

	slice = height2*w/2;

	for(x=0;x<width;x+=w)
	{
		Uint16 *dst,*src;
		DecDCTout(&ctxt,image,slice);
		src = image;
 		dst = buf2+x+(0)*width;
 		for(y=height-1;y>=0;y--)
 		{
 			memcpy(dst,src,w*2);
 			src+=w;
 			dst+=width;
 		}
	}

	free (image);
	free (rl);
}

void bs_decode_rgb15 (
	unsigned short *outbuf,	/* output RGB bytes (width*height*2) */
	bs_header_t *img,		/* input BS image */
	int width, int height,	/* dimension of BS image */
	const unsigned char *myiqtab
	)
{
	unsigned short *buf2 = (unsigned short *) outbuf;
	unsigned short *bufp = (unsigned short *) img;
	bs_context_t ctxt;
	unsigned short *rl,*image;
	int slice;
	/* int rlsize; */
	int mode;
	int x,y;
	int height2 = (height+15)&~15;
	int w;

	ctxt.iq_y = myiqtab ? myiqtab : bs_iqtab();
	mode=0;
	w=24;

	image = (unsigned short *) malloc (height2*w*sizeof(short));
	rl = (unsigned short *) malloc ((bufp[0]+2)*sizeof(long));

	DecDCTReset(&ctxt,0);
	DecDCTvlc(bufp,rl);
	DecDCTin(&ctxt,rl,mode);

	slice = height2*w/2;

	for(x=0;x<width;x+=w)
	{
		Uint16 *dst,*src;
		DecDCTout(&ctxt,image,slice);
		src = image;
		dst = buf2+x+(height-1)*width;
		for(y=height-1;y>=0;y--)
		{
			memcpy(dst,src,w*2);
			src+=w;
			dst-=width;
		}
	}

	free (image);
	free (rl);
}
