// unpacker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdafx.h>
#include <iostream>
#include "depack_pak.h"
#include "Bitmap.h"
#include "mdec\bs.h"
#include "int24.h"
#include "RoomRE1.h"
#include "tinyxml2.h"
#include "re1.h"

void StringsToXml(std::vector<std::string> &str, LPCSTR out_name);

void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

void ListFiles(LPCSTR folder, LPCSTR filter, std::vector<std::string> &names)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char szDir[MAX_PATH];
	sprintf(szDir, "%s\\%s", folder, filter);
	WIN32_FIND_DATA ffd;

	// find the first file in the directory
	hFind = FindFirstFileA(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
		return;

	// list all mod files in the main folder
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		names.push_back(ffd.cFileName);
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
}

void Extract_pak(LPCSTR filename, LPCSTR outname, int ck_bg = 0)
{
	u8 *dst;
	int size;

	MEM_STREAM str;
	MemStreamOpen(&str, filename);
	if (str.data == NULL)
		return;
	pak_depack(&str, &dst, &size);

	if (ck_bg && (/*size != 153620 ||*/ *(DWORD*)&dst[8] < 153600))
	{
		*(WORD*)&dst[0x10] = 316;
		*(WORD*)&dst[0x12] = 236;
	}

	CBitmap bmp;
	bmp.CreateFromTim(dst, 0);
	bmp.SavePng(outname);

	//if (ck_bg)
	//{
	//	std::string o = outname;
	//	o += ".tim";
	//	FILE *f = fopen(o.c_str(), "wb+");
	//	fwrite(dst, size, 1, f);
	//	fclose(f);
	//}

	free(dst);

	MemStreamClose(&str);
}

void Extract_bgs()
{
	char path[MAX_PATH],
		out[MAX_PATH];

	for (int Stage = 0; Stage < 5; Stage++)
	{
		for (int Room = 0; Room < 49; Room++)
		{
			for (int Cut = 0; Cut < 8; Cut++)
			{
				sprintf(path, "D:\\Program Files\\RESIDENT EVIL\\JPN\\STAGE%X\\RC%X%02X%X.pak", Stage + 1, Stage + 1, Room, Cut);
				sprintf(out, "ROOM\\ROOM_%X%02X_%02d.png", Stage + 1, Room, Cut);
				Extract_pak(path, out, 1);

				sprintf(path, "D:\\Program Files\\RESIDENT EVIL\\JPN\\objspr\\OSP0%d%02d%d.pak", Stage, Room, Cut);
				sprintf(out, "ROOM\\ROOM_%X%02X_%02d_mask.png", Stage + 1, Room , Cut);
				Extract_pak(path, out);
			}
		}
	}
}

void Convert_file(LPCSTR in_file, LPCSTR out_file)
{
	CBufferFile f(in_file);
	u16 *palette = (u16*)f.GetBuffer();
	u8 *pix = (u8*)&palette[32];

	CBitmap bmp;
	bmp.Create(512, 192);
	for (int i = 0; i < 256 * 192; i++, pix++)
	{
		*bmp.pDataSeek++ = Convert_color(palette[*pix & 0xf]);
		*bmp.pDataSeek++ = Convert_color(palette[*pix >> 4 ]);
	}
	//bmp.SavePng(out_file);

	CBitmap slice;
	// slice 1
	slice.Create(256, 192);
	slice.Blt(bmp, 0, 0, 0, 0, 256, 192);
	slice.SavePng((std::string(out_file) + "0.png").c_str());
	// slice 2
	slice.Blt(bmp, 256, 0, 0, 0, 256, 192);
	slice.SavePng((std::string(out_file) + "1.png").c_str());
}

void Extract_files(LPCSTR folder_in, LPCSTR folder_out)
{
	std::vector<std::string> list;
	ListFiles(folder_in, "FILEM_*.PIX", list);

	for (size_t i = 0, si = list.size(); i < si; i++)
	{
		std::string out = list[i];
		ReplaceStringInPlace(out, ".PIX", "");
		out = std::string(folder_out) + "\\" + out;
		Convert_file((std::string(folder_in) + std::string("\\") + list[i]).c_str(), out.c_str());
	}
}

