#LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src) # 库目录搜索路径: -L
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
ADD_EXECUTABLE(rb1 Robot1.cpp rb1.cpp)
TARGET_LINK_LIBRARIES(rb1 avatar mysqlclient)   # 指定链接库
