/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2018, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#ifdef __cplusplus
extern "C"
{
#endif

  extern void net_tess_opt_init_sset_ori (struct IN_T In, int level,
					  struct MTESS MTess, struct TESS *Tess,
					  int dtess, int dcell,
					  struct SEEDSET *SSet,
					  struct SEEDSET *pSSet);

#ifdef __cplusplus
}
#endif
