/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2018, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef STRUCTVOX_H
#define STRUCTVOX_H

  struct TESR
  {
    // GENERAL INFORMATION -----------------------------------------------

    int Dim;			// Dimension
    int CellQty;		// number of polyhedra
    int *CellId;		// identifiers of the cells
    double **CellOri;		// crystal orientations of the cells
    int ***CellBBox;		// bbox of a cell [cell][0...2][0,1]
    double **CellCoo;		// centroids of the cells
    double *CellVol;		// volumes/areas of the cells
    double *CellConvexity;      // Convexity of the cells

    int *size;			// number of points in the 3 directions of space
    double *vsize;		// size of points in the 3 directions of space

    // SEED INFORMATION --------------------------------------------------

    double **SeedCoo;		// SeedCoo[i][j] (j=0...2) are the 3 coordinates
    // of the center.
    double *SeedWeight;		// SeedWeight[i] is the Laguerre weight

    // DATA --------------------------------------------------------------

    int ***VoxCell;		// VoxPoly[i][j][k]: cell of point at
    // position i, j, k, with i in [1...size[0]],
    // j in [1...size[1]] and k in [1...size[2]]
  };
  typedef struct TESR TESR;

#endif				/* STRUCTVOX_H */

#ifdef __cplusplus
}
#endif
