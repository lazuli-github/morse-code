cmake_minimum_required (VERSION 3.8)

if (UNIX)
	find_package (SDL2 REQUIRED)
	include_directories (${SDL2_INCLUDE_DIR})
elseif (MSVC)
	include_directories ("${PROJECT_SOURCE_DIR}/SDL2/include")
	link_directories ("${PROJECT_SOURCE_DIR}/SDL2/lib/x64")
endif ()

add_executable (morse_code "morse_code.cc"  "morse_code.hh" "sound.cc" "sound.hh")
target_link_libraries (morse_code SDL2)
install (TARGETS morse_code RUNTIME DESTINATION ${BIN_DIR})

if (MSVC)
	add_custom_command (TARGET morse_code POST_BUILD COMMAND ${CMAKE_COMMAND} -E
				   copy_if_different
				   "${PROJECT_SOURCE_DIR}/SDL2/lib/x64/SDL2.dll"
				   $<TARGET_FILE_DIR:morse_code>)
endif ()
