#include <stdafx.h>
#include "Bitmap.h"
#include <assert.h>
#include "lodepng.h"

static u32 ToRGB888_psx(u16 c)
{
	int r, g, b, a;

	if (c == 0x8000)
	{
		r = 1;
		g = 0;
		b = 0;
	}
	else
	{
		r = c & 0x1f; r <<= 3;
		g = (c >> 5) & 0x1f; g <<= 3;
		b = (c >> 10) & 0x1f; b <<= 3;
	}
	a = c == 0 ? 0 : 0xff;

	return (r << 16) | (g << 8) | b | (a << 24);
}

static u32 ToRGB888_full(u16 c)
{
	int r, g, b, a;

	if (c == 0x8000)
	{
		r = 1;
		g = 0;
		b = 0;
	}
	else
	{
		r = c & 0x1f;         r = (r << 3) | (r >> 2);
		g = (c >> 5) & 0x1f;  g = (g << 3) | (g >> 2);
		b = (c >> 10) & 0x1f; b = (b << 3) | (b >> 2);
	}
	a = c == 0 ? 0 : 0xff;

	return (r << 16) | (g << 8) | b | (a << 24);
}

static u32 ToRGB888_full2(u16 c)
{
	int r, g, b, a;

	r = c & 0x1f;         r = (r << 8) * 255 / 248;
	g = (c >> 5) & 0x1f;  g = (g << 8) * 255 / 248;
	b = (c >> 10) & 0x1f; b = (b << 8) * 255 / 248;
	a = c == 0 ? 0 : 0xff;

	return (r << 16) | (g << 8) | b | (a << 24);
}

static u32 ToRGB888_ddraw(u16 c)
{
	int r, g, b, a;

	if (c == 0x8000)
	{
		r = 7;
		g = 0;
		b = 0;
	}
	else
	{
		r = c & 0x1f;         if (r > 0) r = r * 8 + 7;
		g = (c >> 5) & 0x1f;  if (g > 0) g = g * 8 + 7;
		b = (c >> 10) & 0x1f; if (b > 0) b = b * 8 + 7;
	}
	a = c == 0 ? 0 : 0xff;

	return (r << 16) | (g << 8) | b | (a << 24);
}

static u32(*fcolor[])(u16 color) =
{
	ToRGB888_psx,
	ToRGB888_full,
	ToRGB888_ddraw,
	ToRGB888_full2
};

static u32(*fcolorfnc)(u16 color);

//////////////////////////////
// pure TIM handler
size_t CTim::Open(u8 *pTim)
{
	Release();

	if (*(DWORD*)pTim != 0x10)
		return 0;

	TIM_CHUNK *pix;
	static int bpp_tbl[] = { 4, 8, 16, 24 };

	// has clut
	if (pTim[4] & 8)
	{
		TIM_CHUNK *pal = (TIM_CHUNK*)&pTim[8];
		u16 *_clut = (u16*)&pal[1];
		pix = (TIM_CHUNK*)&_clut[pal->w * pal->h];
		// copy over new palette
		clut = new u16[pal->w * pal->h];
		memcpy(clut, _clut, pal->w * pal->h * 2);
		// set clut data
		pal_w = pal->w;
		pal_h = pal->h;
	}
	// no clut
	else
	{
		clut = nullptr;
		pix = (TIM_CHUNK*)&pTim[8];
	}
	u8 *data = (u8*)&pix[1];

	bpp = bpp_tbl[pTim[4] & 7];
	pixel = new u8[pix->w * pix->h * 2];
	memcpy(pixel, data, pix->w * pix->h * 2);
	pix_w = pix->w;
	pix_h = pix->h;

	return (size_t)&data[pix->w * pix->h * 2] - (size_t)pTim;
}

void CTim::Release()
{
	if (pixel != nullptr)
	{
		delete[] pixel;
		pixel = nullptr;
	}

	if (clut != nullptr)
	{
		delete[] clut;
		clut = nullptr;
	}

	pal_w = pal_h = 0;
	pix_w = pix_h = 0;
}

