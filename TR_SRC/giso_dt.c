

/* giso_dt.c  version 1.0             # page 1   19 Aug 1994 11 19 */


/* Copyright (c) 1994, GD,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD,  Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/*

giso_dt      (return)          double function
_            Date, time in ISO format 

Use fputcw to convert and write the date and time in ISO-format.

Prog: FS, AUG 1994
*/

#include <time.h>

double     giso_dt(
void
)
{

  struct tm       dta, *d_t = &dta;
  time_t          t_d, clock;
  double          date_time;

  t_d   = time(&clock);
  *d_t  = *localtime(&t_d);

  date_time = (double) (d_t->tm_year +1900) * 100000000L +
      (d_t->tm_mon +1) * 1000000L +
      (d_t->tm_mday) * 10000 +
      (d_t->tm_hour) * 100 +
      (d_t->tm_min);

  return(date_time);

}


