#This file parses through the iGeom-Defs.inc file, to format the variables for cmake.

#Each person needs to change this path to their own CGM before running cmake.
file(STRINGS <path/to/installed/CGM>/lib/iGeom-Defs.inc INPUT_FILE)

list(GET INPUT_FILE 6 IGEOM_DEFINES)
separate_arguments(IGEOM_DEFINES)
list(GET IGEOM_DEFINES 2 IGEOM_DEFINES)

list(GET INPUT_FILE 22 IGEOM_INCLUDES)
separate_arguments(IGEOM_INCLUDES)
list(GET IGEOM_INCLUDES 2 IGEOM_INCLUDES)
SET(IGEOM_INCLUDES -I${IGEOM_INCLUDES})

list(GET INPUT_FILE 12 IGEOM_LIBS)
separate_arguments(IGEOM_LIBS)
list(GET IGEOM_LIBS 6 7 10 11 12 IGEOM_LIBS)

list(GET INPUT_FILE 20 IGEOM_CXX_LDFLAGS)
separate_arguments(IGEOM_CXX_LDFLAGS)
list(GET IGEOM_CXX_LDFLAGS 2 IGEOM_CXX_LDFLAGS)
string(LENGTH ${IGEOM_CXX_LDFLAGS} leng)
MATH(EXPR X "${leng} - 2")
string(SUBSTRING ${IGEOM_CXX_LDFLAGS} 2 ${X} IGEOM_CXX_LDFLAGS)

list(GET INPUT_FILE 10 IGEOM_LDFLAGS)
separate_arguments(IGEOM_LDFLAGS)
list(GET IGEOM_LDFLAGS 4 IGEOM_LDFLAGS)
string(LENGTH ${IGEOM_LDFLAGS} leng)
MATH(EXPR X "${leng} - 2")
string(SUBSTRING ${IGEOM_LDFLAGS} 2 ${X} IGEOM_LDFLAGS)

SET(IGEOM_CPPFLAGS "${IGEOM_DEFINES} ${IGEOM_INCLUDES}")