void CTim::Create(int w, int h, int bpp, int pals)
{
	switch (bpp)
	{
	case 4:
		clut = new u16[pals * 16];
		pixel = new u8[w * h / 4];
		pal_w = 16;
		pal_h = pals;
		pix_w = w / 4;
		break;
	case 8:
		clut = new u16[pals * 256];
		pixel = new u8[w * h / 2];
		pal_w = 256;
		pal_h = pals;
		pix_w = w / 2;
		break;
	case 15:
		clut = NULL;
		pixel = new u8[w * h];
		pal_w = 0;
		pal_h = 0;
		pix_w = w;
		break;
	case 24:
		clut = NULL;
		pixel = new u8[w * h * 3 / 2];
		pal_w = 0;
		pal_h = 0;
		pix_w = w * 3 / 2;
		break;
	}

	pix_h = h;
	this->bpp = bpp;
}

//////////////////////////////
// bitmap
void CBitmap::Release()
{
	Trim();
	w = h = 0;
}

size_t CBitmap::CreateFromTim(u8 *pTim, int cpal)
{
	// this is rather hacking and assumes this is a 256 color TIM
	assert(pTim != nullptr);

	TIM_CHUNK *pix;
	u16 *clut = nullptr;
	
	// has clut
	if (pTim[4] & 8)
	{
		TIM_CHUNK *pal = (TIM_CHUNK*)&pTim[8];
		clut = (u16*)&pal[1];
		pix = (TIM_CHUNK*)&clut[pal->w * pal->h];
		// multiclut support
		clut = &clut[pal->w * cpal];
	}
	// no clut
	else pix = (TIM_CHUNK*)&pTim[8];
	u8 *data = (u8*)&pix[1];

	Release();
	switch (pTim[4] & 7)
	{
	case 0:	// 4bpp
		assert(clut != nullptr);
		Allocate(pix->w * 4, pix->h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x+=2, data++)
			{
				// lower nibble
				u16 c = clut[*data & 0xf];
				u32 p = fcolorfnc(c);
				fastPixel(p);
				// higher nibble
				c = clut[*data >> 4];
				p = fcolorfnc(c);
				fastPixel(p);
			}
		}
		break;
	case 1:	// 8bpp
		assert(clut != nullptr);
		Allocate(pix->w * 2, pix->h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				u16 c = clut[*data++];
				u32 p;
				p = fcolorfnc(c);
				fastPixel(p);
			}
		}
		break;
	case 2:	// 15bpp
		Allocate(pix->w, pix->h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++, data += 2)
			{
				//u16 c = clut[*(u16*)data];
				u16 c = *(u16*)data;
				if (cpal != 0 && c == 0x8000)
					c = 0;
				u32 p;
				p = fcolorfnc(c);
				fastPixel(p);
			}
		}
		break;
	case 3:	// 24bpp, not even used
		Allocate(pix->w * 2 / 3, pix->h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++, data += 3)
			{
				u32 p = data[2] | (data[1] << 8) | (data[0] << 16) | 0xff000000;
				fastPixel(p);
			}
		}
		break;
	}
	pDataSeek = pData;	// reset seek

	//RenderDeposterize(*this, *this);

	return (size_t)data - (size_t)pTim;
}

size_t CBitmap::CreateFromTim(CTim &Tim, int cpal)
{
	Release();

	u16 *clut = Tim.clut;
	u8  *data = Tim.pixel;

	switch (Tim.bpp)
	{
	case 4:		// 4bpp
		Allocate(Tim.pix_w * 4, Tim.pix_h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x += 2, data++)
			{
				// lower nibble
				u16 c = clut[*data & 0xf];
				u32 p = fcolorfnc(c);
				fastPixel(p);
				// higher nibble
				c = clut[*data >> 4];
				p = fcolorfnc(c);
				fastPixel(p);
			}
		}
		break;
	case 8:		// 8bpp
		Allocate(Tim.pix_w * 2, Tim.pix_h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				u16 c = clut[*data++];
				u32 p;
				p = fcolorfnc(c);
				fastPixel(p);
			}
		}
		break;
	case 16:	// 15bpp
		Allocate(Tim.pix_w, Tim.pix_h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++, data += 2)
			{
				//u16 c = clut[*(u16*)data];
				u16 c = *(u16*)data;
				if (cpal != 0 && c == 0x8000)
					c = 0;
				u32 p;
				p = fcolorfnc(c);
				fastPixel(p);
			}
		}
		break;
	case 24:	// 24bpp, not even used
		Allocate(Tim.pix_w * 2 / 3, Tim.pix_h);
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++, data += 3)
			{
				u32 p = data[2] | (data[1] << 8) | (data[0] << 16) | 0xff000000;
				fastPixel(p);
			}
		}
		break;
	}
	pDataSeek = pData;	// reset seek

	return 0;
}

