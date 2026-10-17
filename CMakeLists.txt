cmake_minimum_required(VERSION 4.3)
project(mono_api)

set(CMAKE_CXX_STANDARD 20)

# -----------------------------------------------------------------------------

include_directories(include)

add_library(mono_api STATIC
	src/mono_api.cpp
	include/mono_api.hpp
	include/_types.hpp
	include/_func.hpp
	include/_names.hpp
)

# -----------------------------------------------------------------------------

target_include_directories(mono_api PRIVATE

)

target_link_libraries(mono_api PRIVATE

)

# -----------------------------------------------------------------------------

set_target_properties(mono_api PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/.bin"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/.bin"
)