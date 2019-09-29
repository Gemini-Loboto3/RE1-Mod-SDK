#include "stdafx.h"
#include <string>

static const wchar_t encode[] =
{
	L" .@@@()@@«»@012345"	// 00
	L"6789:@,\"!?@ABCDEFG"	// 12
	L"HIJKLMNOPQRSTUVWXY"	// 24
	L"Z[/]'-·abcdefghijk"	// 36
	L"lmnopqrstuvwxyz@@@"	// 48
	L"@@@@@@@@@@@@@@@@@@"	// 5A
	L"@@@@@@@@ＳＴＡＲ“.@@@@"	// 6C
	L"@@@@@@@@@@@@@@@@@@"	// 7E
	L"@@@@@@@@@@@@@@@@@@"	// 90
	L"@@@@@@@@@@@@@@@@@@"	// A2
	L"@@@@@@@@@@@@@@@@@@"	// B4
	L"@@@@@@@@@@@@@@@@@@"	// C6
	L"@@@@@@@@@@@@@@@@@@"	// D8
	L"@@@@@@@@@@@@@@@@@@"	// EA
	L"&@@@"					// FC
};

size_t u8_wc_toutf8(char *dest, uint32_t ch);

std::string DecodeString(u8 *data)
{
	std::string str;
	char temp[32];

	while (1)
	{
		u8 c = *data++;

		switch (c)
		{
		case 0x01:
			if (*data != 0)
			{
				sprintf_s(temp, sizeof(temp), "{timed %d}", *data++);
				str += temp;
			}
			return str;
		case 0x02:
			str += "\\n";
			break;
		case 0x03:	// non-instant text scrolling
			sprintf_s(temp, sizeof(temp), "{clear %d}", *data++);
			str += temp;
			break;
		case 0x04:
			sprintf_s(temp, sizeof(temp), "{scroll %d}", *data++);
			str += temp;
			break;
		case 0x05:
			sprintf_s(temp, sizeof(temp), "{color %d}", *data++);
			str += temp;
			break;
		case 0x06:	// ??
			sprintf_s(temp, sizeof(temp), "{string %d}", *data++);
			str += temp;
			break;
		case 0x07:	// short EOS for items
			return str;
		case 0x08:	// non-instant text scrolling
			sprintf_s(temp, sizeof(temp), "{branch %d %d %d}", data[0], data[1], data[2]);
			str += temp;
			data += 3;
			break;
		case 0x0a:
			sprintf_s(temp, sizeof(temp), "{0a %d}", *data++);
			str += temp;
			break;
		case 0xf8:	// upper ranges
		case 0xf9:
		case 0xfa:
			str += encode[*data++ + 0xea] & 0xff;
			break;
		default:
			if (encode[c] == L'@')
			{
				sprintf_s(temp, sizeof(temp), "{0x%02X}", c);
				str += temp;
			}
			else
			{
				u8_wc_toutf8(temp, encode[c]);
				str += temp;
			}
		}
	}

	return str;
}

static LPCWSTR encode_eu =
{
	L"  @@@@@@@@@@012345"	// 00
	L"6789:;,\"!?@ABCDEFG"	// 12
	L"HIJKLMNOPQRSTUVWXY"	// 24
	L"Z(/)'-·abcdefghijk"	// 36
	L"lmnopqrstuvwxyzÄäÖ"	// 48
	L"öÜüßÀàÂâÈèÉéÊêÏïÎî"	// 5A
	L"ÔôÙùÛûÇçＳＴＡＲ“.…‒–+"	// 6C
	L"=&@@@@ÑñËë°ªÁáÍíÓó"	// 7E
	L"Úú¿¡ÌìÒò$*„‟`æ@@@@"	// 90
};