void CBitmap::CreateFromBG(u16 *pBg)
{
	Release();
	Allocate(320, 240);

	// first segment is 256x256
	for (size_t y = 0; y < 256; y++)
	{
		for (size_t x = 0; x < 256; x++)
		{
			u32 p = *pBg++;
			p = fcolorfnc(p | 0x8000);
			setPixel(x, y, p);
		}
	}

	// second segment is 128x128
	for (size_t y = 0; y < 128; y++)
	{
		for (size_t x = 0; x < 128; x++)
		{
			u32 p = *pBg++;
			p = fcolorfnc(p | 0x8000);
			if (x < 64) setPixel(x + 256, y, p);
			else setPixel(x + 256 - 64, y + 128, p);
		}
	}

	//RenderDeposterize(*this, *this);
}

void CBitmap::CreateFromPng(u8 *data, size_t w, size_t h)
{
	Release();
	Allocate(w, h);
	for (size_t i = 0, si = w*h; i < si; i++, data += 4)
	{
		u32 p = (data[0] << 16) |	// red
			(data[1] << 8) |		// green
			data[2] |				// blue
			(data[3]) << 24;		// alpha
		if ((p & 0xffffff) == 0)
			p |= 0x000001;
		*pDataSeek++ = p;
	}
	pDataSeek = pData;
}

void CBitmap::Create(size_t w, size_t h)
{
	Release();
	Allocate(w, h);
}

void CBitmap::Allocate(size_t w, size_t h)
{
	Release();

	this->w = w;
	this->h = h;
	pData = new u32[w*h];
	pDataSeek = pData;
}

void CBitmap::Trim()
{
	if (pData != nullptr)
		delete[] pData;
	pData = nullptr;
}

void CBitmap::Blt(CBitmap &src, int src_x, int src_y, int dst_x, int dst_y, int dst_w, int dst_h)
{
	for (int y = 0; y < dst_h; y++)
		for (int x = 0; x < dst_w; x++)
			this->setPixel(dst_x + x, dst_y + y, src.getPixel(src_x + x, src_y + y));
}

void CBitmap::SavePng(LPCSTR name)
{
	u32 *image = new u32[this->w * this->h], *out = image;

	for (size_t y = 0; y < this->h; y++)
	{
		for (size_t x = 0; x < this->w; x++)
		{
			u32 p = this->getPixel(x, y);
			*out++ = ((p & 0xff) << 16) | ((p & 0xff0000) >> 16) | p & 0xff00ff00;
		}
	}

	lodepng::encode(name, (u8*)image, this->w, this->h, LCT_RGBA);

	delete[] image;
}

//////////////////////////////////////////
void CBitmapVault::FromBuffer(u8 *pData, size_t size, bool bRelease)
{
	if(bRelease)
		Release();

	size_t pos = 0;
	while (pos < size)
	{
		CBitmap *bmp = new CBitmap;
		if(*(u32*)&pData[pos] == 0x10)
			pos += bmp->CreateFromTim(&pData[pos]);
		else
		{
			bmp->CreateFromBG((u16*)&pData[pos]);
			//pos += (256 * 256 + 128 * 128) * 2;
			pos = size;	// interrupt loading
		}

		pBmp.push_back(bmp);
		count++;
	}
}

void CBitmapVault::FromPngPack(u8 *pData, size_t size, bool bRelease)
{
	if (bRelease)
		Release();

	// regular png
	if (*(u32*)pData == MAGIC(0x89, 'P', 'N', 'G'))
	{
		u8 *out;
		unsigned int w, h;
		lodepng_decode32(&out, &w, &h, pData, size);
		CBitmap *bmp = new CBitmap;
		bmp->CreateFromPng(out, w, h);
		free(out);

		pBmp.push_back(bmp);
		count++;
	}
	// png package
	else
	{
		u32 *ptr = (u32*)pData;
		for (size_t i = 0, si = ptr[0] / 8; i < si; i++, ptr += 2)
		{
			u8 *out;
			unsigned int w, h;
			lodepng_decode32(&out, &w, &h, &pData[ptr[0]], ptr[1]);
			CBitmap *bmp = new CBitmap;
			bmp->CreateFromPng(out, w, h);
			free(out);

			pBmp.push_back(bmp);
			count++;
		}
	}
}

