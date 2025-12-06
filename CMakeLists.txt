cmake_minimum_required(VERSION 3.16)
project(PicMover LANGUAGES CXX)

set(CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)
include(${CMAKE_DIR}/version.cmake)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer Gui Widgets Concurrent)
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB SOURCES "${SRC_DIR}/*.cxx")
file(GLOB HEADERS "${SRC_DIR}/*.hxx")

qt_standard_project_setup()

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Network Qt6::HttpServer Qt6::Gui Qt6::Widgets Qt6::Concurrent)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${QT_BIN_DIR}/windeployqt.exe" --dir "${CMAKE_CURRENT_BINARY_DIR}/Release" $<TARGET_FILE:${PROJECT_NAME}>
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Release"
    DESTINATION .)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${GIT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Backup pictures from iPhone to Windows.")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

include(CPack)
