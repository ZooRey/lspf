PROJECT(lsxpay)

set(tutorialgencpp_sources 
    gen-cpp/Calculator.cpp
    gen-cpp/threads_types.cpp
    gen-cpp/threads_constants.cpp
)


add_executable(AsyncRpcServer CppServer.cpp ${tutorialgencpp_sources})
target_link_libraries(AsyncRpcServer thrift)

add_executable(AsyncRpcClient CppClient.cpp ${tutorialgencpp_sources})
target_link_libraries(AsyncRpcClient thrift)
target_link_libraries(AsyncRpcClient thriftnb)
target_link_libraries(AsyncRpcClient event)
target_link_libraries(AsyncRpcClient boost_system)
target_link_libraries(AsyncRpcClient boost_thread)