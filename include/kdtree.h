#ifndef KDTREE_H
#define KDTREE_H

#include "matrix.h"
#include "vector.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct kdtree_t kdtree_t;

typedef struct kdtree_t
{
    Matrix* data;
    kdtree_t* previous;
    kdtree_t* left;
    kdtree_t* right;
    size_t split_dimension;
    float split_point;
    bool ignore;
} kdtree_t;

kdtree_t* kdtree_init();

void kdtree_build(
    kdtree_t** kdtree,
    Matrix** data,
    const size_t max_leaf_size);

Vector** kdtree_find_k_nearest(
    kdtree_t* kdtree,
    Vector* search_point,
    const size_t k,
    float (*distance)(const Vector*, const Vector*));

void kdtree_free(kdtree_t** kdtree);

#endif