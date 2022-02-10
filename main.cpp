//#include <bits/stdc++.h>
#include "ScreenRecorder.h"

using namespace std;

/* driver function to run the application */
int main()
{
    ScreenRecorder screen_record;

    screen_record.openScreenAndMic();
    screen_record.setVideoAudioDecoders();

    screen_record.init_outputfile("C:/Users/Andre/OneDrive/Desktop/output.mp4");
    screen_record.setVideoAudioEncoders();


    screen_record.recordVideoAudio();
    cout<<"\nprogram executed successfully\n";

    return 0;
}
