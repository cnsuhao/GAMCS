SET(GIMCS_GIM_SRCS
    ./GIOM.cpp
    ./TSGIOM.cpp
    ./StateInfoParser.cpp
    ./Agent.cpp
    ./Avatar.cpp
    ./ExNet.cpp
    ./ExManager.cpp
    ./MemoryViewer.cpp
    )

SET(GIMCS_CS_SRCS
    ./CSOSAgent.cpp
    ./CSThreadAvatar.cpp
    ./CSThreadExNet.cpp
    ./CSThreadExManager.cpp
    ./PrintViewer.cpp
    ./DotViewer.cpp
    ./CDotViewer.cpp
    ./Debug.cpp
    )

SET(GIMCS_MYSQL_SRCS
    ./Mysql.cpp
    )

IF (WITH_MYSQL)
    SET(GIMCS_LIB_SRCS
        ${GIMCS_GIM_SRCS}
        ${GIMCS_CS_SRCS}
        ${GIMCS_MYSQL_SRCS}
        )
ELSE (WITH_MYSQL)
    SET(GIMCS_LIB_SRCS
        ${GIMCS_GIM_SRCS}
        ${GIMCS_CS_SRCS}
        )
ENDIF()

# shared library
ADD_LIBRARY(${GIMCS_NAME} SHARED ${GIMCS_LIB_SRCS})
TARGET_LINK_LIBRARIES(${GIMCS_NAME} ${LIBS})   # dependencies
SET_TARGET_PROPERTIES(${GIMCS_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1 
    VERSION ${GIMCS_MAJOR}.${GIMCS_MINOR} SOVERSION ${GIMCS_MAJOR})

# static library
ADD_LIBRARY(${GIMCS_STATIC_NAME} STATIC ${GIMCS_LIB_SRCS})
TARGET_LINK_LIBRARIES(${GIMCS_STATIC_NAME} ${LIBS})
SET_TARGET_PROPERTIES(${GIMCS_STATIC_NAME} PROPERTIES OUTPUT_NAME ${GIMCS_NAME} CLEAN_DIRECT_OUTPUT 1)

# install library
SET(LIBDIR  ${CMAKE_INSTALL_PREFIX}/lib)
INSTALL(TARGETS ${GIMCS_NAME} ${GIMCS_STATIC_NAME}
    LIBRARY DESTINATION ${LIBDIR}
    ARCHIVE DESTINATION ${LIBDIR})
