######################################################################
# OGITOR PLUGIN BUILD SYSTEM
# Welcome to the CMake build system for  OGITOR PLUGINS
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################

cmake_minimum_required(VERSION 2.6)
project(RoadSplines)

# Works around warnings libraries linked against that don't
# have absolute paths (e.g. -lOgreMain)
cmake_policy(SET CMP0003 NEW)

# Works around warnings about escaped quotes in ADD_DEFINITIONS
# statements.
cmake_policy(SET CMP0005 NEW)

if(WIN32)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${OGITOR_BUILD_DIR}/RunPath/Plugins" CACHE PATH "Ogitor install prefix" FORCE)
    endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
else(WIN32)
    if(NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Ogitor install prefix" FORCE)
    endif (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(WIN32)

set(RoadSplines_VERSION_MAJOR "1")
set(RoadSplines_VERSION_MINOR "0")
set(RoadSplines_VERSION_PATCH "0")
set(RoadSplines_VERSION "${RoadSplines_VERSION_MAJOR}.${RoadSplines_VERSION_MINOR}.${RoadSplines_VERSION_PATCH}")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

find_package(OGRE REQUIRED)

if(APPLE)
	find_package(CG REQUIRED)
endif(APPLE)

if((NOT "${OGRE_VERSION_NAME}" STREQUAL "Byatis") AND (NOT "${OGRE_VERSION_NAME}" STREQUAL "Ghadamon"))
	message(SEND_ERROR "You need Ogre 1.8 Byatis or later to build this.")	
endif()

find_package(Ogitor REQUIRED)
find_package(OFS REQUIRED)

if(WIN32)
	getenv_path(OGRE_BYATIS)
	set(OGRE_SAMPLES_INCLUDEPATH
		${ENV_OGRE_BYATIS}/Samples/include
	)
endif(WIN32)

include_directories(SYSTEM ${OGRE_INCLUDE_DIRS})
include_directories(SYSTEM ${OGRE_Terrain_INCLUDE_DIR})
include_directories(SYSTEM ${OGRE_Paging_INCLUDE_DIR})
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(SYSTEM ${Ogitor_INCLUDE_DIRS})
include_directories(SYSTEM ${OFS_INCLUDE_DIR})

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_definitions(-DPLUGIN_EXPORT)

set(RoadSplinesSource 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DynamicRenderable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Road.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RoadEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RoadPointEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RoadRenderListener.cpp
)

add_library(RoadSplines SHARED ${RoadSplinesSource})
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib")

set_target_properties(RoadSplines PROPERTIES DEBUG_POSTFIX _d)
set_target_properties(RoadSplines PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
set_target_properties(RoadSplines PROPERTIES VERSION ${RoadSplines_VERSION} SOVERSION ${RoadSplines_VERSION_MAJOR})

if (UNIX)
    set(CMAKE_CXX_FLAGS -fPIC)
endif()

if(APPLE)
	find_package(IOKIT REQUIRED)
	find_package(Carbon REQUIRED)

    target_link_libraries(RoadSplines
        ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARY} ${OGRE_Paging_LIBRARY}
        ${Ogitor_LIBRARIES}
        ${OFS_LIBRARIES}
        ${Boost_LIBRARIES}

        ${Carbon_LIBRARIES}
        ${IOKit_LIBRARIES}
    )
else()
    target_link_libraries(RoadSplines
        ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARY} ${OGRE_Paging_LIBRARY}
        ${Ogitor_LIBRARIES}
        ${OFS_LIBRARIES}
        ${Boost_LIBRARIES}
    )
endif(APPLE)

if(APPLE)
	install(TARGETS RoadSplines BUNDLE DESTINATION ${PREFIX}bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
	install(TARGETS RoadSplines BUNDLE DESTINATION ${PREFIX}Debug CONFIGURATIONS Debug)
endif()

if (UNIX AND NOT APPLE)
    set_target_properties( RoadSplines PROPERTIES OUTPUT_NAME "OgRoadSplines" )
    install(TARGETS RoadSplines
            LIBRARY DESTINATION lib/ogitor)
endif()

