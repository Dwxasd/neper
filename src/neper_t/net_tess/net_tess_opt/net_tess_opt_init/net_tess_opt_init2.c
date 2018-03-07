/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2018, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include "net_tess_opt_init_.h"

void
net_tess_opt_init_general (struct TOPT *pTOpt)
{
  (*pTOpt).message = ut_alloc_1d_char (1000);
  strcpy ((*pTOpt).message, " ");

  return;
}

void
net_tess_opt_init_domain (struct IN_T In, struct TESS PTess, int cell,
			  struct TOPT *pTOpt)
{
  (*pTOpt).Dim = In.dim;
  neut_tess_poly_tess (PTess, cell, &((*pTOpt).Dom));
  if (ut_array_1d_int_sum (In.periodic, 3) > 0)
    net_tess_perdomain (In, PTess, cell, &((*pTOpt).DomPer));

  return;
}

void
net_tess_opt_init_parms (struct IN_T In, int level, struct MTESS MTess,
			 struct TESS *Tess, int dtess, int dcell,
			 struct TOPT *pTOpt)
{
  int i, qty1, *qty2 = NULL;
  double diameq;
  char ***parts = NULL;
  char *string = NULL;

  (*pTOpt).eps = -1;
  (*pTOpt).reps = -1;
  (*pTOpt).loopmax = -1;
  (*pTOpt).nlopt_eps = -1;
  (*pTOpt).nlopt_reps = -1;
  (*pTOpt).xeps = -1;
  (*pTOpt).xreps = -1;
  (*pTOpt).itermax = INT_MAX;	// -1 could work, but INT_MAX needed for
  // message printing
  (*pTOpt).time = -1;
  (*pTOpt).val = -HUGE_VAL;
  (*pTOpt).objval = DBL_MAX;
  (*pTOpt).objvalmin = NULL;
  (*pTOpt).iter = 0;

  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptidof[level],
					   &(*pTOpt).dof);

  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptistop[level],
					   &string);

  ut_string_separate2 (string, NEUT_SEP_NODEP, "=", &parts, &qty2, &qty1);
  for (i = 0; i < qty1; i++)
    if (!strcmp (parts[i][0], "eps"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).eps));
    else if (!strcmp (parts[i][0], "reps"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).reps));
    else if (!strcmp (parts[i][0], "loopmax"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).loopmax));
    else if (!strcmp (parts[i][0], "nlopt_eps"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).nlopt_eps));
    else if (!strcmp (parts[i][0], "nlopt_reps"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).nlopt_reps));
    else if (!strcmp (parts[i][0], "xeps"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).xeps));
    else if (!strcmp (parts[i][0], "xreps"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).xreps));
    else if (!strcmp (parts[i][0], "itermax"))
      sscanf (parts[i][1], "%d", &((*pTOpt).itermax));
    else if (!strcmp (parts[i][0], "val"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).val));
    else if (!strcmp (parts[i][0], "time"))
      sscanf (parts[i][1], "%lf", &((*pTOpt).time));
    else
      ut_print_message (1, 4, "Unknown stop criterion `%s'.  Skipping...\n",
			parts[i][0]);

  if (!strcmp (In.morphooptideltamax[level], "HUGE_VAL"))
    (*pTOpt).dist = HUGE_VAL;
  else
  {
    net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
					     In.morphooptideltamax[level],
					     &string);
    sscanf (string, "%lf", &((*pTOpt).dist));
  }

  neut_tess_cellavdiameq (Tess[dtess], ((*pTOpt).SSet).N, &diameq);

  // computing inistep
  char **vars = ut_alloc_2d_char (1, 10);
  double *vals = ut_alloc_1d (1);

  strcpy (vars[0], "avdiameq");
  vals[0] = diameq;
  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
					   In.morphooptiinistep[level],
					   &string);
  ut_math_eval (string, 1, vars, vals, &((*pTOpt).inistep));

  ut_free_2d_char (vars, 1);
  ut_free_1d (vals);

  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
					   In.morphooptiobjective[level],
					   &string);
  net_tess_opt_init_parms_objective (string, pTOpt);

  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptilogtime[level],
					   &(*pTOpt).TDyn.logtime);
  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptilogvar[level],
					   &(*pTOpt).TDyn.logvar);
  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptilogdis[level],
					   &(*pTOpt).TDyn.logdis);
  (*pTOpt).TDyn.logdis_qty = (*pTOpt).tarqty;
  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptilogval[level],
					   &(*pTOpt).TDyn.logval);
  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptilogtesr[level],
					   &(*pTOpt).TDyn.logtesr);

  ut_string_string (In.body, &((*pTOpt).TDyn.body));

  net_tess_opt_init_parms_algo (In, level, MTess, Tess, dtess, dcell, pTOpt);

  net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dcell,
                                           In.morphooptialgoneigh[level],
					   &(*pTOpt).TDyn.algoneigh);

  for (i = 0; i < (*pTOpt).tarqty; i++)
    if (!strcmp ((*pTOpt).tarvar[i], "tesr"))
      net_tess_opt_init_tesrobj (pTOpt, i);

  (*pTOpt).seedoptiqty = (*pTOpt).SSet.N;
  (*pTOpt).seedopti = ut_alloc_1d_int ((*pTOpt).SSet.N);
  ut_array_1d_int_set_id ((*pTOpt).seedopti, (*pTOpt).seedoptiqty);
  ut_array_1d_int_addval ((*pTOpt).seedopti, (*pTOpt).seedoptiqty, 1,
			  (*pTOpt).seedopti);

