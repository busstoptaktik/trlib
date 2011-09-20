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
 
#ifndef TRTHREAD_H
#define TRTHREAD_H

/**
------------------------------------------------------------------------
TRTHREAD.H  --  support thread safe operations in trlib
------------------------------------------------------------------------

Thomas Knudsen, Simon Lyngby Kokkendorff, KMS, 2011-08-11


0. INTRODUCTION

This is a ridiculously simple piece of syntactic sugar, essentially
overloading the "__thread" storage qualifier when providing the
thread safe version of trlib, the KMS geodetic transformation library.


1. RATIONALE

The use of thread local storage in C and C++ is still new in terms of
standard adoption: Although it has been supported by most compilers for
approximately a decade, it was first handled in the standardization
processes more recently.

Hence, the "encouraged" (Drepper 2005) syntax, prepending the storage
qualifier keyword "__thread" to identifier declarations and definitions,
may not be what we end up with once the standardization processes have
come to and end, and the smoke has risen.

Also, Microsoft would not be Microsoft if they had not insisted on a more
verbose, less clear syntax for exactly the same semantics:

Encouraged:
    __thread int i = 1;
    
    
Microsoft:
    __declspec( thread ) int tls_i = 1;

For what we know at time of writing, we could do very well by

    1. using a sufficiently sane compiler
    2. assuming thread safe to be the default build mode
    3. using the __thread keyword where needed, and
    4. defining it away on the compiler command line when building
       a non-thread-safe library (i.e. gcc -D__thread ...)
       
We must, however, if not actually respect, then at least live and suffer
with Microsoft's insistence of imposing misery onto our lives.


2. IMPLEMENTATION

To do this, and to anticipate potential syntax changes, the ideal
solution would be a macro expanding differently for different input
types:

THREAD_SAFE(static int)  ->   static __thread int
THREAD_SAFE(int)         ->   __thread int
THREAD_SAFE(extern int)  ->   extern __thread int

in anticipation that the syntax could change to (e.g.) "thread static int"
or somesuch.

Realizing, however, that this may well be preparing for "shit that just
ain't gonna happen", we have opted for this more minimalist version, that
eliminates the need to define away a keyword (-D__thread), while also
adding a bit of semantic content.

Essentially we assume non-thead-safety by default, and define the macro
THREAD_SAFE as empty. But if THREAD_SAFE is defined already (by using
-DTHREAD_SAFE on the compiler command line), we redefine it as __thread

... unless of course we live in a Microsoft world, and have to define it
as __declspec( thread ).


3. INTENDED USE

This header is intended to be included via geo_lab.h - which is the
closest match we currently have to a universal library internal include
file.


4. REFERENCES

Ulrich Drepper (2005): ELF handling for thread-local storage
    http://www.akkadia.org/drepper/tls.pdf

**/





#ifdef THREAD_SAFE
#undef  THREAD_SAFE

#ifdef _MSC_VER
#define THREAD_SAFE __declspec( thread )
#else
#define THREAD_SAFE __thread

#endif

#else
#define THREAD_SAFE
#endif

#endif
/* TRTHREAD_H */

