execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE TAG_RESULT
    OUTPUT_VARIABLE TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT TAG_RESULT EQUAL 0)
    set(TAG "no-tag")
endif()

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(GIT_VERSION "${TAG}-${BRANCH}-${HASH}")
message("Version: ${GIT_VERSION}")

configure_file(
  ${CMAKE_DIR}/version.hxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/version.hxx
  @ONLY
)