void Extract_bs(LPCSTR in_name, LPCSTR out_name)
{
	MEM_STREAM s;
	MemStreamOpen(&s, in_name);
	if (s.data == NULL)
		return;
	char name[MAX_PATH];
	u16* out = new u16[320 * 240];

	//PsxMdecIDCT_double idct;
	//MdecInputStream input;
	//MdecDecoder_double mdec(idct, 320, 240);
	//mdec.decode(input);

	//CBitmap bmp;
	//bmp.Create(320, 240);
	//for (int i = 0, count = s.size / (32 * 1024); i < count; i++)
	//{
	//	bs_decode_rgb24(out, (bs_header_t*)&s.data[i * 32 * 1024], 320, 240, NULL);
	//	Int24 *o24 = (Int24*)out;
	//	//u16 *o16 = out;
	//	for (int y = 0; y < 240; y++)
	//	{
	//		for (int x = 0; x < 320; x++)
	//		{
	//			u32 p = Convert_color(*o16++) | 0xff000000;
	//			//u32 p = ((int) *o24++) & 0xffffff;
	//			//p = (p >> 16) | ((p & 0x0000ff) << 16) | (p & 0x00ff00);
	//			bmp.setPixel(x, y, p | 0xff000000);
	//		}
	//	}
	//	sprintf_s(name, MAX_PATH, "%s_%02d.png", out_name, i);
	//	bmp.SavePng(name);
	//}
	delete[] out;
	MemStreamClose(&s);
}

void Extract_rdt(LPCSTR in_name, LPCSTR out_name)
{
	RE1::CRoom rdt;
	if(rdt.Open(in_name))
		rdt.DumpMaskBitmap(out_name);
}

std::string DecodeString(u8 *data);

#define EXE_DIFF	0x401000

void Extract_strings(u8 *pExe, int ptr_pos, int cnt, LPCSTR outname)
{
	u32 *ptr = (u32*)&pExe[ptr_pos - EXE_DIFF];
	std::vector<std::string> str;

	for (int i = 0; i < cnt; i++)
		str.push_back(DecodeString(&pExe[ptr[i] - EXE_DIFF]));
	StringsToXml(str, outname);
}

void Extract_bgs_psx()
{
	char path[MAX_PATH],
		out[MAX_PATH];

	//bs_init();

	for (int Stage = 0; Stage < 5; Stage++)
	{
		for (int Room = 0; Room < 49; Room++)
		{
			sprintf(path, "dc\\ROOM%X%02X.BSS", Stage + 8, Room);
			sprintf(out, "ROOM_dc\\ROOM_%X%02X", Stage + 1, Room);
			Extract_bs(path, out);

			sprintf(path, "dc\\ROOM%X%02X0.RDT", Stage + 8, Room);
			sprintf(out, "ROOM_dc\\ROOM_%X%02X", Stage + 1, Room);
			Extract_rdt(path, out);
		}
	}
}

using namespace tinyxml2;

void StringsToXml(std::vector<std::string> &str, LPCSTR out_name)
{
	XMLDocument xml;
	if (str.size() == 0) return;
	xml.SetBOM(true);

	XMLElement *items = xml.NewElement("Strings");
	// extract text
	for (size_t i = 0, si = str.size(); i < si; i++)
	{
		XMLElement *sub = xml.NewElement("Text");
		sub->SetText(str[i].c_str());
		items->InsertEndChild(sub);
	}

	xml.InsertEndChild(items);
	xml.SaveFile(out_name);
}

void Extract_text(LPCSTR rdt_name, LPCSTR xml_name)
{
	CBufferFile rdt(rdt_name);
	if (rdt.data == nullptr) return;
	RE1::RDT_HEADER *h = (RE1::RDT_HEADER*)rdt.GetBuffer();
	if (h->pMessage == 0) return;

	u16 *ptr = (u16*)&rdt.GetBuffer()[h->pMessage];
	std::vector<std::string> str;
	for (int i = 0, si = ptr[0] / 2; i < si; i++)
		str.push_back(DecodeString(&((u8*)ptr)[ptr[i]]));

	StringsToXml(str, xml_name);
}

void Extract_messages(LPCSTR in_folder, LPCSTR out_folder)
{
	char path[MAX_PATH], xml[MAX_PATH];
	for (int Stage = 1; Stage < 8; Stage++)
	{
		for (int Room = 0; Room < 49; Room++)
		{
			//for (int i = 0; i < 2; i++)
			{
				//if (Stage == 4 && Room == 5)
				//	Stage = 4;
				sprintf_s(path, MAX_PATH, "%s\\STAGE%d\\ROOM%x%02x0.RDT", in_folder, Stage, Stage, Room);
				sprintf_s(xml, MAX_PATH, "%s\\ROOM_%x%02x.xml", out_folder, Stage, Room);
				Extract_text(path, xml);
			}
		}
	}
}

