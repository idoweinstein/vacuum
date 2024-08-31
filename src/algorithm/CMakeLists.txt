add_library(greedyalgorithm SHARED path_tree.cc base_algorithm.cc a/greedy_algorithm.cc)
set_target_properties(greedyalgorithm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
add_custom_target(algorithm1
	DEPENDS greedyalgorithm
	COMMENT "Building greedyalgorithm library")

add_library(dfsalgorithm SHARED path_tree.cc base_algorithm.cc b/dfs_algorithm.cc)
set_target_properties(dfsalgorithm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
add_custom_target(algorithm2
	DEPENDS dfsalgorithm
	COMMENT "Building dfsalgorithm library")

add_custom_target(algorithms
	DEPENDS algorithm1 algorithm2
	COMMENT "Building all algorithm libraries")
