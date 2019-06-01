# Dynamic libraries
if (NOT CMAKE_BUILD_TYPE)
	set(RELEASE_CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
	list(REMOVE_ITEM RELEASE_CONFIGURATIONS ${DEBUG_CONFIGURATIONS})
	foreach (config IN ITEMS ${DEBUG_CONFIGURATIONS})
		file(COPY "${EXTERNALS_BIN_DEBUG}/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${config}/")
	endforeach()
	foreach (config IN ITEMS ${RELEASE_CONFIGURATIONS})
		file(COPY "${EXTERNALS_BIN_RELEASE}/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${config}/")
	endforeach()
else()
	list(FIND ${DEBUG_CONFIGURATIONS} ${CMAKE_BUILD_TYPE} is_debug)
	message(${DEBUG_CONFIGURATIONS})
	message(${is_debug})
	if (${is_debug} EQUAL -1)
		set(src_dir ${EXTERNALS_BIN_RELEASE})
	else()
		set(src_dir ${EXTERNALS_BIN_DEBUG})
	endif()
	file(COPY "${src_dir}/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
endif()


# Define for assets

# GameData is meant to replace all asset libraries, except MTL and NODE shaders.
set(INL_GAMEDATA_CMAKE ${CMAKE_SOURCE_DIR}/GameData)
file(TO_NATIVE_PATH ${INL_GAMEDATA_CMAKE} INL_GAMEDATA)
set(INL_GAMEDATA "R\"(${INL_GAMEDATA})\"")
add_definitions(-DINL_GAMEDATA=${INL_GAMEDATA})

set(INL_ASSET_DIRECTORY_CMAKE ${CMAKE_SOURCE_DIR}/Test/QC_Simulator/assets)
file(TO_NATIVE_PATH ${INL_ASSET_DIRECTORY_CMAKE} INL_ASSET_DIRECTORY)
set(INL_ASSET_DIRECTORY "R\"(${INL_ASSET_DIRECTORY})\"")
add_definitions(-DINL_ASSET_DIRECTORY=${INL_ASSET_DIRECTORY})

set(INL_NODE_SHADER_DIRECTORY_CMAKE ${CMAKE_SOURCE_DIR}/Engine/GraphicsFoundationLibrary/Shaders)
file(TO_NATIVE_PATH ${INL_NODE_SHADER_DIRECTORY_CMAKE} INL_NODE_SHADER_DIRECTORY)
set(INL_NODE_SHADER_DIRECTORY "R\"(${INL_NODE_SHADER_DIRECTORY})\"")
set(INL_MTL_SHADER_DIRECTORY_CMAKE ${CMAKE_SOURCE_DIR}/Engine/GraphicsFoundationLibrary/MaterialShaders)
file(TO_NATIVE_PATH ${INL_MTL_SHADER_DIRECTORY_CMAKE} INL_MTL_SHADER_DIRECTORY)
set(INL_MTL_SHADER_DIRECTORY "R\"(${INL_MTL_SHADER_DIRECTORY})\"")
add_definitions(-DINL_NODE_SHADER_DIRECTORY=${INL_NODE_SHADER_DIRECTORY})
add_definitions(-DINL_MTL_SHADER_DIRECTORY=${INL_MTL_SHADER_DIRECTORY})