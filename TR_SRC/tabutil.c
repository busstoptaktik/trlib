/*
 * Copyright (c) 2012, National Survey and Cadastre, Denmark
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



/* based on code from the boogie-project, originally written by Thomas Knudsen */




static unsigned int  first_byte_is_nonzero_if_little_endian = 1;
static unsigned char *is_little_endian = (unsigned char *) &first_byte_is_nonzero_if_little_endian;



inline float swapfloatifneeded(float p) {
    unsigned char *v;
    unsigned char  w;
    
    if (*is_little_endian)
        return p;

    v = (unsigned char *) &p;
    w = v[0];

    v[0] = v[3];
    v[3] = w;

    w = v[1];
    v[1] = v[2];
    v[2] = w;

    return p;
}

#ifdef TEST_ME
#include <stdio.h>

#define STRINGIFY(x)  #x
#define TOSTRING(x)   STRINGIFY(x)
#define LARG(x)        __FILE__ "(" TOSTRING(__LINE__) "): " x

int main (void) {
    float f;
    unsigned int u = *is_little_endian;
    printf (LARG("is_little_endian = %u\n"), u);
    f = swapfloatifneeded(7);
	printf ("f = %f\n", f);
	
	if (is_little_endian && (7==f))
	    return 0;
	return 1;
}
#endif
