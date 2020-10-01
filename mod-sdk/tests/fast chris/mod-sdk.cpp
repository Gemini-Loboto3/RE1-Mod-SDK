#include "stdafx.h"
#include "re1.h"

u8 *pExe;

// This is called as soon as the DLL loads.
// Use to initialize whatever data you may need, like a hook to the exe.
void Modsdk_init(HMODULE hExe)
{
	pExe = (u8*)hExe;
}

// This is called when the DLL is closed.
void Modsdk_close()
{
}

// This is called right before the game is booted.
// This is where you should add hooks to the code or data so that
// it doesn't interfere with Classic REbirth's hooks.
void Modsdk_post_init()
{
	Install_re1((u8*)0x400000);
}

// This is called whenever a save file is being loaded.
// Use this to read whatever extra data your mod might need
// leave empty if no changes are necessary.
void Modsdk_load(u8 *src, size_t pos, size_t size)
{
}

// This is called whenever a save file is being written.
// Use this to append whatever extra data your mod might need
// leave empty if no changes are necessary.
void Modsdk_save(u8* &dst, size_t &size)
{
}
