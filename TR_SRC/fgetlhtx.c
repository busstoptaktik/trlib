

/* fgetlhtx                      # page 1   09 Feb 1995 14 49 */


/* Copyright (c) 1995  GD,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


#include <stdio.h>
#include <stdlib.h>

int                fgetlhtx(
/*________________________*/
FILE              *iofile,
char              *in_str
)

{

  int                        i, c;

    /* collect label forerunner ( = #) */
    i = 1;
    do {
      c = getc(iofile);
      switch(i) {
      case 1:
        switch(c) {
        case '#':
          i = 0;
          break;
        case '*':
          i = 2;
          break;
        case '/':
          i = 3;
          break;
        default :
          break;
        }
        break;



/* fgetlhtx                      # page 2   09 Feb 1995 14 49 */


      case 2: /* comment * ... ; */
        if (c == ';') i = 1;
        break;
      case 3: /* efter / søges * */
        if (c == '*') i++;
        else i = 1;
        break;
      case 4: /* søg stop *      */
        if (c == '*') i++;
        break;
      case 5: /* efter * søges / */
        i = (c == '/') ? 1 : 4;
        break;
      default:
        break;
      }
    }  while (i != 0 && c != EOF);

    if (c == EOF) {
      (void) ungetc(c, iofile);
      (void) fprintf(stdout, "\n\nEND OF FILE\n");
      return(EOF);
    }

    (void) fscanf(iofile, "%s", in_str);

  return(0);

}


