PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_library(message STATIC ${SRC_LIST})

set(HEADERS
	common.h
	message.h
	message_driver.h
	zmq_driver.h
	atmq_driver.h
	message_exception.h
)

INSTALL(FILES ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/message)