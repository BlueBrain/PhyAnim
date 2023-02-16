# Data copy to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION
  ${CMAKE_BINARY_DIR}/data)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/multi_index.py DESTINATION
  ${CMAKE_BINARY_DIR}/)



add_compile_definitions(DATAPATH="${CMAKE_BINARY_DIR}/data")

add_subdirectory(phyanimCheckCollisions)
add_subdirectory(phyanimFormatConverter)
add_subdirectory(phyanimTetrahedralizeMesh)
add_subdirectory(phyanimOverlap)
add_subdirectory(test)

if(GLFW3_FOUND)
  add_subdirectory(phyanimRenderMesh)
  add_subdirectory(phyanimAnimMesh)
  add_subdirectory(phyanimMoveMesh)
  add_subdirectory(phyanimRenderMorpho)
  add_subdirectory(phyanimOverlapCircuit)
  
  add_subdirectory(phyanimSceneGenerator)

  add_subdirectory(phyanimOverlapCollisions)
  add_subdirectory(phyanimSomaGenerator)
  add_subdirectory(phyanimDemo)
endif()
