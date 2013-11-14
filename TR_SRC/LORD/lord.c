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

Possible types are debug, info, warning, error and critical
Possible output streams are stdout, stderr, FILE, NULL
Possible verbosity chars are few, some and all

Example code for initialising the warning module with some output modes to a file would be
set_lord_warning_mode(file, ) .... not finished yet

Then output each call by using
lord_type(int lord_code, char *frmt, ...)

Example code for outputting the lord code 0 and the text filenotfound to the error module
(void) lord_error(0, "filenotfound");
The c code line number will also be outputted if written like this
(void) lord_error(0, LORD("filenotfound"));


The lord_code has different meaning:

for the debug, info and warning messages different verbosity level exist
	lord_code 0 is a default value and means that a level has not been assigned, the message is outputtet
	lord_code 1 means not a very important messages, only output if verbosity level is all
	lord_code 2 means a little important message, output if verbosity is some or all
	lord_code 3 means important, output no matter what the verbosity level is

for error and critical messages 
	The value of the lord_code is outputtet in the same line just after the message.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "trlib_intern.h"
#include "trlib_api.h"
#include "trthread.h"
#include "lord.h"


/* Flag whether type is on or off -deafult assigned */
int use_debug=0;
int use_info=0;
int use_warning=1;
int use_error=1;
int use_critical=1;

/* The level output for each type -defaults to quite verbose */
int    verbosity_debug=2;
int	verbosity_info=2;
int	verbosity_warning=2;


/* The output stream used for each type -NULLS assigned automatically*/
FILE * stream_debug;
FILE * stream_info;
FILE * stream_warning;
FILE * stream_error;
FILE * stream_critical;

/* Log last error */
THREAD_SAFE int last_error=TR_OK;

/*message overflow protection*/
/*#define MAX_MESSAGES (5000)*/
THREAD_SAFE int n_logged=0;
int _max_messages = 5000;

/*call back function*/
static LORD_CALLBACK lord_call_back=NULL;

/*declare the static functions*/
static void lord_wrapper(LORD_CLASS report_class, int err_code, FILE *stream, char *frmt, va_list args);
static void close_lord_outputs();


/*returns last (thread local) error*/
int lord_get_last_error(){
	return last_error;
}

int is_lord_initialised(){
	int is_init=(stream_error!=NULL || stream_critical!=NULL || lord_call_back!=NULL);
	return is_init;
}

/*function which resets message count and last error*/
void reset_lord(){
	last_error=TR_OK;
	n_logged=0;
}

/* Sets the maximum number of lord messages pr. thread in the LORD module. A negative number means no limit */
void set_lord_max_messages(int max_messages) {
	_max_messages = max_messages;
}

void set_lord_callback(LORD_CALLBACK fct){
	lord_call_back=fct;
}


static void lord_wrapper(LORD_CLASS report_class, int err_code, FILE *stream, char *frmt, va_list args){
	char buf[1024], *tmp=buf;
	n_logged++;
	if (n_logged>_max_messages && _max_messages > -1)
		return;
	/*use special message in case of overflow....*/
	if (n_logged==_max_messages){
		sprintf(buf,"%d messages logged - nothing will be reported from now on.",_max_messages);
	}
	else{
		/*compose message identifier*/
		switch (report_class){
			case LORD_DEBUG:
				tmp+=sprintf(tmp,"DEBUG:\t");
				break;
			case LORD_INFO:
				tmp+=sprintf(tmp,"INFO:\t");
				break;
			case LORD_WARNING:
				tmp+=sprintf(tmp,"WARNING:\t");
				break;
			case LORD_ERROR:
				tmp+=sprintf(tmp,"ERROR:\t");
				break;
			case LORD_CRITICAL:
				tmp+=sprintf(tmp,"CRITICAL:\t");
				break;
			default:
				tmp+=sprintf(tmp,"UNKNOWN ERROR:\t");
		}
		/*compose message*/
		tmp+=vsnprintf(tmp,512,frmt,args);
		if (report_class>LORD_WARNING)
			tmp+=sprintf(tmp,"\tLORD_CODE:\t %d", err_code);
		strcat(tmp,"\n");
	}
	/*if  call_back  set - use that*/
	if (lord_call_back!=NULL){
		lord_call_back(report_class, err_code, buf);
	}
	else if (stream!=NULL){
		fputs(buf,stream);
	}
	return;
}
		
/* Output debug messages*/
void lord_debug(int lord_code, char *frmt, ...)
{
	if (use_debug)
	{
		/*Determine if the message shall be outputted according to verbosity level*/
		if (lord_code<=verbosity_debug)
		{
			
			va_list ap;
			va_start(ap, frmt);
			lord_wrapper(LORD_DEBUG,0,stream_debug,frmt,ap);
			va_end(ap);
		}
	}
}

