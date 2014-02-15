AUX_SOURCE_DIRECTORY(. LIBINCARNATION_SRCS)
ADD_LIBRARY(incarnation SHARED ${LIBINCARNATION_SRCS})
TARGET_LINK_LIBRARIES(incarnation mysqlclient pthread cgraph)   # depended libraries
