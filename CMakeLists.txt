cmake_minimum_required(VERSION 3.19)
project(CameraSoftware)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH "A:/Qt/5.12.11/msvc2017/lib/cmake")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets Network)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

if (WIN32)
    add_compile_options(/MP)
    # ffmpeg
    include(3rdparty/ffmpeg/CMakeLists.txt)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

add_executable(${PROJECT_NAME} main.cpp app.rc src/static_resource.cpp src/static_resource.h src/camera_widget.cpp src/camera_widget.h src/camera.cpp src/camera.h src/camera_record.cpp src/camera_record.h window.cpp window.h src/keep_ratio_widget.cpp src/keep_ratio_widget.h src/ui/sidebar_widget.cpp src/ui/sidebar_widget.h src/ui/joystick_button.cpp src/ui/joystick_button.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
if (WIN32)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB ${REQUIRED_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)

    # ffmpeg depend
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/3rdparty/ffmpeg/ffmpeg-4.3-win32-shared/bin")
        file(GLOB FFMPEG_REQUIRED_LIBS ${CMAKE_CURRENT_LIST_DIR}/3rdparty/ffmpeg/ffmpeg-4.3-win32-shared/bin/*.dll)

        foreach (FFMPEG_LIB ${FFMPEG_REQUIRED_LIBS})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${FFMPEG_LIB}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
        endforeach (FFMPEG_LIB)
    else()
        message(FATAL_ERROR "not find ffmpeg pkg")
    endif ()
endif ()
