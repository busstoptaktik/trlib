
/* nll.h                    # page  1 Mon Feb 20 15:17:28 1993 */

/* Copyright (c) 1993 GD, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


#if !defined(H_NLL)
#define      H_NLL

struct  nl_ca_str {
  int                ix_0;   /*index of elem 0 of col*/
  int                sz_f;   /*saved zeroes of col*/
  unsigned char      bl_t;   /*block_t of col*/
  unsigned char      st_f;   /*status of col*/
  unsigned char      loss;   /*lossed binals*/
};
/* col no (0 : new_st * cord_ps + all_inh + 1) */

struct  bl_ca_str {
  int                base;   /*to give natural indexing*/
  int                last;   /*col in block*/
  int                qlng;   /*double elements in block*/
};
/* blocks no (-1 : blocks +1) 
   RHS = blocks + 1.*/

/* adr elem(col, row) = NL + bl_base + col_base + row
       bl_base        = (bl_ca[block_t])->base
       col_base       = (nl_ca[col])->ix_0
 where (bl_ca[block_t-1]->last < col <= (bl_ca[block_t])->last  */

#endif

