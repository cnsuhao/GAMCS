CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(GIMCS CXX)   # Generalized Intelligence Model and Computer Simulation

# version
SET(GIMCS_MAJOR 1)
SET(GIMCS_MINOR 0)
SET(GIMCS_BUILD 0)

SET(GIMCS_NAME gimcs)   
SET(GIMCS_STATIC_NAME gimcs_static)
SET(GIMCS_VERSION ${GIMCS_MAJOR}.${GIMCS_MINOR}.${GIMCS_BUILD})

# configuration options
OPTION(DEBUG "Build with debug output" NO)
OPTION(DEBUG_MORE "Build with debug output" NO)
OPTION(INT_BITS "Bitwise of integer" 64)    # 8, 16, 32, 64
MESSAGE(STATUS "Input/Output bitwise " ${INT_BITS})
OPTION(WITH_MYSQL "Build with Mysql support" YES)   # Mysql is a kind of storage to save memories of agent
MESSAGE(STATUS "Mysql support: " ${WITH_MYSQL})
OPTION(WITH_CGRAPH "Build with CGraph support" NO)  # CGraph supports reading topo structure from .dot file directly
MESSAGE(STATUS "CGraph support: " ${WITH_CGRAPH})

# find dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF (WITH_MYSQL)
    FIND_PACKAGE(MYSQL REQUIRED)
    INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${MYSQL_LIBRARIES})
ENDIF ()

IF (WITH_CGRAPH)
    FIND_PACKAGE(CGraph REQUIRED)
    INCLUDE_DIRECTORIES(${CGRAPH_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${CGRAPH_LIBRARIES})
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
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Zc /Od")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc /O2")
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
SET(GIMCS_GIM_HDRS 
    ${PROJECT_SOURCE_DIR}/include/gimcs/GIOM.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/TSGIOM.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/Agent.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/StateInfoParser.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/OSAgent.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/Avatar.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/ExNet.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/ExManager.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/Storage.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/MemoryViewer.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/config.h
    )

SET(GIMCS_CS_HDRS
    ${PROJECT_SOURCE_DIR}/include/gimcs/CSOSAgent.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/CSThreadAvatar.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/CSThreadExNet.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/CSThreadExManager.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/PrintViewer.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/DotViewer.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/CDotViewer.h
    ${PROJECT_SOURCE_DIR}/include/gimcs/Debug.h
    )

SET(GIMCS_MYSQL_HDRS
    ${PROJECT_SOURCE_DIR}/include/gimcs/Mysql.h
    )

IF (WITH_MYSQL)
    SET(GIMCS_PUBLIC_HDRS
        ${GIMCS_GIM_HDRS}
        ${GIMCS_CS_HDRS}
        ${GIMCS_MYSQL_HDRS}
        )
ELSE (WITH_MYSQL)
    SET(GIMCS_PUBLIC_HDRS
        ${GIMCS_GIM_HDRS}
        ${GIMCS_CS_HDRS}
        )
ENDIF()

# configuration file
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/include/gimcs/config.h)

# install prefix
SET(PREFIX  ${CMAKE_INSTALL_PREFIX})

# install header files
SET(INCLUDEDIR  ${CMAKE_INSTALL_PREFIX}/include/gimcs)
INSTALL(FILES ${GIMCS_PUBLIC_HDRS} DESTINATION ${INCLUDEDIR})

# sub directories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(test)

