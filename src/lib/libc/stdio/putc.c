/*
 * libc/stdio/putc.c
 */

#include <stdio.h>
#include <xboot/module.h>

int putc(int c, FILE * f)
{
	return fputc(c, f);
}
EXPORT_SYMBOL(putc);
