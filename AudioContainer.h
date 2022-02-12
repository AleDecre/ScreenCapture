//
// Created by Andre on 11/02/2022.
//

#ifndef DSHOWPROVA_AUDIOCONTAINER_H
#define DSHOWPROVA_AUDIOCONTAINER_H


extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
}

class AudioContainer {
    AVCodecContext *audioDecoderContext;
    AVCodecContext *audioEncoderContext;

    const AVCodec *audioDecoder;
    const AVCodec *audioEncoder;

    int audioStreamIndx;
    AVStream *audio_st;

    AVAudioFifo *fifo = NULL;

    AVFrame *inAudioFrame;
    AVFrame *outAudioFrame;
    AVPacket *inPacketAudio;
    AVPacket *outPacketAudio;

    AVDictionary *options;
    SwrContext *resample_context = NULL;
    int init_input_audio_frame();

    int init_output_audio_frame();

    int init_fifo();
    int add_samples_to_fifo(uint8_t **converted_input_samples, const int frame_size);
    int init_resampler();
    int init_converted_samples(uint8_t ***converted_input_samples, int frame_size);
    int convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size);


public:
    AudioContainer();
    ~AudioContainer();

    int setAudioDecoder(AVFormatContext *pAVFormatContext);
    int setAudioEncoder(AVFormatContext *outAVFormatContext);

    int recordAudio(AVFormatContext *pAVFormatContext, AVFormatContext *outAVFormatContext);
};


#endif //DSHOWPROVA_AUDIOCONTAINER_H
