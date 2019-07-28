#include <stdafx.h>
#include <vector>
#include "RoomRE1.h"

using namespace RE1;

static void Scan_SCD(u8 *data, int size, std::vector<SCD_DOOR> &doors, SCD_OBA *dest_oba);

CRoom::CRoom(void)
{
}

void CRoom::Reset()
{
	memset(scd_oba,0,sizeof(scd_oba));

	//snd.RemoveAll();
	//res.Reset();
	//ClearPtrVector(camera);
	walkmesh[0].clear();
	walkmesh[1].clear();
	walkmesh[2].clear();
	walkmesh[3].clear();
	floor.clear();

	doors.clear();
	//ClearPtrVector(m_item);
	//ClearPtrVector(m_oba);
	//ClearPtrVector(m_icon);
	id_oba.clear();
	id_item.clear();
	//ClearPtrVector(mdl_item);
	//ClearPtrVector(mdl_oba);
	//ClearPtrVector(tim_item);
	//ClearPtrVector(tim_oba);
}

int CRoom::Open(LPCTSTR filename)
{
	CBufferFile f;// (filename);
	f.Open(filename);

	if(f.GetSize()<=4) return 0;

	return Open((u8*)f.GetBuffer());
}

int CRoom::Open(u8* data)
{
	Reset();

	RDT_HEADER *h=(RDT_HEADER*)data;

	// copy light data
	memcpy(ambient,h->ambient,sizeof(ambient));
	memcpy(light,h->Light,sizeof(LIGHT_DATA)*3);

	// load camera data
	for(int i=0; i<h->nCut; i++)
	{
		CCamera *c=new CCamera;
		c->cut=h->Cut[i];
		if(c->cut.pSp)
			c->OpenMask(&data[c->cut.pSp]);
		if(c->cut.pTim)
		{
			c->has_mask=true;
			c->pic.Open(&data[c->cut.pTim]);
		}
		camera.push_back(c);
	}
	// now assign the switches to each camera
	for(VCUT *vcut=(VCUT*)&data[h->pVcut];; vcut++)
	{
		if(vcut->Fcut==0xFFFF && vcut->Tcut==0xFFFF) break;

		if(vcut->Fcut>=camera.size()) continue;

		camera[vcut->Fcut]->vcut.push_back(*vcut);
	}

	//Scan_SCD(&data[h->pScrl+2],*(u16*)&data[h->pScrl],doors,scd_oba);

	// collisions
	SCA_HEAD *sca_h=(SCA_HEAD*)&data[h->pSca];
	SCA_DATA *sca=(SCA_DATA*)&sca_h[1];
	Cx=sca_h->Cx;
	Cz=sca_h->Cz;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<(int)sca_h->Ptr[i]; j++, sca++)
		{
			walkmesh[i].push_back(*sca);
		}
	}

	// floor
	int flr_cnt=*(u16*)&data[h->pFlr];
	RDT_FLOOR *f=(RDT_FLOOR*)&data[h->pFlr+2];
	for(int i=0; i<flr_cnt; i++, f++)
		floor.push_back(*f);

	// sound
	//snd.OpenVab(&data[h->pVh],&data[h->pVb]);
	//memcpy(snd_tbl,&data[h->pEdt],sizeof(snd_tbl));

	// EMR+EDD
	//if(h->pEdd && h->pEmr)
		//res.Open1(&data[h->pEdd],&data[h->pEmr]);

	// item models
	//MODEL *m=(MODEL*)&data[h->pObj[0]];
	//for(int i=0; i<(int)h->nItem; i++)
	//{
	//	C3DModel *mm=new C3DModel;
	//	if(m[i].Tmd) mm->md1[0].OpenRTmd(&data[m[i].Tmd]);
	//	else _tprintf(_T("No oba tmd %d\n"),i);
	//	if(m[i].Tim) mm->tim.LoadTim(&data[m[i].Tim]);
	//	else _tprintf(_T("No oba tim %d\n"),i);
	//	m_item.push_back(mm);
	//}
	// object models
	//m=(MODEL*)&data[h->pObj[1]];
	//for(int i=0; i<(int)h->nOmodel; i++)
	//{
	//	C3DModel *mm=new C3DModel;
	//	if(m[i].Tmd) mm->md1[0].OpenRTmd(&data[m[i].Tmd]);
	//	else _tprintf(_T("No item tmd %d\n"),i);
	//	if(m[i].Tim) mm->tim.LoadTim(&data[m[i].Tim]);
	//	else _tprintf(_T("No item tim %d\n"),i);
	//	m_oba.push_back(mm);

	//	u8 *icon=new u8[40*30];
	//	memcpy(icon,&data[h->pRaw+i*30*40],40*30);
	//	m_icon.push_back(icon);
	//}

	return 1;
}

