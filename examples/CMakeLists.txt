if(GLFW3_FOUND AND LIBIGL_FOUND)
  add_executable(testAnim testAnim.cpp Scene.cpp Camera.cpp)

  target_include_directories(testAnim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    				 	${GLFW3_INCLUDE_DIR})
  target_link_libraries(testAnim PUBLIC ${GLFW3_LIBRARY} igl::core igl::tetgen phyanim)
endif()
