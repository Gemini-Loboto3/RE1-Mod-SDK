#include <stdafx.h>
#include "re1.h"
#include "re1\scheduler.h"
#include "import.h"
#include "inject.h"

void To_chris()
{
	G.Status_flg |= 0x800000;
	Task_chain((void(*)())0x463890);
}

void To_jill()
{
	G.Status_flg |= 0x800000;
	G.Pl_id = 1;
	Task_chain((void(*)())0x463890);
}

void Install_re1(u8 *pExe)
{
	INJECT(0x466200, To_chris);
}
