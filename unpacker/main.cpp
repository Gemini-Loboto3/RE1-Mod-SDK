// unpacker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdafx.h>
#include <iostream>
#include <algorithm>
#include <inttypes.h>
#include "depack_pak.h"
#include "Bitmap.h"
#include "mdec\bs.h"
#include "int24.h"
#include "RoomRE1.h"
#include "tinyxml2.h"
#include "re1.h"
#include "obj.h"
#include "MarniExModel.h"
#include "3d_math.h"
#include "wav_hash.h"
#include "psound.h"
#include "encvag.h"
#include "xxhash.h"

void StringsToXml(std::vector<std::string> &str, LPCSTR out_name);
std::string DecodeStringDS(u8* data);

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
	//char name[MAX_PATH];
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

int Extract_text(LPCSTR rdt_name, LPCSTR xml_name)
{
	CBufferFile rdt(rdt_name);
	if (rdt.data == nullptr) return 0;
	RE1::RDT_HEADER *h = (RE1::RDT_HEADER*)rdt.GetBuffer();
	if (h->pMessage == 0) return 0;

	u16 *ptr = (u16*)&rdt.GetBuffer()[h->pMessage];
	std::vector<std::string> str;
	for (int i = 0, si = ptr[0] / 2; i < si; i++)
		str.push_back(DecodeString(&((u8*)ptr)[ptr[i]]));

	StringsToXml(str, xml_name);
	return ptr[0] / 2;
}

std::vector<std::pair<u8,u8>> room_tbl;

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
				if (Extract_text(path, xml))
				{
					room_tbl.push_back(std::make_pair(Stage, Room));
					//room_tbl.push_back(Room);
				}
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
		"Knife",	"Handgun",	"Shotgun",	"Dumdum",
		"Magnum",	"Fuel",		"Explosive","Acid",
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

void Extract_icons(LPCSTR folder_name, LPCSTR out_name, u8 *pExe)
{
	char path[MAX_PATH];
	ITEM_DATA *data = (ITEM_DATA*)&pExe[0x4CCCBC - EXE_DIFF_J];
	
	CTim tim;
	CBufferFile icons;

	sprintf_s(path, MAX_PATH, "%s\\STATUS.TIM", folder_name);
	icons.Open(path);
	tim.Open(icons.data);

	sprintf_s(path, MAX_PATH, "%s\\ITEM_ALL.PIX", folder_name);
	icons.Open(path);

	CBitmap bmp;
	//u8 *ico = icons.GetBuffer();
	std::vector<u32> rgb = std::vector<u32>(256);
	Convert_clut(&tim.clut[256 * 2], &rgb[0], 256);
	for (int i = 0, icon = 0; i < 2; i++)
	{
		bmp.Create(256, 256);
		for (int j = 0; j < 6 * 8; j++, icon++)
		{
			u8 *ico = icons.GetBuffer() + (data[icon].Icon - 1) * (40 * 30);
			for (int k = 0; k < 30 * 40; k++)
			{
				bmp.setPixel((k % 40) + (j % 6) * 40,  (k / 40) + (j / 6) * 30, rgb[*ico++]);
			}
		}
		sprintf_s(path, MAX_PATH, "%s_%d.png", out_name, i);
		bmp.SavePng(path);
	}
}

