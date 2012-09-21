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

// muligheder for output_mode
// stdout, stderr, FILE, nothing

// tr_lib_error_code not outputted yet

int use_debug = 1;
int use_info = 1;
int use_warning = 1;
int use_error= 1;
int use_critical = 1;

void lord_debug(int *tr_lib_error_code, char *frmt, ...)
{
	if (use_debug)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void lord_info(int *tr_lib_error_code, char *frmt, ...)
{
	if (use_info)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}
	
void lord_warning(int *tr_lib_error_code, char *frmt, ...)
{
	if (use_warning)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void lord_error(int *tr_lib_error_code, char *frmt, ...)
{
	if (use_error)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}

void lord_critical(int *tr_lib_error_code, char *frmt, ...)
{
	if (use_critical)
	{
		va_list ap;
		va_start(ap, frmt);
		(void) vfprintf(stdout, frmt,ap);
		va_end(ap);
	}
}
	
void lord_set_parameters(int *debug)
{
	use_debug = debug;
}

void lord_setQuitError(int *tr_lib_error_code, char *frmt, ...)
{
	(void) fprintf(stdout, frmt);
}
