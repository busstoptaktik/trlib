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

/* This module is a logging, reporting and debugging module....*/
/* For use, first initialise and open each type by using
set_lord_type_mode(FILE * stream, char *verbosity)
then output each call by using 
lord_type(int tr_lib_error_code, char *frmt, ...)
Possible types are debug, info, warning, error and critical*/
// muligheder for output_mode
// stdout, stderr, FILE, NULL


#include <stdio.h>
#include <stdarg.h>
#include "trlib_intern.h"

// tr_lib_error_code not outputted yet

// Flag whether type is on or off
int use_debug;
int use_info;
int use_warning;
int use_error;
int use_critical;

// The level output for each type
int verbosity_debug;
int	verbosity_info;
int	verbosity_warning;
int	verbosity_error;
int	verbosity_critical;

// The output stream used for each type
FILE * stream_debug;
FILE * stream_info;
FILE * stream_warning;
FILE * stream_error;
FILE * stream_critical;

void lord_debug(int tr_lib_error_code, char *frmt, ...)
{
	if (use_debug)
	{
		va_list ap;
		fprintf(stream_debug, "\nDEBUG:\t");
		va_start(ap, frmt);
		(void) vfprintf(stream_debug, frmt,ap);
		va_end(ap);
	}
}

void lord_info(int tr_lib_error_code, char *frmt, ...)
{
	if (use_info)
	{
		va_list ap;
		fprintf(stream_info, "\nINFO:\t");
		va_start(ap, frmt);
		(void) vfprintf(stream_info, frmt,ap);
		va_end(ap);
	}
}
	
void lord_warning(int tr_lib_error_code, char *frmt, ...)
{
	if (use_warning)
	{
		va_list ap;
		fprintf(stream_warning, "\nWARNING:\t");
		va_start(ap, frmt);
		(void) vfprintf(stream_warning, frmt,ap);
		va_end(ap);
	}
}

void lord_error(int tr_lib_error_code, char *frmt, ...)
{
	if (use_error)
	{
		va_list ap;
		fprintf(stream_error, "\nERROR:\t");
		va_start(ap, frmt);
		(void) vfprintf(stream_error, frmt,ap);
		va_end(ap);
	}
}

void lord_critical(int tr_lib_error_code, char *frmt, ...)
{
	if (use_critical)
	{
		va_list ap;
		fprintf(stream_critical, "\nCRITICAL:\t");
		va_start(ap, frmt);
		(void) vfprintf(stream_critical, frmt,ap);
		va_end(ap);
	}
}

void set_lord_debug_mode(FILE * stream, char *verbosity)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (stream != NULL)
	{
		stream_debug = stream;
		use_debug = 1;
	}
	else
	{
		use_debug = 0;
	}

	if(strcmp (verbosity,"few") == 0)
	{
		verbosity_debug = 1;
	}
	else if (strcmp (verbosity,"some") == 0)
	{
		verbosity_debug = 2;
	}
	else if (strcmp (verbosity,"all") == 0)
	{
		verbosity_debug = 3;
	}
}

void set_lord_info_mode(FILE * stream, char *verbosity)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (stream != NULL)
	{
		stream_info = stream;
		use_info = 1;
	}
	else
	{
		use_info = 0;
	}

	if(strcmp (verbosity,"few") == 0)
	{
		verbosity_info = 1;
	}
	else if (strcmp (verbosity,"some") == 0)
	{
		verbosity_info = 2;
	}
	else if (strcmp (verbosity,"all") == 0)
	{
		verbosity_info = 3;
	}
}

void set_lord_warning_mode(FILE * stream, char *verbosity)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (stream != NULL)
	{
		stream_warning = stream;
		use_warning = 1;
	}
	else
	{
		use_warning = 0;
	}

	if(strcmp (verbosity,"few") == 0)
	{
		verbosity_warning = 1;
	}
	else if (strcmp (verbosity,"some") == 0)
	{
		verbosity_warning = 2;
	}
	else if (strcmp (verbosity,"all") == 0)
	{
		verbosity_warning = 3;
	}
}

void set_lord_error_mode(FILE * stream, char *verbosity)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (stream != NULL)
	{
		stream_error = stream;
		use_error = 1;
	}
	else
	{
		use_error = 0;
	}

	if(strcmp (verbosity,"few") == 0)
	{
		verbosity_error = 1;
	}
	else if (strcmp (verbosity,"some") == 0)
	{
		verbosity_error = 2;
	}
	else if (strcmp (verbosity,"all") == 0)
	{
		verbosity_error = 3;
	}
}

void set_lord_critical_mode(FILE * stream, char *verbosity)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	if (stream != NULL)
	{
		stream_critical = stream;
		use_critical = 1;
	}
	else
	{
		use_critical = 0;
	}

	if(strcmp (verbosity,"few") == 0)
	{
		verbosity_critical = 1;
	}
	else if (strcmp (verbosity,"some") == 0)
	{
		verbosity_critical = 2;
	}
	else if (strcmp (verbosity,"all") == 0)
	{
		verbosity_critical = 3;
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

/*void lord_setQuitError(int tr_lib_error_code, char *frmt, ...)
{
	(void) fprintf(stdout, frmt);
}*/

void init_lord()
{
	if (!TR_IsMainThread())
	{
		return;
	}

	use_debug = 0;
	use_info = 0;
	use_warning = 1;
	use_error= 1;
	use_critical = 1;

	verbosity_debug = 3;
	verbosity_info = 2;
	verbosity_warning = 3;
	verbosity_error = 3;
	verbosity_critical = 3;

	stream_debug = NULL;
	stream_info = NULL;
	stream_warning = stdout;
	stream_error = stdout;
	stream_critical = stdout;
}

//void set_lord_file_all(char *fullfilename)

// implement it so only one file when implemented in pythin binding

void set_lord_file(char *fullfilename)
{
	stream_debug = fopen (fullfilename , "w");
	stream_info = stream_debug;
	stream_warning = stream_debug;
	stream_error = stream_debug;
	stream_critical = stream_debug;
}