void ExportIVM(const char *in_name, const char *out_name)
{
	MarniExTMD tmd;
	CBufferFile b(in_name);
	if (b.GetBuffer() == nullptr)
		return;
	tmd.Open(b.GetBuffer() + 0x10220);

	obj obj;
	int total = 0;
	// total of vertices
	for (int i = 0; i < tmd.Count; i++)
		total += tmd.joint[i].Count;

	obj.vc = total * 3;
	obj.vv = new struct obj_vert[total * 3];

	CMatrix roty;
	MatrixRotateXYZ(&roty, 0.f, DEGTORAD(180.f), DEGTORAD(180.f));

	for (int o = 0, b = 0; o < tmd.Count; o++)
	{
		for (int i = 0; i < tmd.joint[o].Count * 3; i++, b++)
		{
			CVector v(tmd.joint[o].p[i].x, tmd.joint[o].p[i].y, tmd.joint[o].p[i].z);
			CVector r = MatrixTransformVector(v, roty);
			obj.vv[b].v[0] = r.x * 100.f;
			obj.vv[b].v[1] = r.y * 100.f;
			obj.vv[b].v[2] = r.z * 100.f;
			CVector n(tmd.joint[o].p[i].n[0], tmd.joint[o].p[i].n[1], tmd.joint[o].p[i].n[2]);
			r = MatrixTransformVector(n, roty);
			obj.vv[b].n[0] = r.x;
			obj.vv[b].n[1] = r.y;
			obj.vv[b].n[2] = r.z;
			obj.vv[b].t[0] = tmd.joint[o].p[i].u;
			obj.vv[b].t[1] = 1.0f - tmd.joint[o].p[i].v;
		}
	}

	obj.sc = tmd.Count;
	obj.sv = new struct obj_surf[tmd.Count];
	for (int i = 0, b = 0; i < tmd.Count; i++)
	{
		obj.sv[i].pv = new struct obj_poly[tmd.joint[i].Count];
		obj.sv[i].pc = tmd.joint[i].Count;
		for (int j = 0; j < tmd.joint[i].Count; j++)
		{
			obj.sv[i].pv[j].vi[0] = (j + b) * 3 + 0;
			obj.sv[i].pv[j].vi[1] = (j + b) * 3 + 1;
			obj.sv[i].pv[j].vi[2] = (j + b) * 3 + 2;
		}
		b += tmd.joint[i].Count;
	}

	obj_write_obj(&obj, std::string(std::string(out_name) + ".obj").c_str(), nullptr, 8);

	for (int i = 0; i < obj.sc; i++)
	{
		delete[] obj.sv[i].pv;
	}
	delete[] obj.vv;
	delete[] obj.sv;

	CBitmap bmp;
	bmp.CreateFromTim(b.GetBuffer(), 0);
	bmp.SavePng(std::string(std::string(out_name) + ".png").c_str());
}

void DumpSoundTables(u8 *pExe)
{
	char path[MAX_PATH];

	CreateDirectory("Tables", nullptr);
	u32 *tbl = (u32*)&pExe[0xC1D28];

	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 29; j++)
		{
			u32 *ntbl = (u32*)&pExe[*tbl - EXE_DIFF_J];
			sprintf_s(path, MAX_PATH, "Tables\\room_%X%02X.xml", i + 1, j);

			XMLDocument xml;
			xml.SetBOM(true);

			XMLElement *s = xml.NewElement("Sound");
			for (int k = 0; k < 48; k++)
			{
				auto e = xml.NewElement("Entry");
				e->SetText(ntbl[k] ? (char*)&pExe[ntbl[k] - EXE_DIFF_J] : "");
				if(k == 0) s->InsertEndChild(xml.NewComment(" enemy "));
				if(k == 16) s->InsertEndChild(xml.NewComment(" block 1 "));
				if (k == 32) s->InsertEndChild(xml.NewComment(" block 2 "));
				s->InsertEndChild(e);
			}
			xml.InsertEndChild(s);
			xml.SaveFile(path);
			tbl++;
		}
	}
}

void DumpPlayerTables(u8 *pExe)
{
	char path[MAX_PATH];

	CreateDirectory("Tables", nullptr);
	u32 *tbl = (u32*)&pExe[0xC2650];

	for (int i = 0; i < 8; i++)
	{
		u32 *ntbl = (u32*)&pExe[*tbl - EXE_DIFF_J];
		sprintf_s(path, MAX_PATH, "Tables\\player_%02d.xml", i);

		XMLDocument xml;
		xml.SetBOM(true);

		XMLElement *s = xml.NewElement("Sound");
		for (int k = 0; k < 16; k++)
		{
			auto e = xml.NewElement("Entry");
			e->SetText(ntbl[k] ? (char*)&pExe[ntbl[k] - EXE_DIFF_J] : "");
			s->InsertEndChild(e);
		}
		xml.InsertEndChild(s);
		xml.SaveFile(path);
		tbl++;
	}
}

void DumpCoreTables(u8 *pExe)
{
	char path[MAX_PATH];

	CreateDirectory("Tables", nullptr);
	u32 *tbl = (u32*)&pExe[0xC23D8];

	for (int i = 0; i < 16; i++)
	{
		u32 *ntbl = (u32*)&pExe[*tbl - EXE_DIFF_J];
		sprintf_s(path, MAX_PATH, "Tables\\core_%02d.xml", i);

		XMLDocument xml;
		xml.SetBOM(true);

		XMLElement *s = xml.NewElement("Sound");
		for (int k = 0; k < 16; k++)
		{
			auto e = xml.NewElement("Entry");
			e->SetText(ntbl[k] ? (char*)&pExe[ntbl[k] - EXE_DIFF_J] : "");
			s->InsertEndChild(e);
		}
		xml.InsertEndChild(s);
		xml.SaveFile(path);
		tbl++;
	}
}

