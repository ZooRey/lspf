PROJECT(lspf)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

include(DefineOptions)

add_executable(dbclient_demo DBClient.cpp)

target_link_libraries(dbclient_demo boost_system)
target_link_libraries(dbclient_demo boost_thread)
target_link_libraries(dbclient_demo dbpool)
target_link_libraries(dbclient_demo log)
target_link_libraries(dbclient_demo common)
target_link_libraries(dbclient_demo occi)
target_link_libraries(dbclient_demo clntsh)
target_link_libraries(dbclient_demo pthread)

add_executable(db_test db_test.cpp)

target_link_libraries(db_test dbpool)
target_link_libraries(db_test log)
target_link_libraries(db_test common)
target_link_libraries(db_test occi)
target_link_libraries(db_test clntsh)
target_link_libraries(db_test pthread)