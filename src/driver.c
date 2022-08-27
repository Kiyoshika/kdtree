#include "kdtree.h"
#include <stdio.h>

int main()
{
    Matrix* data = NULL;
    mat_init(&data, 27, 3);
    mat_random(&data, 0.0f, 5.0f);
    mat_print(data);

    kdtree_t* kdtree = kdtree_init();
    kdtree_build(&kdtree, &data, 3);

    mat_free(&data);
    kdtree_free(&kdtree);

    return 0;
}