void DumpDoorTables(u8 *pExe)
{
	char path[MAX_PATH];

	CreateDirectory("Tables", nullptr);
	u32 *tbl = (u32*)&pExe[0xC2490];

	for (int i = 0; i < 15; i++)
	{
		u32 *ntbl = (u32*)&pExe[*tbl - EXE_DIFF_J];
		sprintf_s(path, MAX_PATH, "Tables\\door_%02d.xml", i);

		XMLDocument xml;
		xml.SetBOM(true);

		XMLElement *s = xml.NewElement("Sound");
		for (int k = 0; k < 2; k++)
		{
			auto e = xml.NewElement("Entry");
			e->SetText(ntbl[k] ? (char*)&pExe[ntbl[k] - EXE_DIFF_J] : "");
			s->InsertEndChild(e);
		}
		xml.InsertEndChild(s);
		xml.SaveFile(path);
		tbl++;
	}
}

void DumpBgmTables(u8 *pExe)
{
	char path[MAX_PATH];

	CreateDirectory("Tables", nullptr);
	u32 *tbl = (u32*)&pExe[0xC2A00];
	u32 *idx = (u32*)&pExe[0xC2BC8];

	for (int i = 0; i < 57; i++)
	{
		u32 *ntbl = (u32*)&pExe[*tbl - EXE_DIFF_J];
		u8  *itbl = (u8*) &pExe[*idx - EXE_DIFF_J];
		sprintf_s(path, MAX_PATH, "Tables\\bgm_%02d.xml", i);

		XMLDocument xml;
		xml.SetBOM(true);

		XMLElement *s = xml.NewElement("Sound");
		for (int k = 0; k < 4; k++)
		{
			auto e = xml.NewElement("Entry");
			e->SetText(ntbl[k] ? (char*)&pExe[ntbl[k] - EXE_DIFF_J] : "");
			e->SetAttribute("loop", itbl[k]);
			s->InsertEndChild(e);
		}
		xml.InsertEndChild(s);
		xml.SaveFile(path);
		tbl++;
		idx++;
	}

	XMLDocument xml;
	xml.SetBOM(true);

	u8 *dat = &pExe[0xC2E78];

	XMLElement *s = xml.NewElement("Bgm");
	for (int Stage = 1; Stage < 8; Stage++)
	{
		for (int Room = 0; Room < 32; Room++)
		{
			char room[8];
			sprintf_s(room, 8, "%X%02X", Stage, Room);
			auto r = xml.NewElement("Room");
			s->InsertEndChild(xml.NewComment(room));
			for (int i = 0; i < 4; i++)
			{
				if (dat[i] == 0xff) continue;
				sprintf_s(room, 8, "bgm_%d", i);
				r->SetAttribute(room, dat[i]);
				//auto e = xml.NewElement("Entry");
				//e->SetAttribute("val", dat[i]);
				//r->InsertEndChild(e);
			}
			s->InsertEndChild(r);
			dat += 4;
		}
	}
	xml.InsertEndChild(s);
	xml.SaveFile("bgm_tbl.xml");
}

//

class X64Hash
{
public:
	class Entry
	{
	public:
		Entry(XXH64_hash_t _h, std::string _n) :
			h(_h), n(_n) {}

		bool operator==(const Entry& h2) const
		{
			if (h == h2.h)
				return true;

			return false;
		}

		XXH64_hash_t h;
		std::string n;
	};

	bool PushHash(const void* data, size_t size, LPCSTR name)
	{
		XXH64_hash_t hash = XXH64(data, size, 0);
		Entry h = { hash, name };

		auto f = std::find(hashes.begin(), hashes.end(), h);
		if (f != hashes.end())
			return true;
		//for (size_t i = 0, si = hashes.size(); i < si; i++)
		//	if (hash == hashes[i])
		//		return;

		hashes.push_back({ hash, name });
		SortByHash();
		return false;
	}

