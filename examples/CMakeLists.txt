  add_executable(tetrahedralizeMesh tetrahedralizeMesh.cpp)
  target_include_directories(tetrahedralizeMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(tetrahedralizeMesh PUBLIC igl::core igl::tetgen)

  add_executable(checkCollision checkCollision.cpp)
  target_include_directories(checkCollision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(checkCollision PUBLIC phyanim)

  add_executable(mesh_test mesh_test.cpp)
  target_include_directories(mesh_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(mesh_test PUBLIC phyanim)
  
if(GLFW3_FOUND)
  
  add_executable(animMesh animMesh.cpp AnimMeshApp.cpp GLFWApp.cpp Scene.cpp
    Camera.cpp RenderProgram.cpp)
  target_include_directories(animMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIR})
  target_link_libraries(animMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

  add_executable(overlap overlap.cpp OverlapApp.cpp GLFWApp.cpp Scene.cpp
    Camera.cpp RenderProgram.cpp)
  target_include_directories(overlap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIR})
  target_link_libraries(overlap PUBLIC ${GLFW3_LIBRARY} phyanim)

  add_executable(renderMesh renderMesh.cpp GLFWApp.cpp Scene.cpp Camera.cpp
    RenderProgram.cpp)
  target_include_directories(renderMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIR})
  target_link_libraries(renderMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

  add_executable(moveMesh moveMesh.cpp MoveMeshApp.cpp GLFWApp.cpp Scene.cpp
    Camera.cpp RenderProgram.cpp)
  target_include_directories(moveMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIR})
  target_link_libraries(moveMesh PUBLIC ${GLFW3_LIBRARY} phyanim)

  add_definitions(-DPATH_TO_ICO="${CMAKE_CURRENT_SOURCE_DIR}/data/ico6.tet")
  add_executable(somaGen somaGen.cpp SomaApp.cpp GLFWApp.cpp Scene.cpp
    Camera.cpp RenderProgram.cpp Icosphere.cpp SomaGenerator.cpp)
  target_include_directories(somaGen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIR})
  target_link_libraries(somaGen PUBLIC ${GLFW3_LIBRARY} phyanim)

endif()
