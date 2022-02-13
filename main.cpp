#include "ScreenRecorder.h"

using namespace std;

void soloAudio_daFarFunzionare(){
    ScreenRecorder screen_record;
    screen_record.openScreenAndMic();
    screen_record.init_outputfile_audio("C:/Users/aless/Desktop/audio.mp4");
    screen_record.init_outputfile_video("C:/Users/aless/Desktop/video.mp4");
    screen_record.setVideoAudioDecoders();
    screen_record.setVideoAudioEncoders();
    screen_record.recordVideoAudio();
    cout<<"\nOOOKKKKK - program executed successfully\n";
}


int main()
{
    soloAudio_daFarFunzionare();
    return 0;

}

