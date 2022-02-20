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







    screen_record.init_outputfile_AV("C:/Users/aless/Desktop/abc.mp4");

    screen_record.openScreen();
    screen_record.setVideoDecoder();
    screen_record.setVideoEncoder();


    screen_record.openMic();
    screen_record.setAudioDecoder();
    screen_record.setAudioEncoder();

    screen_record.create_outputfile();


    std::thread first (foo);     // spawn new thread that calls foo()
    std::thread second (bar);  // spawn new thread that calls bar()

    std::cout << "main, foo and bar now execute concurrently...\n";

    // synchronize threads:
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes

    screen_record.closeAudio();
    screen_record.closeVideo();

    sleep(5);

    screen_record.openScreen();
    screen_record.openMic();

    std::thread first1 (foo);     // spawn new thread that calls foo()
    std::thread second2 (bar);  // spawn new thread that calls bar()

    std::cout << "main, foo and bar now execute concurrently...\n";

    // synchronize threads:
    first1.join();                // pauses until first finishes
    second2.join();               // pauses until second finishes


    screen_record.close_outputfile();


    std::cout << "foo and bar completed.\n";

    return 0;
}