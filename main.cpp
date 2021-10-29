#include <iostream>
extern "C" {
#include "libavcodec//avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
}
#include "ScreenRecorder.h"

using namespace std;

int main() {
    ScreenRecorder screen_record;

    screen_record.openCamera();
    screen_record.init_outputfile();
    //screen_record.CaptureVideoFrames();
    return 0;
}
