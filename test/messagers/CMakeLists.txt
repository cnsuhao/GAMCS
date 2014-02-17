AUX_SOURCE_DIRECTORY(.  MESSAGERS_SRCS)
ADD_EXECUTABLE(messager_test ${MESSAGERS_SRCS})
# create a list of files to copy
SET(DENET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/test/messagers/randnet4.dot
   ${CMAKE_SOURCE_DIR}/test/messagers/randnet500.dot
   ${CMAKE_SOURCE_DIR}/test/messagers/randnet999.dot
)
# do the copying
FOREACH(file_i ${DENET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET messager_test POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:messager_test>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(messager_test isbl)  
