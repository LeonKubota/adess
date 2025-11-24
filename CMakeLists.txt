cmake_minimum_required(VERSION 3.13)
project(adess)

file(GLOB_RECURSE SRC_FILES "src/*.c")

if(WIN32)
	add_definitions(-DPLATFORM_WINDOWS)
else()
	add_definitions(-DPLATFORM_LINUX)
endif()

add_executable(adess ${SRC_FILES})

target_include_directories(adess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# add_executable(adess src/main.c src/parse.c src/utils.c src/commands/command.c)
