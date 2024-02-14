file(REMOVE ${CMAKE_BINARY_DIR}/include)

file(GLOB_RECURSE PHYANIM_SOURCE_FILES "*.cpp")
file(GLOB_RECURSE PHYANIM_HEADER_FILES "*.h")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR}/include FILES_MATCHING 
  PATTERN "*.h")



add_library(phyanim ${PHYANIM_SOURCE_FILES} ${PHYANIM_HEADER_FILES})

target_include_directories(phyanim PUBLIC ${EIGEN3_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})

target_link_libraries(phyanim PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} 
  igl::core)

if(OpenMP_CXX_FOUND)
  target_link_libraries(phyanim PUBLIC OpenMP::OpenMP_CXX)
endif()

if (MorphIO_FOUND AND sonata_FOUND)
  target_link_libraries(phyanim PUBLIC MorphIO::morphio sonata::sonata_shared Python::Python)
endif()