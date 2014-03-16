AUX_SOURCE_DIRECTORY(.  EXCHANGE_SRCS)
ADD_EXECUTABLE(exchange_test ${EXCHANGE_SRCS})
# create a list of files to copy
SET(DENET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/test/exchange/randnet4.dot
   ${CMAKE_SOURCE_DIR}/test/exchange/randnet500.dot
   ${CMAKE_SOURCE_DIR}/test/exchange/randnet999.dot
)
# do the copying
FOREACH(file_i ${DENET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET exchange_test POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:exchange_test>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(exchange_test ${GAMCS_NAME})  
