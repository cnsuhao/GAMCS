# - Try to find Mysql
# Once done this will define
#  SQLITE3_FOUND - System has Mysql
#  SQLITE3_INCLUDE_DIRS - The Mysql include directories
#  SQLITE3_LIBRARIES - The libraries needed to use Mysql

IF (WIN32)
    FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h
        PATHS
        $ENV{SQLITE3_INCLUDE_DIR}
        $ENV{SQLITE3_DIR}/include
        )

    FIND_LIBRARY(SQLITE3_LIBRARY 
        NAMES sqlite3 libsqlite3
        PATHS
        $ENV{SQLITE3_LIBRARY}
        $ENV{SQLITE3_DIR}/lib
        )

ELSE (WIN32)
    FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h
        PATHS
        /usr/include
        /usr/local/include
        PATH_SUFFIXES sqlite)

    FIND_LIBRARY(SQLITE3_LIBRARY 
        NAMES sqlite3 libsqlite3
        PATHS
        /usr/lib
        /usr/local/lib
        PATH_SUFFIXES sqlite)
ENDIF (WIN32)

SET(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
SET(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY} )

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(sqlite3 DEFAULT_MSG
    SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY)

MARK_AS_ADVANCED(SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY)