size_t u8_wc_toutf8(char *dest, uint32_t ch)
{
	if (ch < 0x80) {
		dest[0] = (char)ch;
		dest[1] = '\0';
		return 1;
	}
	if (ch < 0x800) {
		dest[0] = (ch >> 6) | 0xC0;
		dest[1] = (ch & 0x3F) | 0x80;
		dest[2] = '\0';
		return 2;
	}
	if (ch < 0x10000) {
		dest[0] = (ch >> 12) | 0xE0;
		dest[1] = ((ch >> 6) & 0x3F) | 0x80;
		dest[2] = (ch & 0x3F) | 0x80;
		dest[3] = '\0';
		return 3;
	}
	if (ch < 0x110000) {
		dest[0] = (ch >> 18) | 0xF0;
		dest[1] = ((ch >> 12) & 0x3F) | 0x80;
		dest[2] = ((ch >> 6) & 0x3F) | 0x80;
		dest[3] = (ch & 0x3F) | 0x80;
		dest[4] = '\0';
		return 4;
	}
	return 0;
}

std::string DecodeStringEU(u8 *data)
{
	std::string str;
	char temp[32];

	while (1)
	{
		u8 c = *data++;

		switch (c)
		{
		case 0xee:	// upper ranges
			str += encode[*data++ + 0xea] & 0xff;
			break;
		case 0xef:
		case 0xf0:
		case 0xf1:
		case 0xf2:
		case 0xf3:
		case 0xf4:
		case 0xf5:	// ??
			str += "{p}";
			break;
		case 0xf6:	// ??
			break;
		case 0xf7:	// short EOS for items
			return str;
		case 0xf8:
			sprintf_s(temp, sizeof(temp), "{string %d}", *data++);
			str += temp;
			break;
		case 0xf9:
			str += "{color ";
			str += *data + '0';
			str += "}";
			data++;
			break;
		case 0xfa:	// non-instant text scrolling
			sprintf_s(temp, sizeof(temp), "{scroll %d}", *data++);
			str += temp;
			break;
		case 0xfb:
			sprintf_s(temp, sizeof(temp), "{branch %d}", *data++);
			str += temp;
			break;
		case 0xfc:
			str += "\\n";
			break;
		case 0xfd:
			sprintf_s(temp, sizeof(temp), "{clear %d}", *data++ * 60 / 50);
			str += temp;
			break;
		case 0xfe:
			if (*data != 0)
			{
				sprintf_s(temp, sizeof(temp), "{timed %d}", *data * 60 / 50);
				str += temp;
			}
			return str;
		case 0xff: // ??
			break;
		default:
			u8_wc_toutf8(temp, encode_eu[c]);
			str += temp;
		}
	}

	return str;
}

std::string DecodeStringDS(u8* data)
{
	char temp[32];
	char ucs32[5];
	std::string str;

	while (1)
	{
		int ch = *data++;

		switch (ch)
		{
		case 0:
			return str;
		case 1:
			str += "\\n";
			break;
		case 3:
			sprintf_s(temp, 32, "{scroll %x}", *data++);
			str += temp;
			break;
		case 6:
			if (data[1] == 2)
			{
				sprintf_s(temp, 32, "{clear %d}", *data++);
				data++;
				str += temp;
				continue;
			}
			else if (*data != 0)
			{
				sprintf_s(temp, 32, "{timed %d}", *data++);
				str += temp;
			}
			break;
			//return str;
		case 7:
			//sprintf_s(temp, 32, "{07 %x}", *data++);
			//str += temp;
			data++;
			str += "{branch 0 1 0}";
			break;
		case 0x11:	// long characters
			switch (*data)
			{
			case 1:
				u8_wc_toutf8(ucs32, 0xff33);	// S
				str += ucs32;
				break;
			case 0x81:
				u8_wc_toutf8(ucs32, 0xff34);	// T
				str += ucs32;
				break;
			case 0x83:
				u8_wc_toutf8(ucs32, 0xff21);	// A
				str += ucs32;
				break;
			case 0x86:
				u8_wc_toutf8(ucs32, 0xff32);	// R
				str += ucs32;
				break;
			default:
				printf("What the deuce?\n");
			}
			data++;
			break;
		default:
			if (ch == 0x85)
				str += "...";
			else if (ch < 128)
				str += ch;
			else if (ch < 0xC0)
			{
				str += 0xc2;
				str += ch;
			}
			else
			{
				str += 0xc3;
				str += (ch - 0x40);
			}
		}
	}

	return str;
}
