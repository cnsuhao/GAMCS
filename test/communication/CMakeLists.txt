AUX_SOURCE_DIRECTORY(. COMM_SRCS)
ADD_EXECUTABLE(comm_test ${COMM_SRCS})
ADD_CUSTOM_COMMAND(TARGET comm_test PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/test/communication/commnet.dot $<TARGET_FILE_DIR:comm_test>)     # copy topo file to bin directory
TARGET_LINK_LIBRARIES(comm_test avatar)
