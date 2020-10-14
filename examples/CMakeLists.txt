if(LIBIGL_FOUND)
  add_executable(tetrahedralizeMesh tetrahedralizeMesh.cpp)

  target_include_directories(tetrahedralizeMesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(tetrahedralizeMesh PUBLIC igl::core igl::tetgen)

if(GLFW3_FOUND)
  add_executable(testAnim testAnim.cpp Scene.cpp Camera.cpp)

  target_include_directories(testAnim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    				 	${GLFW3_INCLUDE_DIR})
  target_link_libraries(testAnim PUBLIC ${GLFW3_LIBRARY} igl::core igl::tetgen phyanim)
endif()
endif()
