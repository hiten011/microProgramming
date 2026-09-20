# Add sources to executable/library
target_sources(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/syscall.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/sysmem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/startup_stm32c031xx.S"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/stm_helper/my_stm_helper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/assignment1/my_helpers.c"
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/stm32c031x6_flash.ld" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/stm32c031x6_flash.ld")
