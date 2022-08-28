# kdtree
This is a simple implementation of a (static) kdtree that I will be using in one of my other projects. It does not support inserting/removing as I don't personally need that functionality (but feel free to contribute).

This is a non-canonical implementation of a kdtree. Typically people insert a point on each node, but I rather insert a subset of data into the leaf nodes (and store no data in the traversal nodes).

For details on implementation, see [implementation details](#implementation-details)

# Stress Test
Before talking about performance, I'll list my specs for my shitty laptop:
* Processor: Intel(R) Celeron(R) N4020 CPU @ 1.10GHz
* Cores: 2 (even though this algo is single threaded)
* CPU MHz: 2686.254
* RAM: 4GB

Compiler Options:
* Compiler: gcc
* Optimization: -O2

kdtree Parameters:
* Neighbors: K = 10
* Leaf Size: 2 * K (20)

**Test:** 100,000 random points with 100 dimensions searching for K = 10 neighbors.

**Results:** (measured with `clock_t`)
* Tree build time: 8792ms (8.8s)
* Searching 10 Neighbors for 1 Random Point: under 0 ms (under 0s)
* Searching 10 Neighbors for 10 Random Points: 2ms (0.002s)
* Searching 10 neighbors for 100 random points: 13 ms (0.013s)
* Searching 10 neighbors for 1000 random points: 124ms (0.124s)

# Warnings
For some small leaf sizes (under 10), I have seen some instances of not 100% obtaining the nearest neighbors (1-2 points could be slightly off). For small- to mid-sized data sets you can use `2 * K` and for larger data sets maybe bump it to 25-100+. This needs some more research/testing.

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