	void Write(LPCSTR out_name)
	{
		XMLDocument xml;
		xml.SetBOM(true);

		auto r = xml.NewElement("Hashes");
		for (auto it = hashes.begin(); it != hashes.end(); it++)
		{
			char str[32];
			auto e = xml.NewElement("Hash");
			sprintf_s(str, 32, "%016llX", it->h);
			e->SetAttribute("h", str);
			e->SetAttribute("n", it->n.c_str());

			for (int i = 0; i < 3; i++)
			{
				auto el = xml.NewElement("Alias");
				el->SetText(i);
				e->InsertEndChild(el);
			}

			r->InsertEndChild(e);
		}
		xml.InsertFirstChild(r);
		xml.SaveFile(out_name);
	}

	void SortByHash()
	{
		std::sort(hashes.begin(), hashes.end(), [](Entry& a, Entry& b)
		{
			return a.h < b.h;
		});
	}

	void SortByName()
	{
		std::sort(hashes.begin(), hashes.end(), [](Entry& a, Entry& b)
		{
			return a.n < b.n;
		});
	}

	std::vector<Entry> hashes;
};

typedef struct tagEdh
{
	u8 unk,
		prog,
		id,
		unk2;
} EDH;

void DumpRoomSounds(LPSTR in_rdt, LPSTR id_name, X64Hash &tbl)
{
	CBufferFile f(in_rdt);
	if (f.GetSize() <= 4) return;

	CVab vab;
	u8* b = f.GetBuffer();
	RDT_HEADER* head = (RDT_HEADER*)f.GetBuffer();

	if (head->pEdt == 0 || head->pVh == 0 || head->pVb == 0)
		return;

	std::vector<EDH> edh = std::vector<EDH>(48);
	memcpy(&edh[0], &b[head->pEdt], 48 * sizeof(EDH));
	vab.OpenVab(&b[head->pVh], &b[head->pVb]);

	std::vector<short> pcm = std::vector<short>(2 * 1024 * 1024);
	char path[MAX_PATH];

	for (int p = 0, pi = vab.GetVabCount(), i = 0; p < pi; p++)
	{
		auto prg = vab.GetVabProg(p);
		for (int vv = 0, vi = prg->tones; vv < vi; vv++, i++)
		{
			CVag* v = vab.GetVag(prg->tone[vv].vag - 1);
			vab.GetVabProg(0)->freq;

			int vol = prg->tone[vv].vol;
			int freq = prg->freq[vv];
			size_t size = decode((s8*)v->data, v->GetSize(), &pcm[0]);

			XXH64_hash_t hash = XXH64(v->data, v->GetSize(), 0);
			char id2[32];
			sprintf_s(id2, 32, "%s_%02d", id_name, i);

			for (size_t i = 0; i < size / 2; i++)
				pcm[i] = pcm[i] * vol / 127;

			if (tbl.PushHash(&pcm[0], size / 2, id2))
				continue;

			WAV_HEADER2 wav;
			wav.RIFF = MAGIC('R', 'I', 'F', 'F');
			wav.ChunkSize = size + sizeof(wav) - 8;
			wav.WAVE = MAGIC('W', 'A', 'V', 'E');
			wav.fmt = MAGIC('f', 'm', 't', ' ');
			wav.Subchunk1Size = 16;
			wav.AudioFormat = 1;
			wav.NumOfChan = 1;
			wav.SamplesPerSec = freq;
			wav.bytesPerSec = 2 * freq;
			wav.blockAlign = 2;
			wav.bitsPerSample = 16;
			wav.Subchunk2ID = MAGIC('d', 'a', 't', 'a');
			wav.Subchunk2Size = size;

			sprintf_s(path, MAX_PATH, "SOUND_hashed\\%s.wav", id2);//%016llX.wav", hash);
			FILE* file = fopen(path, "wb");
			fwrite(&wav, sizeof(wav), 1, file);
			fwrite(&pcm[0], size, 1, file);
			fclose(file);
		}
	}
}

