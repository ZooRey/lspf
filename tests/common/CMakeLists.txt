# CMake build script for ZeroMQ tests
cmake_minimum_required(VERSION "2.8.1")

# On Windows: solution file will be called tests.sln
PROJECT(tests)

set(tests
        test_sample
		test_app_conf
		test_tree
		test_thread_local
)

foreach(test ${tests})
  add_executable(${test} ${test}.cpp)
  target_link_libraries(${test} common )
  target_link_libraries(${test} boost_unit_test_framework )
  target_link_libraries(${test} boost_thread )
  target_link_libraries(${test} boost_system )
  
  if(RT_LIBRARY)
    target_link_libraries(${test} ${RT_LIBRARY} )
  endif()

  add_test(NAME ${test} COMMAND ${test})

  set_tests_properties(${test} PROPERTIES TIMEOUT 10)
endforeach()
