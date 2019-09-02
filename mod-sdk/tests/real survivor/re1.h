#pragma once
#include <stdafx.h>

#define KEY_UP		0x0001
#define KEY_DOWN	0x0004
#define KEY_LEFT	0x0008
#define KEY_RIGHT	0x0002
#define KEY_CONFIRM	0x4000
#define KEY_CANCEL	0x8000
#define KEY_SELECT	0x0000
#define KEY_START	0x0000

#define STFLG_LOCK_CAMERA	0x000100000
#define STFLG_JILL			0x000800000
#define STFLG_EASY			0x000000008

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
	MIX_GRENADE1,	// rounds + launcherthan
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

typedef struct ITEM_MIX_DATA
{
	u8 With,		// combines with this item
		Result,		// and this is the result
		Remain,		// this is what can be left
		Type;		// mix type MIX_TYPE
} ITEM_MIX_DATA;

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
	u32 pEsp;			// 0x7C
	u32 pEff;			// 0x80
	u32 pTim;			// 0x84
	u32 pEdt, pVh, pVb;	// 0x88-8C-90
	RCUT Cut[1];		// 0x94 x[nCut] number of elements
} RDT_HEADER;

//////////////////////////////////
// ENTITY STRUCTURES
struct MATRIX
{
	__int16 m[3][3];
	__int16 dm;
	int t[3];
};

/* 316 */
struct VECTOR
{
	int vx;
	int vy;
	int vz;
	int pad;
};

/* 317 */
struct SVECTOR
{
	__int16 vx;
	__int16 vy;
	__int16 vz;
	__int16 pad;
};

////////////////////////////////////////////////
// LIBGS STUFF
typedef struct tagGsPotion
{
	s16 offx,
		offy;
} _GsPOSITION;

typedef struct tagGsEnv
{
	MATRIX GsIDMATRIX,
		GsWSMATRIX,
		GsLIGHTMATRIX;
	_GsPOSITION POSITION;
} _GsENV;

typedef struct SPRT
{
	u32 tag;
	s16 x, y;
	u16 w, h,
		page;
	u8 u, v;
	u16 cx, cy;
	u8 r, g, b, pad;
	s16 sw, sh,
		dw, dh;
} SPRT;

typedef struct LINE
{
	u32 tag;
	s16 x0, y0,
		x1, y1;
	u8 r, g, b,
		pad;
} LINE;

typedef struct TILE
{
	u32 tag;
	s16 x, y;
	u16 w, h;
	u8 r, g, b,
		pad;
} TILE;

/* 318 */
struct PARTS_W
{
	char Be_flg;
	char field_1;
	char field_2;
	char field_3;
	__int16 Cdir_x;
	__int16 field_6;
	__int16 field_8;
	char field_A;
	char field_B;
	char field_C;
	char field_D;
	char field_E;
	char field_F;
	char field_10;
	char field_11;
	char field_12;
	char field_13;
	char field_14;
	char field_15;
	char field_16;
	char field_17;
	char field_18;
	char field_19;
	char field_1A;
	char field_1B;
	char field_1C;
	char field_1D;
	char field_1E;
	char field_1F;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	__int16 M[3][3];
	__int16 field_36;
	int Pos_x;
	int Pos_y;
	int Pos_z;
	MATRIX Workm;
	char field_64;
	char field_65;
	char field_66;
	char field_67;
	char field_68;
	char field_69;
	char field_6A;
	char field_6B;
	char field_6C;
	char field_6D;
	char field_6E;
	char field_6F;
	char field_70;
	char field_71;
	char field_72;
	char field_73;
	char field_74;
	char field_75;
	char field_76;
	char field_77;
	char field_78;
	char field_79;
	char field_7A;
	char field_7B;
};

