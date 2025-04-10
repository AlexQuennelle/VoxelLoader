cmake_minimum_required(VERSION 3.30.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(PRODUCTION_BUILD "Make this a production build" OFF)

# export compile commands for clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-O3;-ffast-math>")

# Change this to match the name of the current folder
project(VoxelLoader CXX C)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
  endif()
endif()

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp")

# include(FetchContent)
# FetchContent_Declare(
# 	RAYLIB
# 	GIT_REPOSITORY https://github.com/Raysan5/raylib.git
# 	GIT_TAG 5.5
# 	GIT_SHALLOW ON
# 	EXCLUDE_FROM_ALL
# 	SYSTEM
# )
# FetchContent_MakeAvailable(RAYLIB)

add_executable("${CMAKE_PROJECT_NAME}") 

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Resources" DESTINATION "${PROJECT_BINARY_DIR}")
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC NAME="${CMAKE_PROJECT_NAME}")

if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./Resources/") 

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 

else()
	# This is useful to get an ASSETS_PATH in your IDE during development
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 

endif()

target_compile_features("${CMAKE_PROJECT_NAME}" PRIVATE cxx_std_20)

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )

target_link_libraries(${PROJECT_NAME} raylib)

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include/")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${PROJECT_BINARY_DIR}/_deps/raylib-build/raylib/include/")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${PROJECT_BINARY_DIR}")
