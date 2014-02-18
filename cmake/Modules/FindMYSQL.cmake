#
# Find the MySQL client includes and library
# 

# This module defines
# MYSQL_INCLUDE_DIRS, where to find mysql.h
# MYSQL_LIBRARIES, the libraries to link against to connect to MySQL
# MYSQL_FOUND, If false, you cannot build anything that requires MySQL.

SET(MYSQL_FOUND 0)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
    /usr/include
    /usr/include/mysql
    /usr/local/include
    /usr/local/include/mysql
    /usr/local/mysql/include
    /usr/local/mysql/include/mysql
    DOC "Specify the directory containing mysql.h."
    )

FIND_LIBRARY(MYSQL_LIBRARY 
    NAMES mysql libmysql mysqlclient
    PATHS
    /usr/lib
    /usr/lib64
    /usr/lib/mysql
    /usr/lib64/mysql
    /usr/local/lib
    /usr/local/lib/mysql
    /usr/local/mysql/lib
    /usr/local/mysql/lib/mysql
    DOC "Specify the mysql library here."
    )

SET(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
SET(MYSQL_LIBRARIES ${MYSQL_LIBRARY} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set EINA_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(mysql DEFAULT_MSG
    MYSQL_LIBRARY)

MARK_AS_ADVANCED(MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
