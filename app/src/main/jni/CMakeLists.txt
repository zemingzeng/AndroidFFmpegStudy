cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
###########################################################FFAudioActivity#################################################################################
project("ffaudio")
include_directories(${CMAKE_SOURCE_DIR}/h/ffmpeg)
include_directories(${CMAKE_SOURCE_DIR}/h/self_jni)
include_directories(${CMAKE_SOURCE_DIR}/h/ffaudio)
include_directories(${CMAKE_SOURCE_DIR}/h/queue)
include_directories(${CMAKE_SOURCE_DIR}/h/test)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")
file(GLOB source_files ${CMAKE_SOURCE_DIR}/cpp/ffaudio/*.cpp)
add_library(
        ffaudio
        SHARED
        ${source_files}
)
find_library(
        log-lib
        log
)
target_link_libraries(
        ffaudio
        #FFmpeg libs
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
)

###########################################################################################################################################################################
###########################################################FFmpegActivity##################################################################################
#project("ffmpeg")
########################ffmpeg libs config##########################
##add h files
#include_directories(${CMAKE_SOURCE_DIR}/h/ffmpeg)
#include_directories(${CMAKE_SOURCE_DIR}/h/self_jni)
#include_directories(${CMAKE_SOURCE_DIR}/h/queue)
##add libs
##this way need config "sourceSets {
##main {
##jniLibs.srcDirs = ['src/main/jni/libs']
##}
##}" in app build.gradle
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")
#file(GLOB source_files ${CMAKE_SOURCE_DIR}/cpp/*.cpp)
########################ffmpeg libs config##########################
#add_library(
#        ffmpeg
#        SHARED
#        ${source_files}
#        )
#find_library(
#        log-lib
#        log
#)
#find_library(
#        lib-android
#        android
#)
#target_link_libraries(
#        ffmpeg
#        #FFmpeg libs
#        avcodec
#        avfilter
#        avformat
#        avutil
#        swresample
#        swscale
#        # Links the target library to the log library
#        # included in the NDK.
#        ${log-lib}
#        ${lib-android}
#)



message("------------------->${CMAKE_CXX_COMPILE_FEATURES}")
message("------------------->${CMAKE_CXX_STANDARD}")
