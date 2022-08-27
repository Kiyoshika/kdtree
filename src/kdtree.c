#include "kdtree.h"

static void err(const char* msg)
{
    printf("ERROR: %s\n", msg);
    exit(-1);
}
kdtree_t* kdtree_init()
{
    void* alloc = malloc(sizeof(kdtree_t));
    if (!alloc)
        err("Couldn't allocate memory for kdtree_t.");
    kdtree_t* kdtree = alloc;
    kdtree->data = NULL;
    kdtree->previous = NULL;
    kdtree->left = NULL;
    kdtree->right = NULL;
    kdtree->split_dimension = 0;
    kdtree->split_point = 0.0f;

    return kdtree;
}

void kdtree_build(
    kdtree_t** kdtree, 
    Matrix** data, 
    const size_t max_leaf_size)
{
    if (!(*kdtree) || !(*data) || ((*kdtree)->data && (*kdtree)->data->n_rows <= max_leaf_size))
        return;

    // there are much more efficient ways to handle this sorting business
    // but for a simple implementation, this will serve its purpose fine enough
    mat_sort(data, (*kdtree)->split_dimension, true);
    size_t midpoint = (*data)->n_rows / 2;
    (*kdtree)->split_point = mat_at(*data, midpoint, (*kdtree)->split_dimension);

    (*kdtree)->left = kdtree_init();
    (*kdtree)->left->split_dimension = ((*kdtree)->split_dimension + 1) % (*data)->n_columns;
    (*kdtree)->left->previous = *kdtree;

    (*kdtree)->right = kdtree_init();
    (*kdtree)->right->split_dimension = ((*kdtree)->split_dimension + 1) % (*data)->n_columns;
    (*kdtree)->right->previous = *kdtree;

    Matrix* left_data = mat_subset(*data, 0, midpoint - 1, 0, (*data)->n_columns - 1);
    Matrix* right_data = mat_subset(*data, midpoint, (*data)->n_rows - 1, 0, (*data)->n_columns - 1);

    if (left_data->n_rows <= max_leaf_size && left_data->n_rows > 0)
    {
        (*kdtree)->left->data = mat_copy(left_data);
        printf("\n\nChild Data:\n");
        mat_print((*kdtree)->left->data);
    }
    if (right_data->n_rows <= max_leaf_size && right_data->n_rows > 0)
    {
        (*kdtree)->right->data = mat_copy(right_data);
        printf("\n\nChild Data:\n");
        mat_print((*kdtree)->right->data);
    }
    
    kdtree_build(&(*kdtree)->left, &left_data, max_leaf_size);
    kdtree_build(&(*kdtree)->right, &right_data, max_leaf_size);

    if (left_data)
        mat_free(&left_data);
    if (right_data)
        mat_free(&right_data);
}

void kdtree_free(kdtree_t** kdtree)
{
    if ((*kdtree)->left)
        kdtree_free(&(*kdtree)->left);
    
    if ((*kdtree)->right)
        kdtree_free(&(*kdtree)->right);
    
    if ((*kdtree)->data)
        mat_free(&(*kdtree)->data);
    
    if (*kdtree)
    {
        free(*kdtree);
        *kdtree = NULL;
    }
}