#include <thread>
#include "ScreenRecorder.h"

using namespace std;

ScreenRecorder screen_record(0);


void foo()
{

    screen_record.recordVideo();

    cout<<"\nOOOKKKKK - program executed successfully\n";
}

void bar()
{


    screen_record.recordAudio();

    cout<<"\nOOOKKKKK - program executed successfully\n";
}


int main()
{



/*
    screen_record.openScreen();
    screen_record.init_outputfile_video("C:/Users/aless/Desktop/video.mp4");
    screen_record.setVideoDecoder();
    screen_record.setVideoEncoder();

    screen_record.openMic();
    screen_record.init_outputfile_audio("C:/Users/aless/Desktop/video.mp4");
    screen_record.setAudioDecoder();
    screen_record.setAudioEncoder();

    std::thread first (foo);     // spawn new thread that calls foo()
    std::thread second (bar);  // spawn new thread that calls bar(0)

    std::cout << "main, foo and bar now execute concurrently...\n";

    // synchronize threads:
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes
*/


    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    screen_record.openScreenMic();
    screen_record.init_outputfile_AV("C:/Users/aless/Desktop/video.mp4");
    screen_record.setVideoAudioDecoders();
    screen_record.setVideoAudioEncoders();
    screen_record.recordVideoAudio();




    std::cout << "foo and bar completed.\n";

    return 0;
}








