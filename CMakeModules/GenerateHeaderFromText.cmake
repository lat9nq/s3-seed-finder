set(TEXT_FILE ${CMAKE_ARGV3})
set(RESULT_HEADER ${CMAKE_ARGV4})
set(VAR_NAME ${CMAKE_ARGV5})
set(HEADER_IN ${CMAKE_ARGV6})

file(READ ${TEXT_FILE} SOURCE_CONTENT)
string(REGEX REPLACE "\n *" "" SOURCE_CONTENT ${SOURCE_CONTENT})
string(REGEX REPLACE "\\\\" "\\\\\\\\" SOURCE_CONTENT ${SOURCE_CONTENT})

configure_file(${HEADER_IN} ${RESULT_HEADER} ESCAPE_QUOTES)
