/*
 * libc/ctype/toupper.c
 */

#include <ctype.h>
#include <xboot/module.h>

int toupper(int c)
{
	if(islower(c))
		return c & 0x5f;
	return c;
}
EXPORT_SYMBOL(toupper);
