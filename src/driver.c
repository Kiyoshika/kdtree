#include "kdtree.h"
#include <stdio.h>

float squared_distance(const Vector* x, const Vector* y)
{
    float sum = 0.0f;
    for (size_t i = 0; i < x->n_elem; ++i)
        sum += (vec_at(x, i) - vec_at(y, i)) * (vec_at(x, i) - vec_at(y, i));
    
    return sum;
}

int main()
{
    Matrix* data = NULL;
    mat_init(&data, 27, 3);
    mat_random(&data, 0.0f, 5.0f);
    mat_print(data);

    kdtree_t* kdtree = kdtree_init();
    kdtree_build(&kdtree, &data, 3);

    Vector* search_point = NULL;
    vec_init(&search_point, 3);
    vec_set(&search_point, 0, 2.0f);
    vec_set(&search_point, 1, 3.5f);
    vec_set(&search_point, 2, 1.2f);

    printf("\n\nSearch Point: (%.2f, %.2f, %.2f)\n", 
        vec_at(search_point, 0), 
        vec_at(search_point, 1),
        vec_at(search_point, 2));

    Vector* find = kdtree_find_k_nearest(kdtree, search_point, 1, &squared_distance);

    printf("\nFound Point: (%.2f, %.2f, %.2f)\n", 
        vec_at(find, 0), 
        vec_at(find, 1), 
        vec_at(find, 2));

    mat_free(&data);
    kdtree_free(&kdtree);

    return 0;
}