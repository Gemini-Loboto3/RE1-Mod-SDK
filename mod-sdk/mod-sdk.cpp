#include "stdafx.h"

// pointer to the game's EXE
u8 *pExe;

// This is called as soon as the DLL loads.
// Use to initialize whatever data you may need, like a hook
// to the EXE.
void Modsdk_init(HMODULE hExe)
{
	pExe = (u8*)hExe;
}

// This is called when the DLL is closed.
void Modsdk_close()
{
}

// This is called right before the game is booted.
// This is where you should add hooks to the code or data so
// that it doesn't interfere with Classic REbirth's hooks.
void Modsdk_post_init()
{
}

// This is called whenever a save file is being loaded.
// Use this to read whatever extra data your mod might need.
// Leave empty if no changes are necessary.
// =========================================================
// PARAMETERS
// - src: buffer containing the whole save file data being
//        loaded. Use this to perform whatever checks or
//        copies necessary to a mod.
// - pos: how many bytes the game read from the save file.
//        This is always 2048.
// - size: how many bytes there are in src. Use this to
//         perform whatever checks on data integrity or
//         backward compatibility.
void Modsdk_load(u8 *src, size_t pos, size_t size)
{
}

// This is called whenever a save file is being written.
// Use this to append whatever extra data your mod might need.
// Leave everything to nullptr and zero if no change are
// necessary.
// =========================================================
// PARAMETERS:
// - dst: a reference pointer containing your custom save data.
//        This has to be a static buffer or something that your
//        mod allocates automatically.
// - size: set this to the size of the extra data that needs to
//         be written.
void Modsdk_save(u8* &dst, size_t &size)
{
	dst = nullptr;
	size = 0;
}