void DumpRoomSounds(LPSTR in_rdt, LPSTR in_xml)
{
	CBufferFile f(in_rdt);
	if (f.GetSize() <= 4) return;

	CVab vab;
	u8* b = f.GetBuffer();
	RDT_HEADER* head = (RDT_HEADER*)f.GetBuffer();

	if (head->pEdt == 0 || head->pVh == 0 || head->pVb == 0)
		return;

	XMLDocument xml;
	xml.LoadFile(in_xml);
	auto root = xml.RootElement();

	std::vector<EDH> edh = std::vector<EDH>(48);
	memcpy(&edh[0], &b[head->pEdt], 48 * sizeof(EDH));
	vab.OpenVab(&b[head->pVh], &b[head->pVb]);

	std::vector<short> pcm = std::vector<short>(2 * 1024 * 1024);
	char path[MAX_PATH];

	auto s = root->FirstChildElement("Entry");
	for (int i = 0; i < 48; i++, s = s->NextSiblingElement("Entry"))
	{
		const char* name = s->GetText();
		if (name == nullptr)
			continue;
		CVabProgram* p = vab.GetVabProg(edh[i].prog);
		int freq = p->freq[edh[i].id];
		int vol = p->tone[edh[i].id].vol;
		CVag* v = vab.GetVag(p->tone[edh[i].id].vag - 1);

		size_t size = decode((s8*)v->data, v->GetSize(), &pcm[0]);

		for (size_t i = 0; i < size / 2; i++)
			pcm[i] = pcm[i] * vol / 127;

		WAV_HEADER2 wav;
		wav.RIFF = MAGIC('R', 'I', 'F', 'F');
		wav.ChunkSize = size + sizeof(wav) - 8;
		wav.WAVE = MAGIC('W', 'A', 'V', 'E');
		wav.fmt = MAGIC('f', 'm', 't', ' ');
		wav.Subchunk1Size = 16;
		wav.AudioFormat = 1;
		wav.NumOfChan = 1;
		wav.SamplesPerSec = freq;
		wav.bytesPerSec = 2 * freq;
		wav.blockAlign = 2;
		wav.bitsPerSample = 16;
		wav.Subchunk2ID = MAGIC('d', 'a', 't', 'a');
		wav.Subchunk2Size = size;

		sprintf_s(path, MAX_PATH, "SOUND\\%s.wav", name);
		FILE* file = fopen(path, "wb");
		fwrite(&wav, sizeof(wav), 1, file);
		fwrite(&pcm[0], size, 1, file);
		fclose(file);
	}
}

void DumpMiscSounds(LPCSTR in_hed, LPCSTR in_vb, LPCSTR in_xml)
{
	CBufferFile h(in_hed), v(in_vb);

	u32* footer = (u32*)& h.GetBuffer()[h.GetSize() - 8];
	CVab vab;
	vab.OpenVab(&h.GetBuffer()[footer[0]], v.GetBuffer());

	int count = footer[0] / 4;
	EDH* edh = (EDH*)h.GetBuffer();

	XMLDocument xml;
	xml.LoadFile(in_xml);
	auto root = xml.RootElement();

	char path[MAX_PATH];
	std::vector<short> pcm = std::vector<short>(2 * 1024 * 1024);

	auto s = root->FirstChildElement("Entry");
	for (int i = 0; i < count || s != nullptr; i++, s = s->NextSiblingElement("Entry"))
	{
		const char* name = s->GetText();
		if (name == nullptr)
			continue;
		CVabProgram* p = vab.GetVabProg(edh[i].prog);
		int freq = p->freq[edh[i].id];
		int vol = p->tone[edh[i].id].vol;
		CVag* v = vab.GetVag(p->tone[edh[i].id].vag - 1);

		size_t size = decode((s8*)v->data, v->GetSize(), &pcm[0]);

		for (size_t i = 0; i < size / 2; i++)
			pcm[i] = pcm[i] * vol / 127;

		WAV_HEADER2 wav;
		wav.RIFF = MAGIC('R', 'I', 'F', 'F');
		wav.ChunkSize = size + sizeof(wav) - 8;
		wav.WAVE = MAGIC('W', 'A', 'V', 'E');
		wav.fmt = MAGIC('f', 'm', 't', ' ');
		wav.Subchunk1Size = 16;
		wav.AudioFormat = 1;
		wav.NumOfChan = 1;
		wav.SamplesPerSec = freq;
		wav.bytesPerSec = 2 * freq;
		wav.blockAlign = 2;
		wav.bitsPerSample = 16;
		wav.Subchunk2ID = MAGIC('d', 'a', 't', 'a');
		wav.Subchunk2Size = size;

		sprintf_s(path, MAX_PATH, "SOUND\\%s.wav", name);
		FILE* file = fopen(path, "wb");
		fwrite(&wav, sizeof(wav), 1, file);
		fwrite(&pcm[0], size, 1, file);
		fclose(file);
	}
}

