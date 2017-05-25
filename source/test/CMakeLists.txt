PROJECT(lspf)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

add_compile_options(-std=c++0x)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

include(DefineOptions)

include_directories(../src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

LINK_DIRECTORIES(../lib)

add_executable(cppredis-test ${SRC_LIST})
target_link_libraries(cppredis-test hiredis)
target_link_libraries(cppredis-test pthread)
target_link_libraries(cppredis-test redisclient)
target_link_libraries(cppredis-test boost_system)
