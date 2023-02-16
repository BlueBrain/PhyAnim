set(APP_NAME phyanimTetrahedralizeMesh)

file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp")
file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES}
  ${${APP_NAME}_HEADER_FILES})

target_link_libraries(${APP_NAME} igl::core igl::tetgen phyanim)

add_custom_command(TARGET ${APP_NAME} POST_BUILD COMMAND ${APP_NAME} 
  ${CMAKE_BINARY_DIR}/data/ico5.ply)
