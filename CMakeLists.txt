cmake_minimum_required(VERSION 3.0)

project(MCNP2CAD)
SET ( MCNP2CAD_VERSION_MAJOR 1 )
SET ( MCNP2CAD_VERSION_MINOR 0 )

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include("${CMAKE_MODULE_PATH}/FindSphinx.cmake")
add_subdirectory (docs)
SET(IGEOM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/") 
SET(IGEOM_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib/")
#Each user must include the path to cubit here
SET(CUBIT_LIBRARY_DIR <path/to/cubit>)

SET(CPP_COMPILE_FLAGS "-g -std=c++11 -Wall -Wextra -DUSING_CGMA -DHAVE_IGEOM_CONE -I${IGEOM_INCLUDE_DIR} -DSPHINX_HTML=ON")
SET(INCLUDED_LIBRARIES "-lgeoman -lcgm -lcubiti19 -lcubit_geom -lcubit_util -larmadillo")
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_COMPILE_FLAGS}" )

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")
include_directories("${CMAKE_SOURCE_DIR}/include/braryfiles/exheads")
include_directories("${CMAKE_SOURCE_DIR}/include/braryfiles/exheads/layer2")
include_directories("${CMAKE_SOURCE_DIR}/include/braryfiles/exheads/layer2/layer3")
include_directories("${CMAKE_SOURCE_DIR}/include/braryfiles/exheads/layer2/layer3/layer4")
include_directories("${CMAKE_SOURCE_DIR}/include/braryfiles/exheads/layer2/layer3/layer4/layer5")
include_directories("${CMAKE_SOURCE_DIR}/include/braryfiles/exheads/layer2/layer3/layer4/layer5/layer6")

add_library(geoman STATIC include/braryfiles/CATag.cpp include/braryfiles/iGeom_CGMA.cc include/braryfiles/iGeomError.cc)
LINK_DIRECTORIES(${IGEOM_LIBRARY_DIR})
LINK_DIRECTORIES(${CUBIT_LIBRARY_DIR}/bin/)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/build/)
add_executable(mcnp2cad ${SOURCES} ${HEADERS})
ADD_DEPENDENCIES (mcnp2cad geoman)
TARGET_LINK_LIBRARIES(mcnp2cad ${INCLUDED_LIBRARIES})
