#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <math.h>
#include <string.h>

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
//#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"

#include "libavformat/avformat.h"
#include "libavformat/avio.h"

// libav resample

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



class ScreenRecorder
{
private:
    const AVInputFormat *pAVInputFormat;//input di acquisizione
    AVFormatContext *pAVFormatContext;//context dell'input di acquisizione

    AVFormatContext *outAVFormatContext;//context del file finale

    const AVOutputFormat *output_format;

    AVCodecContext *videoDecoderContext;
    AVCodecContext *videoEncoderContext;
    AVCodecContext *audioDecoderContext;
    AVCodecContext *audioEncoderContext;

    const AVCodec *videoDecoder;
    const AVCodec *videoEncoder;
    const AVCodec *audioDecoder;
    const AVCodec *audioEncoder;

    AVStream *video_st;
    AVStream *audio_st;

    AVFrame *inAudioFrame;
    AVFrame *outAudioFrame;
    AVFrame *inFrame;
    AVFrame *outFrame;
    AVPacket *inPacket;
    AVPacket *outPacket;

    AVFrame *outFrameAudio;
    AVDictionary *options;
    AVOutputFormat *outAVOutputFormat;

    const char *output_file;

    int videoStreamIndx;
    int audioStreamIndx;

    SwsContext* swsCtx_;

    AVAudioFifo *fifo = NULL;
    SwrContext *resample_context = NULL;

    int64_t pts = 0;
    int init_fifo();
    int init_resampler();
    int convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size);
    int init_input_video_frame();
    int init_output_video_frame();
    int init_input_audio_frame();
    int init_output_audio_frame();
    int decode_encode_write_video(int j);
    int add_samples_to_fifo(uint8_t **converted_input_samples, const int frame_size);
    int init_converted_samples(uint8_t ***converted_input_samples, int frame_size);

public:

    ScreenRecorder();
    ~ScreenRecorder();

    /* function to initiate communication with display library */
    int openScreenAndMic();
    int openScreen();

    int init_outputfile(std::string output_file);//deve essere chiamata prima dei set encoders

    int setVideoDecoder();
    int setVideoEncoder();

    int setAudioDecoder();
    int setAudioEncoder();

    int setVideoAudioEncoders();
    int setVideoAudioDecoders();

    int recordVideo();
    int recordVideoAudio();
};

#endif
