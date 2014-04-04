CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(GAMCS CXX)   # Generalized Intelligence Model and Computer Simulation

# version
SET(GAMCS_MAJOR 1)
SET(GAMCS_MINOR 1)
SET(GAMCS_BUILD 10)

SET(GAMCS_NAME gamcs)   
SET(GAMCS_STATIC_NAME gamcs_static)
SET(GAMCS_VERSION ${GAMCS_MAJOR}.${GAMCS_MINOR}.${GAMCS_BUILD})
MESSAGE(STATUS "GAMCS Version: " ${GAMCS_VERSION})

# configuration options
OPTION(DEBUG "Build with debug output" OFF)

OPTION(DEBUG_MORE "Build with debug output" OFF)

OPTION(INT_BITS "Bitwise of integer" OFF)    # 8, 16, 32, 64
IF (NOT INT_BITS)   # 64bit by default
    SET(INT_BITS 64)
ENDIF()
MESSAGE(STATUS "INT_BITS: " ${INT_BITS})

OPTION(WITH_MYSQL "Build with Mysql support" ON)   # Mysql is a kind of storage to save memories of agent
MESSAGE(STATUS "WITH_MYSQL: " ${WITH_MYSQL})

# find dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF (WITH_MYSQL)
    FIND_PACKAGE(MYSQL REQUIRED)
    INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${MYSQL_LIBRARIES})
ENDIF ()

# macro definitions
IF (DEBUG)
    ADD_DEFINITIONS("-D_DEBUG_")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

IF (DEBUG_MORE)
    ADD_DEFINITIONS("-D_DEBUG_MORE_")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

IF (WITH_MYSQL)
    ADD_DEFINITIONS("-D_WITH_MYSQL_")
ENDIF()

IF (WITH_CGRAPH)
    ADD_DEFINITIONS("-D_WITH_CGRAPH_")
ENDIF()

# detect platforms
IF (WIN32)
    ADD_DEFINITIONS("-D_WIN32_")
ELSE (UNIX)
    ADD_DEFINITIONS("-D_UNIX_")
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

MESSAGE(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

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

SET(GAMCS_MYSQL_HDRS
    ${PROJECT_SOURCE_DIR}/include/gamcs/Mysql.h
    )

IF (WITH_MYSQL)
    SET(GAMCS_PUBLIC_HDRS
        ${GAMCS_GAM_HDRS}
        ${GAMCS_CS_HDRS}
        ${GAMCS_MYSQL_HDRS}
        )
ELSE (WITH_MYSQL)
    SET(GAMCS_PUBLIC_HDRS
        ${GAMCS_GAM_HDRS}
        ${GAMCS_CS_HDRS}
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