void CRoom::DumpMaskBitmap(LPCTSTR folder)
{
	char name[MAX_PATH];

	for(int i=0; i<(int)camera.size(); i++)
	{
		if(!camera[i]->has_mask) continue;
		//Image img;
		//img.CreateFromTim(&camera[i]->pic,0,RGB(0,128,128));
		CBitmap bmp;
		bmp.CreateFromTim(camera[i]->pic, 0);
		sprintf_s(name, MAX_PATH, "%s_%02d_mask.png" ,folder,i);
		//img.SaveBitmap(name);
		//write_png(img,name);
		bmp.SavePng(name);
	}
}

// ================================================
int CCamera::GetMaskCount()
{
	int count=0;
	// count all the masks in the block
	for(int i=0; i<(int)mask.size(); i++)
		count+=mask[i]->GetCount();
	// return total
	return count;
}

void CCamera::OpenMask(u8* data)
{
	PSP_HEADER* h=(PSP_HEADER*)data;

	if(*(u32*)&h[0]==0xFFFFFFFF || *(u32*)&h[0]==0) return;

	PSP_GROUP *pGp=(PSP_GROUP*)&h[1];
	PSP_SPRT *pSprt=(PSP_SPRT*)&pGp[h->group_cnt];

	for(int i=0; i<h->group_cnt; i++, pGp++)
	{
		CMask* m=new CMask;
		memcpy(m,pGp,sizeof(PSP_GROUP));
		m->x=pGp->x;
		m->y=pGp->y;

		for(int j=0; j<pGp->count; j++)
		{
			// check for masks with reduced data
			if(pSprt->size==0)
			{
				PSP_SPRT_VAR *pSprtV=(PSP_SPRT_VAR*)pSprt;
				// next
				pSprt=(PSP_SPRT*)&((u8*)pSprt)[sizeof(PSP_SPRT_VAR)];
				m->pSp.push_back(*pSprtV);
			}
			else
			{
				PSP_SPRT_VAR pSprtV;
				pSprtV.x=pSprt->x, pSprtV.y=pSprt->y;
				pSprtV.w=pSprtV.h=pSprt->size*8;
				pSprtV.u=pSprt->u, pSprtV.v=pSprt->v;
				pSprtV.otz=pSprt->otz;
				pSprtV.tpage=pSprt->tpage;
				m->pSp.push_back(pSprtV);
				pSprt++;
			}
		}
		mask.push_back(m);
	}
}

// ==============================================
/*
 * Scenario Opcode Index
 *
 * BIO1_SCRIPT_OPCODE[n][0] = Binary Opcode
 * BIO1_SCRIPT_OPCODE[n][1] = Structure Size
 * BIO1_SCRIPT_OPCODE[n][2] = Disassembly Text
*/
typedef struct tagScdOpcode
{
	int size;
	const char *name;
} SCD_OPCODE;

