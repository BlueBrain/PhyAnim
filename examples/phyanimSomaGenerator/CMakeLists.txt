set(APP_NAME phyanimSomaGenerator)

add_definitions(-DPATH_TO_ICO="${CMAKE_BINARY_DIR}/data/ico5.tet")

file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.cpp")
file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES} 
  ${${APP_NAME}_HEADER_FILES})

target_link_libraries(${APP_NAME} ${GLFW3_LIBRARY} phyanim)

