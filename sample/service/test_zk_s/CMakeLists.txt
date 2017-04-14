PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

#ADD_DEFINITIONS(-DTIXML_USE_STL) #tinyxml stl

include(DefineOptions)

# include directories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(gen-server)
INCLUDE_DIRECTORIES(main)

aux_source_directory(gen-server SRC_LIST)
aux_source_directory(main SRC_LIST)

SET(TARGET_FILE test_zk_s)
add_executable(${TARGET_FILE} ${SRC_LIST})
target_link_libraries(${TARGET_FILE} pthread)
target_link_libraries(${TARGET_FILE} boost_system)
target_link_libraries(${TARGET_FILE} boost_thread)
target_link_libraries(${TARGET_FILE} thrift)
target_link_libraries(${TARGET_FILE} dbpool)
target_link_libraries(${TARGET_FILE} occi)
target_link_libraries(${TARGET_FILE} clntsh)
target_link_libraries(${TARGET_FILE} json_mt)
target_link_libraries(${TARGET_FILE} zookeeper_mt)
target_link_libraries(${TARGET_FILE} log)
target_link_libraries(${TARGET_FILE} common)
target_link_libraries(${TARGET_FILE} ssl)

