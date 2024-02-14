set(APP_NAME appDemo)

file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp" "../common/*.cpp")
file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES}
  ${${APP_NAME}_HEADER_FILES})

target_link_libraries(${APP_NAME} ${GLFW3_LIBRARY} phyanim)