#ifdef HAVE_NLOPT
  net_tess_opt_init_parms_algomaxiter (In, level, pTOpt);
#endif

  for (i = 0; i < qty1; i++)
    ut_free_2d_char (parts[i], qty2[i]);
  ut_free_1d_ppchar (parts);
  ut_free_1d_int (qty2);
  ut_free_1d_char (string);

  return;
}

void
net_tess_opt_init_target (struct IN_T In, struct MTESS MTess,
			  struct TESS *Tess, int dtess, int dpoly,
			  int level, char *morpho, struct TOPT *pTOpt)
{
  int i, j, partqty, status, diameq_pos[2];
  double mean = 1;
  double sum;
  char **tmp = NULL;
  char **parts = NULL;
  char *string = NULL;
  char *morpho2 = NULL;

  if (!strcmp (morpho, "voronoi"))
  {
    (*pTOpt).tarqty = 0;
    ut_string_string (morpho, &morpho2);
  }

  else
  {
    morpho2 = ut_alloc_1d_char (1000);

    ut_string_separate (morpho, NEUT_SEP_NODEP, &parts, &partqty);

    for (i = 0; i < partqty; i++)
    {
      if (!strcmp (parts[i], "graingrowth") || !strcmp (parts[i], "gg"))
        morpho2 = strcat (morpho2, (*pTOpt).Dim == 3 ?
                          "diameq:lognormal(1,0.35),sphericity:lognormal(0.145,0.03,1-x)":
                          "diameq:lognormal(1,0.42),sphericity:lognormal(0.100,0.03,1-x)");
      else if (!strncmp (parts[i], "graingrowth", 11) || !strncmp (parts[i], "gg", 2))
      {
        if (sscanf (parts[i], "graingrowth(%lf)", &mean) == 1)
        {}
        else if (sscanf (parts[i], "gg(%lf)", &mean) == 1)
        {}
        else
          ut_print_message (2, 3, "Cannot process expression `%s'.\n", parts[i]);

        if ((*pTOpt).Dim == 3)
        {
          string = ut_alloc_1d_char (1000);
          sprintf (string,
                   "diameq:lognormal(%f,%f),sphericity:lognormal(0.145,0.03,1-x)",
                     mean, 0.35 * mean);
          morpho2 = strcat (morpho2, string);
        }
        else
        {
          string = ut_alloc_1d_char (1000);
          sprintf (string,
                   "diameq:lognormal(%f,%f),sphericity:lognormal(0.1,0.03,1-x)",
                     mean, 0.42 * mean);
          morpho2 = strcat (morpho2, string);
        }
      }
      else
        morpho2 = strcat (morpho2, parts[i]);

      if (i < partqty - 1)
        morpho2 = strcat (morpho2, NEUT_SEP_NODEP);
    }

    if (ut_string_filename (morpho2))
    {
      net_multiscale_mtess_arg_0d_char_fscanf (MTess, Tess, dtess, dpoly, morpho2, &string);
      ut_string_separate (string, NEUT_SEP_NODEP, &tmp, &((*pTOpt).tarqty));
    }

    else
      ut_string_separate (morpho2, NEUT_SEP_NODEP, &tmp, &((*pTOpt).tarqty));
  }

  ut_array_1d_int_set (diameq_pos, 2, -1);

  net_tess_opt_init_target_cellqty (In, MTess, Tess[dtess], dpoly, &(*pTOpt).CellQty);

  // allocating/initializing variables
  (*pTOpt).tarvar = ut_alloc_1d_pchar ((*pTOpt).tarqty);
  (*pTOpt).tartype = ut_alloc_1d_pchar ((*pTOpt).tarqty);
  (*pTOpt).tarexpr = ut_alloc_1d_pchar ((*pTOpt).tarqty);
  (*pTOpt).tarexprdisqty = ut_alloc_1d_int ((*pTOpt).tarqty);
  (*pTOpt).tarexprdisprop = ut_alloc_1d_pdouble ((*pTOpt).tarqty);

  (*pTOpt).tarcellval = ut_alloc_1d_ppdouble ((*pTOpt).tarqty);
  (*pTOpt).tarcellvalqty = ut_alloc_1d_int ((*pTOpt).tarqty);
  (*pTOpt).tarrefval = ut_alloc_1d ((*pTOpt).tarqty);

  (*pTOpt).tarpdf0 =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).tarpdf =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).tarcdf0 =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).tarcdf =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).grid = ut_alloc_1d_pchar ((*pTOpt).tarqty);
  (*pTOpt).cvlsig = ut_alloc_1d ((*pTOpt).tarqty);
  (*pTOpt).cvl = (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).tarmodeqty = ut_alloc_1d_int ((*pTOpt).tarqty);
  (*pTOpt).tarmodefact = ut_alloc_1d_pdouble ((*pTOpt).tarqty);
  (*pTOpt).tarmodecdf0 =
    (struct FCT **) calloc ((*pTOpt).tarqty, sizeof (struct FCT *));

  for (i = 0; i < (*pTOpt).tarqty; i++)
    ut_fct_set_zero ((*pTOpt).tarcdf + i);

  (*pTOpt).curval = ut_alloc_1d ((*pTOpt).tarqty);
  (*pTOpt).curval0 = ut_alloc_1d ((*pTOpt).tarqty);

  for (i = 0; i < (*pTOpt).tarqty; i++)
  {
    // Recording tarvar, tarexpr
    ut_string_separate (tmp[i], NEUT_SEP_DEP, &parts, &partqty);
    if (partqty != 2 && partqty != 3)
      ut_print_message (2, 2, "Unknown argument `%s'.\n", morpho);

    ut_string_string (parts[0], (*pTOpt).tarvar + i);
    ut_string_string (parts[1], (*pTOpt).tarexpr + i);

    ut_free_2d_char (parts, partqty);
    parts = NULL;

    if (!strncmp ((*pTOpt).tarexpr[i], "file", 4)
	|| !strcmp ((*pTOpt).tarexpr[i], "seed")
	|| !strncmp ((*pTOpt).tarexpr[i], "interval", 8))
      ut_string_string ("cell", &((*pTOpt).tartype[i]));
    else
      ut_string_string ("stat", &((*pTOpt).tartype[i]));

    // Reading distribution
    if (!strcmp ((*pTOpt).tartype[i], "stat"))
    {
      // Setting grid
      net_tess_opt_init_target_grid (In, level, MTess, Tess, dtess, dpoly, i, pTOpt);

      net_tess_opt_init_target_cvl (In, level, MTess, Tess, dtess, dpoly, i, pTOpt);

      // Setting distributions
      int qty3, fct_qty;
      double *fct_fact = NULL;
      char **fct_expr = NULL;
      char **parts3 = NULL;
      char **parts2 = NULL;
      struct FCT *Fct = NULL;

      ut_string_separate ((*pTOpt).tarexpr[i], "+", &parts2, &fct_qty);
      fct_fact = ut_alloc_1d (fct_qty);
      fct_expr = ut_alloc_1d_pchar (fct_qty);

      (*pTOpt).tarexprdisqty[i] = fct_qty;
      (*pTOpt).tarexprdisprop[i] = ut_alloc_1d ((*pTOpt).tarexprdisqty[i]);

      Fct = (struct FCT *) calloc (fct_qty, sizeof (struct FCT));

      for (j = 0; j < fct_qty; j++)
      {
	ut_string_separate (parts2[j], "*", &parts3, &qty3);
	if (qty3 == 1)
	  fct_fact[j] = 1;
	else if (qty3 == 2)
	  sscanf (parts3[0], "%lf", fct_fact + j);
	else
	  abort ();
	ut_string_string (parts3[qty3 - 1], fct_expr + j);

	ut_free_2d_char (parts3, qty3);

      }
      ut_array_1d_scale (fct_fact, fct_qty,
			 1 / ut_array_1d_sum (fct_fact, fct_qty));
      ut_array_1d_memcpy ((*pTOpt).tarexprdisprop[i],
			  (*pTOpt).tarexprdisqty[i], fct_fact);

      for (j = 0; j < fct_qty; j++)
	ut_fct_set (fct_expr[j], Fct + j);

      // making sure that the mean is equal to 1
      if (!strcmp ((*pTOpt).tarvar[i], "size")
	  || !strcmp ((*pTOpt).tarvar[i], "diameq"))
      {
	mean = 0;
	for (j = 0; j < fct_qty; j++)
	  mean += fct_fact[j] * Fct[j].mean;

	for (j = 0; j < fct_qty; j++)
	{
	  Fct[j].mean /= mean;
	  Fct[j].sig /= mean;
	}
      }

      (*pTOpt).tarmodeqty[i] = fct_qty;
      (*pTOpt).tarmodefact[i] = ut_alloc_1d ((*pTOpt).tarmodeqty[i]);
      (*pTOpt).tarmodecdf0[i] =
	(struct FCT *) calloc ((*pTOpt).tarmodeqty[i], sizeof (struct FCT));
      for (j = 0; j < fct_qty; j++)
	ut_fct_set_zero ((*pTOpt).tarmodecdf0[i] + j);

      for (j = 0; j < fct_qty; j++)
      {
	(*pTOpt).tarmodefact[i][j] = fct_fact[j];
	ut_fct_numericalfct_expr (Fct[j], (*pTOpt).grid[i], Fct + j);
	ut_fct_integralfct (Fct[j], (*pTOpt).tarmodecdf0[i] + j);
      }

      ut_fct_add (Fct, fct_qty, fct_fact, (*pTOpt).tarexpr[i],
		  (*pTOpt).tarpdf0 + i);

      ut_free_2d_char (parts2, fct_qty);
      ut_free_2d_char (fct_expr, fct_qty);
      ut_free_1d (fct_fact);
      for (j = 0; j < fct_qty; j++)
	ut_fct_free (Fct + j);
      free (Fct);

      ut_fct_integralfct ((*pTOpt).tarpdf0[i], (*pTOpt).tarcdf0 + i);
      ut_fct_convolution ((*pTOpt).tarpdf0[i], (*pTOpt).cvlsig[i],
			  (*pTOpt).tarpdf + i);

      ut_fct_integralfct ((*pTOpt).tarpdf[i], (*pTOpt).tarcdf + i);

      (*pTOpt).tarcellvalqty[i] = 1;
    }

    // Recording tarcellvalqty, tarcellval or tartesr
    else
    {
      if (strncmp ((*pTOpt).tarvar[i], "tesr", 4))
      {
	if (!strcmp ((*pTOpt).tarvar[i], "size")
	    || !strcmp ((*pTOpt).tarvar[i], "diameq")
	    || !strcmp ((*pTOpt).tarvar[i], "sphericity"))
	{
	  if (strncmp ((*pTOpt).tarexpr[i], "interval", 8))
	    (*pTOpt).tarcellvalqty[i] = 1;
	  else
	    (*pTOpt).tarcellvalqty[i] = 2;
	}
	else if (!strcmp ((*pTOpt).tarvar[i], "centroid"))
	  (*pTOpt).tarcellvalqty[i] = (*pTOpt).Dim;
	else if (!strcmp ((*pTOpt).tarvar[i], "centroidsize")
		 || !strcmp ((*pTOpt).tarvar[i], "centroiddiameq"))
	  (*pTOpt).tarcellvalqty[i] = (*pTOpt).Dim + 1;
	else
	  abort ();

	if ((*pTOpt).CellQty == -1)
	{
	  if (ut_string_filename ((*pTOpt).tarexpr[i]))
	    (*pTOpt).CellQty = ut_file_nbwords ((*pTOpt).tarexpr[i])
				/ (*pTOpt).tarcellvalqty[i];
	  else
	    abort ();
	}

	(*pTOpt).tarcellval[i] = ut_alloc_2d ((*pTOpt).CellQty + 1,
					      (*pTOpt).tarcellvalqty[i]);

	if (ut_string_filename ((*pTOpt).tarexpr[i]))
	{
	  status =
	    ut_array_2d_fscanfn ((*pTOpt).tarexpr[i],
				 (*pTOpt).tarcellval[i] + 1, (*pTOpt).CellQty,
				 (*pTOpt).tarcellvalqty[i]);
	  if (status != 1)
	    abort ();
	}

	else if (!strncmp ((*pTOpt).tarexpr[i], "interval", 8))
	{
	  double min, max;
	  sscanf ((*pTOpt).tarexpr[i], "interval(%lf,%lf)", &min, &max);
	  for (j = 1; j <= (*pTOpt).CellQty; j++)
	  {
	    (*pTOpt).tarcellval[i][j][0] = min;
	    (*pTOpt).tarcellval[i][j][1] = max;
	  }
	}

        else if (!strcmp ((*pTOpt).tarvar[i], "centroid")
	      && !strcmp ((*pTOpt).tarexpr[i], "seed"))
	{
	  // doing nothing as we don't have the data yet (sset)
	}

	else
	  abort ();

	// recording position of diameq specification, for scaling
	if (!strcmp ((*pTOpt).tarvar[i], "centroidsize")
	 || !strcmp ((*pTOpt).tarvar[i], "centroiddiameq"))
	  ut_array_1d_int_set_2 (diameq_pos, i, (*pTOpt).Dim);
	else if (!strcmp ((*pTOpt).tarvar[i], "size")
	      || !strcmp ((*pTOpt).tarvar[i], "diameq"))
	  ut_array_1d_int_set_2 (diameq_pos, i, 0);

	if (!strcmp ((*pTOpt).tarvar[i], "centroidsize"))
	{
	  ut_array_1d_int_set_2 (diameq_pos, i, (*pTOpt).Dim);
	  for (j = 1; j <= (*pTOpt).CellQty; j++)
	    ut_space_size_diameq ((*pTOpt).Dim,
				  (*pTOpt).tarcellval[i][j][(*pTOpt).Dim],
				  &(*pTOpt).tarcellval[i][j][(*pTOpt).Dim]);
	  ut_string_string ("centroiddiameq", (*pTOpt).tarvar + i);
	}

	else if (!strcmp ((*pTOpt).tarvar[i], "size"))
	{
	  sum = 0;
	  for (j = 1; j <= (*pTOpt).CellQty; j++)
	    sum += (*pTOpt).tarcellval[i][j][0];
	  sum /= (*pTOpt).CellQty;
	  for (j = 1; j <= (*pTOpt).CellQty; j++)
	    (*pTOpt).tarcellval[i][j][0] /= sum;
	}
      }

      else
      {
	neut_tesr_name_fscanf ((*pTOpt).tarexpr[i], &((*pTOpt).tartesr));
	if ((*pTOpt).tartesr.Dim != In.dim)
	  ut_print_message (2, 4,
			    "Input tesr is of dimension %d, but -dim = %d.\n",
			    (*pTOpt).tartesr.Dim, In.dim);
	(*pTOpt).tarcellvalqty[i] = 1;
      }
    }

    net_tess_opt_init_ref (pTOpt, mean, i);

    if (strcmp ((*pTOpt).tarvar[i], "tesr") && (*pTOpt).tarcellvalqty[i] == 0)
      abort ();
  }

  if ((*pTOpt).CellQty <= 0)
    ut_print_message (2, 4, "Could not process '-n from_morpho'.\n");

  if (diameq_pos[0] != -1)
    net_tess_opt_init_target_scale (pTOpt, diameq_pos);

  ut_free_1d_char (morpho2);
  ut_free_2d_char (tmp, (*pTOpt).tarqty);

  return;
}

