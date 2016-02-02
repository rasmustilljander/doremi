# CMake settings
cmake_minimum_required(VERSION 3.2.1)

# Root project settings
set(PROJECT_NAME Graphic)
project(${PROJECT_NAME})

# Set the files used in the target
file(GLOB_RECURSE HEADERS Headers/ *.h*)
file(GLOB_RECURSE SOURCES Source/ *.cpp)
file(GLOB_RECURSE VertexShader ShaderFiles/ *VertexShader.hlsl)
file(GLOB_RECURSE GeometryShader ShaderFiles/ *GeometryShader.hlsl)
file(GLOB_RECURSE PixelShader ShaderFiles/ *PixelShader.hlsl)
file(GLOB_RECURSE ComputeShader ShaderFiles/ *ComputeShader.hlsl)
file(GLOB_RECURSE EffectShader ShaderFiles/ *CommonInclude.hlsl)
#set(SHADER_FILES ${VertexShader} ${GeometryShader} ${PixelShader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/Include" ${HEADERS})
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/Source" ${SOURCES})
#create_source_group("Shader Files" "${CMAKE_CURRENT_SOURCE_DIR}/ShaderFiles/" ${SHADER_FILES})

SET_SOURCE_FILES_PROPERTIES( ${VertexShader} PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT VS_main )
SET_SOURCE_FILES_PROPERTIES( ${GeometryShader} PROPERTIES VS_SHADER_TYPE Geometry VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT GS_main )
SET_SOURCE_FILES_PROPERTIES( ${PixelShader} PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT PS_main )
SET_SOURCE_FILES_PROPERTIES( ${ComputeShader} PROPERTIES VS_SHADER_TYPE Compute VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT CS_main )
SET_SOURCE_FILES_PROPERTIES( ${EffectShader} PROPERTIES VS_SHADER_TYPE Effect VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT entry )

set(LIBRARIES)

# Set preprocessor definitions
SET(DEFINITIONS 
	_WINDLL
)

# Add the target
add_library(${PROJECT_NAME} ${ENGINE_LINK_TYPE} ${HEADERS} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE "${DEFINITIONS}")
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_link_libraries(${PROJECT_NAME} optimized "${EXTERNAL_LIBRARIES_GRAPHIC_RELEASE}")
target_link_libraries(${PROJECT_NAME} debug "${EXTERNAL_LIBRARIES_GRAPHIC_DEBUG}")
target_link_libraries(${PROJECT_NAME} optimized "${EXTERNAL_LIBRARIES_INPUT_RELEASE}")
target_link_libraries(${PROJECT_NAME} debug "${EXTERNAL_LIBRARIES_INPUT_DEBUG}")