cmake_minimum_required(VERSION 2.6)

project(muzdb)

FILE(GLOB SOURCES src/*.cc)

add_library(muzdb ${SOURCES})

INSTALL(FILES include/muzdb.hpp
	DESTINATION include)

INSTALL(TARGETS muzdb
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

