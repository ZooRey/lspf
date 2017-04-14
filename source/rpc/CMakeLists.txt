PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/gen-scribe SRC_LIST)

add_library(thrift-rpc STATIC ${SRC_LIST})

set(HEADERS
  rpc.h
  connection_pool.h
  tcp_connection_pool.h
  connection_provider.h
  connection_pool_mgr.h
  connection_exception.h
  )
install(FILES ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/rpc)
