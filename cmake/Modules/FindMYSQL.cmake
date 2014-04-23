# - Try to find Mysql
# Once done this will define
#  MYSQL_FOUND - System has Mysql
#  MYSQL_INCLUDE_DIRS - The Mysql include directories
#  MYSQL_LIBRARIES - The libraries needed to use Mysql

IF (WIN32)
    FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
        PATHS
        $ENV{MYSQL_INCLUDE_DIR}
        $ENV{MYSQL_DIR}/include
        $ENV{ProgramFiles}/MySQL/*/include
        $ENV{SystemDrive}/MySQL/*/include)

    FIND_LIBRARY(MYSQL_LIBRARY 
        NAMES mysqlclient mysql libmysql
        PATHS
        $ENV{MYSQL_LIBRARY}
        $ENV{MYSQL_DIR}/lib
        $ENV{ProgramFiles}/MySQL/*/lib
        $ENV{SystemDrive}/MySQL/*/lib)
ELSE (WIN32)
    FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
        PATHS
        /usr/include
        /usr/local/include
        PATH_SUFFIXES mysql)

    FIND_LIBRARY(MYSQL_LIBRARY 
        NAMES mysqlclient mysql libmysql
        PATHS
        /usr/lib
        /usr/local/lib
        PATH_SUFFIXES mysql)
ENDIF (WIN32)

SET(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
SET(MYSQL_LIBRARIES ${MYSQL_LIBRARY} )

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MYSQL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(mysql DEFAULT_MSG
    MYSQL_INCLUDE_DIR MYSQL_LIBRARY)

MARK_AS_ADVANCED(MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
