# kdtree
This is a simple implementation of a (static) kdtree that I will be using in one of my other projects. It does not support inserting/removing as I don't personally need that functionality (but feel free to contribute).

For details on implementation, see [implementation details](#implementation-details)

# Stress Test
Before talking about performance, I'll list my specs for my shitty laptop:
* Processor: Intel(R) Celeron(R) N4020 CPU @ 1.10GHz
* Cores: 2 (even though this algo is single threaded)
* CPU MHz: 2686.254
* RAM: 4GB

Other notes:
* Compiler: gcc
* Optimization: -O2
* Leaf Size: 100 (parameter in kdtree)
* The search point is randomly sampled from the original matrix each iteration

**Test:** 100,000 random points with 100 dimensions searching for K = 20 neighbors.

**Results:** (measured with `clock_t`)
* Building tree: 7811ms (7.8s)
* Finding 20 neighbors for 1 random point: 159ms (0.16s)
* Finding 20 neighbors for 10 random points: 1480ms (1.48s)
* Finding 20 neighbors for 100 random points: 15635ms (15.6s)
* Finding 20 neighbors for 1000 random points: 156542ms (2m36s)

From this testing it appears searching a point scales linearly at around 150ms each (for a 100000 x 100 matrix).

I don't have it listed here but playing around with the leaf sizes seem to only have a minor impact (changing the 100 benchmark by maybe 1-2 seconds).

This seems to barely be better than the naive implementation so I feel there is definitely room for improvement...


# Building from Source
This library depends my other library [CMatrix](https://github.com/Kiyoshika/CMatrix).

To properly clone, use `git clone --recursive git@github.com:Kiyoshika/kdtree.git`.

Inside root directory, follow the typical CMake procedure:
* `mkdir build && cd build`
* `cmake ..`
* `make`

This will generate a `libkdtree.a` static library that you can include in other projects. Otherwise, see below section for how to directly link it to another project in CMake.

# Including in Other CMake Projects
* (optional) create a directory to store dependency, e.g., `ext/`
* add the library as a submodule
    * `git submodule add https://github.com/Kiyoshika/kdtree.git`
* in your root CMakeLists.txt, add the directory
    * `add_subdirectory(ext/kdtree)`
* for your library/executable target, link the library and headers
    * `target_link_libraries([your target] kdtree)`
    * `target_include_directories([your target] PUBLIC ${kdtree_SOURCE_DIR}/include)`

Now it should be apart of your build and you can use `#include "kdtree.h"`

# Implementation Details
The tree only stores data in leaf nodes. All other nodes are "signposts" on where to go (left vs right).

This uses a parameter `max_leaf_size` to control how much data (at most) should fit inside a leaf node. We're essentially breaking the dataset into small chunks. The larger this parameter, the less branches we have but the more data we have to iterate (e.g., more distance computations).

Once inside a leaf node, we iterate over all data points inside and compare it against a list of K data points we consider "best" making any adjustments as necessary. Once we finish this leaf node, we close it off so we don't iterate it again. We then start traversing backwards up the tree comparing the perpendicular distance to the previous split point against our current worst point's distance; if this distance is smaller, we move over to that other branch to see if we're potentially missing any other points. This process is done until we traverse all the way back up to the root node.

Each leaf node in the tree is visited at most once, so worst case scenario we iterate over the entire data set one time.

# Example
See [this](src/driver.c) for a complete example.
