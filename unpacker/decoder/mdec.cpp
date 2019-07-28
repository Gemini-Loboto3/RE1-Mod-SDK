//#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "mdec.h"
//#include "psx.h"
//#include "chunkfile.h"
//#include "psx_mon.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

#define USE_PCSX		1

#define DSIZE			8
#define DSIZE2			(DSIZE * DSIZE)

#define SCALE(x, n)		((x) >> (n))
#define SCALER(x, n)	(((x) + ((1 << (n)) >> 1)) >> (n))

#define AAN_CONST_BITS			12
#define AAN_PRESCALE_BITS		16

#define AAN_CONST_SIZE			24
#define AAN_CONST_SCALE			(AAN_CONST_SIZE - AAN_CONST_BITS)

#define AAN_PRESCALE_SIZE		20
#define AAN_PRESCALE_SCALE		(AAN_PRESCALE_SIZE-AAN_PRESCALE_BITS)
#define AAN_EXTRA				12

#define FIX_1_082392200		SCALER(18159528, AAN_CONST_SCALE) // B6
#define FIX_1_414213562		SCALER(23726566, AAN_CONST_SCALE) // A4
#define FIX_1_847759065		SCALER(31000253, AAN_CONST_SCALE) // A2
#define FIX_2_613125930		SCALER(43840978, AAN_CONST_SCALE) // B2

#define MULS(var, const)	(SCALE((var) * (const), AAN_CONST_BITS))

#define	RLE_RUN(a)	((a) >> 10)
#define	RLE_VAL(a)	(((int)(a) << (sizeof(int) * 8 - 10)) >> (sizeof(int) * 8 - 10))

static int aanscales[DSIZE2] =
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

//
//
//

//#define debug_mdec

//
//
//

static const int rl_nop=0xfe00;

static const int zscan[64] =
{
	0 ,1 ,8 ,16,9 ,2 ,3 ,10,
	17,24,32,25,18,11,4 ,5 ,
	12,19,26,33,40,48,41,34,
	27,20,13,6 ,7 ,14,21,28,
	35,42,49,56,57,50,43,36,
	29,22,15,23,30,37,44,51,
	58,59,52,45,38,31,39,46,
	53,60,61,54,47,55,62,63
};

static const int r_coef=(int)(1.402f*4096.0f),
								 g_coef=(int)(-0.3437f*4096.0f),
								 g_coef2=(int)(-0.7143f*4096.0f),
								 b_coef=(int)(1.772f*4096.0f);

//
//
//

psx_mdec::psx_mdec()
	: ctrl(0),
		status(0)	//0x2000)
{
	max_rldata=6144*16;
	rldata=new unsigned short [max_rldata>>1];
	rlhead=rltail=rlsz=0;
}

//
//
//

psx_mdec::~psx_mdec()
{
	delete[] rldata;
}

//
//
//

unsigned int psx_mdec::read_long(const unsigned int addr)
{
	unsigned int ret=0;

	switch (addr&7)
	{
		case 0: ret=ctrl; break;
		case 4: ret=status; break;
	}

	return ret;
}

//
//
//

void psx_mdec::write_long(const unsigned int addr, const unsigned int data)
{
	switch (addr&7)
	{
		case 0:
			ctrl=data;
			if ((data&0xf0000000)==0x30000000)
			{
				rlsz=0;
				rlhead=rltail=0;
				status=0;	//&=~0x20000000;
			}
			break;

		case 4:
			if (data&0x80000000)
			{
				rlhead=rltail=rlsz=0;
				status=0;
			}
			break;

		//case 4: status=data; break;
	}
}

//
//
//