///////////////////////////////////////////////////
std::string NameToID(std::string &str)
{
	if (str.size() == 0)
		return "NOTHING";

	std::string out = "ID_";
	for (size_t i = 0, si = str.size(); i < si; i++)
	{
		char c = str[i];
		if (c == ' ')
			out += '_';
		else if (c == '.' ||
			c == '-');
		else if (c == '&')
			out += '*';
		else out += (char)toupper(str[i]);
	}

	return out;
}

void FixDuplicates(std::vector<std::string> &str)
{
	for (size_t i = 0, si = str.size(); i < si; i++)
	{
		std::string s = str[i];
		int dup = 1;
		for (size_t j = i + 1; j < si; j++)
		{
			if (s == str[j])
			{
				str[j] += "_";
				str[j] += dup + '0';
				dup++;
			}
		}
		if (dup != 1)
			str[i] = s + "_0";
	}
}

LPCSTR item_names[] =
{
	"-Nothing-",		// 00
	"Combat Knife",		// 01
	"Beretta",			// 02
	"Shotgun",			// 03
	"Colt Python",		// 04
	"Colt Python",		// 05
	"Flamethrower",		// 06
	"Grenade Gun",		// 07
	"Grenade Gun",		// 08
	"Grenade Gun",		// 09
	"Rocket Launcher",	// 0A
	"Hand Gun Bullets",	// 0B
	"Shotgun Shells",	// 0C
	"DumDum Bullets",	// 0D
	"Magnum Bullets",	// 0E
	"Fuel",				// 0F
	"Grenade Rounds",	// 10
	"Acid Rounds",		// 11
	"Flame Rounds",		// 12
	"Empty Bottle",		// 13
	"Water",			// 14
	"UMB No.2",
	"UMB No.4",
	"UMB No.7",
	"UMB No.13",
	"Yellow-6",
	"NP-003",
	"V-Jolt",
	"Broken Shotgun",
	"Square Crank",
	"Hexagonal Crank",
	"Emblem",
	"Gold Emblem",		// 20
	"Blue Jewel",
	"Red Jewel",
	"Music Score",
	"Wolf Medal",
	"Eagle Medal",
	"Herbicide",
	"Battery",
	"MO-Disk",
	"Wind Crest",
	"Flare",
	"Slides",
	"Moon Crest",
	"Star Crest",
	"Sun Crest",
	"Ink Ribbon",
	"Lighter",			// 30
	"Lockpick",
	"",
	"Sword Key",
	"Armor Key",
	"Shield Key",
	"Helmet Key",
	"Master Key",
	"Closet Key",
	"Key for Room 002",
	"Key for Room 003",
	"Control Room Key",
	"Power Room Key",
	"Desk Key",
	"Blank Book",
	"Doom Book 2",
	"Doom Book 1",		// 40
	"First Aid Spray",
	"Serum",
	"Red Herb",
	"Green Herb",
	"Blue Herb",
	"Mixed Herbs",
	"Mixed Herbs",
	"Mixed Herbs",
	"Mixed Herbs",
	"Mixed Herbs",
	"Mixed Herbs",
	"",
	"Comm. Radio",
	"",
	"",
	"",					// 50
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Researcher's Will",
	"Researcher's Will",
	"Keeper's Diary",
	"Orders",
	"Pass Number",
	"Plant-42 Report",
	"Fax",
	"Scrapbook",
	"Security System",
	"Researcher's Letter",
	"V-Jolt Report",
	"Barry's Picture",
	"Pass Code 01",
	"Pass Code 02",
	"Pass Code 03",
	"Botany Book",
	"Ingram",
	"Minimi",
	"Crank",
	"Crank",
	"Chemical",
	"Mansion Key",
	"Mansion Key",
	"Mansion Key",
	"Mansion Key",
	"Laboratory Key",
	"Special Key",
	"Guardhouse Key",
	"Guardhouse Key",
	"Laboratory Key",
	"Small Key",
	"Red Book",
	"Doom Book 2",
	"Doom Book 1",
};

#define EXE_DIFF_J	0x402600

