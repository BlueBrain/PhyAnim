add_library(phyamin Node.cpp Edge.cpp Triangle.cpp Tetrahedron.cpp Mesh.cpp DrawableMesh.cpp AnimSystem.cpp MassSpringSystem.cpp FEMSystem.cpp)
target_include_directories (phyamin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  ${EIGEN3_INCLUDE_DIR})