void psx_mdec::start_in_dma(const unsigned int madr,
														const unsigned int bcr,
														const unsigned int chcr)
{
	unsigned int sz=((bcr>>16)*(bcr&0xffff))<<2;

	//psx_dma_check(psxdma_mdec_in,madr,madr+sz);

	unsigned char *sp = nullptr;//=ps1hw.cpu->translate_address(madr);
	if (sp)
	{
		if (ctrl==0x60000000)
		{
			init_costab((signed short *)sp);
		} else
		if (ctrl==0x40000001)
		{

			init_iqtab(iq_y,sp);
			init_iqtab(iq_uv,sp+64);
		} else
		if ((ctrl&0xf5ff0000)==0x30000000)
		{

			status|=0x20000000;

			if ((rlsz+sz)>max_rldata)
			{
				int oldsz=max_rldata;
				while ((rlsz+sz)>max_rldata) max_rldata<<=1;

				#ifdef debug_mdec
					log_static(log_mdec,"mdec: increasing rldata buffer size to %d",max_rldata);
				#endif

				unsigned short *nr=new unsigned short [max_rldata>>1];
				if (rlhead>rltail)
				{
					memcpy(nr,
								 rldata+(rlhead>>1),
								 (oldsz-rlhead));
					memcpy(nr+((oldsz-rlhead)>>1),
								 rldata,
								 rltail);
				} else
				{
					memcpy(nr,
								 rldata+(rlhead>>1),
								 (rltail-rlhead));
				}

				delete[] rldata;
				rldata=nr;
				rlhead=0;
				rltail=rlsz;
			}

			while (sz)
			{
				int n=min(sz,max_rldata-rltail);

				rlsz+=n;
				memcpy(rldata+(rltail>>1),sp,n);
				rltail+=n;
				rltail%=max_rldata;
				sz-=n;
				sp+=n;
			}
		} else
		{
			assert(0);
		}
	}
}

//
//
//

void psx_mdec::start_out_dma(const unsigned int madr,
									 					 const unsigned int bcr,
													 	 const unsigned int chcr)
{
	unsigned int sz=((bcr>>16)*(bcr&0xffff));	//<<2;

	#ifdef debug_mdec
		log_static(log_mdec,"mdec: start out dma %d bytes from %08x\n",sz,madr);
	#endif

	//psx_dma_check(psxdma_mdec_out,madr,madr+sz);

	if (! rlsz)
	{
		printf("rldata buffer empty\n");
		#ifdef debug_mdec
			log_static(log_mdecreg,"mdec: rldata empty\n");
		#endif
		return;
	}

	unsigned char *dp = nullptr;//=ps1hw.cpu->translate_address(madr);
	if (dp)
	{
		int blk[64*6],blksz;
		if (ctrl&0x08000000) blksz=16*16; else blksz=24*16;

		//ps1hw.cpu->invalidate_code(madr,madr+(sz<<2));

		//

		while (sz>0)
		{
			decode_rl(blk);
			if (ctrl&0x08000000)
			{
				yuv2rgb15(blk,(unsigned short *)dp);
			} else
			{
				yuv2rgb24(blk,dp);
			}

			sz-=(blksz>>1);
			dp+=blksz<<1;
		}
	}

	#ifdef debug_mdec
		log_static(log_mdec,"mdec: rlsz=%d\n",rlsz);
	#endif

	skip_nops();

	if (! rlsz) status&=~0x20000000;
}

//
//
//

void psx_mdec::init_iqtab(int *iq, unsigned char *sp)
{
	//iq+=64;
	//for (int i=1; i<64; i++, iq+=64)
	//	for (int j=0; j<64; j++)
	//	{
	//		float v=(4.0f/(16.0f/(sp[j]*i)))*4096.0f;
	//		iq[j]=(int)v;
	//	}
    int i;

    for (i = 0; i < DSIZE2; i++) {
        iq[i] = (sp[i] * SCALER(aanscales[zscan[i]], AAN_PRESCALE_SCALE));
    }
}

//
//
//

void psx_mdec::init_costab(signed short *sp)
{
	for (int i=0; i<64; i++)
		costab[i]=sp[i];

	int ct0=costab[0];
	for (int i=0; i<64; i++)
		costab[i]=(int)(((float)costab[i]/(float)ct0)*4096.0f);

	for (int x=0; x<8; x++)
		for (int y=0; y<8; y++)
			icostab[(x*8)+y]=costab[(y*8)+x];
}

//
//
//

static void matrix_mult_8x8(int *m2, int *m1, int *mout)
{
	memset(mout,0,8*8*4);

	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			for (int k=0; k<8; k++)
				mout[(j*8)+i]+=(m1[(j*8)+k]*m2[(k*8)+i]);

	for (int i=0; i<64; i++)
		mout[i]>>=12+2;
}

