# Data copy to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION 
  ${CMAKE_BINARY_DIR}/data)

add_subdirectory(phyanimCheckCollisions)
add_subdirectory(phyanimFormatConverter)
add_subdirectory(phyanimTetrahedralizeMesh)

if(GLFW3_FOUND)
  add_subdirectory(phyanimAnimMesh)
  add_subdirectory(phyanimMoveMesh)
  add_subdirectory(phyanimRenderMesh)
  add_subdirectory(phyanimSceneGenerator) 

  add_subdirectory(phyanimOverlapCollisions)
  add_subdirectory(phyanimOverlapResolution)
  add_subdirectory(phyanimSomaGenerator)
endif()

#   add_executable(renderMesh renderMesh.cpp GLFWApp.cpp Scene.cpp Camera.cpp
#     RenderProgram.cpp)
#   target_include_directories(renderMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(renderMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

# add_executable(tetrahedralizeMesh tetrahedralizeMesh.cpp)
# target_include_directories(tetrahedralizeMesh PUBLIC 
#   ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(tetrahedralizeMesh PUBLIC igl::core igl::tetgen)

# add_custom_command( TARGET  tetrahedralizeMesh POST_BUILD COMMAND   
#   tetrahedralizeMesh ${CMAKE_BINARY_DIR}/data/*)

# add_executable(checkCollision checkCollision.cpp)
# target_include_directories(checkCollision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(checkCollision PUBLIC phyanim)


# add_executable(formatConvert formatConvert.cpp)
# target_include_directories(formatConvert PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(formatConvert PUBLIC phyanim)
  
# if(GLFW3_FOUND)
  
#   add_executable(animMesh animMesh.cpp AnimMeshApp.cpp GLFWApp.cpp Scene.cpp
#     Camera.cpp RenderProgram.cpp)
#   target_include_directories(animMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(animMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

#   add_executable(overlap overlap.cpp OverlapApp.cpp GLFWApp.cpp Scene.cpp
#     Camera.cpp RenderProgram.cpp)
#   target_include_directories(overlap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(overlap PUBLIC ${GLFW3_LIBRARY} phyanim)


#   add_executable(overlapSlices overlapSlices.cpp OverlapSlicesApp.cpp 
#     GLFWApp.cpp Scene.cpp Camera.cpp RenderProgram.cpp)
#   target_include_directories(overlapSlices PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(overlapSlices PUBLIC ${GLFW3_LIBRARY} phyanim)

#   add_executable(renderMesh renderMesh.cpp GLFWApp.cpp Scene.cpp Camera.cpp
#     RenderProgram.cpp)
#   target_include_directories(renderMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(renderMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

#   add_executable(moveMesh moveMesh.cpp MoveMeshApp.cpp GLFWApp.cpp Scene.cpp
#     Camera.cpp RenderProgram.cpp)
#   target_include_directories(moveMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(moveMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

#   add_definitions(-DPATH_TO_ICO="${CMAKE_BINARY_DIR}/data/ico6.tet")
#   add_executable(somaGen somaGen.cpp SomaApp.cpp GLFWApp.cpp Scene.cpp
#     Camera.cpp RenderProgram.cpp Icosphere.cpp SomaGenerator.cpp)
#   target_include_directories(somaGen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#     ${GLFW3_INCLUDE_DIR})
#   target_link_libraries(somaGen PUBLIC ${GLFW3_LIBRARY} phyanim)

# endif()

# Data copy to build folder