/* 315 */
typedef struct tagEnemyWork
{
	u8 Be_flg;
	u8 Id;
	u8 Sc_id;
	char field_3;
	DWORD *pAt_data;
	char field_8;
	char field_9;
	char field_A;
	char field_B;
	char field_C;
	char field_D;
	char field_E;
	char field_F;
	char field_10;
	char field_11;
	char field_12;
	char field_13;
	char field_14;
	char field_15;
	char field_16;
	char field_17;
	char field_18;
	char field_19;
	char field_1A;
	char field_1B;
	char field_1C;
	char field_1D;
	char field_1E;
	char field_1F;
	__int16 M[3][3];
	__int16 M_dm;
	int Pos_x;
	int Pos_y;
	int Pos_z;
	char field_40;
	char field_41;
	char field_42;
	char field_43;
	char field_44;
	char field_45;
	char field_46;
	char field_47;
	char field_48;
	char field_49;
	char field_4A;
	char field_4B;
	char field_4C;
	char field_4D;
	char field_4E;
	char field_4F;
	char field_50;
	char field_51;
	char field_52;
	char field_53;
	char field_54;
	char field_55;
	char field_56;
	char field_57;
	char field_58;
	char field_59;
	char field_5A;
	char field_5B;
	char field_5C;
	char field_5D;
	char field_5E;
	char field_5F;
	char field_60;
	char field_61;
	char field_62;
	char field_63;
	char field_64;
	char field_65;
	char field_66;
	char field_67;
	char field_68;
	char field_69;
	char field_6A;
	char field_6B;
	char field_6C;
	char field_6D;
	char field_6E;
	char field_6F;
	char field_70;
	char field_71;
	__int16 Cdir_x;
	__int16 Cdir_y;
	__int16 Cdir_z;
	char field_78;
	char field_79;
	char field_7A;
	char field_7B;
	char field_7C;
	char field_7D;
	char field_7E;
	char field_7F;
	char field_80;
	char field_81;
	char field_82;
	char field_83;
	u8 Routine_0,
		Routine_1,
		Routine_2,
		Routine_3;
	__int16 Life;
	char field_8A;
	char field_8B;
	char Hokan_flg;
	char Parts_no;
	char field_8E;
	char field_8F;
	DWORD *pKan;
	DWORD *pSeq;
	PARTS_W *pSin_parts;
	char field_9C;
	char field_9D;
	char field_9E;
	char field_9F;
	char field_A0;
	char field_A1;
	char field_A2;
	char field_A3;
	char field_A4;
	char field_A5;
	char field_A6;
	char field_A7;
	char field_A8;
	char field_A9;
	char field_AA;
	char field_AB;
	char field_AC;
	char field_AD;
	char field_AE;
	char field_AF;
	char field_B0;
	char field_B1;
	char field_B2;
	char field_B3;
	char field_B4;
	char field_B5;
	char field_B6;
	char field_B7;
	char field_B8;
	char field_B9;
	char field_BA;
	char field_BB;
	char field_BC;
	char Move_no;
	char Move_cnt;
	char field_BF;
	char field_C0;
	char field_C1;
	WORD field_C2;
	WORD field_C4;
	char field_C6;
	char field_C7;
	char field_C8;
	char field_C9;
	__int16 field_CA;
	char field_CC;
	char field_CD;
	char field_CE;
	char field_CF;
	char field_D0;
	char field_D1;
	char field_D2;
	char field_D3;
	char field_D4;
	char field_D5;
	char field_D6;
	char field_D7;
	char field_D8;
	char field_D9;
	char field_DA;
	char field_DB;
	char field_DC;
	char field_DD;
	char field_DE;
	char field_DF;
	WORD field_E0;
	char field_E2;
	char field_E3;
	char field_E4;
	char field_E5;
	char field_E6;
	char field_E7;
	char field_E8;
	char field_E9;
	char field_EA;
	char field_EB;
	char field_EC;
	char field_ED;
	char field_EE;
	char field_EF;
	char field_F0;
	char field_F1;
	char field_F2;
	char field_F3;
	char field_F4;
	char field_F5;
	char field_F6;
	char field_F7;
	char field_F8;
	char field_F9;
	char field_FA;
	char field_FB;
	char field_FC;
	char field_FD;
	char field_FE;
	char field_FF;
	char field_100;
	char field_101;
	char field_102;
	char field_103;
	char field_104;
	char field_105;
	char field_106;
	char field_107;
	char field_108;
	char field_109;
	char field_10A;
	char field_10B;
	char field_10C;
	char field_10D;
	char field_10E;
	char field_10F;
	char field_110;
	char field_111;
	char field_112;
	char field_113;
	char field_114;
	char field_115;
	char field_116;
	char field_117;
	char field_118;
	char field_119;
	char field_11A;
	char field_11B;
	char field_11C;
	char field_11D;
	char field_11E;
	char field_11F;
	char field_120;
	char field_121;
	char field_122;
	char field_123;
	char field_124;
	char field_125;
	char field_126;
	char field_127;
	char field_128;
	char field_129;
	char field_12A;
	char field_12B;
	char field_12C;
	char field_12D;
	char field_12E;
	char field_12F;
	char field_130;
	char field_131;
	char field_132;
	char field_133;
	char field_134;
	char field_135;
	char field_136;
	char field_137;
	char field_138;
	char field_139;
	char field_13A;
	char field_13B;
	char field_13C;
	char field_13D;
	char field_13E;
	char field_13F;
	char field_140;
	char field_141;
	char field_142;
	char field_143;
	char field_144;
	char field_145;
	char field_146;
	char field_147;
	char field_148;
	char field_149;
	char field_14A;
	char field_14B;
	char field_14C;
	char field_14D;
	char field_14E;
	char field_14F;
	char field_150;
	char field_151;
	char field_152;
	char field_153;
	char field_154;
	char field_155;
	char field_156;
	char field_157;
	char field_158;
	char field_159;
	char field_15A;
	char field_15B;
	char field_15C;
	char field_15D;
	char field_15E;
	char field_15F;
	char field_160;
	char field_161;
	char field_162;
	char field_163;
	char field_164;
	char field_165;
	char field_166;
	char field_167;
	char field_168;
	char field_169;
	char field_16A;
	char field_16B;
	char Ccpage;
	char field_16D;
	char field_16E;
	char field_16F;
	char field_170;
	char field_171;
	char field_172;
	char field_173;
	char field_174;
	char field_175;
	char field_176;
	char field_177;
	char field_178;
	char field_179;
	char field_17A;
	char field_17B;
	char field_17C;
	char field_17D;
	char field_17E;
	char field_17F;
	char field_180;
	char field_181;
	char field_182;
	char field_183;
	char field_184;
	char field_185;
	char field_186;
	char field_187;
	char field_188;
	char field_189;
	char field_18A;
	char field_18B;
} ENEMY_WORK;

