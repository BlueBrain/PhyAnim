if(MorphIO_FOUND AND sonata_FOUND)
  set(APP_NAME appCollidingSomas)

file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp" "../common/Morpho.cpp")
file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

  add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES} 
    ${${APP_NAME}_HEADER_FILES})

  target_link_libraries(${APP_NAME} phyanim)
endif()
