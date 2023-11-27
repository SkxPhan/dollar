cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(dollar-protractor)

option(ENABLE_DEV "Enable tests and other dev related features" OFF)

add_library(dollar include/dollar.hpp src/dollar.cpp)
target_include_directories(dollar PUBLIC include)
target_compile_features(dollar PUBLIC cxx_std_17)


if (${ENABLE_DEV})
include(cmake/CPM.cmake)
CPMAddPackage("gh:catchorg/Catch2@3.4.0")
CPMAddPackage(
  NAME Format.cmake
  VERSION 1.7.3
  GITHUB_REPOSITORY TheLartians/Format.cmake
  OPTIONS 
      # set to yes skip cmake formatting
      "FORMAT_SKIP_CMAKE YES"
)
CPMAddPackage(
  NAME pugixml
  VERSION 1.14
  GITHUB_REPOSITORY zeux/pugixml
)
add_executable(test_dollar test/test.cpp)
target_compile_features(test_dollar PUBLIC cxx_std_17)
target_link_libraries(test_dollar Catch2::Catch2WithMain dollar pugixml::pugixml)
include(CTest)
add_test(NAME test_dollar COMMAND ${CMAKE_BINARY_DIR}/test_dollar WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()


