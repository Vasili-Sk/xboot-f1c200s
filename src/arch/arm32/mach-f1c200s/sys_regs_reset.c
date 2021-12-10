/*
 * sys-int.c
 *
 *  Created on: 7 дек. 2021 г.
 *      Author: VasiliSk
 */

#include <string.h>

void sys_regs_reset(void)
{
	//reset all interrupt registers for debugging
	memset((void*)0x01C20400, 0, 0x70);
	//reset DMA
	memset((void*)0x01C02000, 0, 0x4);
	//reset GPIO int
	memset((void*)0x01C20800+0x200, 0, 0x50);
}

