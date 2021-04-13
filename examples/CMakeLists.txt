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
  add_executable(testAnim testAnim.cpp Scene.cpp Camera.cpp RenderProgram.cpp)
  target_include_directories(testAnim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    				 	${GLFW3_INCLUDE_DIR})
  target_link_libraries(testAnim PUBLIC ${GLFW3_LIBRARY} igl::tetgen phyanim)

  # add_executable(overlap overlap.cpp OverlapScene.cpp Camera.cpp)
  # target_include_directories(overlap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  #   ${GLFW3_INCLUDE_DIR})
  # target_link_libraries(overlap PUBLIC ${GLFW3_LIBRARY} igl::tetgen phyanim)

  
  add_executable(renderMesh renderMesh.cpp Scene.cpp Camera.cpp RenderProgram.cpp)
  target_include_directories(renderMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLFW3_INCLUDE_DIR})
  target_link_libraries(renderMesh PUBLIC ${GLFW3_LIBRARY} phyanim)
  
endif()
