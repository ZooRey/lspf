PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

aux_source_directory(. SRC_LIST)

add_library(dbpool STATIC ${SRC_LIST})

set(HEADERS
	dbexception.h
	db_oracle.h
	dbcommand.h
	dbconn_check.h
	dbconn_manage.h
)

INSTALL(FILES ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/database)
