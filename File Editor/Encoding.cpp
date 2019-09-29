#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <algorithm>
#include "Encoding.h"
#include "tinyxml2.h"

using namespace tinyxml2;

int u8_to_wchar_t(const char *src, ucs_char_t &ch)
{
	u8 *s = (u8*)src;

	/* 0xxxxxxx */
	if (*s < 0x80)
	{
		ch = *s;
		return 1;
	}
	/* 110XXXXx 10xxxxxx */
	else if((*s & 0xe0) == 0xc0)
	{
		ch = ((s[0] & 0x1f) << 6) | (s[1] & 0x3f);
		return 2;
	}
	/* 1110XXXX 10Xxxxxx 10xxxxxx */
	else if((s[0] & 0xf0) == 0xe0)
	{
		ch = ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f);
		return 3;
	}
	/* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
	else if ((s[0] & 0xf8) == 0xf0)
	{
		ch = ((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
		return 4;
	}

	return 1;
}

// build a default table
CEncoding::CEncoding()
{
}

bool CEncoding::sort_encode(SYMBOL &a, SYMBOL &b)
{
	return a.ch < b.ch;
}

int CEncoding::Find(ucs_char_t ch)
{
	size_t size = table.size();
	int first = 0,
		last = (int)size - 1,
		middle = last / 2;

	while (first <= last)
	{
		if (table[middle].ch < ch)
			first = middle + 1;
		else if (table[middle].ch == ch)
			return table[middle].encode;
		else last = middle - 1;

		middle = (first + last) / 2;
	}

	return unk_sym;
}

void CEncoding::Set_encoding(LPCSTR filename)
{
	XMLDocument xml;
	xml.LoadFile(filename);

	auto root = xml.RootElement();
	if (root == nullptr || strcmp("Encoding", root->Value()) != 0)
		return;

	int w, in;
	w = root->IntAttribute("width", 8);
	in = root->IntAttribute("indent", 0);

	auto s = root->FirstChildElement("Entry");
	std::vector<SYMBOL> tbl;
	while (s)
	{
		SYMBOL sym;
		ucs_char_t t;
		const char *str;
		s->QueryStringAttribute("Char", &str);
		u8_to_wchar_t(str, t);
		sym.ch = (wchar_t)t;
		sym.encode = s->IntAttribute("Encode", 0);

		s = s->NextSiblingElement("Entry");
		tbl.push_back(sym);
	}
	Set_encoding(tbl);

	// build width table
	ucs_char_t highest = 0;
	for (size_t i = 0, si = tbl.size(); i < si; i++)
	{
		if ((tbl[i].encode >> 8) >= 0xee) continue;
		if (tbl[i].encode > highest)
			highest = tbl[i].encode;
	}

	width_tbl.resize(highest + 1);
	// fill all entries, just in case
	for (size_t i = 0, si = tbl.size(); i < si; i++)
	{
		width_tbl[i].w = w;
		width_tbl[i].i = in;
	}

	s = root->FirstChildElement("Entry");
	while (s)
	{
		const char *str;
		ucs_char_t t;
		s->QueryStringAttribute("Char", &str);
		u8_to_wchar_t(str, t);
		int encode = encoding.Find(t);
		if ((encode >> 8) >= 0xEE) continue;

		width_tbl[encode].w = s->IntAttribute("width", w);
		width_tbl[encode].i = s->IntAttribute("indent", in);

		s = s->NextSiblingElement("Entry");
	}
}

void CEncoding::Set_encoding(std::vector<SYMBOL> &sym_tbl)
{
	table = sym_tbl;
	std::sort(table.begin(), table.end(), sort_encode);
	unk_sym = Find(L'?');
}

CEncoding encoding;

u16 charToSymbol(ucs_char_t c)
{
	return encoding.Find(c);
}

size_t EncodeString(LPCSTR str, u8* dst, int alt_end)
{
	size_t len = strlen(str),
		elen = 0;
	bool warning = false;

	for (size_t i = 0; i < len; i++)
	{
		char c = str[i];

		// tags
		if (c == '{')
		{
			size_t j;
			std::string tag;
			warning = true;	// enable warning if no proper close tag is found
			for (j = i + 1; j < len; j++)
			{
				c = str[j];
				// detected actual end to this
				if (c == '}')
				{
					warning = false;
					break;
				}
				tag += c;
			}
			if (!warning)
			{
				i = j;
				u32 val, val1, val2;
				const char *tags = tag.c_str();
				if (sscanf(tags, "string %d", &val) == 1)
				{
					dst[elen++] = 0x06;
					dst[elen++] = val & 0xff;
				}
				else if (sscanf(tags, "color %d", &val) == 1)
				{
					dst[elen++] = 0x05;
					dst[elen++] = val & 0xff;
				}
				else if (sscanf(tags, "scroll %d", &val) == 1)
				{
					dst[elen++] = 0x04;
					dst[elen++] = val & 0xff;
				}
				else if (sscanf(tags, "branch %d %d %d", &val, &val1, &val2) == 3)
				{
					dst[elen++] = 0x08;
					dst[elen++] = val & 0xff;
					dst[elen++] = val1 & 0xff;
					dst[elen++] = val2 & 0xff;
				}
				else if (sscanf(tags, "clear %d", &val) == 1)
				{
					dst[elen++] = 0x03;
					dst[elen++] = val & 0xff;
				}
				else if (sscanf(tags, "timed %d", &val) == 1)
				{
					dst[elen++] = 0x01;
					dst[elen++] = val & 0xff;
					return elen;
				}
				else if (sscanf(tags, "tab %d", &val) == 1)
				{
					dst[elen++] = 0xfc;
					dst[elen++] = val & 0xff;
				}
				else if (strcmp(tags, "center") == 0)
				{
					dst[elen++] = 0xfd;
				}
				else if (strcmp(tags, "s") == 0)
				{
					dst[elen++] = 0xfb;
				}
			}
			// no tag has been detected, parse as '?'
			else
			{
				dst[elen++] = charToSymbol('{') & 0xff;
			}
		}
		else
		{
			if (c == '\r');
			else if (c == '\n')
				dst[elen++] = 0x02;
			else
			{
				ucs_char_t t;
				i += u8_to_wchar_t(&str[i], t) - 1;
				int sym = charToSymbol(t /*& 0xffff*/);

				//if (c == '\xC2')
				//	c = str[++i];

				//u16 sym = charToSymbol(c);
				// byte symbol
				if ((sym >> 8) < 0xF8)
					dst[elen++] = sym & 0xff;
				// wide symbol
				else
				{
					dst[elen] = sym >> 8;
					dst[elen + 1] = sym & 0xff;
					//dst[elen] = 0xee;
					//dst[elen + 1] = sym - 0xea;
					elen += 2;
				}
			}
		}
	}

	// string terminator
	if (alt_end)
		dst[elen++] = 0x07;
	else
	{
		dst[elen++] = 0x01;
		dst[elen++] = 0x00;
	}

	return elen;
}

