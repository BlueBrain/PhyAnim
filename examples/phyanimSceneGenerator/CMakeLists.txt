set(APP_NAME phyanimSceneGanerator)

include_directories(${CMAKE_BINARY_DIR}/include)

file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.cpp")
file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common/")

add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES} 
  ${${APP_NAME}_HEADER_FILES})

target_link_libraries(${APP_NAME} ${GLFW3_LIBRARY} phyanim)

