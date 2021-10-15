cmake_minimum_required(VERSION 3.19)
project(ScreenCapture)
set(CMAKE_CXX_STANDARD 20)

add_executable(ScreenCapture main.cpp)

target_include_directories(ScreenCapture PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
target_link_libraries(ScreenCapture PRIVATE ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY})