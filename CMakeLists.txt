# CMakeLists.txt

# Copyright (C) 2013-2014 Andy Huang

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(GIMCS CXX)   # Generalized Intelligence Model and Computer Simulation

SET(GIMCSLIB_MAJOR 0)
SET(GIMCSLIB_MINOR 1)
SET(GIMCSLIB_BUILD 0)
SET(GIMCS_NAME gimcs)
SET(GIMCSLIB_NAME libgimcs${GIMCSLIB_MAJOR}${GIMCSLIB_MINOR})
SET(GIMCSLIB_VERSION ${GIMCSLIB_MAJOR}.${GIMCSLIB_MINOR}.${GIMCSLIB_BUILD})

# needed packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(CGraph REQUIRED)
INCLUDE_DIRECTORIES(${CGRAPH_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${CGRAPH_LIBRARIES})

FIND_PACKAGE(MYSQL REQUIRED)
INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${MYSQL_LIBRARIES})

SET(LIBS ${LIBS} pthread)

# configuration options
option(GIMCS_DEBUG "Build with debug output" NO)

# definitions
IF (GIMCS_DEBUG)
    ADD_DEFINITIONS("-D_DEBUG_")
ENDIF()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

# public headers
SET(LIBGIMCS_PUBLIC_HDRS
    ${PROJECT_SOURCE_DIR}/include/GIOM.h
    ${PROJECT_SOURCE_DIR}/include/TSGIOM.h
    ${PROJECT_SOURCE_DIR}/include/Agent.h
    ${PROJECT_SOURCE_DIR}/include/MAgent.h
    ${PROJECT_SOURCE_DIR}/include/CSMAgent.h
    ${PROJECT_SOURCE_DIR}/include/Incarnation.h
    ${PROJECT_SOURCE_DIR}/include/CSThreadIncarnation.h
    ${PROJECT_SOURCE_DIR}/include/ExNet.h
    ${PROJECT_SOURCE_DIR}/include/CSThreadExNet.h
    ${PROJECT_SOURCE_DIR}/include/ExManager.h
    ${PROJECT_SOURCE_DIR}/include/CSThreadExManager.h
    ${PROJECT_SOURCE_DIR}/include/Storage.h
    ${PROJECT_SOURCE_DIR}/include/Mysql.h
    ${PROJECT_SOURCE_DIR}/include/MemoryViewer.h
    ${PROJECT_SOURCE_DIR}/include/PrintViewer.h
    ${PROJECT_SOURCE_DIR}/include/DotViewer.h
    )

SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -Wno-write-strings")

# install prefix
SET(PREFIX  ${CMAKE_INSTALL_PREFIX})

# install header files
SET(INCLUDEDIR  ${CMAKE_INSTALL_PREFIX}include/gimcs)
INSTALL(FILES ${LIBGIMCS_PUBLIC_HDRS} DESTINATION ${INCLUDEDIR})

# sub directories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(test)

