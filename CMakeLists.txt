cmake_minimum_required(VERSION 2.6)

project(muzdb)

FILE(GLOB SOURCES src/*.cc)

find_library(AVFORMAT avformat)
find_library(BOOST_FS boost_filesystem)

add_library(muzdb SHARED ${SOURCES})
add_executable(muzscan example/muzscan.cc)

target_link_libraries(muzdb ${AVFORMAT} ${BOOST_FS})
target_link_libraries(muzscan muzdb)

set_target_properties(muzdb PROPERTIES VERSION "0.0.1")

INSTALL(FILES include/muzdb.hpp
	DESTINATION include)

INSTALL(DIRECTORY include/muzdb
	DESTINATION include)

INSTALL(TARGETS muzdb
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

