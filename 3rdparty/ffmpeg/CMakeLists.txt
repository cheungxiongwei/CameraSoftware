cmake_minimum_required(VERSION 3.19)

include_directories(${CMAKE_CURRENT_LIST_DIR}/ffmpeg-4.3-win32-dev/include)

link_directories(${CMAKE_CURRENT_LIST_DIR}/ffmpeg-4.3-win32-dev/lib)

link_libraries(
        avcodec.lib
        avdevice.lib
        avfilter.lib
        avformat.lib
        avutil.lib
        postproc.lib
        swresample.lib
        swscale.lib
)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()