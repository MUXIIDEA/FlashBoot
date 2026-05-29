cmake_minimum_required(VERSION 3.20)

project(FlashBoot VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent LinguistTools Network)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MINGW)
    add_compile_options(-Wall -Wextra)
endif()

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.h
)

file(GLOB_RECURSE RESOURCES
    resources/*.qrc
)

set(TS_FILES
    resources/translations/flashboot_zh.ts
    resources/translations/flashboot_en.ts
)

qt_add_executable(FlashBoot
    ${SOURCES}
    ${RESOURCES}
)

qt_add_translations(FlashBoot
    TS_FILES ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

target_link_libraries(FlashBoot PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Network
)

if(WIN32)
    target_link_libraries(FlashBoot PRIVATE
        setupapi
        cfgmgr32
        rpcrt4
        ole32
        oleaut32
        advapi32
        shell32
        user32
        kernel32
        gdi32
        comdlg32
        comctl32
        winmm
        ws2_32
        version
        wintrust
        crypt32
        shlwapi
        imm32
        wininet
    )
endif()

target_include_directories(FlashBoot PRIVATE
    src
    src/ui
    src/core
    src/core/models
    src/hal
    src/utils
)

target_precompile_headers(FlashBoot PRIVATE
    src/pch.h
)

install(TARGETS FlashBoot
    RUNTIME DESTINATION bin
)

install(FILES ${QM_FILES}
    DESTINATION translations
)