PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

add_executable(scribe_log scribe_log.cpp)
target_link_libraries(scribe_log log)
target_link_libraries(scribe_log common)
target_link_libraries(scribe_log thrift)
target_link_libraries(scribe_log boost_thread)
target_link_libraries(scribe_log boost_system)
