#pragma once
#include <vector>

class CTim
{
public:
	CTim() { pixel = nullptr, clut = nullptr; }
	~CTim() { Release(); }

	size_t Open(u8 *buffer);
	void Create(int w, int h, int bpp, int pals);
	void Release();

	typedef struct tagTimHeader
	{
		u32 magic;
		u32 attr;
	} TIM_HEADER;

	typedef struct tagTimChunk
	{
		u32 nsize;	// size of the chunk + data
		u16 x, y,
			w, h;
	} TIM_CHUNK;

	u16 *clut;
	u8 *pixel;
	int pal_w, pal_h;
	int pix_w, pix_h;
	int bpp;
};

class CBitmap
{
public:
	CBitmap() { pData = nullptr; w = h = 0; }
	~CBitmap() { Trim(); }

	__inline void setPixel(size_t x, size_t y, u32 p)
	{
		// check boundaries
		if (x >= w || y >= h)
			return;

		pData[w * y + x] = p;
	}

	__inline u32 getPixel(size_t x, size_t y)
	{
		// check boundaries
		if (x >= w || y >= h)
			return 0xff000000;

		return pData[w * y + x];
	}

	__inline void fastPixel(u32 p)
	{
		*pDataSeek = p;
		pDataSeek++;
	}

	void Allocate(size_t w, size_t h);
	void Release();
	void Trim();

	size_t CreateFromTim(u8 *pTim, int cpal = 0);
	size_t CreateFromTim(CTim &pTim, int cpal);
	void   CreateFromBG(u16 *pBg);
	void   CreateFromPng(u8 *data, size_t w, size_t h);
	void   Create(size_t w, size_t h);

	void   Blt(CBitmap &src, int src_x, int src_y, int dst_x, int dst_y, int dst_w, int dst_h);
	void   SavePng(LPCSTR name);

	bool   IsOpen() { return pData != nullptr ? true : false; }

	size_t w, h;
	u32 *pData,
		*pDataSeek;
	u32 TextureHandle;

	typedef struct tagTimChunk
	{
		u32 size;	// size of the whole chunk
		u16 x, y;	// vram coordinates
		u16 w, h;	// size in vram
	} TIM_CHUNK;
};

class CBitmapVault
{
public:
	CBitmapVault() {}
	~CBitmapVault() { Release(); }

	void FromBuffer(u8 *pData, size_t size, bool bRelease = true);
	void FromPngPack(u8 *pData, size_t size, bool bRelease = true);
	__inline CBitmap* Pull()
	{
		if (current >= count)
			return nullptr;

		return pBmp[current++];
	}

	void Release();

	std::vector<CBitmap*> pBmp;
	size_t current, count;
};

class CEmBitmap
{
public:
	CEmBitmap();
	~CEmBitmap() { Release(); }

	void Reset();
	void OpenTim(int Id, u8 *pTim);
	void OpenPng(CBitmap &png);

	void Release();

	inline size_t Count() { return bmp.size(); }

	int Id;	// enemy id
	std::vector<CBitmap*> bmp;
};

void Convert_clut(u16 *clut, u32 *dst, size_t count);
u32  Convert_color(u16 clut);
void Set_color_mode(int mode);
//void RenderDeposterize(CBitmap &Src, CBitmap &Dst);
