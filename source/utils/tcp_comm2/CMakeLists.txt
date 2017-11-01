PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

#ADD_DEFINITIONS(-DTIXML_USE_STL) #tinyxml stl

include(DefineOptions)

# include directories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(. SRC_LIST)

SET(TARGET_FILE tcp_comm)
add_library(${TARGET_FILE} STATIC ${SRC_LIST})

target_link_libraries(${TARGET_FILE} log)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_LOCAL_PATH}/../local/lib)
FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
MESSAGE(${headers}) 
FILE(COPY ${headers} DESTINATION ${CMAKE_LOCAL_PATH}/../local/include/tcp_comm)
