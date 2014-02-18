# - Try to find GraphViz
# Once done this will define
#  CGRAPH_FOUND - System has GraphViz
#  CGRAPH_INCLUDE_DIRS - The GraphViz include directories
#  CGRAPH_LIBRARIES - The libraries needed to use GraphViz
#  CGRAPH_LDFLAGS_OTHER - Other LDFLAGS needed te use GraphViz
#  CGRAPH_DEFINITIONS - Compiler switches required for using GraphViz

find_package(PkgConfig)
if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER "2.8.1")
   # "QUIET" was introduced in 2.8.2
   set(_QUIET QUIET)
endif ()
pkg_check_modules(PC_LIBCGRAPH ${_QUIET} libcgraph)

set (CGRAPH_MIN_VERSION "2.0.0")
if ("${PC_LIBCGRAPH_VERSION}" VERSION_LESS ${CGRAPH_MIN_VERSION})
   MESSAGE(STATUS "libcgraph version required: ${CGRAPH_MIN_VERSION}. Current version: ${PC_LIBCGRAPH_VERSION}.")
else()
   SET(LIBCGRAPH_REQUIRED_VERSION 1)
endif()

if (LIBCGRAPH_REQUIRED_VERSION)
   find_path(CGRAPH_INCLUDE_DIR cgraph.h
      HINTS ${PC_LIBCGRAPH_INCLUDEDIR} ${PC_LIBCCGRAPH_INCLUDE_DIRS}
      PATH_SUFFIXES cgraph )

   find_library(CGRAPH_LIBRARY NAMES cgraph libcgraph
      HINTS ${PC_LIBCGRAPH_LIBDIR} ${PC_LIBCGRAPH_LIBRARY_DIRS} )

   set(CGRAPH_INCLUDE_DIRS ${CGRAPH_INCLUDE_DIR})
   set(CGRAPH_LIBRARIES ${CGRAPH_LIBRARY} )

   include(FindPackageHandleStandardArgs)
   # handle the QUIETLY and REQUIRED arguments and set EINA_FOUND to TRUE
   # if all listed variables are TRUE
   find_package_handle_standard_args(cgraph DEFAULT_MSG
       CGRAPH_INCLUDE_DIR CGRAPH_LIBRARY)
endif (LIBCGRAPH_REQUIRED_VERSION)

mark_as_advanced(CGRAPH_INCLUDE_DIR CGRAPH_LIBRARY)
