PROJECT(lspf)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_LOCAL_PATH}/cmake")

message("Build CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

include(DefineOptions)

# include directories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(gen-server SRC_LIST)
aux_source_directory(gen-channel SRC_LIST)
aux_source_directory(route SRC_LIST)
aux_source_directory(riskctrl SRC_LIST)
aux_source_directory(service SRC_LIST)
aux_source_directory(main SRC_LIST)
aux_source_directory(oldapi SRC_LIST)

add_executable(apay_flowctrl ${SRC_LIST})
target_link_libraries(apay_flowctrl pthread)
target_link_libraries(apay_flowctrl boost_system)
target_link_libraries(apay_flowctrl boost_thread)
target_link_libraries(apay_flowctrl thrift)
target_link_libraries(apay_flowctrl thrift-rpc)
target_link_libraries(apay_flowctrl dbpool)
target_link_libraries(apay_flowctrl occi)
target_link_libraries(apay_flowctrl clntsh)
target_link_libraries(apay_flowctrl json_mt)
target_link_libraries(apay_flowctrl xservice_64)
target_link_libraries(apay_flowctrl toolkits4_64)
target_link_libraries(apay_flowctrl message)
target_link_libraries(apay_flowctrl log)
target_link_libraries(apay_flowctrl common)