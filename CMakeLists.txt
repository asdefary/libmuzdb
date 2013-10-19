cmake_minimum_required(VERSION 2.6)

project(muzdb)

FILE(GLOB SOURCES src/*.cc)

find_library(AVFORMAT avformat)
find_library(BOOST_FS boost_filesystem)

add_library(muzdb ${SOURCES})

target_link_libraries(muzdb AVFORMAT BOOST_FS)

INSTALL(FILES include/muzdb.hpp
	DESTINATION include)

INSTALL(TARGETS muzdb
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

