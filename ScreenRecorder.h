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


class ScreenRecorder {
private:
public:

    int mux;
    const AVInputFormat *pAVInputFormat_audio;//input di acquisizione
    AVFormatContext *pAVFormatContext_audio;//context dell'input di acquisizione
    AVFormatContext *outAVFormatContext_audio;//context del file finale
    AVCodecContext *audioDecoderContext;
    AVCodecContext *audioEncoderContext;
    const AVCodec *audioDecoder;
    const AVCodec *audioEncoder;
    AVStream *audio_st;
    AVFrame *inAudioFrame;
    AVFrame *outFrameAudio;
    AVPacket *inPacket_audio;
    AVPacket *outPacket_audio;
    const char *output_file_audio;
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
    const char *output_file_video;
    int videoStreamIndx;
    SwsContext *swsCtx_;

    AVDictionary *options;

    ScreenRecorder(int i);

    ~ScreenRecorder();


    int openMic();

    int init_outputfile_audio(std::string output_file);//deve essere chiamata prima dei set encoders

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

    int stop = 0;


    /* function to initiate communication with display library */
    int openScreen();

    int init_outputfile_video(std::string output_file);//deve essere chiamata prima dei set encoders

    int setVideoDecoder();

    int setVideoEncoder();

    int init_input_video_frame();

    int init_output_video_frame();

    int recordVideo();



    int openScreenMic();

    int init_outputfile_AV(std::string output_file);//deve essere chiamata prima dei set encoders

    int setVideoAudioDecoders();

    int setVideoAudioEncoders();

    int recordVideoAudio();

};

#endif
