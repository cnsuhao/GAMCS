AUX_SOURCE_DIRECTORY(. LIBISBL_SRCS)
ADD_LIBRARY(isbl SHARED ${LIBISBL_SRCS})
TARGET_LINK_LIBRARIES(isbl mysqlclient pthread cgraph)   # depended libraries
