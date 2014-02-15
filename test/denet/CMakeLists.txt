AUX_SOURCE_DIRECTORY(. DENETTEST_SRCS)
ADD_EXECUTABLE(denet_test ${DENETTEST_SRCS})
# create a list of files to copy
SET(DENET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/test/denet/randnet4.dot
   ${CMAKE_SOURCE_DIR}/test/denet/randnet500.dot
   ${CMAKE_SOURCE_DIR}/test/denet/randnet999.dot
)
# do the copying
FOREACH(file_i ${DENET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET denet_test POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:denet_test>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(denet_test incarnation)
