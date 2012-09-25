/*
 * Copyright (c) 2011, National Survey and Cadastre, Denmark
 * (Kort- og Matrikelstyrelsen), kms@kms.dk
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

#include  <stdio.h>
#include <stdarg.h>
#include "trlib_intern.h"

// muligheder for output_mode
// stdout, stderr, FILE, nothing

// tr_lib_error_code not outputted yet

int use_debug = 0;
int use_info = 0;
int use_warning = 0;
int use_error= 0;
int use_critical = 0;

void lord_debug(int tr_lib_error_code, char *frmt, ...)
{
	if (use_debug)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void lord_info(int tr_lib_error_code, char *frmt, ...)
{
	if (use_info)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}
	
void lord_warning(int tr_lib_error_code, char *frmt, ...)
{
	if (use_warning)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void lord_error(int tr_lib_error_code, char *frmt, ...)
{
	if (use_error)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void lord_critical(int tr_lib_error_code, char *frmt, ...)
{
	if (use_critical)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void set_lord_debug_mode(int mode)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (mode ==0 || mode ==1)
	{
		use_debug = mode;
	}
}

void set_lord_info_mode(int mode)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (mode ==0 || mode ==1)
	{
		use_info = mode;
	}
}

void set_lord_warning_mode(int mode)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (mode ==0 || mode ==1)
	{
		use_warning = mode;
	}
}

void set_lord_error_mode(int mode)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (mode ==0 || mode ==1)
	{
		use_error = mode;
	}
}

void set_lord_critical_mode(int mode)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (mode ==0 || mode ==1)
	{
		use_critical = mode;
	}
}

void lord_set_parameters(int debug)
{
	//implement that these parameters shall only be corrected from the main thread
	if (!TR_IsMainThread())
	{
		return;
	}
	use_debug = debug;
}

void lord_setQuitError(int tr_lib_error_code, char *frmt, ...)
{
	(void) fprintf(stdout, frmt);
}
