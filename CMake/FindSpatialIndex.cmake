
# Search for the header
FIND_PATH(SpatialIndex_INCLUDE_DIR "spatial_index/index.hpp" )
FIND_PATH(nlohmann_INCLUDE_DIR "nlohmann/json.hpp")
list(APPEND SpatialIndex_INCLUDE_DIR ${nlohmann_INCLUDE_DIR})
set(SpatialIndex_FOUND TRUE)

