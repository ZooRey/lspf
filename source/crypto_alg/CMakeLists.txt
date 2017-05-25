PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/zk SRC_LIST)

aux_source_directory(. SRC_LIST)

add_library(crypto_alg STATIC ${SRC_LIST})

#set(HEADERS
#  *.h
#  )
#FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
#install(headers ${HEADERS} DESTINATION ${INCLUDE_OUTPUT_PATH}/crypto_alg)

# include directories
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
#
#aux_source_directory(. SRC_LIST)
#
#SET(TARGET_FILE crypto_alg)
#add_library(${TARGET_FILE} STATIC ${SRC_LIST})
#
#target_link_libraries(${TARGET_FILE} ssl)
#target_link_libraries(${TARGET_FILE} common)
#
#SET(LIBRARY_OUTPUT_PATH ${CMAKE_LOCAL_PATH}/../local/lib)
#FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
#MESSAGE(${headers}) 
#FILE(COPY ${headers} DESTINATION ${CMAKE_LOCAL_PATH}/../local/include/crypto_alg)


