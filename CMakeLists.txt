cmake_minimum_required(VERSION 3.16)
project(HelloQt6 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

set(APP_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/updater/updatemanager.cpp
    src/updater/updatemanager.h
)

if(WIN32)
    list(APPEND APP_SOURCES src/app.rc)
endif()

qt_add_executable(HelloQt6 ${APP_SOURCES})

# Expose the project version to C++ as APP_VERSION ("1.0.0")
target_compile_definitions(HelloQt6 PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(HelloQt6 PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

if(WIN32)
    set_target_properties(HelloQt6 PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(WIN32)
    find_program(WINDEPLOYQT windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT)
        add_custom_command(TARGET HelloQt6 POST_BUILD
            COMMAND "${WINDEPLOYQT}"
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    "$<TARGET_FILE:HelloQt6>"
            COMMENT "Running windeployqt..."
        )
    endif()
endif()

install(TARGETS HelloQt6
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ── CPack ─────────────────────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME        "HelloQt6")
set(CPACK_PACKAGE_VERSION     "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR      "Maitretech Solution")
set(CPACK_PACKAGE_DESCRIPTION "Hello Qt6 Application")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME "HelloQt6-${PROJECT_VERSION}-windows-x64")
else()
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "HelloQt6-${PROJECT_VERSION}-linux-x64")
endif()

include(CPack)
