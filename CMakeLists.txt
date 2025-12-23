cmake_minimum_required(VERSION 3.13)
project(adess)

file(GLOB_RECURSE SRC_FILES "src/*.c")

if(WIN32)
	add_definitions(-DPLATFORM_WINDOWS)
else()
	add_definitions(-DPLATFORM_LINUX)
endif()

# Set some flags based on debug/release

add_executable(adess ${SRC_FILES})

target_include_directories(adess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(adess PRIVATE -std=c99 -pedantic -Wall -Wextra -Werror)

target_link_libraries(adess m)
