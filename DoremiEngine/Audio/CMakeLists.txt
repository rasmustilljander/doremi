# CMake settings
cmake_minimum_required(VERSION 3.2.1)

# Root project settings
set(PROJECT_NAME Audio)
project(${PROJECT_NAME})

# Set the files used in the target
file(GLOB_RECURSE HEADERS Headers/ *.h*)
file(GLOB_RECURSE SOURCES Source/ *.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/Include" ${HEADERS})
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/Source" ${SOURCES})
set(LIBRARIES DebugLog)

# Set preprocessor definitions
SET(DEFINITIONS 
	_WINDLL
)

# Add the target
add_library(${PROJECT_NAME} ${ENGINE_LINK_TYPE} ${HEADERS} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE "${DEFINITIONS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_link_libraries(${PROJECT_NAME} optimized "${EXTERNAL_LIBRARIES_AUDIO_RELEASE}")
target_link_libraries(${PROJECT_NAME} debug "${EXTERNAL_LIBRARIES_AUDIO_DEBUG}")