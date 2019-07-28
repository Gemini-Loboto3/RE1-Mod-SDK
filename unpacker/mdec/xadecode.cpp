/*
	author: unknown, probably bitmaster?
	slightly modified by dbalster
*/

#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "xadecode.h"

#if USE_FXD
static FXD      K0[4] = {
	0x00000000,
	0x0000F000,
	0x0001CC00,
	0x00018800
};
static FXD      K1[4] = {
	0x00000000,
	0x00000000,
	0xFFFF3000,
	0xFFFF2400
};
FXD t1, t2, at1[256], at2[256];
FXD t1_x, t2_x, at1_x[256], at2_x[256];
#else
static double   K0[4] = {
	0.0,
	0.9375,
	1.796875,
	1.53125
};
static double   K1[4] = {
	0.0,
	0.0,
	-0.8125,
	-0.859375
};
double t1, t2, at1[256], at2[256];
double t1_x, t2_x, at1_x[256], at2_x[256];
#endif

void initXaDecode(void)
{
	int i;
	
	for (i=0; i<256; ++i)
	{
		at1[i] = at2[i] = at1_x[i] = at2_x[i] = 0;
	}
}
void reinitXaDecode(int i)
{
	at1[i] = at2[i] = at1_x[i] = at2_x[i] = 0;
}
void switchXaDecode(int i)
{
	t1 = at1[i];
	t2 = at2[i];
	t1_x = at1_x[i];
	t2_x = at2_x[i];
}
void saveXaDecode(int i)
{
	at1[i]	=	t1;
	at2[i]	=	t2;
	at1_x[i]=	t1_x;
	at2_x[i]=	t2_x;
}

char xachannel(SoundSector *ss)
{
	return(ss->sectorFiller[XAChannel]);
}

unsigned char xatype(SoundSector *ss)
{
	return(unsigned char) (ss->sectorFiller[XAType]);
}

char xafileno(SoundSector *ss)
{
	return(ss->sectorFiller[XAFile]);
}

char xastereo(SoundSector *ss)
{
	return(ss->sectorFiller[XAFlags]&XAFStereo);
}

char xahalfhz(SoundSector *ss)
{
	return(ss->sectorFiller[XAFlags]&XAFHalfHz);
}

long convXaToWave(char *adp, char *wav, int cn, int fn_s, int fn_e)
{
	SoundSector ssct;
	int i;

	memcpy(ssct.sectorFiller,adp,sizeof(ssct.sectorFiller));
	for(i=0;i<18;i++)
		memcpy(ssct.SoundGroups[i],adp+sizeof(ssct.sectorFiller)+(128*i),128);
	if ((xachannel(&ssct) == cn) && (xatype(&ssct) == XAAUDIO))
	{
		if (xafileno(&ssct) >= fn_s
				&& xafileno(&ssct) <= fn_e)
		{
			if (xastereo(&ssct))
				return(decodeSoundSect1(&ssct, wav));
			else
				return(decodeSoundSect(&ssct, wav));
		}
	}
	return(0);
}

long decodeSoundSect(SoundSector *ssct, char *wav)
{
	long count, outputBytes;
	signed char snddat, filt, range;
	short decoded;
	long unit, sample;
	long sndgrp;
#if USE_FXD
	FXD tmp2, tmp3, tmp4, tmp5;
#else
	double tmp2, tmp3, tmp4, tmp5;
#endif
	
	outputBytes = 0;

	for (sndgrp = 0; sndgrp < kNumOfSGs; sndgrp++)
	{
		count = 0;
		for (unit = 0; unit < 8; unit++)
		{
			range = getRange(ssct->SoundGroups[sndgrp], unit);
			filt = getFilter(ssct->SoundGroups[sndgrp], unit);
			for (sample = 0; sample < 28; sample++)
			{
				snddat = getSoundData(ssct->SoundGroups[sndgrp], unit, sample);
#if USE_FXD
				tmp2 = (long)(snddat) << (12 - range);
				tmp3 = FXD_Pcm16ToFxd(tmp2);
				tmp4 = FXD_FixMul(K0[filt], t1);
				tmp5 = FXD_FixMul(K1[filt], t2);
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded =(short) FXD_FxdToPcm16(t1);
#else
				tmp2 = (double)(1 << (12 - range));
				tmp3 = (double)snddat * tmp2;
				tmp4 = t1 * K0[filt];
				tmp5 = t2 * K1[filt];
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded = DblToPCM(t1);
#endif
				wav[outputBytes+count++] = (char)(decoded & 0x0000ffff);
				wav[outputBytes+count++] = (char)(decoded >> 8);
			}
		}
		outputBytes += count;
	}
	return outputBytes;
}

