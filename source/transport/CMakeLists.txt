PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/zk SRC_LIST)

set(SRC_LIST
  zk_client.cpp
  zk_server.cpp
  sync_comm.cpp
  async_mq_client.cpp
  async_mq_server.cpp
  )

add_library(transport STATIC ${SRC_LIST})

set(HEADERS
  zk_client.h
  zk_server.h
  session_cache.h
  comm_session.h
  sync_comm.h
  async_mq_client.h
  async_mq_server.h
  )
install(FILES ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/transport)


