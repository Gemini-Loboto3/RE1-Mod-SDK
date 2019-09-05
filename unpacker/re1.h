#pragma once
#include <stdafx.h>

////////////////////////////////////////////////
enum MIX_TYPE
{
	MIX_COMMON,		// herbs and vjolt use this
	MIX_RELOAD0,	// weapon + ammo
	MIX_RELOAD1,	// ammo + weapon
	MIX_AMMO,		// ammo + ammo
	MIX_VJOLT0,		// used by vjolt
	MIX_VJOLT1,
	MIX_GRENADE0,	// launcher + rounds
	MIX_GRENADE1,	// rounds + launcher
};

typedef struct ITEM_WORK
{
	u8 Id,
		Num;
} ITEM_WORK;

typedef struct ITEM_DATA
{
	u8 Max,			// counter max amount
		Icon,		// icon id from ITEMALL.PIX
		Mix_num,	// mix table id for ITEM_MIX_DATA
		Alt_name;	// alternative name id
} ITEM_DATA;

typedef struct _ITEM_DATA_MIX
{
	u8 With,		// combines with this item
		Result,		// and this is the result
		Remain,		// this is what can be left
		Type;		// mix type MIX_TYPE
} _ITEM_DATA_MIX;

typedef struct ITEM_MIX_DATA
{
	u8 count;
	_ITEM_DATA_MIX Data[1];
} MIX_DATA;

////////////////////////////////////////////////
typedef struct tagRCut
{
	u32 pSp;			// offset to RID data
	u32 pTim;
	s32 View_p[3];		// camera eye
	s32 View_r[3];		// camera at
	u32 Zero[2];
	u32 ViewR;			// screen projection height
} RCUT;

typedef struct tagLightData
{
	s32 x, y, z;
	u8 color[3];
	u8 dummy01;		// unused
	u16 mode;
	u16 luminosity;
} LIGHT_DATA;

typedef struct tagRdtHeader
{
	u8 nSprite;			// caching value for current mask sprite amount
	u8 nCut;			// number of cameras
	u8 nItem;			// number of item objects
	u8 nOmodel;			// number of static objects
	u8 nDoor;			// this is always zero, used at runtime?
	char nRoom_at;		// unused?
	u16 ambient[3];
	LIGHT_DATA Light[3];
	u32 pVcut;			// 0x48 [DONE]
	u32 pSca;			// 0x4C [DONE]
	u32 pObj[2];		// 0x50-54
	u32 pBlk;			// 0x58
	u32 pFlr;			// 0x5C
	u32 pScrl;			// 0x60 SCD initializer
	u32 pScdx;			// 0x64 SCD threading
	u32 pScd;			// 0x68 useless entry, keep for legacy
	u32 pEmr;			// 0x6C [DONE]
	u32 pEdd;			// 0x70 [DONE]
	u32 pMessage;		// 0x74
	u32 pRaw;			// 0x78
	u32 pEsp;			// 0x7C (ESP header)
	u32 pEff;			// 0x80 (pointer to ESP data)
	u32 pTim;			// 0x84
	u32 pEdt, pVh, pVb;	// 0x88-8C-90
	RCUT Cut[1];		// 0x94 x[nCut] number of elements
} RDT_HEADER;