void DumpMiscSounds(LPCSTR in_hed, LPCSTR in_vb, LPCSTR id_name, X64Hash& tbl)
{
	CBufferFile h(in_hed), v(in_vb);

	u32* footer = (u32*)& h.GetBuffer()[h.GetSize() - 8];
	CVab vab;
	vab.OpenVab(&h.GetBuffer()[footer[0]], v.GetBuffer());

	int count = footer[0] / 4;
	char path[MAX_PATH];
	std::vector<short> pcm = std::vector<short>(2 * 1024 * 1024);

	for (int p = 0, pi = vab.GetVabCount(), i = 0; p < pi; p++)
	{
		auto prg = vab.GetVabProg(p);
		for (int vv = 0, vi = prg->tones; vv < vi; vv++, i++)
		{
			CVag* v = vab.GetVag(prg->tone[vv].vag - 1);
			vab.GetVabProg(0)->freq;

			int vol = prg->tone[vv].vol;
			int freq = prg->freq[vv];
			size_t size = decode((s8*)v->data, v->GetSize(), &pcm[0]);

			XXH64_hash_t hash = XXH64(v->data, v->GetSize(), 0);
			char id2[32];
			sprintf_s(id2, 32, "%s_%02d", id_name, i);

			for (size_t i = 0; i < size / 2; i++)
				pcm[i] = pcm[i] * vol / 127;

			if (tbl.PushHash(&pcm[0], size / 2, id2))
				continue;

			WAV_HEADER2 wav;
			wav.RIFF = MAGIC('R', 'I', 'F', 'F');
			wav.ChunkSize = size + sizeof(wav) - 8;
			wav.WAVE = MAGIC('W', 'A', 'V', 'E');
			wav.fmt = MAGIC('f', 'm', 't', ' ');
			wav.Subchunk1Size = 16;
			wav.AudioFormat = 1;
			wav.NumOfChan = 1;
			wav.SamplesPerSec = freq;
			wav.bytesPerSec = 2 * freq;
			wav.blockAlign = 2;
			wav.bitsPerSample = 16;
			wav.Subchunk2ID = MAGIC('d', 'a', 't', 'a');
			wav.Subchunk2Size = size;

			sprintf_s(path, MAX_PATH, "SOUND_hashed\\%s.wav", id2);//%016llX.wav", hash);
			FILE* file = fopen(path, "wb");
			fwrite(&wav, sizeof(wav), 1, file);
			fwrite(&pcm[0], size, 1, file);
			fclose(file);
		}
	}
}

void DumpSounds()
{
	X64Hash tbl;
	char path[MAX_PATH], pxml[MAX_PATH];

	std::vector<std::string> hed, vb;
	ListFiles("PSX\\SOUND\\", "*.HED", hed);
	ListFiles("PSX\\SOUND\\", "*.VB", vb);

	char pvb[MAX_PATH], phed[MAX_PATH];
	for (size_t i = 0, si = hed.size(); i < si; i++)
	{
		std::string id = vb[i].substr(0, strrchr(vb[i].c_str(), '.') - vb[i].c_str());
		sprintf_s(pvb, "PSX\\SOUND\\%s", vb[i].c_str());
		sprintf_s(phed, "PSX\\SOUND\\%s", hed[i].c_str());
		DumpMiscSounds(phed, pvb, id.c_str(), tbl);
	}

	for (int Stage = 1; Stage < 8; Stage++)
	{
		for (int Room = 0; Room < 32; Room++)
		{
			char name[32];
			printf("Doing room %d%02X\n", Stage, Room);
			sprintf_s(path, MAX_PATH, "D:\\Program Files\\RESIDENT EVIL\\JPN\\STAGE%d\\ROOM%d%02X0.RDT", Stage, Stage, Room);
			sprintf_s(pxml, MAX_PATH, "..\\xml\\tables\\room_%d%02X.xml", Stage, Room);
			sprintf_s(name, 32, "ROOM%d%02X", Stage, Room);
			DumpRoomSounds(path, name, tbl);
		}
	}

	tbl.Write("sound_hashes.xml");
	tbl.SortByName();
	tbl.Write("sound_hashes_n.xml");
}

