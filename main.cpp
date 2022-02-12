#include <bits/stdc++.h>
#include "ScreenRecorder.h"
#include "AudioVideoRecorder.h"

using namespace std;

void soloVideo(){
    ScreenRecorder screen_record;
    screen_record.openScreenAndMic();
    screen_record.setVideoAudioDecoders();
    screen_record.init_outputfile_audio("C:/Users/Andre/OneDrive/Desktop/fanculo.m4a");
    screen_record.init_outputfile_video("C:/Users/Andre/OneDrive/Desktop/fanculo.mp4");
    screen_record.setVideoAudioEncoders();
    screen_record.recordVideoAudio();
}

void soloAudio(){
    AudioVideoRecorder recorder;
    recorder.openMic();
    recorder.init_outputfile("C:/Users/Andre/OneDrive/Desktop/output1.mp4");
    recorder.setAudioDecoder();
    recorder.setAudioEncoder();
    recorder.recordAudio();
}

int main()
{
    //soloVideo();
    soloAudio();

    return 0;
    /* CODICE ORIGINALE*/
    ScreenRecorder screen_record;
    screen_record.openScreenAndMic();
    screen_record.setAudioDecoder();
    screen_record.init_outputfile_audio("C:/Users/Andre/OneDrive/Desktop/output2.m4a");
    screen_record.setAudioEncoder();
    screen_record.recordVideoAudio();


    /* SOLO AUDIO FUNZIONANTE
    AudioVideoRecorder recorder;
    recorder.openMic();
    recorder.init_outputfile("C:/Users/Andre/OneDrive/Desktop/output1.mp4");
    recorder.setAudioDecoder();
    recorder.setAudioEncoder();
    recorder.recordAudio();*/


    /*SOLO VIDEO non FUNZIONANTE
    AudioVideoRecorder recorder;
    recorder.openScreen();
    recorder.setVideoDecoder();
    recorder.init_outputfile("C:/Users/Andre/OneDrive/Desktop/output.mp4");
    recorder.setVideoEncoder();
    recorder.recordVideo();*/

    cout<<"\nprogram executed successfully\n";

    return 0;
}