static SCD_OPCODE BIO1_SCRIPT_OPCODE[]=
{
	{0x00000002, "nop"},					// 00
	{0x00000002, "if"},						// 01
	{0x00000002, "else"},					// 02
	{0x00000002, "endif"},					// 03
	{0x00000004, "BitTest"},				// 04
	{0x00000004, "BitOperation"},			// 05
	{0x00000004, "CompareRoomBytes"},		// 06
	{0x00000006, "CompareGameBytes"},		// 07
	{0x00000004, "SetRoomFlag"},			// 08
	{0x00000002, "SetNextCamera"},			// 09
	{0x00000002, "SetCurrentCamera"},		// 0A
	{0x00000004, "unknown opcode"},			// 0B
	{0x0000001A, "InitDoor"},				// 0C
	{0x00000012, "InitModel"},				// 0D
	{0x00000002, "nop"},					// 0E
	{0x00000008, "unknown opcode"},			// 0F
	{0x00000002, "ByteTest"},				// 10
	{0x00000002, "ByteOperation"},			// 11
	{0x0000000A, "unknown opcode"},			// 12
	{0x00000004, "unknown opcode"},			// 13
	{0x00000004, "unknown opcode"},			// 14
	{0x00000002, "SetSoundVolume"},			// 15
	{0x00000002, "SetMusicVolume"},			// 16
	{0x0000000A, "unknown opcode"},			// 17
	{0x0000001A, "SetItemModel"},			// 18
	{0x00000004, "SetByte"},				// 19
	{0x00000002, "unknown opcode"},			// 1A
	{0x00000016, "SetEnemy"},				// 1B
	{0x00000006, "unknown opcode"},			// 1C
	{0x00000002, "unknown opcode"},			// 1D
	{0x00000004, "unknown opcode"},			// 1E
	{0x0000001C, "Sce_oba_set"},			// 1F
	{0x0000000E, "SetModelPosition"},		// 20
	{0x0000000E, "unknown opcode"},			// 21
	{0x00000004, "unknown opcode"},			// 22
	{0x00000002, "unknown opcode"},			// 23
	{0x00000004, "unknown opcode"},			// 24
	{0x00000004, "unknown opcode"},			// 25
	{0x00000002, "unknown opcode"},			// 26
	{0x00000002, "unknown opcode"},			// 27
	{0x00000000, "unknown opcode"},			// 28 ??
	{0x00000002, "SetSoundEffect"},			// 29
	{0x0000000C, "unknown opcode"},			// 2A
	{0x00000004, "unknown opcode"},			// 2B
	{0x00000002, "unknown opcode"},			// 2C
	{0x00000004, "unknown opcode"},			// 2D
	{0x00000000, "unknown opcode"},			// 2E ??
	{0x00000004, "unknown opcode"},			// 2F
	{0x0000000C, "unknown opcode"},			// 30
	{0x00000004, "unknown opcode"},			// 31
	{0x00000004, "unknown opcode"},			// 32
	{0x00000004, "unknown opcode"},			// 33 ??
	{0x00000008, "unknown opcode"},			// 34
	{0x00000004, "unknown opcode"},			// 35
	{0x00000004, "unknown opcode"},			// 36
	{0x00000004, "unknown opcode"},			// 37
	{0x00000004, "unknown opcode"},			// 38
	{0x00000002, "unknown opcode"},			// 39
	{0x00000004, "unknown opcode"},			// 3A
	{0x00000006, "unknown opcode"},			// 3B
	{0x00000006, "unknown opcode"},			// 3C
	{0x0000000C, "unknown opcode"},			// 3D
	{0x00000002, "unknown opcode"},			// 3E
	{0x00000006, "unknown opcode"},			// 3F
	{0x00000010, "unknown opcode"},			// 40
	{0x00000004, "unknown opcode"},			// 41
	{0x00000004, "unknown opcode"},			// 42
	{0x00000004, "unknown opcode"},			// 43
	{0x00000002, "unknown opcode"},			// 44
	{0x00000002, "unknown opcode"},			// 45
	{2+12*3+2*3, "46"},						// 46 ??
	{0x0000000E, "unknown opcode"},			// 47
	{0x00000002, "unknown opcode"},			// 48
	{0x00000002, "unknown opcode"},			// 49
	{0x00000002, "unknown opcode"},			// 4A
	{0x00000002, "unknown opcode"},			// 4B
	{0x00000004, "unknown opcode"},			// 4C
	{0x00000002, "unknown opcode"},			// 4D
	{0x00000004, "unknown opcode"},			// 4E
	{0x00000002, "unknown opcode"},			// 4F
	{0x00000002, "unknown opcode"}			// 50
};

u8 code28_len[]={6,8,6,6,0,6,4,0,6,6};

static void Scan_SCD(u8 *data, int size, std::vector<SCD_DOOR> &doors, SCD_OBA *dest_oba)
{
	SCD_OPCODE *code;
	u8 *end=&data[size];
	int pos=0;

	do
	{
		code=&BIO1_SCRIPT_OPCODE[*data];

		//_tprintf(_T("%04X: %02X %S\n"),pos,*data,code->name);

		switch(*data)
		{
		case 0xC:
			{
				SCD_DOOR *d=(SCD_DOOR*)data;
				//_tprintf(_T("<<Stage %d, room %02X, pos %d/%d %dx%d, XYZ %d/%d/%d, rot %d\nkey %d, type %X\nunk %02X %02X %02X %02X %02X>>\n"),
				//	d->Next_neg ? -(d->Next_stage+1) : d->Next_stage,d->Next_room,
				//	d->x0,d->z0,d->x1,d->z1,
				//	d->Next_x,d->Next_y,d->Next_z,
				//	d->Next_dir,
				//	(s8)d->key,d->type,
				//	d->unk[0],d->unk[1],d->unk[2],d->unk[3],d->unk[4]);
				doors.push_back(*d);
			}
			break;
		case 0x1F:
			{
				SCD_OBA *o=(SCD_OBA*)data;
				//_tprintf(_T("<<OBA %d (%d) Be_flg %X, XYZ %d/%d/%d WHD %d/%d/%d\nunk %04X %04X %04X %04X %04X %04X>>\n"),
				//	o->id,o->ex,o->Be_flg,
				//	o->x,o->y,o->z,
				//	o->w,o->h,o->d,
				//	o->unk[0],o->unk[1],o->unk[2],o->unk[3],o->unk[4],o->unk[5]);
				memcpy(&dest_oba[o->id],o,sizeof(SCD_OBA));
			}
			break;
		case 0x28:
			{
				int size=code28_len[data[2]];
				data+=size;
				pos+=size;
			}
			break;
		}

		data+=code->size;
		pos+=code->size;
	} while(data<end);
}