void SpotPCSoundDupes()
{
	std::vector<std::string> list;
	ListFiles("D:\\Program Files\\RESIDENT EVIL\\JPN\\sound\\", "*.WAV", list);

	X64HashComplex tbl;

	for (auto it = list.begin(); it != list.end(); it++)
	{
		if (_strnicmp(it->c_str(), "bgm_", 4) == 0)
			continue;
		if (_strnicmp(it->c_str(), "se_", 3) == 0)
			continue;

		char path[MAX_PATH];
		sprintf_s(path, MAX_PATH, "D:\\Program Files\\RESIDENT EVIL\\JPN\\sound\\%s", it->c_str());
		CBufferFile f(path);

		std::string id_name = it->substr(0, strrchr(it->c_str(), '.') - it->c_str());
		if (id_name == "CH_EF01" || id_name == "CHRIS01")
			it = it;
		tbl.PushHash(f.GetBuffer(), f.GetSize(), id_name);
	}

	tbl.Write("pcm.xml");
}

void DumpMiscSounds()
{
	DumpMiscSounds("PSX\\SOUND\\WP001.HED", "PSX\\SOUND\\WP001.VB", "..\\xml\\tables\\core_01.xml");
	DumpMiscSounds("PSX\\SOUND\\WP002.HED", "PSX\\SOUND\\WP002.VB", "..\\xml\\tables\\core_02.xml");
	DumpMiscSounds("PSX\\SOUND\\WP003.HED", "PSX\\SOUND\\WP003.VB", "..\\xml\\tables\\core_03.xml");
	DumpMiscSounds("PSX\\SOUND\\WP004.HED", "PSX\\SOUND\\WP004.VB", "..\\xml\\tables\\core_04.xml");
	DumpMiscSounds("PSX\\SOUND\\WP005.HED", "PSX\\SOUND\\WP005.VB", "..\\xml\\tables\\core_05.xml");
	DumpMiscSounds("PSX\\SOUND\\WP006.HED", "PSX\\SOUND\\WP006.VB", "..\\xml\\tables\\core_06.xml");
	DumpMiscSounds("PSX\\SOUND\\WP007.HED", "PSX\\SOUND\\WP007.VB", "..\\xml\\tables\\core_07.xml");
	DumpMiscSounds("PSX\\SOUND\\WP008.HED", "PSX\\SOUND\\WP008.VB", "..\\xml\\tables\\core_08.xml");
	DumpMiscSounds("PSX\\SOUND\\WP009.HED", "PSX\\SOUND\\WP009.VB", "..\\xml\\tables\\core_09.xml");
	DumpMiscSounds("PSX\\SOUND\\WP00A.HED", "PSX\\SOUND\\WP00A.VB", "..\\xml\\tables\\core_10.xml");

	//DumpMiscSounds("PSX\\SOUND\\CHAR00.HED", "PSX\\SOUND\\CHAR00.VB", "..\\xml\\tables\\player_00.xml");
	//DumpMiscSounds("PSX\\SOUND\\CHAR01.HED", "PSX\\SOUND\\CHAR01.VB", "..\\xml\\tables\\player_01.xml");
	//DumpMiscSounds("PSX\\SOUND\\CHAR02.HED", "PSX\\SOUND\\CHAR02.VB", "..\\xml\\tables\\player_02.xml");
	////DumpMiscSounds("PSX\\SOUND\\CHAR03.HED", "PSX\\SOUND\\CHAR03.VB", "..\\xml\\tables\\player_03.xml");
	//DumpMiscSounds("PSX\\SOUND\\CHAR04.HED", "PSX\\SOUND\\CHAR04.VB", "..\\xml\\tables\\player_04.xml");
	//DumpMiscSounds("PSX\\SOUND\\CHAR05.HED", "PSX\\SOUND\\CHAR05.VB", "..\\xml\\tables\\player_05.xml");
}

#include "stringpool.h"

void ListAndDump()
{
	std::vector<std::string> xml_list, wav_list;
	ListFiles("..\\xml\\tables\\", "*.xml", xml_list);
	ListFiles("D:\\Program Files\\RESIDENT EVIL\\JPN\\sound", "*.wav", wav_list);

	CStringPool pool;

	std::vector<std::string> found_list;

	// gather all strings into pool
	for (size_t i = 0, si = xml_list.size(); i < si; i++)
	{
		char path[MAX_PATH];
		XMLDocument xml;
		sprintf_s(path, MAX_PATH, "..\\xml\\tables\\%s", xml_list[i].c_str());
		xml.LoadFile(path);
		auto root = xml.RootElement();
		auto s = root->FirstChildElement("Entry");
		while (s)
		{
			const char* str = s->GetText();
			if (str)
				pool.AddString(std::string(str + std::string(".wav")).c_str());
			s = s->NextSiblingElement("Entry");
		}
	}

	// check any wav names and dump
	FILE* flog = fopen("missing_pcm.txt", "wt");
	FILE* flol = fopen("working_pcm.txt", "wt");
	for (size_t i = 0, si = wav_list.size(); i < si; i++)
	{
		if (!pool.GetString(wav_list[i].c_str()))
			fprintf(flog, "%s\n", wav_list[i].c_str());
		else fprintf(flol, "%s\n", wav_list[i].c_str());
	}
	fclose(flog);
	fclose(flol);
}

