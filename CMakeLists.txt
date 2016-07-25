cmake_minimum_required(VERSION 3.0)

project(MCNP2CAD)
SET ( MCNP2CAD_VERSION_MAJOR 1 )
SET ( MCNP2CAD_VERSION_MINOR 0 )

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include("${CMAKE_MODULE_PATH}/FindSphinx.cmake")
add_subdirectory (docs)

#Each user must replace these by the indicated paths in their system

#path to the folder from which cgm was built. for all of the include_directories to work
#as written, the build folder must be on the same level as the cgm folder.
SET(CGM_BUILD_DIR <path/to/CGM/bld>)
#path to cubit folder
SET(CUBIT_LIBRARY_DIR <path/to/cubit>)

SET(CPP_COMPILE_FLAGS "-g -std=c++11 -Wall -Wextra -DUSING_CGMA -DHAVE_IGEOM_CONE -DSPHINX_HTML=ON")
SET(INCLUDED_LIBRARIES "-lcgm -lcubiti19 -lcubit_geom -lcubit_util -larmadillo")
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_COMPILE_FLAGS}" )

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")
include_directories("${CMAKE_SOURCE_DIR}/include/")
include_directories("${CGM_BUILD_DIR}/../cgm/geom/")
include_directories("${CGM_BUILD_DIR}/../cgm/util/")
include_directories("${CGM_BUILD_DIR}/../cgm/init/")
include_directories("${CGM_BUILD_DIR}/geom/")

add_library(iGeom STATIC lib/CATag.cpp lib/iGeom_CGMA.cc lib/iGeomError.cc)
LINK_DIRECTORIES(${CUBIT_LIBRARY_DIR}/bin/)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/build/)
LINK_DIRECTORIES(${CGM_BUILD_DIR}/.libs/)
add_executable(mcnp2cad ${SOURCES} ${HEADERS})
ADD_DEPENDENCIES (mcnp2cad iGeom)
TARGET_LINK_LIBRARIES(mcnp2cad ${CMAKE_SOURCE_DIR}/build/libiGeom.a ${INCLUDED_LIBRARIES})
