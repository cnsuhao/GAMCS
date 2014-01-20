#LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src) # 库目录搜索路径: -L
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
ADD_EXECUTABLE(mg Mouse.cpp main.cpp)
TARGET_LINK_LIBRARIES(mg avatar mysqlclient pthread)   # 指定链接库
