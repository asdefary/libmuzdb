cmake_minimum_required(VERSION 2.6)

project(muzdb)

FILE(GLOB SOURCES src/*.cc)

find_library(AVFORMAT avformat)
find_library(BOOST_FS boost_filesystem)
find_library(BOOST_AR boost_serialization)
find_library(BOOST_LC boost_locale)

add_library(muzdb SHARED ${SOURCES})
add_executable(muzscan example/muzscan.cc)
add_executable(muzdump example/muzdump.cc)

target_link_libraries(muzdb ${AVFORMAT} ${BOOST_FS} ${BOOST_AR} ${BOOST_LC})
target_link_libraries(muzscan muzdb)
target_link_libraries(muzdump muzdb)

set_target_properties(muzdb PROPERTIES VERSION "0.1.0")

INSTALL(FILES include/muzdb.hpp
	DESTINATION include)

INSTALL(DIRECTORY include/muzdb
	DESTINATION include)

INSTALL(TARGETS muzdb
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

