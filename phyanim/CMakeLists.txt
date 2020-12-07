add_library(phyanim Node.cpp Edge.cpp Triangle.cpp Tetrahedron.cpp Mesh.cpp
		    DrawableMesh.cpp BoundingVolume.cpp BoundingSphere.cpp
                    AxisAlignedBoundingBox.cpp
                    CollisionDetection.cpp AnimSystem.cpp
                    ExplicitMassSpringSystem.cpp ImplicitMassSpringSystem.cpp
                    ExplicitFEMSystem.cpp ImplicitFEMSystem.cpp)
target_include_directories(phyanim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}             
				   ${EIGEN3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
target_link_libraries(phyanim PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
if(OpenMP_CXX_FOUND)
    target_link_libraries(phyanim PUBLIC OpenMP::OpenMP_CXX)
endif()
