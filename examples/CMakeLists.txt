# Data copy to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION 
  ${CMAKE_BINARY_DIR}/data)

add_subdirectory(phyanimCheckCollisions)
add_subdirectory(phyanimFormatConverter)
add_subdirectory(phyanimTetrahedralizeMesh)
add_subdirectory(phyanimOverlap)

if(GLFW3_FOUND)
  add_subdirectory(phyanimAnimMesh)
  add_subdirectory(phyanimMoveMesh)
  add_subdirectory(phyanimRenderMesh)
  add_subdirectory(phyanimSceneGenerator) 

  add_subdirectory(phyanimOverlapCollisions)
  add_subdirectory(phyanimSomaGenerator)
endif()