void
net_tess_opt_init_current (struct TOPT *pTOpt)
{
  int i;

  (*pTOpt).curcellpenalty = ut_alloc_1d ((*pTOpt).CellQty + 1);
  (*pTOpt).curcellval = ut_alloc_1d_ppdouble ((*pTOpt).tarqty);
  for (i = 0; i < (*pTOpt).tarqty; i++)
    (*pTOpt).curcellval[i] = ut_alloc_2d ((*pTOpt).CellQty + 1,
					  (*pTOpt).tarcellvalqty[i]);

  (*pTOpt).oldcellpenalty = ut_alloc_1d ((*pTOpt).CellQty + 1);
  (*pTOpt).oldcellval = ut_alloc_1d_ppdouble ((*pTOpt).tarqty);
  for (i = 0; i < (*pTOpt).tarqty; i++)
    (*pTOpt).oldcellval[i] = ut_alloc_2d ((*pTOpt).CellQty + 1,
					  (*pTOpt).tarcellvalqty[i]);

  (*pTOpt).curpenalty = ut_alloc_1d ((*pTOpt).tarqty);
  (*pTOpt).curpdf =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).curcdf =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  (*pTOpt).curcdf0 =
    (struct FCT *) calloc ((*pTOpt).tarqty, sizeof (struct FCT));
  for (i = 0; i < (*pTOpt).tarqty; i++)
  {
    ut_fct_set_zero ((*pTOpt).curpdf + i);
    ut_fct_set_zero ((*pTOpt).curcdf + i);
    ut_fct_set_zero ((*pTOpt).curcdf0 + i);
  }

  return;
}

