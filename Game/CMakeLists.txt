# CMake settings
cmake_minimum_required(VERSION 3.2.1)

# Root project settings
set(PROJECT_NAME Game)
project(${PROJECT_NAME})

# Set the files used in the target
file(GLOB_RECURSE HEADERS Headers/ *.h*)
file(GLOB_RECURSE SOURCES Source/ *.cpp)
include_directories(Include)
create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/Include" ${HEADERS})
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/Source" ${SOURCES})
set(LIBRARIES ${EXTERNAL_LIBRARIES_ALL})

# Add the target
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Set SUBSYSTEM:WINDOW
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOW")