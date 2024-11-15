cmake_minimum_required(VERSION 3.16)

project(client_lab3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
include_directories(${Qt${QT_VERSION_MAJOR}_INCLUDE_DIRS})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(client_lab3
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
    )
else()
    if(ANDROID)
        add_library(client_lab3 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(client_lab3
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(client_lab3 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Set properties for macOS/iOS or Windows executable types
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.client_lab3)
endif()
set_target_properties(client_lab3 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS client_lab3
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(client_lab3)
endif()

# Google Test setup using FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Define the test executable and link libraries
add_executable(client_lab3_tests
    mainwindow_tests.cpp  # Test file
    mainwindow.cpp  # Include mainwindow sources required for testing
)

target_link_libraries(client_lab3_tests PRIVATE
    gtest gtest_main
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Add the test to CMake
include(GoogleTest)
gtest_discover_tests(client_lab3_tests)
