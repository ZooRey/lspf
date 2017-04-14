PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

aux_source_directory(. SRC_LIST)

add_library(common STATIC ${SRC_LIST})

set(HEADERS
	base64.h
	mutex.h
	thread.h
	thread_pool.h
	string_utility.h
	time_utility.h
	file_util.h
	app_conf.h
	blocking_queue.h
	condition_variable.h
	process_control.h
)

INSTALL(FILES ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/common)

