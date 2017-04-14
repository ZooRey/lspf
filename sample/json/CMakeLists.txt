PROJECT(lsxpay)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_executable(json ${SRC_LIST} ${CONSOLE_LIST})
target_link_libraries(json json_mt)
target_link_libraries(json common)
target_link_libraries(json boost_system)
target_link_libraries(json boost_thread)