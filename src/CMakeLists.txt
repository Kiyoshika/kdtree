add_library(kdtree kdtree.c)
target_include_directories(kdtree PUBLIC ${kdtree_SOURCE_DIR}/include)
target_include_directories(kdtree PUBLIC ${CMatrix_SOURCE_DIR}/include/matrix)
target_include_directories(kdtree PUBLIC ${CMatrix_SOURCE_DIR}/include/vector)
target_link_libraries(kdtree matrix vector)

add_executable(driver driver.c)
target_include_directories(driver PUBLIC ${kdtree_SOURCE_DIR}/include)
target_link_libraries(driver kdtree)