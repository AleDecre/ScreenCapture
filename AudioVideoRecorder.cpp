//
// Created by Andre on 11/02/2022.
//

#include <iostream>
#include "AudioVideoRecorder.h"

using namespace std;

AudioVideoRecorder::AudioVideoRecorder(){
    avdevice_register_all();
    //videoDecoderContext = new AVCodecContext;
    cout<<"\nall required functions are registered successfully";
}

AudioVideoRecorder::~AudioVideoRecorder(){

}

int AudioVideoRecorder::openMic(){
    int value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();

    pAVInputFormat = av_find_input_format("dshow");
//C:/Users/Andre/OneDrive/Desktop/q.m4a
//audio=Microphone Array (Tecnologia Intel® Smart Sound)
    value = avformat_open_input(&pAVFormatContext, "audio=Microphone Array (Tecnologia Intel® Smart Sound)", pAVInputFormat, NULL);
    if(value != 0){
        cout<<"\nerror in opening input device";
        exit(value);
    }

    value = avformat_find_stream_info(pAVFormatContext,NULL);
    if(value < 0){
        cout<<"\nunable to find the stream information";
        exit(1);
    }

    return 0;
}

int AudioVideoRecorder::openScreen(){
    int value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();

    value = av_dict_set( &options,"framerate","30",0 );
    if(value < 0){
        cout<<"\nerror in setting dictionary value";
        exit(1);
    }

    pAVInputFormat = av_find_input_format("dshow");
//C:/Users/Andre/OneDrive/Desktop/q.m4a
//audio=Microphone Array (Tecnologia Intel® Smart Sound)
    value = avformat_open_input(&pAVFormatContext, "video=screen-capture-recorder", pAVInputFormat, NULL);
    if(value != 0){
        cout<<"\nerror in opening input device";
        exit(value);
    }

    value = avformat_find_stream_info(pAVFormatContext,NULL);
    if(value < 0){
        cout<<"\nunable to find the stream information";
        exit(1);
    }

    return 0;
}

int AudioVideoRecorder::init_outputfile(std::string file){
    int value = 0;
    outAVFormatContext = NULL;
    output_file = file.data();//conversione string a char*
    //output_file = "C:/Users/Andre/OneDrive/Desktop/output.mp4";

    avformat_alloc_output_context2(&outAVFormatContext, NULL, NULL, output_file);
    if (!outAVFormatContext){
        cout<<"\nerror in allocating av format output context";
        exit(1);
    }

    return 0;
}

int AudioVideoRecorder::setAudioDecoder() {
    audioContainer.setAudioDecoder(pAVFormatContext);
    return 0;
}

int AudioVideoRecorder::setAudioEncoder() {
    audioContainer.setAudioEncoder(outAVFormatContext);
    createOutputFile();

    int value;
    value = avformat_write_header(outAVFormatContext , NULL);
    if(value < 0){
        cout<<"\nerror in writing the header context";
        exit(value);
    }
    if(outAVFormatContext->nb_streams != 1){
        cout<<"\noutput file dose not contain any stream, number is: "<< outAVFormatContext->nb_streams << endl; ;
        exit(1);
    }
    return 0;
}

int AudioVideoRecorder::recordAudio() {
    audioContainer.recordAudio(pAVFormatContext, outAVFormatContext);

    if( av_write_trailer(outAVFormatContext) < 0)
    {
        cout<<"\nerror in writing av trailer";
        exit(1);
    }
    return 0;
}

int AudioVideoRecorder::recordVideo() {
    videoContainer.recordVideo(pAVFormatContext, outAVFormatContext);

    if( av_write_trailer(outAVFormatContext) < 0)
    {
        cout<<"\nerror in writing av trailer";
        exit(1);
    }
    return 0;
}

int AudioVideoRecorder::setVideoDecoder(){
    videoContainer.setVideoDecoder(pAVFormatContext, options);
    return 0;
}

int AudioVideoRecorder::setVideoEncoder(){
    videoContainer.setVideoEncoder(outAVFormatContext, output_file);

    int value;
    value = avformat_write_header(outAVFormatContext , NULL);
    if(value < 0){
        cout<<"\nerror in writing the header context";
        exit(value);
    }
    if(outAVFormatContext->nb_streams != 1){
        cout<<"\noutput file dose not contain any stream, number is: "<< outAVFormatContext->nb_streams << endl; ;
        exit(1);
    }

    return 0;
}

int AudioVideoRecorder::createOutputFile(){
    /* create empty video file */
    if ( !(outAVFormatContext->flags & AVFMT_NOFILE) ){
        if( avio_open2(&outAVFormatContext->pb , output_file , AVIO_FLAG_WRITE ,NULL, NULL) < 0 ){
            cout<<"\nerror in creating the video file";
            exit(1);
        }
    }
    return 0;
}