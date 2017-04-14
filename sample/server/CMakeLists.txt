PROJECT(lspf)

# include directories
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gen-cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/server
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/gen-cpp SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/server SRC_LIST)

FOREACH(F ${SRC_LIST})
     MESSAGE(${F})
ENDFOREACH(F)

add_executable(server_demo ${SRC_LIST} ${CONSOLE_LIST})
target_link_libraries(server_demo pthread)
target_link_libraries(server_demo boost_system)
target_link_libraries(server_demo boost_thread)
target_link_libraries(server_demo thrift)
target_link_libraries(server_demo thrift-rpc)
target_link_libraries(server_demo dbpool)
target_link_libraries(server_demo common)
target_link_libraries(server_demo message)
target_link_libraries(server_demo occi)
target_link_libraries(server_demo clntsh)
