if(NOT DEFINED GIT_VERSION)
    execute_process(
        COMMAND git describe --tags --always --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(NOT GIT_VERSION)
        set(GIT_VERSION "unknown")
    endif()
endif()

message("Version: ${GIT_VERSION}")

configure_file(
  ${CMAKE_DIR}/version.hxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/version.hxx
  @ONLY
)
