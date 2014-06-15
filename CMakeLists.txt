CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(GAMCS CXX)   # Generalized Agent Model and Computer Simulation

# version
SET(GAMCS_MAJOR 1)
SET(GAMCS_MINOR 3)
SET(GAMCS_BUILD 4)

SET(GAMCS_NAME gamcs)   
SET(GAMCS_STATIC_NAME gamcs_static)
SET(GAMCS_VERSION ${GAMCS_MAJOR}.${GAMCS_MINOR}.${GAMCS_BUILD})

# configuration options
OPTION(DEBUG "Build with debug output" OFF)

OPTION(DEBUG_MORE "Build with debug output" OFF)

OPTION(INT_BITS "Bitwise of integer" OFF)    # 8, 16, 32, 64
IF (NOT INT_BITS)   # 64bit by default
    SET(INT_BITS 64)
ENDIF()

# find dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# find Mysql
FIND_PACKAGE(MYSQL)
IF (MYSQL_FOUND)
    INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${MYSQL_LIBRARIES})
ENDIF ()

# find Sqlite3
FIND_PACKAGE(SQLITE3)
IF (SQLITE3_FOUND)
    INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${SQLITE3_LIBRARIES})
ENDIF ()

# other components come here

# macro definitions
IF (DEBUG)
    ADD_DEFINITIONS("-D_DEBUG_")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

IF (DEBUG_MORE)
    ADD_DEFINITIONS("-D_DEBUG_MORE_")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

IF (MYSQL_FOUND)
    ADD_DEFINITIONS("-D_MYSQL_FOUND_")
ENDIF()

IF (SQLITE3_FOUND)
    ADD_DEFINITIONS("-D_SQLITE_FOUND_")
ENDIF()

# detect compilers and set particular options
IF (MSVC)
    # do something for msvs
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  /O2")
ENDIF()

IF (CMAKE_COMPILER_IS_GNUCXX)
    # do sth for gcc
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wno-write-strings -g -std=c++0x")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -Wno-write-strings -std=c++0x")

    SET(LIBS ${LIBS} pthread)   # add pthread library
ENDIF()

# compile flags, Release by default
IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# include directory
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

# public headers
SET(GAMCS_GAM_HDRS 
    ${PROJECT_SOURCE_DIR}/include/gamcs/GIOM.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/TSGIOM.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/Agent.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/StateInfoParser.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/OSAgent.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/Avatar.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/Storage.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/MemoryViewer.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/config.h
    )

SET(GAMCS_CS_HDRS
    ${PROJECT_SOURCE_DIR}/include/gamcs/CSOSAgent.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/PrintViewer.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/DotViewer.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/CDotViewer.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/debug.h
    ${PROJECT_SOURCE_DIR}/include/gamcs/platforms.h
    )

SET(GAMCS_PUBLIC_HDRS ${GAMCS_GAM_HDRS} ${GAMCS_CS_HDRS})

SET(GAMCS_MYSQL_HDRS
    ${PROJECT_SOURCE_DIR}/include/gamcs/Mysql.h
    )

IF (MYSQL_FOUND)
    SET(GAMCS_PUBLIC_HDRS
        ${GAMCS_PUBLIC_HDRS}
        ${GAMCS_MYSQL_HDRS}
        )
ENDIF()

SET(GAMCS_SQLITE_HDRS
    ${PROJECT_SOURCE_DIR}/include/gamcs/Sqlite.h
    )

IF (SQLITE3_FOUND)
    SET(GAMCS_PUBLIC_HDRS
        ${GAMCS_PUBLIC_HDRS}
        ${GAMCS_SQLITE_HDRS}
        )
ENDIF()

# configuration file
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/include/gamcs/config.h)

# install prefix
SET(PREFIX  ${CMAKE_INSTALL_PREFIX})

# install header files
SET(INCLUDEDIR  ${CMAKE_INSTALL_PREFIX}/include/gamcs)
INSTALL(FILES ${GAMCS_PUBLIC_HDRS} DESTINATION ${INCLUDEDIR})

# sub directories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(test)

# print out configuration
MESSAGE(STATUS "GAMCS Version: " ${GAMCS_VERSION})
MESSAGE(STATUS "INT_BITS: " ${INT_BITS})
MESSAGE(STATUS "Build Type: " ${CMAKE_BUILD_TYPE})
MESSAGE(STATUS "Mysql: " ${MYSQL_FOUND})
MESSAGE(STATUS "Sqlite3: " ${SQLITE3_FOUND})
