# - Try to find GraphViz
# Once done this will define
#  CGRAPH_FOUND - System has GraphViz
#  CGRAPH_INCLUDE_DIRS - The GraphViz include directories
#  CGRAPH_LIBRARIES - The libraries needed to use GraphViz

IF (WIN32)
    FIND_PATH(CGRAPH_INCLUDE_DIR cgraph.h
        PATHS
        $ENV{CGRAPH_INCLUDE_DIR}
        $ENV{CGRAPH_DIR}/include
        $ENV{ProgramFiles}/Graphviz*/include
        $ENV{SystemDrive}/Graphviz*/include)

    IF (CMAKE_BUILD_TYPE STREQUAL Debug)
        SET(libsuffix debug)
    ELSE (CMAKE_BUILD_TYPE STREQUAL Debug)
        SET(libsuffix release)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL Debug)

    FIND_LIBRARY(CGRAPH_LIBRARY 
        NAMES cgraph libcgraph
        PATHS
        $ENV{CGRAPH_LIBRARY}
        $ENV{CGRAPH_DIR}/lib/${libsuffix}/lib
        $ENV{ProgramFiles}/Graphviz*/lib/${libsuffix}/lib
        $ENV{SystemDrive}/Graphviz*/lib/${libsuffix}/lib)
ELSE (WIN32)
    FIND_PATH(CGRAPH_INCLUDE_DIR cgraph.h
        PATHS
        /usr/include
        /usr/local/include
        PATH_SUFFIXES graphviz)

    FIND_LIBRARY(CGRAPH_LIBRARY 
        NAMES cgraph libcgraph
        PATHS
        /usr/lib
        /usr/local/lib
        PATH_SUFFIXES graphviz)
ENDIF (WIN32)

SET(CGRAPH_INCLUDE_DIRS ${CGRAPH_INCLUDE_DIR})
SET(CGRAPH_LIBRARIES ${CGRAPH_LIBRARY} )

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set EINA_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(cgraph DEFAULT_MSG
    CGRAPH_INCLUDE_DIR CGRAPH_LIBRARY)

MARK_AS_ADVANCED(CGRAPH_INCLUDE_DIR CGRAPH_LIBRARY)
