AUX_SOURCE_DIRECTORY(. TSP_SRCS)
ADD_EXECUTABLE(tsp main.cpp)
TARGET_LINK_LIBRARIES(tsp avatar)   # 指定链接库
ADD_EXECUTABLE(test test.cpp)
TARGET_LINK_LIBRARIES(test avatar)   # 指定链接库
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
