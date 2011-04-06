//------------------------------------------------------------------------------
//
// block header
// initializes, seeds, loads and unloads data blocks
//
// Tom Peterka
// Argonne National Laboratory
// 9700 S. Cass Ave.
// Argonne, IL 60439
// tpeterka@mcs.anl.gov
//
// All rights reserved. May not be used, modified, or copied
// without permission
//
//--------------------------------------------------------------------------

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <list>
#include <vector>
#include <iterator>
#include <math.h>
#include <sys/resource.h>
#include "Lattice4D.h"
#include "LatticeAMR.h"

#ifdef _MPI
#include <mpi.h>
#include "blocking.hpp"
#include "assignment.hpp"
#include "diy.h"
#endif

// headers for computational libs
#ifdef _OSUFLOW
#include "OSUFlow.h"
#endif

enum {
  OSUFLOW,
  VOID,
};

// utility function to report memory usage
// not part of the Blocks class
int mem_size(double *vsizeMB, double *residentMemoryMB);

class Blocks {

 public:

#ifdef _MPI
  Blocks(Blocking *blocking, Assignment *assignment, void *compute, 
	 int compute_type, char **dataset_files, int num_dataset_files, 
	 DataMode data_mode, int ghost = 0);
#endif
  Blocks(Lattice4D *lat, void *compute, int compute_type,
  	    char **dataset_files, int num_dataset_files, DataMode data_mode);
  Blocks(LatticeAMR *lat, void *compute, int compute_type,
  	    char **dataset_files, int num_dataset_files, DataMode data_mode);
  void UpdateCompute(void *compute, int compute_type);
  ~Blocks();
  float ***BilLoadTimeGroupBlocks(int t_group, int nblocks, float *size,
				  int tsize, int tb);
  void DeleteBlocks(int grp, int tsize, int tb, int nblocks);
  int LoadBlocks4D(int grp, double *time, int nblocks, float *size,
		   int tsize, int tb, float ***data = NULL);
  int LoadBlock4D(int grp, int blk, double *time, float *size,
		  int tsize, int tb, int nblocks, float **data = NULL);
  int LoadBlocksAMR(int grp, double *time, DataMode dm);
  int IsBlockInTimeGroup(int g, int b, int tsize, int tb);
#ifdef _MPI
  void SetLoad(int lid) { blocks[lid].loaded = true; }
  void ClearLoad(int lid) { blocks[lid].loaded = false; }
  int GetLoad(int lid) { return blocks[lid].loaded; }
#endif

 private:

#ifdef _MPI
  vector<lb_t> blocks; // local block info
  Blocking *blocking;
  Assignment *assign;
#endif
  int ghost;
  char filename[256];
  DataMode data_mode;
  char **dataset_files;
  int num_dataset_files;
  int compute_type;
  Lattice4D *lat4D;
  LatticeAMR *latAMR;

};

#endif
