/** @file   test_kdtree.c
 ** @brief  Test kd-tree
 ** @author Paolo D'Apice
 **/

/*
Copyright (C) 2013 Paolo D'Apice.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <vl/random.h>
#include <vl/mathop.h>
#include <vl/kdtree.h>

int
main (int argc VL_UNUSED, char** argv VL_UNUSED)
{
  vl_size numDimensions = 1000 ;
  vl_size numSamples    = 2000 ;
  double * data ;

  VlKDForest * forest ;

  unsigned int i;

  {
    /* Create data */

    VlRand * rand = vl_get_rand() ;
    vl_uindex i ;
    double * Xi = data = vl_malloc(sizeof(double) * numDimensions * numSamples) ;
    for (i = 0 ; i < numDimensions * numSamples ; ++ i) {
      *Xi++ = vl_rand_real1(rand) ;
    }
  }

  {
    /* Build */

    /*VlVectorComparisonType distance = VlDistanceL2 ;*/
    VlVectorComparisonType distance = VlDistanceHistInt ;
    /*VlVectorComparisonType distance = VlKernelL1 ;*/

    vl_type dataType = VL_TYPE_DOUBLE ;
    VlKDTreeThresholdingMethod thresholdingMethod = VL_KDTREE_MEDIAN ;
    vl_size numTrees = 1 ;

    forest = vl_kdforest_new (dataType, numDimensions, numTrees, distance) ;
    vl_kdforest_set_thresholding_method (forest, thresholdingMethod) ;

    vl_kdforest_build(forest, numSamples, data);

    /*VL_PRINTF("vl_kdforestbuild: number of trees: %d\n",*/
              /*vl_kdforest_get_num_trees(forest)) ;*/
    assert (numTrees == vl_kdforest_get_num_trees(forest)) ;

    for (i = 0; i < numTrees ; ++i ) {
      VL_PRINTF("vl_kdforestbuild: tree %d: data %s [%d x %d]\n",
                i + 1,
                vl_get_type_name (dataType),
                vl_kdforest_get_data_dimension(forest),
                vl_kdforest_get_num_nodes_of_tree(forest, i)) ;
    }
  }

  {
    /* Query */

    double * query ;
    vl_size numNeighbors = 1 ;
    unsigned int maxNumComparisons = 0 ;

    VlKDForestSearcher * searcher ;
    VlKDForestNeighbor * neighbors = vl_malloc(sizeof(VlKDForestNeighbor) * numNeighbors) ;

    vl_kdforest_set_max_num_comparisons (forest, maxNumComparisons) ;

    query = data + numDimensions; /* query is the first record in data */

    /*vl_kdforest_query(forest, neighbors, numNeighbors, query);*/
    searcher = vl_kdforest_new_searcher (forest) ;
    vl_kdforestsearcher_query (searcher, neighbors, numNeighbors, query) ;

    for (i = 0 ; i < numNeighbors ; ++ i) {
      VlKDForestNeighbor * curr = neighbors + i ;
      assert (curr);
      vl_uindex index = curr->index ;
      double distance = curr->distance ;
      VL_PRINTF("%d)\t%d\t%f\n", i, index, distance);
    }

    vl_free (neighbors); /* automatically freed with the forest */
    vl_kdforestsearcher_delete(searcher);
  }

  vl_kdforest_delete (forest) ;
  vl_free (data) ;

  return 0 ;
}
