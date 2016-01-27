# CMake settings
cmake_minimum_required(VERSION 3.2.1)

set(SOLUTION_NAME Doremi)
project(${SOLUTION_NAME})

# Add options
option(BUILD_UNIT_TEST_BUILD "Unchecked is no testing" OFF)
option(BUILD_WITH_EXTENDED_WARNINGS "Unchecked is VS default, checked is /W4" OFF)
option(BUILD_TREAT_WARNINGS_AS_ERROR "Unchecked is off" OFF)
option(BUILD_CUSTOM_MEASUREMENT_TIMING "Unckeched is off" ON)
option(BUILD_DRM_EDITOR_PLUGIN "Must be runned in 64 bit." OFF)

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


# Check if 32 or 64 bit, not a perfect solution. However, it works for fine for this project.
if(CMAKE_SIZEOF_VOID_P MATCHES "8")
	set(ARCHITECTURE x64)
	set(ARCHITECTURE_x64 ON)
else()
	set(ARCHITECTURE x86)
	set(ARCHITECTURE_x64 OFF)
endif()

# Set output folders
set(OUTPUT_FOLDER ${CMAKE_BINARY_DIR}/build/${ARCHITECTURE})

# Visual studio adds their own folder onto this, notice uppercase Debug/Release, thus do not add them manually.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_FOLDER})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_FOLDER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_FOLDER})

# Find external dlls
set(EXTERNAL_DLLS ${RESOURCE_REPOSITORY_ROOT}/External/dll/${ARCHITECTURE}/)

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
find_package(MAYAPLUGIN REQUIRED)
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

# Set global compiler options
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(/MP)

if(BUILD_CUSTOM_MEASUREMENT_TIMING)
	add_definitions(-DCUSTOM_TIMING_MEASUREMENT)
endif()

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

add_subdirectory(DoremiEditor)

# Copy game resources
add_custom_target(_resourceCopy ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CODE_REPOSITORY_ROOT}/DoremiEngine/Graphic/ShaderFiles ${OUTPUT_FOLDER}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CODE_REPOSITORY_ROOT}/DoremiEngine/Graphic/ShaderFiles ${OUTPUT_FOLDER}/Release
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CODE_REPOSITORY_ROOT}/DoremiEngine/Graphic/ShaderFiles ${OUTPUT_FOLDER}/RelWithDebInfo
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_REPOSITORY_ROOT}/Resources/ ${OUTPUT_FOLDER}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_REPOSITORY_ROOT}/Resources/ ${OUTPUT_FOLDER}/Release
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_REPOSITORY_ROOT}/Resources/ ${OUTPUT_FOLDER}/RelWithDebInfo
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CODE_REPOSITORY_ROOT}/DoremiEngine/Graphic/ShaderFiles ${CMAKE_BINARY_DIR}/${SOLUTION_NAME} # This row is only required for debugging from visual studio GUI
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_REPOSITORY_ROOT}/Resources ${CMAKE_BINARY_DIR}/${SOLUTION_NAME} # This row is only required for debugging from visual studio GUI
)

# Move external dlls to executing folder
add_custom_target(_dependencies ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_DLLS}/Debug ${OUTPUT_FOLDER}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_DLLS}/Release ${OUTPUT_FOLDER}/Release
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_DLLS}/Release ${OUTPUT_FOLDER}/RelWithDebInfo
)