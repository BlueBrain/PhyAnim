add_executable(test0 test.cpp Scene.cpp Camera.cpp)

target_include_directories (test0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
target_link_libraries(test0 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES}
  ${GLEW_LIBRARIES} phyamin)
