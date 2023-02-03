cmake_minimum_required(VERSION 3.24)

project(
	AmmoEnchanting
	VERSION 1.0.2
	LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_BACKPORT "Build for old Skyrim SE versions (1.6.318-1.6.353)." OFF)

if(BUILD_BACKPORT)
	add_compile_definitions(SKYRIMSE_PRE_1_6_629)
endif()

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/tools/SKSE-CMakeModules/Modules"
	"${PROJECT_SOURCE_DIR}/tools/BethesdaCMakeModules/Modules"
)
include(SKSEPlugin)
include(Papyrus)

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

Papyrus_Add(
	"PapyrusAPI"
	GAME ${SkyrimSE_PATH}
	IMPORTS
		"${PROJECT_SOURCE_DIR}/scripts"
	SOURCES
		"${PROJECT_SOURCE_DIR}/scripts/AmmoEnchanting.psc"
	OPTIMIZE
	ANONYMIZE
	COMPONENT ${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME}
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

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/fomod/ModuleConfig.xml"
		"${PROJECT_SOURCE_DIR}/data/fomod/info.xml"
	DESTINATION
		"fomod"
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/fomod/images/Arcane_Archer_Pack.jpg"
	DESTINATION
		"fomod/images"
)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
if(BUILD_BACKPORT)
	string(APPEND CPACK_PACKAGE_FILE_NAME "-1.6.353")
endif()
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
include(CPack)
