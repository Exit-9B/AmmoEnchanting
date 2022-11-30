cmake_minimum_required(VERSION 3.24)

project(
	AmmoEnchanting
	VERSION 1.0.0
	LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/tools/SKSE-CMakeModules/Modules"
)
include(SKSEPlugin)

set(SKSE_USE_XBYAK ON)
set(SKSE_SUPPORT_VR OFF)

SKSEPlugin_Add(
	${PROJECT_NAME}
	SOURCE_DIR src
	SOURCES
		.clang-format
		.editorconfig
		README.md
		vcpkg.json
	PRECOMPILE_HEADERS
		src/PCH/PCH.h
)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${SIMPLEINI_INCLUDE_DIRS}
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/AmmoEnchanting.esp"
		"${PROJECT_SOURCE_DIR}/data/AmmoEnchanting_ExoticArrows.esp"
		"${PROJECT_SOURCE_DIR}/data/AmmoEnchanting_KID.ini"
	DESTINATION
		"."
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/SKSE/Plugins/${PROJECT_NAME}.ini"
	DESTINATION
		"SKSE/Plugins"
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/Meshes/AmmoEnchanting/FireArrowFX.nif"
		"${PROJECT_SOURCE_DIR}/data/Meshes/AmmoEnchanting/FireQuiverFX.nif"
		"${PROJECT_SOURCE_DIR}/data/Meshes/AmmoEnchanting/FrostArrowFX.nif"
		"${PROJECT_SOURCE_DIR}/data/Meshes/AmmoEnchanting/FrostQuiverFX.nif"
		"${PROJECT_SOURCE_DIR}/data/Meshes/AmmoEnchanting/ShockArrowFX.nif"
		"${PROJECT_SOURCE_DIR}/data/Meshes/AmmoEnchanting/ShockQuiverFX.nif"
	DESTINATION
		"Meshes/AmmoEnchanting"
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/Interface/Translations/AmmoEnchanting_ENGLISH.txt"
	DESTINATION
		"Interface/Translations"
)
