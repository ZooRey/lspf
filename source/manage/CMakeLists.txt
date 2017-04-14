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
  )

add_library(manage STATIC ${SRC_LIST})

set(HEADERS
  zk_client.h
  zk_server.h
  )
install(FILES ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/manage)


