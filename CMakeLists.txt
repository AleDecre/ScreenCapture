cmake_minimum_required(VERSION 3.20)
project(ScreenCapture)

set(CMAKE_CXX_STANDARD 20)

#find_path(AVFIFO_INCLUDE_DIR libavutil/audio_fifo.h)
#find_library(AVFIFO_LIBRARY audio_fifo)

find_path(AVRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_library(AVRESAMPLE_LIBRARY swresample)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

#find_path(AVFILTER_INCLUDE_DIR libavfilter/avfiltergraph.h)
#find_library(AVFILTER_LIBRARY avfiltergraph)

find_path(AVSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(AVSCALE_LIBRARY swscale)

add_executable(ScreenCapture main.cpp ScreenRecorder.cpp ScreenRecorder.h AudioContainer.cpp AudioContainer.h AudioVideoRecorder.cpp AudioVideoRecorder.h)

target_include_directories(ScreenCapture PRIVATE  ${AVRESAMPLE_INCLUDE_DIR} ${AVSCALE_INCLUDE_DIR}  ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
target_link_libraries(ScreenCapture PRIVATE  ${AVRESAMPLE_LIBRARY} ${AVSCALE_LIBRARY}  ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY})