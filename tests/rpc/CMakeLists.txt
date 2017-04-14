# CMake build script for ZeroMQ tests
cmake_minimum_required(VERSION "2.8.1")

# On Windows: solution file will be called tests.sln
PROJECT(tests)

set(tests
        test_object_pool
)

foreach(test ${tests})
  add_executable(${test} ${test}.cpp)
  target_link_libraries(${test} common )
  target_link_libraries(${test} boost_unit_test_framework)
  
  if(RT_LIBRARY)
    target_link_libraries(${test} ${RT_LIBRARY} )
  endif()

  add_test(NAME ${test} COMMAND ${test})

  set_tests_properties(${test} PROPERTIES TIMEOUT 10)
endforeach()