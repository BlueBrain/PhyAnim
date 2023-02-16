if(MorphIO_FOUND AND sonata_FOUND)
  set(APP_NAME test)

  file(GLOB ${APP_NAME}_SOURCE_FILES "*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/Morpho.cpp")
  file(GLOB ${APP_NAME}_HEADER_FILES "*.h")

  add_executable(${APP_NAME} ${${APP_NAME}_SOURCE_FILES} 
    ${${APP_NAME}_HEADER_FILES})

  target_link_libraries(${APP_NAME} phyanim)
else()
  message("---------------------------------------------------------------------")

endif()