void ExtractItems(u8 *pExe)
{
	const int item_cnt = 77 + 1,
		file_start = 94 + 1,
		file_end = 110 + 1,
		alt_start = 112 + 1;

	XMLDocument xml;
	xml.SetBOM(true);

	ITEM_DATA *data = (ITEM_DATA*)&pExe[0x4CCCBC - EXE_DIFF_J];
	MIX_DATA **pmix = (MIX_DATA**)&pExe[0x4CCC08 - EXE_DIFF_J];
	// fix pmix pointers
	for (int i = 0; i < 35; i++)
	{
		DWORD dword = (DWORD)pmix[i];
		dword = (DWORD)&pExe[dword - EXE_DIFF_J];
		pmix[i] = (MIX_DATA*)dword;
	}

	std::vector<std::string> item_id;
	for (int i = 0; i < _countof(item_names); i++)
	{
		std::string s = item_names[i];
		item_id.push_back(NameToID(s));
	}
	FixDuplicates(item_id);

	static LPCSTR mix_type[] =
	{
		"IM_COMMON",
		"IM_RELOAD_0",
		"IM_RELOAD_1",
		"IM_AMMO",
		"IM_VJOLT_0",
		"IM_VJOLT_1",
		"IM_GRENADE_0",
		"IM_GRENADE_1"
	};
	//static FILE_DATA file_page[] =
	//{
	//	5,0x70, 5,0x50, 1,0x50, 2,0x50,
	//	9,0x50, 9,0x50, 9,0x50, 6,0x70,
	//	4,0x70, 6,0x50, 1,0x80, 2,0x90,
	//	3,0x90, 2,0x90, 3,0x70, 8,0x50,
	//	6,0x50, 9,0x50, 5,0x50, 1,0x90,
	//	6,0x50, 5,0x50, 4,0x60, 15,0x50,
	//	18,0x50
	//};

	XMLElement *items = xml.NewElement("Items");
	for (int i = 0; i < _countof(item_names); i++)
	{
		XMLElement *el = xml.NewElement("Item");
		std::string s = item_names[i];
		el->SetAttribute("name", s.c_str());
		// regular items
		if (i < item_cnt)
		{
			el->SetAttribute("id", item_id[i].c_str());
			XMLElement *sub = xml.NewElement("Data");
			sub->SetAttribute("max", data[i].Max);
			sub->SetAttribute("icon", data[i].Icon);
			if(!(data[i].Alt_name & 0x80) && i != 0)
				el->SetAttribute("alt_name", item_id[data[i].Alt_name + alt_start].c_str());
			if (!(data[i].Mix_num & 0x80) && i != 0)
			{
				MIX_DATA *mix = pmix[data[i].Mix_num];
				for (int j = 0, js = mix->count; j < js; j++)
				{
					XMLElement *x = xml.NewElement("Mix");
					x->SetAttribute("with", item_id[mix->Data[j].With].c_str());
					x->SetAttribute("result", item_id[mix->Data[j].Result].c_str());
					x->SetAttribute("reminder", item_id[mix->Data[j].Remain].c_str());
					x->SetAttribute("type", mix_type[mix->Data[j].Type]);
					sub->InsertEndChild(x);
				}
			}
			el->InsertEndChild(sub);
		}
		// FILEs
		else if (i >= file_start && i < file_end)
		{
			//XMLElement *sub = xml.NewElement("File");
			//sub->SetAttribute("pages", file_page[i - 104].Page_max);
			//sub->SetAttribute("back_h", file_page[i - 104].Back_h);
			//el->InsertEndChild(sub);
		}
		// alternate names for keys and some items
		else if (i >= alt_start)
		{
			el->SetAttribute("id", item_id[i].c_str());
		}
		items->InsertEndChild(el);
	}
	xml.InsertEndChild(items);
	xml.SaveFile("item.xml");
}

typedef struct tagWeaponAt
{
	s16 coord[3];
	WORD damage;
	u8 data[3],
		pad;
} WEAPON_AT;

typedef struct tagWeaponAtTbl
{
	WEAPON_AT data[10];
} WEAPON_AT_TBL;