long decodeSoundSect1(SoundSector *ssct, char *wav)
{
	long count, outputBytes;
	signed char snddat, filt, range;
	signed char filt1, range1;
	short decoded;
	long unit, sample;
	long sndgrp;
#if USE_FXD
	FXD tmp2, tmp3, tmp4, tmp5;
#else
	double tmp2, tmp3, tmp4, tmp5;
#endif

	outputBytes = 0;

	for (sndgrp = 0; sndgrp < kNumOfSGs; sndgrp++)
	{
		count = 0;
		for (unit = 0; unit < 8; unit+= 2)
		{
			range = getRange(ssct->SoundGroups[sndgrp], unit);
			filt = getFilter(ssct->SoundGroups[sndgrp], unit);
			range1 = getRange(ssct->SoundGroups[sndgrp], unit+1);
			filt1 = getFilter(ssct->SoundGroups[sndgrp], unit+1);

			for (sample = 0; sample < 28; sample++)
			{
				/* Channel 1 */
				snddat = getSoundData(ssct->SoundGroups[sndgrp], unit, sample);
#if USE_FXD
				tmp2 = (long)(snddat) << (12 - range);
				tmp3 = FXD_Pcm16ToFxd(tmp2);
				tmp4 = FXD_FixMul(K0[filt], t1);
				tmp5 = FXD_FixMul(K1[filt], t2);
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded =(short) FXD_FxdToPcm16(t1);
#else
				tmp2 = (double)(1 << (12 - range));
				tmp3 = (double)snddat * tmp2;
				tmp4 = t1 * K0[filt];
				tmp5 = t2 * K1[filt];
				t2 = t1;
				t1 = tmp3 + tmp4 + tmp5;
				decoded = DblToPCM(t1);
#endif
				wav[outputBytes + count++] = (char)(decoded & 0x0000ffff);
				wav[outputBytes + count++] = (char)(decoded >> 8);

				/* Channel 2 */
				snddat = getSoundData(ssct->SoundGroups[sndgrp], unit+1, sample);
#if USE_FXD
				tmp2 = (long)(snddat) << (12 - range1);
				tmp3 = FXD_Pcm16ToFxd(tmp2);
				tmp4 = FXD_FixMul(K0[filt1], t1_x);
				tmp5 = FXD_FixMul(K1[filt1], t2_x);
				t2_x = t1_x;
				t1_x = tmp3 + tmp4 + tmp5;
				decoded =(short) FXD_FxdToPcm16(t1_x);
#else
				tmp2 = (double)(1 << (12 - range1));
				tmp3 = (double)snddat * tmp2;
				tmp4 = t1_x * K0[filt1];
				tmp5 = t2_x * K1[filt1];
				t2_x = t1_x;
				t1_x = tmp3 + tmp4 + tmp5;
				decoded = DblToPCM(t1_x);
#endif
				wav[outputBytes + count++] = (char)(decoded & 0x0000ffff);
				wav[outputBytes + count++] = (char)(decoded >> 8);
			}
		}
		outputBytes += count;
	}
	return outputBytes;
}

signed char getSoundData(char *buf, long unit, long sample)
{
	signed char ret;
	char *p;
	long offset, shift;

	p = buf;
	shift = (unit%2) * 4;

	offset = 16 + (unit / 2) + (sample * 4);
	p += offset;

	ret = (*p >> shift) & 0x0F;

	if (ret > 7) {
		ret -= 16;
	}
	return ret;
}

signed char getFilter(char *buf, long unit)
{
	return (*(buf + 4 + unit) >> 4) & 0x03;
}


signed char getRange(char *buf, long unit)
{
	return *(buf + 4 + unit) & 0x0F;
}

#if USE_FXD
FXD FXD_FixMul(FXD a, FXD b)
{
	long                high_a, low_a, high_b, low_b;
	long                hahb, halb, lahb;
	unsigned long       lalb;
	FXD                 ret;

	high_a = a >> 16;
	low_a = a & 0x0000FFFF;
	high_b = b >> 16;
	low_b = b & 0x0000FFFF;

	hahb = (high_a * high_b) << 16;
	halb = high_a * low_b;
	lahb = low_a * high_b;
	lalb = (unsigned long)(low_a * low_b) >> 16;

	ret = hahb + halb + lahb + lalb;

	return ret;
}
#endif
