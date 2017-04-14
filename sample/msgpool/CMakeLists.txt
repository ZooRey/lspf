PROJECT(lspf)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_executable(msgpool ${SRC_LIST} ${CONSOLE_LIST})
target_link_libraries(msgpool common)
target_link_libraries(msgpool boost_system)
target_link_libraries(msgpool boost_thread)