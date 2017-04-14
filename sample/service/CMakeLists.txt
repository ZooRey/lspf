#PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/../../build/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

add_executable(zk_client zk_client.cpp)
target_link_libraries(zk_client zookeeper_mt)
target_link_libraries(zk_client service)
target_link_libraries(zk_client log)
target_link_libraries(zk_client common)
target_link_libraries(zk_client boost_thread)
target_link_libraries(zk_client boost_system)

add_executable(zk_server zk_server.cpp)
target_link_libraries(zk_server zookeeper_mt)
target_link_libraries(zk_server service)
target_link_libraries(zk_server log)
target_link_libraries(zk_server common)
target_link_libraries(zk_server boost_thread)
target_link_libraries(zk_server boost_system)