/* Output info messages*/
void lord_info(int lord_code, char *frmt, ...)
{
	if (use_info)
	{
		/*Determine if the message shall be outputted according to verbosity */
		if (lord_code<=verbosity_info) /*less verbose than the limit*/
		{
			va_list ap;
			va_start(ap, frmt);
			lord_wrapper(LORD_INFO,0,stream_info,frmt,ap);
			va_end(ap);
		}
	}
}
	
/* Output warning messages*/
void lord_warning(int lord_code, char *frmt, ...)
{
	if (use_warning)
	{
			/* Determine if the message shall be outputted according to verbosity level*/
		if (lord_code<=verbosity_warning)
		{
			va_list ap;
			va_start(ap, frmt);
			lord_wrapper(LORD_WARNING,0,stream_warning,frmt,ap);
			va_end(ap);
		}
	}
}

/* Output error messages*/
void lord_error(int lord_code, char *frmt, ...)
{      
	/*buffer last error*/
	last_error=lord_code;
	if (use_error)
	{
		va_list ap;
		va_start(ap, frmt);
		lord_wrapper(LORD_ERROR,lord_code,stream_error,frmt,ap);
		va_end(ap);
		
	}
}

/*Output critical mesaages*/
void lord_critical(int lord_code, char *frmt, ...)
{
	/*buffer last error*/
	last_error=lord_code;
	if (use_critical)
	{
		va_list ap;
		va_start(ap, frmt);
		lord_wrapper(LORD_ERROR,lord_code,stream_critical,frmt,ap);
		va_end(ap);
		
	}
}

/*Set and initialise the debug module*/
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

/* Set and initialise the info module*/
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

/* Set and initialise the warning module*/
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

/* Set and initialise the error module*/
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

	
}

/* Set and initialise the critical module*/
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

	
}

/* Initialise the lord module with default */
void init_lord()
{
	if (!TR_IsMainThread())
	{
		return;
	}
        n_logged=0;
	last_error=TR_OK;
	use_debug = 0;
	use_info = 0;
	use_warning = 1;
	use_error= 1;
	use_critical = 1;

	verbosity_debug = 2;
	verbosity_info = 2;
	verbosity_warning = 2;
	

	stream_debug = NULL;
	stream_info = NULL;
	stream_warning = stdout;
	stream_error = stderr;
	stream_critical = stderr;
}

/*Turn the lord modes on or off */
void set_lord_modes(int debug, int info, int warning, int error, int critical)
{
	if (!TR_IsMainThread())
	{
		return;
	}
	use_debug = debug;
	use_info = info;
	use_warning = warning;
	use_error = error;
	use_critical = critical;
}

/* Set the output pointers for each lord_type */
void set_lord_outputs(FILE * stream_debug_outside, FILE * stream_info_outside, FILE * stream_warning_outside, FILE * stream_error_outside, FILE * stream_critical_outside)
{
	stream_debug = stream_debug_outside;
	stream_info = stream_info_outside;
	stream_warning = stream_warning_outside;
	stream_error = stream_error_outside;
	stream_critical = stream_critical_outside;
}

/* Set the verbosity level for the modes - simlk: consider changing to only the three first classes in input */
void set_lord_verbosity_levels(int verb_debug, int verb_info, int verb_warning, int verb_error, int verb_critical)
{
	if (!TR_IsMainThread())
	{
		return;
	}

	verbosity_debug = verb_debug;
	verbosity_info = verb_info;
	verbosity_warning = verb_warning;
}

/*close output streams if they are set, not previously closed and not equal to stdout,stderr */
static void close_lord_outputs(){
	FILE *is_closed[5]={NULL,NULL,NULL,NULL,NULL};
	FILE **streams[5]={&stream_debug,&stream_info,&stream_warning,&stream_error,&stream_critical};
	int i;
	for (i=0; i<5; i++){
		FILE **pstream=streams[i];
		if (*pstream!=stdout && *pstream!=stderr && *pstream!=NULL){
			FILE **test_closed=is_closed;
			while (*test_closed && *test_closed!=*pstream) test_closed++;
			/*if test_closed points to NULL the stream did not match any previously closed stream and its safe to close*/
			if (!*test_closed){
				fclose(*pstream);
				*test_closed=*pstream;
				*pstream=NULL;
			}
		}
	}
}

/* useful to set a log file from binding languages where FILE* is hard to map exactly 
*  NOT intended to be mixed with calls to set_lord_outputs, use one or the other
*/
int set_lord_file(char *fullfilename)
{
	if (!TR_IsMainThread())
	{
		return TR_ERROR;
	}
	close_lord_outputs();
	stream_debug = fopen (fullfilename , "w");
	stream_info = stream_debug;
	stream_warning = stream_debug;
	stream_error = stream_debug;
	stream_critical = stream_debug;
	return (stream_debug) ? (TR_OK) : (TR_ALLOCATION_ERROR);
}




