# CMake settings
cmake_minimum_required(VERSION 3.2.1)

# Root project settings
set(PROJECT_NAME UnitTest)
project(${PROJECT_NAME})

# Set up where DoremiEngine is located
set(DOREMIENGINE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../DoremiEngine)
# message(${"DOREMIENGINE_LOCATION"})
# Set the files used in the target
file(GLOB_RECURSE HEADERS Headers/ *.h*)
file(GLOB_RECURSE SOURCES Source/ *.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${DOREMIENGINE_LOCATION}/Graphic/Include)
create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/Include" ${HEADERS})
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/Source" ${SOURCES})
set(LIBRARIES ${EXTERNAL_LIBRARIES_GTEST} Graphic GameCore)


# Add the target
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Set SUBSYSTEM
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")


