#pragma once
//#include "PSound.h"
//#include "3DModel.h"
#include "Bitmap.h"

#ifndef ClearPtrVector
	#define ClearPtrVector(x)	while(!x.empty()) {	delete x.back(); x.pop_back(); }
#endif

namespace RE1
{
	typedef struct tagModel
	{
		u32 Tmd;
		u32 Tim;
	} MODEL;

	typedef struct tagScdDoor
	{
		u8 opcode;
		u8 id;
		u16 x0, z0, x1, z1;
		u8 unk[5];
		u8 Next_room:5;
		//u8 Next_neg:1;
		u8 Next_stage:3;
		u16 Next_x;
		s16 Next_y;
		u16 Next_z;
		u16 Next_dir;
		u8 key;
		u8 type;
	} SCD_DOOR;

	typedef struct tagScdOba
	{
		u8 opcode;		// 0
		u8 id:6;		// 1
		u8 id_unk:1;
		u8 ex:1;
		u8 Be_flg;		// 2
		u8 flag;		// 3
		u16 x;			// 4
		s16 y;			// 6
		u16 z;			// 8
		s16 cdir_y;		// A
		u16 unk[5];		// C-E-10-12-14
		u16 h, w, d;	// 16-18-1A
	} SCD_OBA;

	typedef struct tagRCut
	{
		u32 pSp;			// offset to RID data
		u32 pTim;
		s32 View_p[3];		// camera eye
		s32 View_r[3];		// camera at
		u32 Zero[2];
		u32 ViewR;			// screen projection height
	} RCUT;

	typedef struct tagVCut
	{
		u16 Tcut;			// next camera index
		u16 Fcut;			// current camera index
		u16 Xz[4][2];		// point coordinates
	} VCUT;

	typedef struct tagPspHeader
	{
		u16 group_cnt;		/* Number of mask groups, -1 if empty */
		u16 sprt_cnt;		/* Number of mask sprites, -1 if empty */
	} PSP_HEADER;

	typedef struct tagPspGroup
	{
		u16 count;			// number of sprites for a mask
		u16 clut;			// is it really needed?
		s16 x;				// base coordinates
		s16 y;
	} PSP_GROUP;

	typedef struct tagPspSprt
	{
		u8 u, v;
		u8 x, y;			// screen coordinates
		u16 otz;
		u16 tpage:12;
		u16 size:4;			// if=0 apply RDT_MASK_EX, otherwise w=h=size*8
	} PSP_SPRT;

	typedef struct tagPspSprtVar
	{
		u8 u, v;
		u8 x, y;
		u16 otz;
		u16 tpage;
		u16 w, h;
	} PSP_SPRT_VAR;

	typedef struct tagLightData
	{
		s32 x, y, z;
		u8 color[3];
		u8 dummy01;		// unused
		u16 mode;
		u16 luminosity;
	} LIGHT_DATA;

	typedef struct tagScaHead
	{
		u16 Cx, Cz;
		u32 Ptr[5];
	} SCA_HEAD;

	typedef struct tagScaData
	{
		u16 x0, z0;
		u16 x1, z1;
		u16 Id;
		u16 Type;
	} SCA_DATA;

	typedef struct tagFloor
	{
		u16 x0, z0;
		u16 x1, z1;
		u16 Id;
		//u16 Type;
	} RDT_FLOOR;

	typedef struct tagSoundTable
	{
		u8 id_check;
		u8 pan_prog;
		u8 tone_pri;
		u8 monopoly;
	} SOUND_TABLE;

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
		u32 pEsp;			// 0x7C
		u32 pEff;			// 0x80
		u32 pTim;			// 0x84
		u32 pEdt, pVh, pVb;	// 0x88-8C-90
		RCUT Cut[1];		// 0x94 x[nCut] number of elements
	} RDT_HEADER;

	typedef struct tagPair
	{
		u8 Id;
		u8 Sc_id;
	} PAIR;

	//class CCollision : SCA_DATA
	//{
	//public:
	//};

	class CMask : public PSP_GROUP
	{
	public:
		std::vector<PSP_SPRT_VAR> pSp;

		void Reset() { pSp.clear(); }

		__inline int GetCount() { return (int)pSp.size(); }
	};

	class CCamera
	{
	public:
		~CCamera() { Reset(); }
		RCUT cut;
		CTim pic;
		std::vector<CMask*> mask;
		std::vector<VCUT> vcut;
		bool has_mask;

		void Reset() { ClearPtrVector(mask); vcut.clear(); has_mask=false; }

		__inline CCamera() { has_mask=false; /*pic.Reset();*/ }

		void OpenMask(u8* data);
		int GetMaskCount();
	};

	class CRoom
	{
	public:
		CRoom(void);
		~CRoom(void) { Reset(); }

		int Open(LPCTSTR filename);
		int Open(u8* data);

		void Reset();
		void DumpMaskBitmap(LPCTSTR folder);

		//CEdt snd;
		SOUND_TABLE snd_tbl[48];
		std::vector<CCamera*> camera;

		int Cx, Cz;
		std::vector<SCA_DATA/*CCollision*/> walkmesh[4];
		std::vector<RDT_FLOOR> floor;
		//CModelResource res;
		//std::vector<C3DModel*> m_item, m_oba;
		std::vector<PAIR> id_item, id_oba;
		std::vector<SCD_DOOR> doors;
		std::vector<u8*> m_icon;

		u16 ambient[3];
		LIGHT_DATA light[3];
		SCD_OBA scd_oba[16];
	};
}
