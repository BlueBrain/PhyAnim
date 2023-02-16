set(APP_NAME phyanimAnimMesh)

file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp"
  "${PROJECT_SOURCE_DIR}/examples/common/*.cpp")
file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES} 
  ${${APP_NAME}_HEADER_FILES})

target_link_libraries(${APP_NAME} ${GLFW3_LIBRARY} phyanim)

