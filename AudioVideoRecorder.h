//
// Created by Andre on 11/02/2022.
//

#ifndef DSHOWPROVA_AUDIOVIDEORECORDER_H
#define DSHOWPROVA_AUDIOVIDEORECORDER_H

#include "AudioContainer.h"

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavdevice/avdevice.h>
}

class AudioVideoRecorder {
    const AVInputFormat *pAVInputFormat;//input di acquisizione
    AVFormatContext *pAVFormatContext;//context dell'input di acquisizione

    const AVOutputFormat *output_format;
    AVFormatContext *outAVFormatContext;//context del file finale

    char *output_file;

    AVDictionary *options;

    AudioContainer audioContainer;
    VideoContainer videoContainer;

public:
    AudioVideoRecorder();
    ~AudioVideoRecorder();

    int openScreenAndMic();
    int openScreen();
    int openMic();

    int init_outputfile(std::string output_file);//deve essere chiamata prima dei set encoders

    int setAudioDecoder();
    int setAudioEncoder();

    int setVideoDecoder();
    int setVideoEncoder();

    int createOutputFile();

    int recordAudio();
    int recordVideo();
    int recordAudioVideo();
};


#endif //DSHOWPROVA_AUDIOVIDEORECORDER_H
