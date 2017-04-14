PROJECT(lspf)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp SRC_GEN_LIST)


add_library(log STATIC ${SRC_LIST} ${SRC_GEN_LIST})

set(HEADERS
	log.h
	scribe_log.h
	scribe_log_handler.h
)

INSTALL(FILES ${HEADERS} DESTINATION include/log)
