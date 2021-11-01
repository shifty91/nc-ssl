#
# Copyright (C) 2015-2016,2021 Kurt Kanzenbach <kurt@kmk-computers.de>
# SPDX-License-Identifier: GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.2)
project(nc-ssl)

set(SRCS
  src/main.c
  src/config.c
  src/net.c
  src/utils.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pedantic -Wall -march=native")

# Search for OpenSSL
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl>=1.0.2)

# Search for Linux headers
include(CheckIncludeFiles)
check_include_files("sys/select.h" HAVE_SELECT_H)
check_include_files("sys/types.h" HAVE_TYPES_H)
check_include_files("sys/socket.h" HAVE_SOCKET_H)
check_include_files("netdb.h" HAVE_NETDB_H)

if (NOT HAVE_SELECT_H)
  message(FATAL_ERROR "sys/select.h not found")
endif()
if (NOT HAVE_TYPES_H)
  message(FATAL_ERROR "sys/types.h not found")
endif()
if (NOT HAVE_SOCKET_H)
  message(FATAL_ERROR "sys/socket.h not found")
endif()
if (NOT HAVE_NETDB_H)
  message(FATAL_ERROR "netdb.h not found")
endif()

# Add binary
add_executable(nc-ssl ${SRCS})
target_include_directories(nc-ssl PRIVATE "src")

# Build against OpenSSL
target_include_directories(nc-ssl PRIVATE ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(nc-ssl PRIVATE ${OPENSSL_LIBRARIES})
link_directories(${OPENSSL_LIBRARY_DIRS})
message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")

# Provide an install target
install(TARGETS nc-ssl DESTINATION bin COMPONENT binaries)
