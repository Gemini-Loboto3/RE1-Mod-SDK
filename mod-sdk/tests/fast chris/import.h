#pragma once
#include <stdafx.h>
//#include "Marni.h"
#include "re1.h"

EXTERN_C_START

extern HWND hWnd;
//extern jMARNI *pMarni;
extern int ExitAppFlg1,
	nPlayMovie,
	Movie_usevfw;
extern SPRT Mess_sprt;
extern TILE Marni_tile;
extern LINE Marni_line;

int  __cdecl Open_register_data();
int  __cdecl Load_register_data(const char *path);
int  __cdecl Save_register_data();
void __cdecl Init_system();
void __cdecl DisplayRects();
void __cdecl DriverNames();

u8*  __cdecl Get_item_name(int Id);
void __cdecl Draw_glyph(SPRT *s, int z);
int __cdecl Add_sprt(SPRT *s, int a2, int a3, int a4);
int __cdecl Add_line(LINE *t, int z);
int __cdecl Add_tile(TILE *t, int mode, int z);

void PlayMovie();
void DrawDummyFrame();

int   __cdecl pak_dec(u8 *pSrc, u8 *pDst);
int   __cdecl load_file(char *name, u8* pDst, int mode);
//DWORD __cdecl UploadTexture(jMARNI_SURFACE *pSurf, int mode);
void  __cdecl UnloadTexture(DWORD hTexture);
void  __cdecl Load_itemboxTim();

EXTERN_C_END
