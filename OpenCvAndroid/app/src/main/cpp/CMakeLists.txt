cmake_minimum_required(VERSION 3.10)

project(OpenCVAndroid)

set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../sdk/native/jni)

find_package(OpenCV REQUIRED)

add_library(
        native-lib
        SHARED
        native-lib.cpp
)

find_library(
        log-lib
        log
)

target_include_directories(
        native-lib PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
        native-lib
        ${OpenCV_LIBS}
        ${log-lib}
)