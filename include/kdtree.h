#ifndef KDTREE_H
#define KDTREE_H

#include "matrix.h"
#include "vector.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct kdtree_t kdtree_t;

typedef struct kdtree_t
{
    Matrix* data;
    kdtree_t* previous;
    kdtree_t* left;
    kdtree_t* right;
    size_t split_dimension;
    float split_point;
} kdtree_t;

kdtree_t* kdtree_init();
void kdtree_build(kdtree_t** kdtree, Matrix** data, const size_t max_leaf_size);
void kdtree_free(kdtree_t** kdtree);

#endif