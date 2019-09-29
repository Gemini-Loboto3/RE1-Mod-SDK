#pragma once
#include <vector>
#include <string>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char *LPSTR;
typedef const char *LPCSTR;

// utf8 conversion
typedef unsigned int ucs_char_t;
int u8_to_wchar_t(const char *src, ucs_char_t &ch);

typedef struct Vwf_c
{
	u8 i,
		w;
} Vwf_s;

typedef struct tagSymbol
{
	wchar_t ch;	// utf16 symbol
	ucs_char_t encode;	// what it encodes to
} SYMBOL;

// handler for custom encodings
class CEncoding
{
public:
	CEncoding();

	int  Find(ucs_char_t ch);
	void Set_encoding(LPCSTR filename);
	void Set_encoding(std::vector<SYMBOL> &sym_tbl);

	__inline int GetWidth(ucs_char_t ch);
	__inline int GetWidth(u8 ch)  { return width_tbl[ch].w; }

private:
	static bool sort_encode(SYMBOL &a, SYMBOL &b);

	std::vector<SYMBOL> table;
	std::vector<Vwf_s> width_tbl;
	int unk_sym;
};

extern CEncoding encoding;
