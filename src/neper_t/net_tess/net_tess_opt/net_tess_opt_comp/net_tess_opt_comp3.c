/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2016, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include "net_tess_opt_comp_.h"

void
net_tess_opt_comp_rand_init (struct TOPT *pTOpt)
{
  if ((*pTOpt).itermax <= 0)
  {
    (*pTOpt).itermax = INT_MAX;
    ut_print_clearline (stdout, 72);
    ut_print_message (1, 4, "`itermax' was not set: optimization will never stop.\n");
    ut_print_lineheader (0);
    printf ("    >  ");
  }

  return;
}

void
net_tess_opt_comp_rand_read (struct TOPT TOpt, int *pseedqty, int *pdimqty,
                             double *pmin, double *pmax, int *pid,
			     int **palldims, int *palldimqty)
{
  int fct_varqty;
  char *fct = NULL;
  char **fct_vars = NULL;
  char **fct_vals = NULL;
  int varqty;
  char **vars = NULL;
  double *vals = NULL;

  ut_string_function_separate (TOpt.algoname, &fct, &fct_vars, &fct_vals, &fct_varqty);

  varqty = 4;
  vars = ut_alloc_1d_pchar (varqty);
  vals = ut_alloc_1d (varqty);
  ut_string_string ("seednb", vars);
  ut_string_string ("dim", vars + 1);
  ut_string_string ("inistep", vars + 2);
  ut_string_string ("avdiameq", vars + 3);
  vals[0] = TOpt.SSet.N;
  vals[1] = TOpt.Dim;
  vals[2] = TOpt.inistep;
  neut_tess_cellavdiameq (TOpt.Dom, TOpt.CellQty, vals + 3);

  if (fct_varqty != 5)
  {
    printf ("\n");
    ut_print_message (2, 3, "Could not process expression `%s'.\n", TOpt.algoname);
  }

  *palldimqty = 0;
  if (ut_string_inlist (TOpt.dof, NEUT_SEP_NODEP, "x"))
    ut_array_1d_int_list_addelt (palldims, palldimqty, 0);
  if (ut_string_inlist (TOpt.dof, NEUT_SEP_NODEP, "y"))
    ut_array_1d_int_list_addelt (palldims, palldimqty, 1);
  if (ut_string_inlist (TOpt.dof, NEUT_SEP_NODEP, "z"))
    ut_array_1d_int_list_addelt (palldims, palldimqty, 2);
  if (ut_string_inlist (TOpt.dof, NEUT_SEP_NODEP, "w"))
    ut_array_1d_int_list_addelt (palldims, palldimqty, TOpt.Dim);

  ut_math_eval_int (fct_vals[0], varqty, vars, vals, pseedqty);
  ut_math_eval_int (fct_vals[1], varqty, vars, vals, pdimqty);
  ut_math_eval     (fct_vals[2], varqty, vars, vals, pmin);
  ut_math_eval     (fct_vals[3], varqty, vars, vals, pmax);
  ut_math_eval_int (fct_vals[4], varqty, vars, vals, pid);

  ut_free_2d_char (fct_vars, fct_varqty);
  ut_free_2d_char (fct_vals, fct_varqty);
  ut_free_2d_char (vars, varqty);
  ut_free_1d (vals);
  ut_free_1d_char (fct);

  return;
}

void
net_tess_opt_comp_rand_shift (double *x, struct TOPT *pTOpt, int seedqty,
			      int dimqty, double min, double max,
			      int *alldims, int alldimqty, gsl_rng *r)
{
  int i, j, seed, dim, id;
  int* seeds = ut_alloc_1d_int ((*pTOpt).SSet.N);
  double *tmp = ut_alloc_1d (ut_num_max ((*pTOpt).SSet.N, 4));
  double *v = ut_alloc_1d ((*pTOpt).Dim);
  int *dims = ut_alloc_1d_int (4);

  // seeds
  ut_array_1d_set_rand (tmp, (*pTOpt).SSet.N, 0, 1, r);
  ut_array_1d_sort_index (tmp, (*pTOpt).SSet.N, seeds);
  ut_array_1d_int_addval (seeds, seedqty, 1, seeds);

  // change x
  for (i = 0; i < seedqty; i++)
  {
    seed = seeds[i];

    // dims
    ut_array_1d_int_choose (alldims, alldimqty, dims, dimqty, r);
    ut_space_random (r, dims, dimqty, min, max, v);

    for (j = 0; j < dimqty; j++)
    {
      dim = dims[j];
      id = (*pTOpt).seedvar_x[seed][dim];
      x[id] += v[dim];
      x[id] = ut_num_max (x[id], (*pTOpt).boundl[id]);
      x[id] = ut_num_min (x[id], (*pTOpt).boundu[id]);
    }
  }

  ut_free_1d_int (seeds);
  ut_free_1d (tmp);
  ut_free_1d (v);
  ut_free_1d_int (dims);

  return;
}

void
net_tess_opt_comp_rand_revert (double *x, struct TOPT TOpt, double *x_cpy)
{
  ut_array_1d_memcpy (x, TOpt.xqty, x_cpy);

  return;
}
