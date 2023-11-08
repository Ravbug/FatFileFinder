cmake_minimum_required(VERSION 3.17)   

project("FatFileFinder")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
# link time optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE CACHE INTERNAL "")	# only enable on release
set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE INTERNAL "")	# don't embed symbols in the binaries themselves

if(MSVC)
  add_definitions(/MP)
  add_definitions(/Qpar)
endif()

if (UNIX AND NOT APPLE)
	set(LINUX ON CACHE INTERNAL "")
endif()

file(GLOB source "source/*.cpp" "source/*.hpp" "source/*.h")
add_executable("${PROJECT_NAME}" WIN32 ${source} "source/wxmac.icns" "windows.rc")

#wxwidgets
set(wxBUILD_SHARED OFF CACHE INTERNAL "")
add_subdirectory("wxWidgets" EXCLUDE_FROM_ALL)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC 
    wx::base
    wx::core
)

# mac app
set_target_properties("${PROJECT_NAME}" PROPERTIES 
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/source/Info.plist"
)
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/source/wxmac.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
if(APPLE)
INSTALL(CODE 
	"include(BundleUtilities)
	fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}.app\" \"\" \"\")
	" 
	COMPONENT Runtime
)
elseif(LINUX)
	INSTALL(CODE 
	  "include(../appimage.cmake)
	  make_appimage(
	        EXE \"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}\"
	        NAME \"FatFileFinder\"
	        ICON \"../source/wxlin.xpm\"
	        DIR_ICON \"../source/wxlin.xpm\"
	        OUTPUT_NAME \"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}.AppImage\"
	        ASSETS \"\"
	    )
		" 
		COMPONENT Runtime
	)
endif()
# windows app
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DPI_AWARE "PerMonitor")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

# Strip binary for release builds
if (CMAKE_BUILD_TYPE STREQUAL Release)
	if(LINUX)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
	elseif(APPLE)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME})
	endif()
endif ()