typedef struct tagPlayerWork
{
	char Be_flg;
	unsigned __int8 Id;
	char Sc_id;
	char field_3;
	char field_4;
	char field_5;
	char field_6;
	char field_7;
	char field_8;
	char field_9;
	char field_A;
	char field_B;
	char field_C;
	char field_D;
	char field_E;
	char field_F;
	char field_10;
	char field_11;
	char field_12;
	char field_13;
	char field_14;
	char field_15;
	char field_16;
	char field_17;
	char field_18;
	char field_19;
	char field_1A;
	char field_1B;
	char field_1C;
	char field_1D;
	char field_1E;
	char field_1F;
	__int16 M[3][3];
	__int16 M_dm;
	int Pos_x;
	int Pos_y;
	int Pos_z;
	char field_40;
	char field_41;
	char field_42;
	char field_43;
	char field_44;
	char field_45;
	char field_46;
	char field_47;
	char field_48;
	char field_49;
	char field_4A;
	char field_4B;
	char field_4C;
	char field_4D;
	char field_4E;
	char field_4F;
	char field_50;
	char field_51;
	char field_52;
	char field_53;
	char field_54;
	char field_55;
	char field_56;
	char field_57;
	char field_58;
	char field_59;
	char field_5A;
	char field_5B;
	char field_5C;
	char field_5D;
	char field_5E;
	char field_5F;
	char field_60;
	char field_61;
	char field_62;
	char field_63;
	char field_64;
	char field_65;
	char field_66;
	char field_67;
	char field_68;
	char field_69;
	char field_6A;
	char field_6B;
	__int16 field_6C;
	char field_6E;
	char field_6F;
	__int16 field_70;
	__int16 Cdir_x;
	__int16 Cdir_y;
	__int16 Cdir_z;
	char field_78;
	char field_79;
	char field_7A;
	char field_7B;
	char field_7C;
	char field_7D;
	char field_7E;
	char field_7F;
	char field_80;
	char field_81;
	char field_82;
	char field_83;
	char Routine_0;
	char R1;
	char R2;
	char R3;
	__int16 Life;
	char field_8A;
	char field_8B;
	char Hokan_flg;
	char Parts_no;
	char field_8E;
	char field_8F;
	DWORD *pKan;
	DWORD *pSeq;
	PARTS_W *pSin_parts;
	char field_9C;
	char field_9D;
	char field_9E;
	char field_9F;
	char field_A0;
	char field_A1;
	char field_A2;
	char field_A3;
	char field_A4;
	char field_A5;
	char field_A6;
	char field_A7;
	char field_A8;
	char field_A9;
	char field_AA;
	char field_AB;
	char field_AC;
	char field_AD;
	char field_AE;
	char field_AF;
	char field_B0;
	char field_B1;
	char field_B2;
	char field_B3;
	char field_B4;
	char field_B5;
	char field_B6;
	char field_B7;
	char field_B8;
	char field_B9;
	char field_BA;
	char field_BB;
	char field_BC;
	char field_BD;
	char field_BE;
	char field_BF;
	char field_C0;
	char field_C1;
	__int16 field_C2;
	char field_C4;
	char field_C5;
	char field_C6;
	char field_C7;
	char field_C8;
	char field_C9;
	__int16 field_CA;
	char field_CC;
	char field_CD;
	char field_CE;
	char field_CF;
	char field_D0;
	char field_D1;
	char field_D2;
	char field_D3;
	char field_D4;
	char field_D5;
	char field_D6;
	char field_D7;
	char field_D8;
	char field_D9;
	char field_DA;
	char field_DB;
	u8 St_flg;
	char field_DD;
	char field_DE;
	char field_DF;
	WORD field_E0;
	char field_E2;
	char field_E3;
	char Kage_work;
	char field_E5;
	char field_E6;
	char field_E7;
	char field_E8;
	char field_E9;
	char field_EA;
	char field_EB;
	char field_EC;
	char field_ED;
	char field_EE;
	char field_EF;
	char field_F0;
	char field_F1;
	char field_F2;
	char field_F3;
	char field_F4;
	char field_F5;
	char field_F6;
	char field_F7;
	char field_F8;
	char field_F9;
	char field_FA;
	char field_FB;
	char field_FC;
	char field_FD;
	char field_FE;
	char field_FF;
	char field_100;
	char field_101;
	char field_102;
	char field_103;
	char field_104;
	char field_105;
	char field_106;
	char field_107;
	char field_108;
	char field_109;
	char field_10A;
	char field_10B;
	char field_10C;
	char field_10D;
	char field_10E;
	char field_10F;
	char field_110;
	char field_111;
	char field_112;
	char field_113;
	char field_114;
	char field_115;
	char field_116;
	char field_117;
	char field_118;
	char field_119;
	char field_11A;
	char field_11B;
	char field_11C;
	char field_11D;
	char field_11E;
	char field_11F;
	char field_120;
	char field_121;
	char field_122;
	char field_123;
	char field_124;
	char field_125;
	char field_126;
	char field_127;
	char field_128;
	char field_129;
	char field_12A;
	char field_12B;
	char field_12C;
	char field_12D;
	char field_12E;
	char field_12F;
	char field_130;
	char field_131;
	char field_132;
	char field_133;
	char field_134;
	char field_135;
	char field_136;
	char field_137;
	char field_138;
	char field_139;
	char field_13A;
	char field_13B;
	char field_13C;
	char field_13D;
	char field_13E;
	char field_13F;
	char field_140;
	char field_141;
	char field_142;
	char field_143;
	char field_144;
	char field_145;
	char field_146;
	char field_147;
	char field_148;
	char field_149;
	char field_14A;
	char field_14B;
	char field_14C;
	char field_14D;
	char field_14E;
	char field_14F;
	char field_150;
	char field_151;
	char field_152;
	char field_153;
	char field_154;
	char field_155;
	char field_156;
	char field_157;
	char field_158;
	char field_159;
	char field_15A;
	char field_15B;
	char pKan_sub0;
	char field_15D;
	char field_15E;
	char field_15F;
	char pSeq_sub0;
	char field_161;
	char field_162;
	char field_163;
	char field_164;
	char field_165;
	char field_166;
	char field_167;
	char field_168;
	char field_169;
	char field_16A;
	char field_16B;
	__int16 Ccclut;
	__int16 Ccpage;
	char field_170;
	char field_171;
	char field_172;
	char field_173;
	char field_174;
	u8 Max_life;
	char field_176;
	char field_177;
	char field_178;
	char field_179;
	char field_17A;
	char field_17B;
	char field_17C;
	char field_17D;
	char field_17E;
	char field_17F;
} PLAYER_WORK;

