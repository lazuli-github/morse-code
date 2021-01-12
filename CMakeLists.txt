cmake_minimum_required (VERSION 3.8)

project ("morse_code")

add_subdirectory ("morse_code")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
set (CMAKE_EXE_LINKER_FLAGS "-static")

if (UNIX)
	set (BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
endif (UNIX)
