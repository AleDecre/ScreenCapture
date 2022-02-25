#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <math.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <assert.h>


#define __STDC_CONSTANT_MACROS

//FFMPEG LIBRARIES
extern "C"
{
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>

#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"

#include "libavdevice/avdevice.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/opt.h"
#include "libavutil/common.h"
#include "libavutil/channel_layout.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/file.h"

// lib swresample

#include "libswscale/swscale.h"

}
using namespace std;

class ScreenRecorder {
private:
public:

    int audio;
    int64_t pts = 0;
    int stop = 0;
    mutex recording;
    mutex stop_m;

    int origin_x;
    int origin_y;
    std::string videoSize;

    const AVInputFormat *pAVInputFormat_audio;//input di acquisizione
    AVFormatContext *pAVFormatContext_audio;//context dell'input di acquisizione
    AVCodecContext *audioDecoderContext;
    AVCodecContext *audioEncoderContext;
    const AVCodec *audioDecoder;
    const AVCodec *audioEncoder;
    AVStream *audio_st;
    AVFrame *inAudioFrame;
    AVFrame *outFrameAudio;
    AVPacket *inPacket_audio;
    AVPacket *outPacket_audio;
    int audioStreamIndx;
    SwrContext *resample_context = NULL;
    AVAudioFifo *fifo = NULL;

    const AVInputFormat *pAVInputFormat_video;//input di acquisizione
    AVFormatContext *pAVFormatContext_video;//context dell'input di acquisizione
    AVFormatContext *outAVFormatContext_video;//context del file finale
    AVCodecContext *videoDecoderContext;
    AVCodecContext *videoEncoderContext;
    const AVCodec *videoDecoder;
    const AVCodec *videoEncoder;
    AVStream *video_st;
    AVFrame *inVideoFrame;
    AVFrame *outVideoFrame;
    AVPacket *inPacket_video;
    AVPacket *outPacket_video;
    std::string output_file_video;
    int videoStreamIndx;
    SwsContext *swsCtx_;

    AVDictionary *options;

    ScreenRecorder(int audio);

    ~ScreenRecorder();


    int init_outputfile_AV(std::string output_file);//deve essere chiamata prima dei set encoders

    int create_outputfile();

    int close_outputfile();

    int openMic();

    int setAudioDecoder();

    int setAudioEncoder();

    int init_input_audio_frame();

    int init_output_audio_frame();

    int init_resampler();

    int init_fifo();

    int init_converted_samples(uint8_t ***converted_input_samples, int frame_size);

    int convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size);

    int add_samples_to_fifo(uint8_t **converted_input_samples, const int frame_size);

    int recordAudio();

    int closeAudio();




    /* function to initiate communication with display library */
    int openScreen();

    int setVideoDecoder();

    int setVideoEncoder();

    int init_input_video_frame();

    int init_output_video_frame();

    int recordVideo();

    int closeVideo();


};

#endif
