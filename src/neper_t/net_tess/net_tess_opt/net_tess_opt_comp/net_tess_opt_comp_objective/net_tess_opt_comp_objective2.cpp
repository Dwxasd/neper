/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2018, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include "net_tess_opt_comp_objective_.h"
#include<ANN/ANN.h>

extern void net_polycomp (struct POLY Domain, struct SEEDSET SeedSet,
			  ANNkd_tree ** pkdTree, struct POLY **pPoly,
			  int *seed_changed, int seed_changedqty,
			  struct TDYN *);

void
net_tess_opt_comp_objective_x_seedset (const double *x, struct TOPT *pTOpt)
{
  int i, seed;

  (*pTOpt).TDyn.varchangedqty = 0;
  (*pTOpt).TDyn.seedchangedqty = 0;

  for (i = 0; i < (*pTOpt).xqty; i++)
    if ((*pTOpt).iter <= 1 || (*((*pTOpt).x_pvar[i])) != x[i])
    {
      (*pTOpt).TDyn.varchangedqty++;
      *((*pTOpt).x_pvar[i]) = x[i];
      ut_array_1d_int_list_addelt (&(*pTOpt).TDyn.seedchanged,
	  &(*pTOpt).TDyn.seedchangedqty, (*pTOpt).x_seed[i]);
    }

  ut_array_1d_int_sort_uniq ((*pTOpt).TDyn.seedchanged,
			     (*pTOpt).TDyn.seedchangedqty,
			     &(*pTOpt).TDyn.seedchangedqty);

  for (i = 0; i < (*pTOpt).TDyn.seedchangedqty; i++)
  {
    seed = (*pTOpt).TDyn.seedchanged[i];
    neut_seedset_seed_update_fromseedcoo0 (&((*pTOpt).SSet), seed);
    if (!strncmp ((*pTOpt).SSet.Type, "periodic", 8))
      neut_seedset_seed_updateslaves (&((*pTOpt).SSet), seed);
  }

  return;
}

void
net_tess_opt_comp_objective_centroidal_update (struct TOPT *pTOpt)
{
  int i, seed;

  for (i = 0; i < (*pTOpt).TDyn.seedchangedqty; i++)
  {
    seed = (*pTOpt).TDyn.seedchanged[i];

    ut_array_1d_memcpy ((*pTOpt).tarcellval[0][seed],
			(*pTOpt).tarcellvalqty[0],
			(*pTOpt).SSet.SeedCoo[seed]);
  }

  return;
}

int
net_tess_opt_comp_objective_poly (struct TOPT *pTOpt)
{
  int i, cell, scell;
  struct POLY DomPoly;

  ut_string_string ((char *) "custom", &((*pTOpt).SSet).weight);

  ANNkd_tree *kdTree = NULL;

  neut_poly_set_zero (&DomPoly);
  if (!strcmp (((*pTOpt).SSet).Type, "standard"))
    net_tess_poly ((*pTOpt).Dom, 1, &DomPoly);
  else
    net_tess_poly ((*pTOpt).DomPer, 1, &DomPoly);

  net_polycomp (DomPoly, (*pTOpt).SSet,
		&kdTree, &((*pTOpt).Poly),
		(*pTOpt).TDyn.seedchanged, (*pTOpt).TDyn.seedchangedqty,
		&((*pTOpt).TDyn));

  (*pTOpt).scellchangedqty = ((*pTOpt).TDyn).cellchangedqty;
  (*pTOpt).scellchanged = ut_realloc_1d_int ((*pTOpt).scellchanged,
					     (*pTOpt).scellchangedqty);
  ut_array_1d_int_memcpy ((*pTOpt).scellchanged, (*pTOpt).scellchangedqty,
			  ((*pTOpt).TDyn).cellchanged);

  (*pTOpt).cellchangedqty = 0;
  for (i = 0; i < (*pTOpt).scellchangedqty; i++)
  {
    scell = (*pTOpt).scellchanged[i];
    cell = (*pTOpt).SCellCell[scell];
    ut_array_1d_int_list_addelt (&((*pTOpt).cellchanged),
				 &((*pTOpt).cellchangedqty), cell);
  }
  ut_array_1d_int_sort ((*pTOpt).cellchanged, (*pTOpt).cellchangedqty);

  delete kdTree;
  annClose ();

  neut_poly_free (&DomPoly);

  return 0;
}

void
net_tess_opt_comp_objective_debugtest (struct TOPT TOpt)
{
  int i;
  double *vol = ut_alloc_1d (TOpt.CellQty + 1);
  double domvol, totvol;

  neut_tess_volume (TOpt.Dom, &domvol);
  for (i = 1; i <= TOpt.CellQty; i++)
    neut_polys_volume (TOpt.Poly, TOpt.CellSCellList[i],
	               TOpt.CellSCellQty[i], vol + i);

  totvol = ut_array_1d_sum (vol + 1, TOpt.CellQty);

  if (!ut_num_requal (totvol, domvol, 1e-6))
  {
    printf ("\n");
    ut_print_lineheader (2);
    printf ("Wrong cell update in net_polycomp.\n");
    printf ("TDyn.seedchanged = ");
    ut_array_1d_int_fprintf (stdout, TOpt.TDyn.seedchanged,
	TOpt.TDyn.seedchangedqty, "%d");
    printf ("TDyn.cellchanged = ");
    ut_array_1d_int_fprintf (stdout, TOpt.TDyn.cellchanged,
	TOpt.TDyn.cellchangedqty, "%d");
    printf ("cell vols: ");
    ut_array_1d_fprintf (stdout, vol + 1, TOpt.CellQty, "%.15f");
    printf ("totvol = %.15f != domvol = %.15f (ratio %.12f)\n",
	              totvol, domvol, totvol / domvol);
    abort ();
  }

  ut_free_1d (vol);

  return;
}