//
//
//
static inline void fillcol(int *blk, int val) {
	blk[0 * DSIZE] = blk[1 * DSIZE] = blk[2 * DSIZE] = blk[3 * DSIZE]
		= blk[4 * DSIZE] = blk[5 * DSIZE] = blk[6 * DSIZE] = blk[7 * DSIZE] = val;
}

static inline void fillrow(int *blk, int val) {
	blk[0] = blk[1] = blk[2] = blk[3]
		= blk[4] = blk[5] = blk[6] = blk[7] = val;
}

void psx_mdec::idct(int *block, int used_col)
{
#if !USE_PCSX
	int tmp[64];
	matrix_mult_8x8(costab,block,tmp);
	matrix_mult_8x8(tmp,icostab,block);
#else
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
		tmp6 = SCALE(z10*(FIX_2_613125930) + z5, AAN_CONST_BITS) - tmp7; 
		tmp5 = MULS(z11 - z13, FIX_1_414213562) - tmp6;
		tmp4 = SCALE(z12*(FIX_1_082392200) - z5, AAN_CONST_BITS) + tmp5; 

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
	} else {
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
#endif
}

//
//
//

unsigned short psx_mdec::get_rldata()
{
	static bool did_warn;

	if (rlsz)
	{
		unsigned short rl=rldata[rlhead>>1];
		rlhead+=2;
		rlsz-=2;
		rlhead%=max_rldata;

		did_warn=false;

		return rl;
	} else
	{
		if (! did_warn)
		{
			printf("psx_mdec: rldata buffer underrun\n");
			did_warn=true;
		}

		return 0;
	}
}

//
//
//

void psx_mdec::skip_nops()
{
	while ((rlsz) && (rldata[rlhead>>1]==rl_nop))
	{
		rlhead+=2;
		rlsz-=2;
		rlhead%=max_rldata;
	}
}

//
//
//

