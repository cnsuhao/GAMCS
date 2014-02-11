AUX_SOURCE_DIRECTORY(. COMM_SRCS)
ADD_EXECUTABLE(comm_test ${COMM_SRCS})
# create a list of files to copy
SET(COMMNET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/test/communication/commnet.dot
   ${CMAKE_SOURCE_DIR}/test/communication/randcommnet500.dot
   ${CMAKE_SOURCE_DIR}/test/communication/randcommnet999.dot
)
# do the copying
FOREACH(file_i ${COMMNET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET comm_test POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:comm_test>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(comm_test avatar)
