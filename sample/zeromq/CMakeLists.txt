PROJECT(lsxpay)

add_executable(push_pull push_pull.cpp)
target_link_libraries(push_pull message)
target_link_libraries(push_pull zmq)
target_link_libraries(push_pull common)
target_link_libraries(push_pull boost_thread)
target_link_libraries(push_pull boost_system)
