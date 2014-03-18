AUX_SOURCE_DIRECTORY(.  EXCHANGE_SRCS)
ADD_EXECUTABLE(direct_exchange ${EXCHANGE_SRCS})
# create a list of files to copy
SET(EXNET_TOPO_FILES
   ${CMAKE_SOURCE_DIR}/examples/direct_exchange/network4.exnet
)
# do the copying
FOREACH(file_i ${EXNET_TOPO_FILES})
  ADD_CUSTOM_COMMAND(TARGET direct_exchange POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E copy
      ${file_i} $<TARGET_FILE_DIR:direct_exchange>)     # copy topo file to bin directory
ENDFOREACH(file_i)
TARGET_LINK_LIBRARIES(direct_exchange ${GAMCS_NAME})  
