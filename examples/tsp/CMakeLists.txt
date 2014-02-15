AUX_SOURCE_DIRECTORY(. TSP_SRCS)
ADD_EXECUTABLE(tsp main.cpp)
# create a list of files to copy
SET(PARALNET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/examples/tsp/randnet500.dot
   ${CMAKE_SOURCE_DIR}/examples/tsp/randnet100.dot
   ${CMAKE_SOURCE_DIR}/examples/tsp/randnet50.dot
   ${CMAKE_SOURCE_DIR}/examples/tsp/single.dot
)
# do the copying
FOREACH(file_i ${PARALNET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET tsp POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:tsp>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(tsp incarnation)  
#ADD_EXECUTABLE(test test.cpp)
#TARGET_LINK_LIBRARIES(test incarnation) 
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
