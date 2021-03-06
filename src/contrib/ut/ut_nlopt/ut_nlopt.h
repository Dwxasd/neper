/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2018, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#ifdef __cplusplus
extern "C"
{
#endif

/// \file ut_nlopt.h
/// \brief Manipulate data associated to tessellations or meshes (TESSDATA, NLOPT)
/// \author Romain Quey
/// \bug No known bugs

#ifndef UT_NLOPT_H
#define UT_NLOPT_H

#ifdef HAVE_NLOPT
#include<nlopt.h>
  extern void ut_nlopt_algo (char* optialgo, nlopt_algorithm *palgo);
#endif // HAVE_NLOPT

#endif				/* UT_NLOPT_H */

#ifdef __cplusplus
}
#endif
