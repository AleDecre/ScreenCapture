#include <thread>
#include <windows.h>
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

void click(){

    char letter;
    cin>>letter;
    if(letter == 's'){
        screen_record.stop_m.lock();
        screen_record.stop = 1;
        screen_record.stop_m.unlock();
    }

}


int main()
{

    HKEY hk;
    long n = RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Software\\screen-capture-recorder"), 0, KEY_SET_VALUE , &hk );

    DWORD height = 800;
    n = RegSetValueEx(hk, TEXT("capture_height"), 0, REG_DWORD, (LPBYTE)&height, sizeof(height));
    DWORD width = 500;
    n = RegSetValueEx(hk, TEXT("capture_width"), 0, REG_DWORD, (LPBYTE)&width, sizeof(width));
    DWORD start_x = 200;
    n = RegSetValueEx(hk, TEXT("start_x"), 0, REG_DWORD, (LPBYTE)&start_x, sizeof(start_x));
    DWORD start_y = 300;
    n = RegSetValueEx(hk, TEXT("start_y"), 0, REG_DWORD, (LPBYTE)&start_y, sizeof(start_y));

    RegCloseKey(hk);

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
    std::thread check_letter(click);

    std::cout << "main, foo and bar now execute concurrently...\n";

    // synchronize threads:
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes
    check_letter.join();         // pauses until second finishes




    screen_record.stop_m.lock();
    screen_record.stop = 0;
    screen_record.stop_m.unlock();


    screen_record.closeAudio();
    screen_record.closeVideo();
/*
    sleep(5);

    screen_record.openScreen();
    screen_record.openMic();

    std::thread check_letter2(click);
    std::thread first1 (foo);     // spawn new thread that calls foo()
    std::thread second2 (bar);  // spawn new thread that calls bar()


    std::cout << "main, foo and bar now execute concurrently...\n";

    // synchronize threads:
    first1.join();                // pauses until first finishes
    second2.join();               // pauses until second finishes
    check_letter2.join();
*/
    screen_record.close_outputfile();


    std::cout << "foo and bar completed.\n";

    return 0;
}