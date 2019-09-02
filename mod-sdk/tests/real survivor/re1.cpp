#include <stdafx.h>
#include "re1.h"
#include "re1\scheduler.h"
#include "import.h"

#define SAVE_SIZE	2690		// max size of vanilla files
								// can also be 2048 or 2336 according to the original code

enum BOXES
{
	BOX_UNMANAGED,	// failsafe
	BOX_INFIRMARY,
	BOX_STOREROOM,
	BOX_UNDERGROUND,
	BOX_GUARDHOUSE,
	BOX_LAB_ACCESS,
	BOX_LABORATORY,
	BOX_COUNT
};

// new save data for item boxes
static ITEM_WORK Boxes[BOX_COUNT][48];

void InitBoxes()
{
	// reset boxes upon result screen or new game
	memset(Boxes, 0, sizeof(Boxes));
#ifdef _DEBUG
	// drop a shotgun into the infirmary box
	Boxes[BOX_INFIRMARY][0].Id = 3;
	Boxes[BOX_INFIRMARY][0].Num = 100;
	// drop a dumdum magnum into the storeroom box
	Boxes[BOX_STOREROOM][0].Id = 4;
	Boxes[BOX_STOREROOM][0].Num = 100;
#endif
	// old code to initialize items
	Stage_init_items();
}

void LoadBoxes(u8 *src, size_t pos, size_t size)
{
	// sailsafe for vanilla saves
	if(size <= SAVE_SIZE) memset(Boxes, 0, sizeof(Boxes));
	// copy data from save
	else memcpy(Boxes, &src[pos], sizeof(Boxes));
}

void WriteBoxes(u8* &data, size_t &size)
{
	// assign formatted data
	data = (u8*)Boxes;
	size = sizeof(Boxes);
}

static int Box_from_room()
{
	switch (G.Stage_no)
	{
	case 0:		// Mansion 1F
	case 5:
		switch (G.Room_no)
		{
		case 0:		// 100 Infirmary
			return BOX_INFIRMARY;
		case 0x18:	// 118 Storeroom
			return BOX_STOREROOM;
		}
		break;
	case 1:		// Mansion 2F and B1
	case 6:
		break;
	case 2:		// Outdoor, basement and heliport
		switch (G.Room_no)
		{
		case 0x0E:	// 30E Underground saveroom
			return BOX_UNDERGROUND;
		}
		break;
	case 3:		// Guardhouse
		switch (G.Room_no)
		{
		case 0x03:	// 403 Guardhouse saveroom
			return BOX_GUARDHOUSE;
		}
		break;
	case 4:		// Laboratory
		switch (G.Room_no)
		{
		case 0x02:	// 502 Laboratory access
			return BOX_LAB_ACCESS;
		case 0x0E:	// 50E Laborartory saveroom
			return BOX_LABORATORY;
		}
		break;
	}

	return BOX_UNMANAGED;
}

static bool restore = false;

// copy stuff from out item boxes into the real one
void __cdecl Init_itembox()
{
	// only execute this code when in an item box
	restore = true;
	memcpy(G.Itembox_work, Boxes[Box_from_room()], sizeof(G.Itembox_work));

	Load_itemboxTim();
}

// store real itembox contents to our itembox
void __cdecl Close_itembox()
{
	// only execute this code when in an item box
	if (restore)
	{
		memcpy(Boxes[Box_from_room()], G.Itembox_work, sizeof(G.Itembox_work));
		restore = false;
	}
	Task_exit();	// original code
}

void Install_re1(u8 *pExe)
{
	// hooks for new game and cleared data
	INJECT_CALL(&pExe[0x48DB20 - EXE_DIFF], InitBoxes, 5);	// new game
	INJECT_CALL(&pExe[0x43D875 - EXE_DIFF], InitBoxes, 5);	// results
	// hooks for the itembox
	INJECT_CALL(&pExe[0x40B444 - EXE_DIFF], Init_itembox, 5);
	INJECT_CALL(&pExe[0x40B920 - EXE_DIFF], Close_itembox, 5);
	INJECT_CALL(&pExe[0x40B95D - EXE_DIFF], Close_itembox, 5);
}