void psx_mdec::decode_rl(int *blk)
{
#if !USE_PCSX
	memset(blk,0,6*64*4);
	int used_col;

	for (int i=0; i<6; i++)
	{
		int *iqtab=(i<2)?iq_uv:iq_y,rl,k,q_scl,v;
		rl=get_rldata();
		q_scl=(rl>>10);
		v=(rl<<22)>>22;
		blk[0]=v<<2;
		k=0;

		iqtab+=(q_scl*64);

		for (k = 0, used_col = 0;;)
		/*while (1)*/
		{
			rl=get_rldata();
			if (rl==rl_nop) break;
			k+=(rl>>10)+1;
			if (k>63) break;
			v=(rl<<22)>>22;

			int z=zscan[k];
			blk[z]=(v*iqtab[k])>>12;

			used_col |= (zscan[k] > 7) ? 1 << (zscan[k] & 7) : 0;
		}
		if (k == 0) used_col = -1;

	 	idct(blk,used_col);
		blk+=64;
	}
#else
	int i, k, q_scale, rl, used_col;
 	int *iqtab;

	memset(blk, 0, 6 * DSIZE2 * sizeof(int));
	iqtab = iq_uv;
	for (i = 0; i < 6; i++)
	{
		// decode blocks (Cr,Cb,Y1,Y2,Y3,Y4)
		if (i == 2) iqtab = iq_y;

		rl = get_rldata()/*SWAP16(*mdec_rl); mdec_rl++*/;
		q_scale = RLE_RUN(rl);
		blk[0] = SCALER(iqtab[0] * RLE_VAL(rl), AAN_EXTRA - 3);
		for (k = 0, used_col = 0;;) {
			rl = get_rldata()/*SWAP16(*mdec_rl); mdec_rl++*/;
			if (rl == 0xFE00) break;
			k += RLE_RUN(rl) + 1;	// skip zero-coefficients

			if (k > 63) {
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
#endif
}

//
//
//

static inline int clamp(const int v)
{
	if (v<0) return 0;
	if (v>255) return 255;
	return v;
}

#define MAX(x, y)	((x) > (y) ? (x) : (y))
#define MIN(x, y)	((x) < (y) ? (x) : (y))
#define ABS(x)		((x) < 0 ? -(x) : (x))

//
//
//
#define	MULR(a)			((1434 * (a))) 
#define	MULB(a)			((1807 * (a))) 
#define	MULG2(a, b)		((-351 * (a) - 728 * (b)))
#define MULY(a)			((a) << 10)

//#define	MAKERGB15(r, g, b, a)	(SWAP16(a | ((b) << 10) | ((g) << 5) | (r)))
#define MAKERGB15(r,g,b)		(r)|((g)<<5)|((b)<<10)
#define	SCALE8(c)				SCALER(c, 20) 
#define SCALE5(c)				SCALER(c, 23)

#define CLAMP5(c)	( ((c) < -16) ? 0 : (((c) > (31 - 16)) ? 31 : ((c) + 16)) )
#define CLAMP8(c)	( ((c) < -128) ? 0 : (((c) > (255 - 128)) ? 255 : ((c) + 128)) )

#define CLAMP_SCALE8(a)   (CLAMP8(SCALE8(a)))
#define CLAMP_SCALE5(a)   (CLAMP5(SCALE5(a)))

static inline void putquadrgb15(unsigned short *image, int x, int *Yblk, int Cr, int Cb, unsigned short stp)
{
#if !USE_PCSX
	int r=(Cr*r_coef)>>12,
		g=((Cb*g_coef)+(Cr*g_coef2))>>12,
		b=(Cb*b_coef)>>12,
		y;

	y=Yblk[0]+128;
	image[0]=to555(x*2,r+y,g+y,b+y)|stp;
	y=Yblk[1]+128;
	image[1]=to555(x*2+1,r+y,g+y,b+y)|stp;
	y=Yblk[8]+128;
	image[16]=to555(x*2+4,r+y,g+y,b+y)|stp;
	y=Yblk[9]+128;
	image[17]=to555(x*2+5,r+y,g+y,b+y)|stp;
#else
	int Y, R, G, B;
	R = MULR(Cb);
	G = MULG2(Cr, Cb);
	B = MULB(Cr);

	// added transparency
# if 1
	Y = MULY(Yblk[0]);
	image[0] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
	Y = MULY(Yblk[1]);
	image[1] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
	Y = MULY(Yblk[8]);
	image[16] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
	Y = MULY(Yblk[9]);
	image[17] = MAKERGB15(CLAMP_SCALE5(Y + R), CLAMP_SCALE5(Y + G), CLAMP_SCALE5(Y + B)) | stp;
# else
	Y = MULY(Yblk[0]);
	image[0] = dither_xy_matrix(x*2,Y + R, Y + G, Y + B) | stp;
	Y = MULY(Yblk[1]);
	image[1] = dither_xy_matrix(x*2+1,Y + R, Y + G, Y + B) | stp;
	Y = MULY(Yblk[8]);
	image[16] = dither_xy_matrix(x*2+4,Y + R, Y + G, Y + B) | stp;
	Y = MULY(Yblk[9]);
	image[17] = dither_xy_matrix(x*2+5,Y + R, Y + G, Y + B) | stp;
# endif
#endif
}

void psx_mdec::yuv2rgb15(int *blk, unsigned short *dp)
{
	unsigned short stp=(ctrl&0x02000000)?0x8000:0;

	int *yblk=blk+128;
	for (int yy=0, yi=0; yy<16; yy+=2, blk+=4, yblk+=8, dp+=8+16, yi++)
	{
		if (yy==8) yblk+=64;
		for (int x=0; x<4; x++, blk++, yblk+=2, dp+=2)
		{
#if 0
			int r=(blk[64]*r_coef)>>12,
				g=((blk[0]*g_coef)+(blk[64]*g_coef2))>>12,
				b=(blk[0]*b_coef)>>12,
				y;

			y=yblk[0]+128;
			dp[0]=to555(x*2,r+y,g+y,b+y)|stp;
			y=yblk[1]+128;
			dp[1]=to555(x*2+1,r+y,g+y,b+y)|stp;
			y=yblk[8]+128;
			dp[16]=to555(x*2+4,r+y,g+y,b+y)|stp;
			y=yblk[9]+128;
			dp[17]=to555(x*2+5,r+y,g+y,b+y)|stp;

			r=(blk[4+64]*r_coef)>>12;
			g=((blk[4]*g_coef)+(blk[4+64]*g_coef2))>>12;
			b=(blk[4]*b_coef)>>12;

			y=yblk[64]+128;
			dp[8]=to555(x*2,r+y,g+y,b+y)|stp;
			y=yblk[64+1]+128;
			dp[8+1]=to555(x*2+1,r+y,g+y,b+y)|stp;
			y=yblk[64+8]+128;
			dp[8+16]=to555(x*2+4,r+y,g+y,b+y)|stp;
			y=yblk[64+9]+128;
			dp[8+17]=to555(x*2+5,r+y,g+y,b+y)|stp;
#else
			putquadrgb15(&dp[0],x,&yblk[0] ,blk[64],blk[0],stp);
			putquadrgb15(&dp[8],x,&yblk[64],blk[68],blk[4],stp);
#endif
		}
	}
}

//
//
//
#define CLAMP8(c)	( ((c) < -128) ? 0 : (((c) > (255 - 128)) ? 255 : ((c) + 128)) )
#define CLAMP_SCALE8(a)   (CLAMP8(SCALE8(a)))

static inline void putquadrgb24(unsigned char * image, int *Yblk, int Cr, int Cb)
{
	int Y, R, G, B;

	R = MULR(Cr);
	G = MULG2(Cb,Cr);
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

void psx_mdec::yuv2rgb24(int *blk, unsigned char *dp)
{
	int *yblk=blk+128;
	for (int yy=0; yy<16; yy+=2, blk+=4, yblk+=8, dp+=(8+16)*3)
	{
		if (yy==8) yblk+=64;
		for (int x=0; x<4; x++, blk++, yblk+=2, dp+=2*3)
		{
#if USE_PCSX
			putquadrgb24(&dp[0], &yblk[0], blk[0],blk[64]);
			putquadrgb24(&dp[24],&yblk[64],blk[4],blk[68]);
#else
			int r=(blk[64]*r_coef)>>12,
					g=((blk[0]*g_coef)+(blk[64]*g_coef2))>>12,
					b=(blk[0]*b_coef)>>12,
					y;

			y=yblk[0]+128;
			dp[2]=clamp(r+y);
			dp[1]=clamp(g+y);
			dp[0]=clamp(b+y);
			y=yblk[1]+128;
			dp[3+2]=clamp(r+y);
			dp[3+1]=clamp(g+y);
			dp[3+0]=clamp(b+y);
			y=yblk[8]+128;
			dp[(16*3)+2]=clamp(r+y);
			dp[(16*3)+1]=clamp(g+y);
			dp[(16*3)+0]=clamp(b+y);
			y=yblk[9]+128;
			dp[(17*3)+2]=clamp(r+y);
			dp[(17*3)+1]=clamp(g+y);
			dp[(17*3)+0]=clamp(b+y);

			r=(blk[4+64]*r_coef)>>12;
			g=((blk[4]*g_coef)+(blk[4+64]*g_coef2))>>12;
			b=(blk[4]*b_coef)>>12;

			y=yblk[64]+128;
			dp[(8*3)+2]=clamp(r+y);
			dp[(8*3)+1]=clamp(g+y);
			dp[(8*3)+0]=clamp(b+y);
			y=yblk[64+1]+128;
			dp[(9*3)+2]=clamp(r+y);
			dp[(9*3)+1]=clamp(g+y);
			dp[(9*3)+0]=clamp(b+y);
			y=yblk[64+8]+128;
			dp[((8+16)*3)+2]=clamp(r+y);
			dp[((8+16)*3)+1]=clamp(g+y);
			dp[((8+16)*3)+0]=clamp(b+y);
			y=yblk[64+9]+128;
			dp[((8+17)*3)+2]=clamp(r+y);
			dp[((8+17)*3)+1]=clamp(g+y);
			dp[((8+17)*3)+0]=clamp(b+y);
#endif
		}
	}
}

//
//
//