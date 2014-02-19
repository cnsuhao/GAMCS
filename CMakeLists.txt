## -----------------------------------------------------------------------------
##
## GIMCS -- Generalized Intelligence Model and Computer Simulation
##
## Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
##
## -----------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(GIMCS CXX)   # Generalized Intelligence Model and Computer Simulation

# version
SET(GIMCS_MAJOR 0)
SET(GIMCS_MINOR 1)
SET(GIMCS_BUILD 0)

SET(GIMCS_NAME gimcs)   
SET(GIMCS_STATIC_NAME gimcs_static)
SET(GIMCS_VERSION ${GIMCS_MAJOR}.${GIMCS_MINOR}.${GIMCS_BUILD})

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

# detect platforms
IF (WIN32)
    ADD_DEFINITIONS("-D_WIN32_")
ELSE (UNIX)
    ADD_DEFINITIONS("-D_UNIX_")
ENDIF()

# detect compilers and set particular options
IF (MSVC)
    # do something for msvs
ENDIF()

IF (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++0x -Wall -Wextra -Wno-write-strings")
ENDIF()

# include directory
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

# public headers
SET(LIBGIMCS_PUBLIC_HDRS
    ${PROJECT_SOURCE_DIR}/include/GIOM.h
    ${PROJECT_SOURCE_DIR}/include/TSGIOM.h
    ${PROJECT_SOURCE_DIR}/include/Agent.h
    ${PROJECT_SOURCE_DIR}/include/MAgent.h
    ${PROJECT_SOURCE_DIR}/include/CSMAgent.h
    ${PROJECT_SOURCE_DIR}/include/Avatar.h
    ${PROJECT_SOURCE_DIR}/include/CSThreadAvatar.h
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

# install prefix
SET(PREFIX  ${CMAKE_INSTALL_PREFIX})

# install header files
SET(INCLUDEDIR  ${CMAKE_INSTALL_PREFIX}/include/gimcs)
INSTALL(FILES ${LIBGIMCS_PUBLIC_HDRS} DESTINATION ${INCLUDEDIR})

# sub directories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(test)

