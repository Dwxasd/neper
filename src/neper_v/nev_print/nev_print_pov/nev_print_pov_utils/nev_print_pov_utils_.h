/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2016, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include"ut.h"
#include"neut_v.h"
// #include"../../nefm/nefm.h"
#include "nev_print_pov_utils.h"
#include "../nev_print_pov_header/nev_print_pov_header.h"
#include "../nev_print_pov_foot/nev_print_pov_foot.h"

extern void nev_print_pov_triangle (FILE *, double *, double *, double *,
				char *, char *, char *);
extern void nev_print_pov_triangle_edge (FILE *, double *, double *, double *,
				     char *, char *);
extern void nev_print_pov_segment (FILE *, double *, double *, char *, char *);
extern void nev_print_pov_tet_edge (FILE *, double *, double *, double *,
				double *, char *, char *);
extern void nev_print_pov_tet (FILE *, double *, double *, double *, double *,
			   char *, char *, char *);

extern int nev_print_pov_cylinder_test (double *coo1, double *coo2, char
    *edgerad);