typedef struct tagEspWork
{
	char Routine_0;
	char Routine_1;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char field_6;
	char field_7;
	WORD field_8;
	WORD field_A;
	char field_C;
	char field_D;
	WORD field_E;
	WORD field_10;
	WORD field_12;
	char field_14;
	char field_15;
	char Color;
	char field_17;
	WORD field_18;
	WORD field_1A;
	WORD field_1C;
	char field_1E;
	char field_1F;
	WORD Pos16_x;
	WORD Pos16_y;
	WORD Pos16_z;
	char field_26;
	char field_27;
	WORD Pos_x;
	WORD Pos_y;
	WORD Pos_z;
	WORD field_2E;
	MATRIX Mat;
	DWORD field_50;
	char field_54;
	char field_55;
	char field_56;
	char field_57;
	char field_58;
	char field_59;
	char field_5A;
	char field_5B;
	char field_5C;
	char field_5D;
	char field_5E;
	char field_5F;
	char field_60;
	char field_61;
	char field_62;
	char field_63;
	MATRIX *pSuper;
	DWORD field_68;
	DWORD field_6C;
	DWORD field_70;
	DWORD field_74;
	DWORD field_78;
	DWORD field_7C;
	DWORD field_80;
} ESP_WORK;

// starts at C33090
typedef struct tagGlobal
{
	u32 Status_flg,
		System_flg,
		Flg_4[2];
	u8 PSDIDX,
		ClearR,
		ClearG,
		ClearB,
		bC330A4,
		bC330A5;
	WORD wC330A6,
		wC330A8,
		wC330AA;
	u8 VSync_rate,
		bC330AD[3],
		C330B0,
		C330B1,
		Enemy_count;
	ESP_WORK Esp[64];
	PLAYER_WORK Pl_work;
	ENEMY_WORK Em_work[32];
	u8 bC384B4[48],
		bC384E4,
		bC384E5,
		bC384E6,
		bC384E7,
		bC384E8[3],
		bC384EB[2],
		bC384ED,
		bC384EE,
		bC384EF;
	u8 Card_header[512];
	// where saved data starts
	u8 Stage_no,
		Room_no,
		Cut_no,
		bC386F3,
		bC386F4,
		bC386F5,
		bC386F6,
		bC386F7,
		bC386F8,
		bC386F9,
		bC386FA,
		bC386FB,
		bC386FC,
		bC386FD,
		bC386FE,
		bC386FF;
	u32 dwC38700;
	u16 wC38704,
		wC38706,
		wC38708,
		wC3870A,
		wC3870C;
	s16 Pl_life;
	u16 Key,
		Key_trg;
	u32 dwC38714;
	u8 Save_count,
		bC38719,
		bC3871A,
		Pl_id;
	u16 Pl_pos_x,
		Pl_pos_z,
		Pl_cdir_y;
	u8 Pl_st_flg,
		bC38723;
	u32 Flg_1[8],
		Flg_2[2],
		Flg_3[8],
		Flg_6,
		dwC38770,
		dwC38774,
		dwC38778,
		dwC3877C,
		dwC38780,
		dwC38784,
		dwC38788,
		dwC3878C,
		Flg_0[4],
		Flg_7[5];
	ITEM_WORK Itembox_work[48],
		Item_work[12];
	u8 bC3882C[224];
	u16 Pad_custom[16];
	u8 Pad_config,
		bC3892D,
		bC3892E,
		bC3892F;
	u32 dwC38930,
		dwC38934[4],
		dwC389344,
		dwC389348[4],
		dwC389358,
		dwC38935C[4],
		dwC38936C,
		dwC389370[4],
		dwC389380,
		dwC389384[4],
		dwC389394,
		dwC389398[4];
	u8 Card_data[8192],
		bC3A9A8[128],
		bC3AA28[128],
		bC3AAA8[28];
	u32 dwC3AAC4[12],
		dwC3AAF4[5],
		dwC3AB08[3],
		dwC3AB14,
		dwC3AB18[2],
		dwC3AB20,
		dwC3AB24[3],
		dwC3AB30[4],
		dwC3AB40[2],
		dwC3AB48[4],
		dwC3AB58[2],
		dwC3AB60[3],
		dwC3AB6C,
		dwC3AB70,
		dwC3AB74[6],
		dwC3AB8C;
	u16 Flg_5,
		wC3AB92;
	u8 Ccpage,
		Ccclut,
		Ccpage_old,
		Ccclut_old,
		bC3AB98,
		bC3AB99;
	u16 wC3AB9A;
	u32 dwC3AB9C;
	RDT_HEADER *pRoom;	// C3ABA0
	ENEMY_WORK *pEm,
		*pEm_end;
	u8 *pBuffer;
	u32 dwC3ABB0[2],
		dwC3ABB8[2400],
		dwC3D138[2401];
	u16 wC3F6BC;
	u8 Esp_count;
} GLOBAL;


EXTERN_C GLOBAL G;
EXTERN_C _GsENV GsEnv;
EXTERN_C DWORD Game_timer;

EXTERN_C void __cdecl Cut_set();
EXTERN_C void __cdecl Snd_em(int snd, int s);
EXTERN_C void __cdecl Snd_se_on(int bank, int sound);
EXTERN_C u8   __cdecl Joint_move(int rewind, DWORD* pKan, DWORD* pSeq, u32 Hokan);
EXTERN_C void __cdecl Goto00(int rot);

EXTERN_C void __cdecl StMask(int allow, int wait);

EXTERN_C void __cdecl Title();

EXTERN_C void __cdecl Remove_item();

EXTERN_C u32 __cdecl Flg_ck(u32 *Flg, int bit);

EXTERN_C void Stage_init_items();

void Install_re1(u8 *pExe);

void LoadBoxes(u8 *src, size_t pos, size_t size);
void WriteBoxes(u8* &dst, size_t &size);
