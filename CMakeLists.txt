cmake_minimum_required(VERSION 3.5)

set(PROJECT_VERSION 1.0.3)
project(CheckPatchPanelAndPorts VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_definitions(AUTHOR="Brayan M. Salazar")
add_compile_definitions(VERSION="${PROJECT_VERSION}")
add_compile_definitions(
    LICENSE="https://github.com/brookiestein/CheckPatchPanelsAndPorts/blob/main/LICENSE"
)
add_compile_definitions(QT_MAIN_PAGE="https://qt.io")
add_compile_definitions(LIBXLSXWRITER_REPO="https://github.com/jmcnamara/libxlsxwriter")
add_compile_definitions(
    LIBXLSXWRITER_LICENSE="https://github.com/jmcnamara/libxlsxwriter/blob/main/License.txt"
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Xlsxwriter REQUIRED IMPORTED_TARGET xlsxwriter)

set(PROJECT_SOURCES
    main.cpp
    about.hpp
    about.cpp
    mainwindow.hpp
    mainwindow.cpp
    mainwindow.ui
    patchpanels.hpp
    patchpanels.cpp
    patchpanels.ui
)

list(TRANSFORM PROJECT_SOURCES PREPEND "src/")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CheckPatchPanelAndPorts
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CheckPatchPanelAndPorts APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CheckPatchPanelAndPorts SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CheckPatchPanelAndPorts
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CheckPatchPanelAndPorts PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(CheckPatchPanelAndPorts PUBLIC PkgConfig::Xlsxwriter)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CheckPatchPanelAndPorts)
endif()
set_target_properties(CheckPatchPanelAndPorts PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CheckPatchPanelAndPorts
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES CheckPatchPanelAndPorts.desktop
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications
)

install(DIRECTORY resources
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/CheckPatchPanelsAndPorts
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CheckPatchPanelAndPorts)
endif()
