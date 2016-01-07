# CMake settings
cmake_minimum_required(VERSION 3.2.1)

set(SOLUTION_NAME Doremi)
project(${SOLUTION_NAME})

# Add options
option(BUILD_LINK_AND_COMPILE_RELEASE_BUILD "Unchecked is debug" OFF)
option(BUILD_LINK_64_BIT_BUILD "Unchecked is 32 bit" OFF)
option(BUILD_UNIT_TEST_BUILD "Unchecked is no testing" OFF)
option(BUILD_WITH_EXTENDED_WARNINGS "Unchecked is VS default, checked is /W4" OFF)
option(BUILD_TREAT_WARNINGS_AS_ERROR "Unchecked is off" OFF)

if(BUILD_UNIT_TEST_BUILD)
	set(ENGINE_LINK_TYPE STATIC)
else()
	set(ENGINE_LINK_TYPE SHARED)
endif()

# Fetch environment variable
set(CODE_REPOSITORY_ROOT $ENV{SSP_CODE_HOME})
set(RESOURCE_REPOSITORY_ROOT $ENV{SSP_RESOURCE_HOME})

# Replace backwards slash with forward slash
STRING(REGEX REPLACE "\\\\" "/" CODE_REPOSITORY_ROOT ${CODE_REPOSITORY_ROOT})
STRING(REGEX REPLACE "\\\\" "/" RESOURCE_REPOSITORY_ROOT ${RESOURCE_REPOSITORY_ROOT})

# Check if 32 or 64 bit
if(BUILD_LINK_64_BIT_BUILD)
	set(ARCHITECTURE_BIT_VALUE x64)
	else()
	set(ARCHITECTURE_BIT_VALUE x86)
endif()

# Set output folders
set(OUTPUT_FOLDER ${CMAKE_BINARY_DIR}/build/${ARCHITECTURE_BIT_VALUE})

# Visual studio adds their own folder onto this, notice uppercase Debug/Release, thus do not add them manually.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_FOLDER})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_FOLDER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_FOLDER})

# Path for copying .dll to correct folder
if(BUILD_LINK_AND_COMPILE_RELEASE_BUILD)
	set(COMPILE_RELEASE_BUILD_VALUE "release") # Atm this wont work on non-windows systems
else()
	set(COMPILE_RELEASE_BUILD_VALUE "debug") # Atm this wont work on non-windows systems
endif()

# Set the final
set(OUTPUT_FOLDER ${OUTPUT_FOLDER}/${COMPILE_RELEASE_BUILD_VALUE})
set(EXTERNAL_DLLS ${RESOURCE_REPOSITORY_ROOT}/External/dll/${ARCHITECTURE_BIT_VALUE}/${COMPILE_RELEASE_BUILD_VALUE})

# Find external libraries and modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RESOURCE_REPOSITORY_ROOT}/CMake/Modules/")
include(create_source_group)

# Set external library lookuppath
set(EXTERNAL_LIBRARY_LOOKUP_PATH ${RESOURCE_REPOSITORY_ROOT}/External/lib/)

# Find external dependencies
find_package(FMod REQUIRED)
find_package(DirectX11 REQUIRED)
find_package(WinSock2 REQUIRED)
find_package(PhysX REQUIRED)
find_package(SDL2 REQUIRED)
if(BUILD_UNIT_TEST_BUILD)
	find_package(Gtest REQUIRED)
endif()

# Set includepaths for external dependencies
set(EXTERNAL_INCLUDE_DIRECTORY
	${FMOD_INCLUDE_DIRECTORY}
	${WINSOCK2_INCLUDE_DIRECTORY}
	${PHYSX_INCLUDE_DIRECTORY}
	${SDL2_INCLUDE_DIRECTORY}
	${GTEST_INCLUDE_DIRECTORY}
)

# Combine all for debugging
set(EXTERNAL_LIBRARIES_ALL
	${EXTERNAL_LIBRARIES_AUDIO}
	${EXTERNAL_LIBRARIES_GRAPHICS}
	${EXTERNAL_LIBRARIES_NETWORK}
	${EXTERNAL_LIBRARY_PHYSICS}
	${EXTERNAL_LIBRARIES_INPUT}
	${EXTERNAL_LIBRARIES_GTEST}
)

# Set global compiler options
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(/MP)

if(BUILD_WITH_EXTENDED_WARNINGS)
	add_definitions(/W4)
endif()
if (BUILD_TREAT_WARNINGS_AS_ERROR)
	add_definitions(/WX)
endif()


# Add default include directories
include_directories(${EXTERNAL_INCLUDE_DIRECTORY})
include_directories(${CMAKE_SOURCE_DIR})

# Add subdirectories
add_subdirectory(Doremi)
add_subdirectory(DoremiEngine)
add_subdirectory(Utility)
if(BUILD_UNIT_TEST_BUILD)
	add_subdirectory(UnitTest)
endif()


# Copy game resources
add_custom_target(_resourceCopy ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CODE_REPOSITORY_ROOT}/DoremiEngine/Graphic/ShaderFiles ${OUTPUT_FOLDER}
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CODE_REPOSITORY_ROOT}/DoremiEngine/Graphic/ShaderFiles ${CMAKE_BINARY_DIR}/${SOLUTION_NAME} # This row is only required for debugging from visual studio GUI
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_REPOSITORY_ROOT}/Resources ${OUTPUT_FOLDER}
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_REPOSITORY_ROOT}/Resources ${CMAKE_BINARY_DIR}/${SOLUTION_NAME} # This row is only required for debugging from visual studio GUI
)

# Move external dlls to executing folder
add_custom_target(_dependencies ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_DLLS} ${OUTPUT_FOLDER})

# Set the default configuration
if (BUILD_LINK_AND_COMPILE_RELEASE_BUILD)
    set(CMAKE_BUILD_TYPE "Release")
else()
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Required as cmake does not rebuild correctly
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)