PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/../../build/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

set(tutorialgencpp_sources 
    gen-cpp/Demo.cpp
    gen-cpp/demo_types.cpp
    gen-cpp/demo_constants.cpp
)

#add_library(tutorialgencpp STATIC ${tutorialgencpp_SOURCES})
#TARGET_LINK_LIBRARIES(tutorialgencpp thrift)
#LINK_AGAINST_THRIFT_LIBRARY(tutorialgencpp thrift)

#add_custom_command(OUTPUT gen-cpp/Calculator.cpp gen-cpp/SharedService.cpp gen-cpp/shared_constants.cpp gen-cpp/shared_types.cpp gen-cpp/tutorial_constants.cpp gen-cpp/tutorial_types.cpp
#    COMMAND thrift-compiler --gen cpp -r ${PROJECT_SOURCE_DIR}/tutorial/tutorial.thrift
#)

add_executable(RpcServer RpcServer.cpp ${tutorialgencpp_sources})
target_link_libraries(RpcServer thrift)

add_executable(RpcClient RpcClient.cpp ${tutorialgencpp_sources})
target_link_libraries(RpcClient thrift)
target_link_libraries(RpcClient boost_system)
target_link_libraries(RpcClient boost_thread)

add_executable(RpcNioClient RpcNioClient.cpp ${tutorialgencpp_sources})
target_link_libraries(RpcNioClient thrift)
target_link_libraries(RpcNioClient boost_system)
target_link_libraries(RpcNioClient boost_thread)

add_executable(RpcNioServer RpcNioServer.cpp ${tutorialgencpp_sources})
target_link_libraries(RpcNioServer thrift)
target_link_libraries(RpcNioServer thriftnb)
target_link_libraries(RpcNioServer event)
target_link_libraries(RpcNioServer boost_system)

#add_executable(RpcPoolClient RpcPoolClient.cpp ${tutorialgencpp_sources})
#target_link_libraries(RpcPoolClient thrift)
#target_link_libraries(RpcPoolClient thrift-rpc)
#target_link_libraries(RpcPoolClient common)
#target_link_libraries(RpcPoolClient boost_system)
#target_link_libraries(RpcPoolClient boost_thread)

