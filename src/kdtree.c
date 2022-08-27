#include "kdtree.h"
#include <stdio.h>

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
    kdtree->ignore = false;

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
        (*kdtree)->left->data = mat_copy(left_data);
    if (right_data->n_rows <= max_leaf_size && right_data->n_rows > 0)
        (*kdtree)->right->data = mat_copy(right_data);
    
    kdtree_build(&(*kdtree)->left, &left_data, max_leaf_size);
    kdtree_build(&(*kdtree)->right, &right_data, max_leaf_size);

    if (left_data)
        mat_free(&left_data);
    if (right_data)
        mat_free(&right_data);
}

struct best_points_t
{
    Vector** addresses;
    float* distances;
    size_t index;
    size_t capacity;
};

static void __insert_point_asc(
    struct best_points_t* best_points,
    Vector* point,
    const float point_distance)
{
    for (size_t i = 0; i <= best_points->index; ++i)
    {
        if (point_distance < best_points->distances[i])
        {
            // free edge point
            if (best_points->addresses[best_points->index])
                vec_free(&best_points->addresses[best_points->index]);

            // shift everything to the right (if necessary)
            for (size_t end = best_points->index; end > i; end--)
            {
                best_points->addresses[end] = best_points->addresses[end - 1];
                best_points->distances[end] = best_points->distances[end - 1];
            }

            // insert new point in best position
            best_points->addresses[i] = point;
            best_points->distances[i] = point_distance;

            break;
        }
    }

    if (best_points->index < best_points->capacity - 1)
        best_points->index++;
}

void __kdtree_find_k_nearest(
    kdtree_t* kdtree,
    Vector* search_point,
    float (*distance)(const Vector*, const Vector*),
    struct best_points_t* best_points)
{
    if (!kdtree)
        return;
    
    if (kdtree->left && !kdtree->left->ignore && vec_at(search_point, kdtree->split_dimension) < kdtree->split_point)
        __kdtree_find_k_nearest(kdtree->left, search_point, distance, best_points);
    else if (kdtree->right && !kdtree->right->ignore)
        __kdtree_find_k_nearest(kdtree->right, search_point, distance, best_points);
    
    if (!kdtree->data)
        return;

    //mat_print(kdtree->data);
    //printf("\n\n");

    // search leaf node for nearest neighbors
    for (size_t i = 0; i < kdtree->data->n_rows; ++i)
    {
        Vector* point = mat_get_row(kdtree->data, i);

        float point_distance = distance(search_point, point);

        if (!best_points->addresses[0])
        {
            best_points->addresses[best_points->index] = point;
            best_points->distances[best_points->index] = point_distance;
            if (best_points->index < best_points->capacity - 1)
                best_points->index++;
        }
        else if (best_points->index < best_points->capacity - 1
                || point_distance < best_points->distances[best_points->index])
        {
            __insert_point_asc(best_points, point, point_distance);
        }
        else
            vec_free(&point);
        
    }

    kdtree->ignore = true;

    while (kdtree->previous)
    {
        kdtree = kdtree->previous;
        bool traverse_left = false;
        for (size_t i = 0; i <= best_points->index; ++i)
        {
            if (best_points->addresses[i])
            {
                float perp_dist = kdtree->split_point - vec_at(best_points->addresses[i], kdtree->split_dimension);
                traverse_left = false;
                if (perp_dist < 0.0f)
                    traverse_left = true;
                //perp_dist = -1.0f;
                if (perp_dist * perp_dist < best_points->distances[best_points->index])
                {
                    if (traverse_left && kdtree->left && !kdtree->left->ignore)
                        __kdtree_find_k_nearest(kdtree->left, search_point, distance, best_points);
                    else if (kdtree->right && !kdtree->right->ignore)
                        __kdtree_find_k_nearest(kdtree->right, search_point, distance, best_points);
                }
            }
        }
    }

    return;
}

static void __reset_leaves(kdtree_t* kdtree)
{
    if (kdtree->left)
        __reset_leaves(kdtree->left);
    if (kdtree->right)
        __reset_leaves(kdtree->right);
    
    kdtree->ignore = false;
}

Vector** kdtree_find_k_nearest(
    kdtree_t* kdtree,
    Vector* search_point,
    const size_t k,
    float (*distance)(const Vector*, const Vector*))
{
    struct best_points_t* best_points = malloc(sizeof(struct best_points_t));
    best_points->index = 0;
    best_points->capacity = k;
    best_points->addresses = malloc(sizeof(Vector*) * k);
    best_points->distances = malloc(sizeof(float) * k);
    for (size_t i = 0; i < k; ++i)
    {
        best_points->addresses[i] = NULL;
        best_points->distances[i] = 99999999.0f;
    }

    __kdtree_find_k_nearest(kdtree, search_point, distance, best_points);

    // reset leaf nodes to NOT ignore them on any subsequent calls
    // NOTE: it would probably be better to store the leaves in an array
    // but it would have to be dynamically sized while traversing which
    // I think is kind of annoying so we're just gonna re-recurse
    __reset_leaves(kdtree);

    // move the points over into a different vector so we can clean up memory
    // from best_points_t struct
    Vector** best_points_ret = malloc(sizeof(Vector*) * k);
    for (size_t i = 0; i < k; ++i)
        best_points_ret[i] = best_points->addresses[i];

    free(best_points->distances);
    best_points->distances = NULL;
    free(best_points->addresses);
    best_points->addresses = NULL;
    free(best_points);
    best_points = NULL;

    return best_points_ret;
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