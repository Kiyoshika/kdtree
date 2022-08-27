#include "kdtree.h"
#include <stdio.h>
#include <time.h>

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
    //mat_random(&data, 0.0f, 100.0f);
    
    // static dataset I used for manual validation (validated up to K = 27, i.e., the entire set)
    mat_set(&data, 0, 0, 2.36149); 	    mat_set(&data, 0, 1, 2.783264); 	mat_set(&data, 0, 2, 0.775234); 
    mat_set(&data, 1, 0, 0.828394); 	mat_set(&data, 1, 1, 3.814772); 	mat_set(&data, 1, 2, 0.565167); 
    mat_set(&data, 2, 0, 1.89549); 	    mat_set(&data, 2, 1, 1.595959); 	mat_set(&data, 2, 2, 4.730657); 
    mat_set(&data, 3, 0, 4.205801); 	mat_set(&data, 3, 1, 2.341894); 	mat_set(&data, 3, 2, 0.597335); 
    mat_set(&data, 4, 0, 3.000657); 	mat_set(&data, 4, 1, 2.019014); 	mat_set(&data, 4, 2, 1.532715); 
    mat_set(&data, 5, 0, 0.113309); 	mat_set(&data, 5, 1, 1.482934); 	mat_set(&data, 5, 2, 0.300352); 
    mat_set(&data, 6, 0, 3.580823); 	mat_set(&data, 6, 1, 1.324869); 	mat_set(&data, 6, 2, 0.618143); 
    mat_set(&data, 7, 0, 3.017669); 	mat_set(&data, 7, 1, 0.709847); 	mat_set(&data, 7, 2, 2.6318); 
    mat_set(&data, 8, 0, 4.576342); 	mat_set(&data, 8, 1, 1.176991); 	mat_set(&data, 8, 2, 4.10803); 
    mat_set(&data, 9, 0, 0.266268); 	mat_set(&data, 9, 1, 0.314216); 	mat_set(&data, 9, 2, 3.414809); 
    mat_set(&data, 10, 0, 3.395444); 	mat_set(&data, 10, 1, 2.675707); 	mat_set(&data, 10, 2, 1.198072); 
    mat_set(&data, 11, 0, 4.170678); 	mat_set(&data, 11, 1, 3.504101); 	mat_set(&data, 11, 2, 0.012844); 
    mat_set(&data, 12, 0, 4.735846); 	mat_set(&data, 12, 1, 0.399591); 	mat_set(&data, 12, 2, 1.608803); 
    mat_set(&data, 13, 0, 4.466502); 	mat_set(&data, 13, 1, 4.605392); 	mat_set(&data, 13, 2, 3.950697); 
    mat_set(&data, 14, 0, 0.063837); 	mat_set(&data, 14, 1, 2.606049); 	mat_set(&data, 14, 2, 0.96971); 
    mat_set(&data, 15, 0, 1.596552); 	mat_set(&data, 15, 1, 2.719358); 	mat_set(&data, 15, 2, 2.452645); 
    mat_set(&data, 16, 0, 1.896904); 	mat_set(&data, 16, 1, 1.300181); 	mat_set(&data, 16, 2, 3.777514); 
    mat_set(&data, 17, 0, 2.515047); 	mat_set(&data, 17, 1, 4.31785); 	mat_set(&data, 17, 2, 4.487361); 
    mat_set(&data, 18, 0, 0.146847); 	mat_set(&data, 18, 1, 3.894192); 	mat_set(&data, 18, 2, 0.664352); 
    mat_set(&data, 19, 0, 4.254877); 	mat_set(&data, 19, 1, 4.16046); 	mat_set(&data, 19, 2, 0.978569); 
    mat_set(&data, 20, 0, 2.669686); 	mat_set(&data, 20, 1, 2.555904); 	mat_set(&data, 20, 2, 3.654275); 
    mat_set(&data, 21, 0, 3.867759); 	mat_set(&data, 21, 1, 1.726582); 	mat_set(&data, 21, 2, 2.158376); 
    mat_set(&data, 22, 0, 3.880602); 	mat_set(&data, 22, 1, 1.462428); 	mat_set(&data, 22, 2, 2.557967); 
    mat_set(&data, 23, 0, 0.489405); 	mat_set(&data, 23, 1, 0.92893); 	mat_set(&data, 23, 2, 2.163359); 
    mat_set(&data, 24, 0, 4.440102); 	mat_set(&data, 24, 1, 0.992767); 	mat_set(&data, 24, 2, 4.769408); 
    mat_set(&data, 25, 0, 0.409812); 	mat_set(&data, 25, 1, 2.589319); 	mat_set(&data, 25, 2, 2.488766); 
    mat_set(&data, 26, 0, 2.862457); 	mat_set(&data, 26, 1, 4.486223); 	mat_set(&data, 26, 2, 3.788948);
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

    size_t k = 5;
    Vector** find = kdtree_find_k_nearest(kdtree, search_point, k, &squared_distance);

    printf("\nFound Points:\n");
    for (size_t i = 0; i < k; ++i)
    {
        printf("(%.2f, %.2f, %.2f)\n",
            vec_at(find[i], 0),
            vec_at(find[i], 1),
            vec_at(find[i], 2));
    }

    printf("\n\n");

    mat_free(&data);
    kdtree_free(&kdtree);
    vec_free(&search_point);
    for (size_t i = 0; i < k; ++i)
        vec_free(&find[i]);
    free(find);

    return 0;
}