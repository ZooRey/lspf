PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

add_executable(zeromq_client zeromq_client.cpp)
target_link_libraries(zeromq_client message)
target_link_libraries(zeromq_client zmq)
target_link_libraries(zeromq_client common)
target_link_libraries(zeromq_client boost_thread)
target_link_libraries(zeromq_client boost_system)

add_executable(zeromq_server zeromq_server.cpp)
target_link_libraries(zeromq_server message)
target_link_libraries(zeromq_server zmq)
target_link_libraries(zeromq_server common)
target_link_libraries(zeromq_server boost_thread)
target_link_libraries(zeromq_server boost_system)

add_executable(atmq_client atmq_client.cpp)
target_link_libraries(atmq_client message)
target_link_libraries(atmq_client activemq-cpp)
target_link_libraries(atmq_client boost_thread)
target_link_libraries(atmq_client boost_system)
target_link_libraries(atmq_client common)

set(exe_name atmq_server) 
add_executable(${exe_name} atmq_server.cpp)
target_link_libraries(${exe_name} clntsh)
target_link_libraries(${exe_name} message)
target_link_libraries(${exe_name} activemq-cpp)
#target_link_libraries(${exe_name} dbpool)
#target_link_libraries(${exe_name} log)
#target_link_libraries(${exe_name} common)
#target_link_libraries(${exe_name} occi)
#target_link_libraries(${exe_name} boost_system)
#target_link_libraries(${exe_name} boost_thread)
#target_link_libraries(${exe_name} pthread)