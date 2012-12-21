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

#if !defined(H_LORD)
#define      H_LORD


#include  <stdio.h>


/* Pepper the format string with file(line) information */
#define STRINGIFY(x)  #x
#define TOSTRING(x)   STRINGIFY(x)
#define LORD(x)        __FILE__ "(" TOSTRING(__LINE__) "): " x


extern void lord_debug(int lord_code, char *frmt, ...);

extern void lord_info(int lord_code, char *frmt, ...);
	
extern void lord_warning(int lord_code, char *frmt, ...);

extern void lord_error(int lord_code, char *frmt, ...);

extern void lord_critical(int lord_code, char *frmt, ...);

extern void set_lord_debug_mode(FILE * stream, char *verbosity);

extern void set_lord_info_mode(FILE * stream, char *verbosity);

extern void set_lord_warning_mode(FILE * stream, char *verbosity);

extern void set_lord_error_mode(FILE * stream, char *verbosity);

extern void set_lord_critical_mode(FILE * stream, char *verbosity);

extern void init_lord();

extern void set_lord_modes(int debug, int info, int warning, int error, int critical);

extern void set_lord_outputs(FILE * stream_debug, FILE * stream_info, FILE * stream_warning, FILE * stream_error, FILE * stream_critical);

extern void set_lord_file(char *fullfilename);

#endif