void
net_tess_opt_init_bounds (struct TOPT *pTOpt)
{
  int i, j, k, seed, qty, dim;
  double x;
  char **parts = NULL;

  ut_string_separate ((*pTOpt).dof, NEUT_SEP_NODEP, &parts, &qty);

  (*pTOpt).boundl = ut_alloc_1d (qty * (*pTOpt).seedoptiqty + 1);
  (*pTOpt).boundu = ut_alloc_1d (qty * (*pTOpt).seedoptiqty + 1);

  k = 0;
  for (j = 0; j < (*pTOpt).seedoptiqty; j++)
  {
    seed = (*pTOpt).seedopti[j];
    for (i = 0; i < qty; i++)
    {
      if (!strcmp (parts[i], "x") || !strcmp (parts[i], "y")
	  || !strcmp (parts[i], "z"))
      {
	dim = parts[i][0] - 'x';
	x = ((*pTOpt).SSet).SeedCoo0[seed][dim];
	(*pTOpt).boundl[k] = x - (*pTOpt).dist;
	(*pTOpt).boundu[k] = x + (*pTOpt).dist;
	k++;
      }
      else if (!strcmp (parts[i], "w"))
      {
	x = ((*pTOpt).SSet).SeedWeight[seed];
	(*pTOpt).boundl[k] = ut_num_max (0, x - (*pTOpt).dist);
	(*pTOpt).boundu[k] = x + (*pTOpt).dist;
	k++;
      }
    }
  }

  ut_free_2d_char (parts, qty);

  return;
}

void
net_tess_opt_init_post (struct IN_T In, struct TOPT *pTOpt)
{
  int i, j;

  if ((*pTOpt).tartesrscale)
  {
    neut_tess_scale (&((*pTOpt).Dom),
		     (*pTOpt).tartesrscale[0],
		     (*pTOpt).tartesrscale[1],
		     (*pTOpt).tartesrscale[2]);

    if (ut_array_1d_int_sum (In.periodic, 3) > 0)
      neut_tess_scale (&((*pTOpt).DomPer),
		       (*pTOpt).tartesrscale[0],
		       (*pTOpt).tartesrscale[1],
		       (*pTOpt).tartesrscale[2]);

    for (i = 1; i <= (*pTOpt).SSet.N; i++)
      for (j = 0; j < (*pTOpt).Dim; j++)
      {
	(*pTOpt).SSet.SeedCoo0[i][j] *= (*pTOpt).tartesrscale[j];
	(*pTOpt).SSet.SeedCoo[i][j] *= (*pTOpt).tartesrscale[j];
      }
  }

  return;
}
