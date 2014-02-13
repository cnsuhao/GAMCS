AUX_SOURCE_DIRECTORY(. PARA_SRCS)
ADD_EXECUTABLE(paral_test ${PARA_SRCS})
# create a list of files to copy
SET(PARALLELNET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/test/parallel/paralnet.dot
   ${CMAKE_SOURCE_DIR}/test/parallel/randparalnet500.dot
   ${CMAKE_SOURCE_DIR}/test/parallel/randparalnet999.dot
)
# do the copying
FOREACH(file_i ${PARALLELNET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET paral_test POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:paral_test>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(paral_test avatar)
