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
    # do sth for gcc
ENDIF()

# compile flags, Release by default
IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wno-write-strings -pg -g -std=c++0x")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -Wno-write-strings -std=c++0x")

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

