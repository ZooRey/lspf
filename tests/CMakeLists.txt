PROJECT(lsxpay)

# include directories
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/source
  ${PROJECT_SOURCE_DIR}/thirdparty/include
  ${PROJECT_SOURCE_DIR}/thirdparty/include/occi
#  /usr/local/include
  /usr/include
)

# lib directories
LINK_DIRECTORIES(
#  ${PROJECT_SOURCE_DIR}/thirdparty/lib
  ${PROJECT_BINARY_DIR}/build_release/lib
  
#  /usr/local/lib
  /use/lib
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test/bin)

#link_libraries(boost_unit_test_framework)

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(rpc)
