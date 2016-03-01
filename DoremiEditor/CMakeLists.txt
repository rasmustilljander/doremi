# CMake settings
cmake_minimum_required(VERSION 3.2.1)
add_subdirectory(Core)
add_subdirectory(Editor)

if(BUILD_DRM_EDITOR_PLUGIN)
	add_subdirectory(DRMEditorPlugin)
endif()