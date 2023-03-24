# Data copy to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION
  ${CMAKE_BINARY_DIR}/data)

file(GLOB  PYTHON_FILES "*.py")
file(COPY ${PYTHON_FILES} DESTINATION
  ${CMAKE_BINARY_DIR})

add_compile_definitions(DATAPATH="${CMAKE_BINARY_DIR}/data")

add_subdirectory(appCheckCollisions)
add_subdirectory(appFormatConverter)
add_subdirectory(appTetrahedralizeMesh)
add_subdirectory(appOverlap)
add_subdirectory(appTest)
add_subdirectory(appOverlapCircuit)
add_subdirectory(appCollidingSomas)



if(GLFW3_FOUND)
  add_subdirectory(appCollidingSomasViz)
  add_subdirectory(appRenderMesh)
  add_subdirectory(appAnimMesh)
  add_subdirectory(appMoveMesh)
  add_subdirectory(appRenderMorpho)
  add_subdirectory(appOverlapCircuitViz)

  
  add_subdirectory(appSceneGenerator)

  add_subdirectory(appOverlapCollisions)
  add_subdirectory(appSomaGenerator)
  add_subdirectory(appDemo)
endif()