void CBitmapVault::Release()
{
	while (!pBmp.empty())
	{
		delete pBmp.back();
		pBmp.pop_back();
	}
	count = 0;
	current = 0;
}

//////////////////////////////////////////
CEmBitmap::CEmBitmap() {}
//CEmBitmap::~CEmBitmap() {}

void CEmBitmap::Reset() {}

typedef struct tagEmBmp
{
	int count;
	u8 *data;
} EM_BMP;

void CEmBitmap::OpenTim(int Id, u8 *pTim)
{
	size_t cnt;

	// has clut
	if (pTim[4] & 8)
	{
		CBitmap::TIM_CHUNK *pal = (CBitmap::TIM_CHUNK*)&pTim[8];
		cnt = pal->h;
	}
	// doesn't have clut, should never happen
	else cnt = 1;

	// create a number of full textures for entities
	std::vector<CBitmap*> dump;
	for (size_t i = 0; i < cnt; i++)
	{
		CBitmap *b = new CBitmap;
		b->CreateFromTim(pTim, i);
		dump.push_back(b);
	}

	CBitmap *canvas = nullptr;

	// birkin 5
	if (Id == 0x36)
	{
		canvas = new CBitmap;
		canvas->Create(256, 256);
		canvas->Blt(*dump[0], 0, 0, 0, 0, 256, 256);			// first 256x256 texture
		bmp.push_back(canvas);

		canvas = new CBitmap;
		canvas->Create(256, 256);
		canvas->Blt(*dump[1], 256, 0, 0, 0, 128, 256);			// second 128x256 texture
		canvas->Blt(*dump[2], 256 + 128, 0, 128, 0, 128, 256);	// third 128x256 texture
		bmp.push_back(canvas);
	}
	// non special cases
	else
	{
		int i = 0;
		for (size_t x = 0; x < cnt * 128; x += 128, i++)
		{
			if(canvas == nullptr)
			{
				canvas = new CBitmap;
				canvas->Create(256, 256);
			}
			// copy a 128x256 page as a slice
			canvas->Blt(*dump[i], x, 0, x % 256, 0, 128, 256);
			// push every 2 128x256 pages
			if ((i % 2) == 1)
			{
				bmp.push_back(canvas);
				canvas = nullptr;
			}
		}
		// push any incomplete textures
		if ((i % 2) == 1)
			bmp.push_back(canvas);
	}

#if 0
	for (size_t i = 0, si = bmp.size(); i < si; i++)
	{
		char path[MAX_PATH];
		sprintf_s(path, sizeof(path), "em%02d.png", i);
		bmp[i]->SavePng(path);
	}
#endif

	// clear garbage
	while (!dump.empty())
	{
		delete dump.back();
		dump.pop_back();
	}
}

#define align(x, y)		((x + y - 1) / y * y)

void CEmBitmap::OpenPng(CBitmap &png)
{
	int count = align(png.w, 256) / 256;

	for (int i = 0; i < count; i++)
	{
		CBitmap *canvas = new CBitmap;
		canvas->Create(256, 256);
		canvas->Blt(png, i * 256, 0, 0, 0, 256, 256);
		bmp.push_back(canvas);
	}
}

void CEmBitmap::Release()
{
	while (!bmp.empty())
	{
		delete bmp.back();
		bmp.pop_back();
	}
}

//////////////////////////////////////////
void Convert_clut(u16 *clut, u32 *dst, size_t count)
{
	for (size_t i = 0; i < count; i++)
		dst[i] = fcolorfnc(clut[i]);
}

u32 Convert_color(u16 clut)
{
	return fcolorfnc(clut);
}

void Set_color_mode(int mode)
{
	fcolorfnc = fcolor[mode];
}
