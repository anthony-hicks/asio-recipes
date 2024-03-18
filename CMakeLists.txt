cmake_minimum_required(VERSION 3.21)

project(Recipes
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

include(cmake/CPM.cmake)
include(cmake/Sanitizers.cmake)

CPMAddPackage("gh:gabime/spdlog@1.13.0")

find_package(Threads REQUIRED)
CPMAddPackage("gh:chriskohlhoff/asio#asio-1-28-0@1.28.0")
if(asio_ADDED)
  add_library(asio INTERFACE)
  target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)
  target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
  target_link_libraries(asio INTERFACE Threads::Threads)
endif()

add_subdirectory(src)

# TODO: Confirm .clang-tidy works
# TODO: Specify whether sync or async
