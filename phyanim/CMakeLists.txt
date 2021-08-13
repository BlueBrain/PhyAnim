file(REMOVE ${CMAKE_BINARY_DIR}/include)

file(GLOB PHYANIM_SOURCE_FILES "*.cpp")
file(GLOB PHYANIM_HEADER_FILES "*.h")
file(COPY ${DIRECTORY} DESTINATION ${CMAKE_BINARY_DIR}/include FILES_MATCHING 
  PATTERN "*.h")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Phyanim.h
  ${CMAKE_BINARY_DIR}/include/Phyanim.h COPYONLY)

add_library(phyanim ${PHYANIM_SOURCE_FILES} ${PHYANIM_HEADER_FILES})

target_include_directories(phyanim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
  ${EIGEN3_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})

target_link_libraries(phyanim PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} 
  igl::core)

if(OpenMP_CXX_FOUND)
  target_link_libraries(phyanim PUBLIC OpenMP::OpenMP_CXX)
endif()
