cmake_minimum_required(VERSION 2.6)
project(get)

set(SRCS
  src/main.c
  src/config.c
  src/net.c
  src/utils.c
)

set(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} -std=c99 -pedantic -Wall -march=native")
set(CMAKE_BUILD_TYPE "Release")

# Search OpenSSL
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if (OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIRS})
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()

include_directories("src")
add_executable(nc-ssl ${SRCS})
target_link_libraries(nc-ssl ${OPENSSL_LIBRARIES})
install(TARGETS nc-ssl DESTINATION /usr/bin COMPONENT binaries)