void Extract_weapon_tbl(const u8 *pExe, u32 ptr, LPCSTR out_name)
{
	WEAPON_AT_TBL *wp_tbl0 = (WEAPON_AT_TBL*)&pExe[ptr];

	static LPCSTR wp_name[] =
	{
		"Knife",	"Handgun",	"Shotgun",	"Magnum",
		"Dumdum",	"Fuel",		"Explosive","Acid",
		"Flame",	"Rocket"
	};
	static LPCSTR em_name[]=
	{
		"Zombie",	"Desnudo",	"Cerberus",	"Spinner",
		"Tiger",	"Crow",		"Hunter",	"Bee",
		"Plant42",	"Chimera",	"Snake",	"Neptune",
		"Tyrant",	"Yawn 1",	"Root",		"Tentacle",
		"S-Tyrant",	"Zombie",	"Yawn 2",	"Web"
	};

	XMLDocument xml;
	xml.SetBOM(true);

	XMLElement *damage = xml.NewElement("Damage");
	for (int i = 0; i < 20; i++)
	{
		auto em = xml.NewElement("Enemy");
		for (int j = 0; j < 10; j++)
		{
			auto wp = xml.NewElement("Weapon");
			WEAPON_AT *w = &wp_tbl0[i].data[j];
			wp->SetAttribute("r", w->coord[0]);
			wp->SetAttribute("h", w->coord[1]);
			wp->SetAttribute("dmg_0", w->damage);
			wp->SetAttribute("dmg_1", w[20 * 10].damage);
			wp->SetAttribute("timer_0", w->data[0]);
			wp->SetAttribute("timer_1", w->data[1]);
			wp->SetAttribute("timer_2a", w->data[2]);
			wp->SetAttribute("timer_2b", w[20 * 10].data[2]);
			em->InsertEndChild(xml.NewComment(wp_name[j]));
			em->InsertEndChild(wp);
		}
		damage->InsertEndChild(xml.NewComment(em_name[i]));
		damage->InsertEndChild(em);
	}
	xml.InsertEndChild(damage);
	xml.SaveFile(out_name);
}

typedef struct tagEspTbl
{
	u8 Data[8][32][4];
} ESP_TBL;

void Extract_esp_tbl(u8 *pExe, LPCSTR out_name)
{
	ESP_TBL *tbl = (ESP_TBL*)&pExe[0x4C7CD0 - EXE_DIFF_J];
	XMLDocument xml;
	xml.SetBOM(true);

	XMLElement *esp = xml.NewElement("Esp");
	char str[32];
	for (int Stage = 0; Stage < 7; Stage++)
	{
		auto st = xml.NewElement("Stage");
		for (int Room = 0; Room < 32; Room++)
		{
			sprintf_s(str, 32, "%X%02X", Stage + 1, Room);
			st->InsertEndChild(xml.NewComment(str));
			auto rm = xml.NewElement("Room");
			if (tbl->Data[Stage][Room][0] != 0xff) rm->SetAttribute("0", tbl->Data[Stage][Room][0]);
			if (tbl->Data[Stage][Room][1] != 0xff) rm->SetAttribute("1", tbl->Data[Stage][Room][1]);
			if (tbl->Data[Stage][Room][2] != 0xff) rm->SetAttribute("2", tbl->Data[Stage][Room][2]);
			if (tbl->Data[Stage][Room][3] != 0xff) rm->SetAttribute("3", tbl->Data[Stage][Room][3]);
			st->InsertEndChild(rm);
		}
		esp->InsertEndChild(st);
	}
	xml.InsertEndChild(esp);
	xml.SaveFile(out_name);
}

int main()
{
	Set_color_mode(0);

	//Extract_files("file", "file_png");

	//CBufferFile exe("NEWEUR.EXE");
	//Extract_strings(exe.GetBuffer(), 0x4BFC70, 63, "misc.xml");
	//Extract_strings(exe.GetBuffer(), 0x4C6170, 79, "desc.xml");

	//exe.Open("BIOHAZARD.EXE");
	//ExtractItems(exe.GetBuffer());
	//Extract_weapon_tbl(exe.GetBuffer(), 0xAD308, "damage_tbl_0.xml");
	//Extract_esp_tbl(exe.GetBuffer(), "esp.xml");
	//Extract_weapon_tbl(exe.GetBuffer(), 0xADC68, "damage_tbl_1.xml");

	//Extract_strings(exe.GetBuffer(), 0x4BF0B8, 128, "test.txt");
	Extract_messages("D:\\Program Files\\RESIDENT EVIL\\USA", "dialog");
	//Extract_bgs_psx();
	//Extract_bgs();
	//Extract_bg("D:\\Program Files\\RESIDENT EVIL\\JPN\\STAGE1\\RC1050.pak", "test");
	//Extract_bg("D:\\Program Files\\RESIDENT EVIL\\JPN\\objspr\\OSP00001.pak", "dest");
}
