AUX_SOURCE_DIRECTORY(. LIBAVATAR_SRCS)
ADD_LIBRARY(avatar SHARED ${LIBAVATAR_SRCS})
TARGET_LINK_LIBRARIES(avatar mysqlclient pthread cgraph)   # depended libraries
