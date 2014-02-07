ADD_EXECUTABLE(printviewer print_viewer.cpp)
TARGET_LINK_LIBRARIES(printviewer avatar mysqlclient)   # 指定链接库
ADD_EXECUTABLE(dotviewer dot_viewer.cpp)
TARGET_LINK_LIBRARIES(dotviewer avatar mysqlclient)   # 指定链接库
