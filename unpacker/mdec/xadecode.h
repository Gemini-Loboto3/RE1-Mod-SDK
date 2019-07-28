/*
	author: unknown, probably bitmaster?
	slightly modified by dbalster
*/

#include "generic.h"

#ifndef XADECODE_H
#define XADECODE_H

#define USE_FXD 1

#define kNumOfSamples   224
#define kNumOfSGs       18

#define XAFile			0
#define XAChannel		1
#define XAType			2
#define XAFlags			3
/* bits in XAFlags byte */
#define XAFStereo		1<<0
#define XAFHalfHz		1<<2

#define XAAUDIO		0x64
#define XAVIDEO		0x48
#define XABREAK		0xE4
#define XACURRENT	0x100	/* for application use only! */
#define XANONE		0x200	/* for application use only! */
#define XAAV		0x400	/* for application use only! */

#ifndef max
#define max(a,b) (a<b?b:a)
#define min(a,b) (a>b?b:a)
#endif

#define FXD_FxdToPCM(dt)			(max(min((short)((dt)>>16), 32767), -32768))
#define DblToPCM(dt)				(short)(max(min((dt), 32767), -32768))

#define WHP_READ68_AUTO(fp, dt)		WHP_Read68(dt, sizeof(*(dt)), 1, fp)
#define WHP_WRITE68_AUTO(fp, dt)	WHP_Write68(dt, sizeof(*(dt)), 1, fp)

#define WHP_CNV_SHORT68(dt, ndt)	WHP_CnvEndianShort((dt), (ndt))
#define WHP_CNV_LONG68(dt, ndt)		WHP_CnvEndianLong((dt), (ndt))

#if USE_FXD
#define FXD_FxdToPcm16(dt)			(max(min((dt)/2, 32767), -32768))
#define FXD_Pcm16ToFxd(dt)			((long)dt*2)
#endif

#define XAWAVBUFSIZE (kNumOfSamples*kNumOfSGs*2)

typedef char SoundGroup[128];

typedef struct SoundSector {
	char            sectorFiller[8];
	SoundGroup      SoundGroups[kNumOfSGs];
} PACKED SoundSector;

typedef unsigned long DWORD;
typedef unsigned short WORD;

#if USE_FXD
typedef long FXD;
#endif

#ifdef __cplusplus
extern "C" {
#endif

long decodeSoundSect(SoundSector *ssct, char *wav);
long decodeSoundSect1(SoundSector *ssct, char *wav);
long convXaToWave(char *adp, char *wav, int cn, int fn_s, int fn_e);
void initXaDecode(void);
void switchXaDecode(int channel);
void saveXaDecode(int channel);
void reinitXaDecode(int channel);
signed char getSoundData(char *buf, long unit, long sample);
signed char getFilter(char *buf, long unit);
signed char getRange(char *buf, long unit);
char xachannel(SoundSector *ss);
unsigned char xatype(SoundSector *ss);
char xafileno(SoundSector *ss);
char xastereo(SoundSector *ss);
char xahalfhz(SoundSector *ss);

#if USE_FXD
FXD FXD_FixMul(FXD a, FXD b);
#endif

#ifdef __cplusplus
}
#endif

#endif
