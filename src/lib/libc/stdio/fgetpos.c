/*
 * libc/stdio/fgetpos.c
 */

#include <stdio.h>
#include <xboot/module.h>

int fgetpos(FILE * f, fpos_t * pos)
{
	*pos = f->pos;
	return 0;
}
EXPORT_SYMBOL(fgetpos);