void DumpDsMessages(LPCSTR in_file, LPCSTR out_folder)
{
	CBufferFile f(in_file);

	char path[MAX_PATH];
	CreateDirectoryA(out_folder, nullptr);

	static std::pair<u8, u8> tbl[] =
	{
		{ 1, 1 }, {-1, -1},	// 100
		{1, 4},	// 104
	};

	u32* ptr = (u32*)f.GetBuffer();
	int count = ptr[0] / 4;

	// banks
	for (int i = 0; i < count - 1; i++)
	{
		if (i % 2 == 0) continue;

		u32* pptr = (u32*)(&f.GetBuffer()[ptr[i]]);
		//if (i < 230)
			//sprintf(path, "%s\\Room_%d%02X.xml", out_folder, room_tbl[i / 2].first, room_tbl[i / 2].second);
		//else
			sprintf(path, "%s\\%03d.xml", out_folder, i);
		std::vector<std::string> sstr;

		int scount = (ptr[i + 1] - ptr[i]) / 4;
		// bank
		for (int j = 0; j < scount; j++)
			sstr.push_back(DecodeStringDS(&f.GetBuffer()[pptr[j]]));
		StringsToXml(sstr, path);
	}
}

int main()
{
	CBufferFile exe;
	exe.Open("BIOHAZARD.EXE");

	//Extract_messages("D:\\Program Files\\RESIDENT EVIL\\USA", "dialog");
	DumpDsMessages("DS\\msgdat.en", "DS Dumps");

	ListAndDump();

	//SpotPCSoundDupes();
	DumpSounds();
	//ListAndDump();
	//
	//DumpMiscSounds();

	Set_color_mode(1);

	DumpSoundTables(exe.GetBuffer());
	DumpPlayerTables(exe.GetBuffer());
	DumpCoreTables(exe.GetBuffer());
	DumpDoorTables(exe.GetBuffer());
	DumpBgmTables(exe.GetBuffer());

	//for (int i = 0; i < 80; i++)
	//{
	//	char path1[MAX_PATH], path2[MAX_PATH];
	//	sprintf_s(path1, MAX_PATH, "D:\\Program Files\\RESIDENT EVIL\\JPN\\ITEM_M2\\I%02dV.IVM", i);
	//	sprintf_s(path2, MAX_PATH, "ivm\\I%02dV", i);
	//	ExportIVM(path1, path2);
	//}
	//ExportIVM("D:\\Program Files\\RESIDENT EVIL\\JPN\\ITEM_M2\\ING.IVM", "ivm\\ING");
	//ExportIVM("D:\\Program Files\\RESIDENT EVIL\\JPN\\ITEM_M2\\MINI.IVM", "ivm\\MINI");

	//Extract_icons("D:\\Program Files\\RESIDENT EVIL\\JPN\\DATA", "item_all", exe.GetBuffer());

	//Extract_files("file", "file_png");

	//CBufferFile exe("NEWEUR.EXE");
	//Extract_strings(exe.GetBuffer(), 0x4BFC70, 63, "misc.xml");
	//Extract_strings(exe.GetBuffer(), 0x4C6170, 79, "desc.xml");

	//ExtractItems(exe.GetBuffer());
	Extract_weapon_tbl(exe.GetBuffer(), 0xAD308, "damage_tbl.xml");
	//Extract_esp_tbl(exe.GetBuffer(), "esp.xml");
	//Extract_weapon_tbl(exe.GetBuffer(), 0xADC68, "damage_tbl_1.xml");

	//Extract_strings(exe.GetBuffer(), 0x4BF0B8, 128, "test.txt");
	//Extract_bgs_psx();
	//Extract_bgs();
	//Extract_bg("D:\\Program Files\\RESIDENT EVIL\\JPN\\STAGE1\\RC1050.pak", "test");
	//Extract_bg("D:\\Program Files\\RESIDENT EVIL\\JPN\\objspr\\OSP00001.pak", "dest");
}
