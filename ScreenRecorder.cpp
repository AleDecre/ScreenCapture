#include "ScreenRecorder.h"

using namespace std;



/* initialize the resources*/
ScreenRecorder::ScreenRecorder()
{
    avdevice_register_all();
    pAVCodecContext = new AVCodecContext;
    cout<<"\nall required functions are registered successfully";
}

/* uninitialize the resources */
ScreenRecorder::~ScreenRecorder()
{

    avformat_close_input(&pAVFormatContext);
    if( !pAVFormatContext )
    {
        cout<<"\nfile closed sucessfully";
    }
    else
    {
        cout<<"\nunable to close the file";
        exit(1);
    }

    avformat_free_context(pAVFormatContext);
    if( !pAVFormatContext )
    {
        cout<<"\navformat free successfully";
    }
    else
    {
        cout<<"\nunable to free avformat context";
        exit(1);
    }

}

/* establishing the connection between camera or screen through its respective folder */
int ScreenRecorder::openCamera()
{

    value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();//Allocate an AVFormatContext.
/*

X11 video input device.
To enable this input device during configuration you need libxcb installed on your system. It will be automatically detected during configuration.
This device allows one to capture a region of an X11 display.
refer : https://www.ffmpeg.org/ffmpeg-devices.html#x11grab
*/
    /* current below is for screen recording. to connect with camera use v4l2 as a input parameter for av_find_input_format */
    pAVInputFormat = av_find_input_format("gdigrab");
    value = avformat_open_input(&pAVFormatContext, "desktop", pAVInputFormat, NULL);
    if(value != 0)
    {
        cout<<"\nerror in opening input device";
        exit(value);
    }

    /* set frame per second */
    value = av_dict_set( &options,"framerate","30",0 );
    if(value < 0)
    {
        cout<<"\nerror in setting dictionary value";
        exit(value);
    }

    value = av_dict_set( &options, "preset", "medium", 0 );
    if(value < 0)
    {
        cout<<"\nerror in setting preset values";
        exit(value);
    }

    VideoStreamIndx = -1;

    /* find the first video stream index . Also there is an API available to do the below operations */
    for(int i = 0; i < pAVFormatContext->nb_streams; i++ ) // find video stream posistion/index.
    {
        if( pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO )
        {
            VideoStreamIndx = i;
            break;
        }

    }

    if( VideoStreamIndx == -1)
    {
        cout<<"\nunable to find the video stream index. (-1)";
        exit(1);
    }

    // assign pAVFormatContext to VideoStreamIndx

    value = avcodec_parameters_to_context(pAVCodecContext , pAVFormatContext->streams[VideoStreamIndx]->codecpar);

    if( value < 0 )
    {
        cout<<"\nunable to create avcodec context.";
        exit(value);
    }
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if( pAVCodec == NULL )
    {
        cout<<"\nunable to find the decoder";
        exit(1);
    }

    value = avcodec_open2(pAVCodecContext , pAVCodec , NULL);//Initialize the AVCodecContext to use the given AVCodec.
    if( value < 0 )
    {
        cout<<"\nunable to open the av codec";
        exit(1) ;
    }
    return 0;
}


int ScreenRecorder::init_outputfile()
{
    outAVFormatContext = NULL;
    value = 0;
    output_file = "C:/Users/aless/Desktop/output.mp4";

    avformat_alloc_output_context2(&outAVFormatContext, NULL, NULL, output_file);
    if (!outAVFormatContext)
    {
        cout<<"\nerror in allocating av format output context";
        exit(1);
    }

/* Returns the output format in the list of registered output formats which best matches the provided parameters, or returns NULL if there is no match. */
    output_format = av_guess_format(NULL, output_file ,NULL);
    if( !output_format )
    {
        cout<<"\nerror in guessing the video format. try with correct format";
        exit(1);
    }

    video_st = avformat_new_stream(outAVFormatContext ,NULL);
    if( !video_st )
    {
        cout<<"\nerror in creating a av format new stream";
        exit(1);
    }

    outAVCodecContext = avcodec_alloc_context3(outAVCodec);
    if( !outAVCodecContext )
    {
        cout<<"\nerror in allocating the codec contexts";
        exit(1);
    }

    value = avcodec_parameters_to_context(outAVCodecContext , video_st->codecpar);
    if( value < 0 )
    {
        cout<<"\nunable to create avcodec context.";
        exit(value);
    }

    /* set property of the video file */

    value = avcodec_parameters_to_context(outAVCodecContext , video_st->codecpar);
    if( value < 0 )
    {
        cout<<"\nunable to create avcodec context.";
        exit(value);
    }
    outAVCodecContext->codec_id = AV_CODEC_ID_MPEG4;// AV_CODEC_ID_MPEG4; // AV_CODEC_ID_H264 // AV_CODEC_ID_MPEG1VIDEO
    outAVCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    outAVCodecContext->pix_fmt  = AV_PIX_FMT_YUV420P;
    outAVCodecContext->bit_rate = 400000; // 2500000
    outAVCodecContext->width = 1920;
    outAVCodecContext->height = 1080;
    outAVCodecContext->gop_size = 3;
    outAVCodecContext->max_b_frames = 2;
    outAVCodecContext->time_base.num = 1;
    outAVCodecContext->time_base.den = 30; // 15fps

    if (codec_id == AV_CODEC_ID_H264)
    {
        av_opt_set(outAVCodecContext->priv_data, "preset", "slow", 0);
    }

    outAVCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    if( !outAVCodec )
    {
        cout<<"\nerror in finding the av codecs. try again with correct codec";
        exit(1);
    }

    /* Some container formats (like MP4) require global headers to be present
       Mark the encoder so that it behaves accordingly. */

    if ( outAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
    {
        outAVCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    value = avcodec_open2(outAVCodecContext, outAVCodec, NULL);
    if( value < 0)
    {
        cout<<"\nerror in opening the avcodec";
        exit(1);
    }

    /* create empty video file */
    if ( !(outAVFormatContext->flags & AVFMT_NOFILE) )
    {
        if( avio_open2(&outAVFormatContext->pb , output_file , AVIO_FLAG_WRITE ,NULL, NULL) < 0 )
        {
            cout<<"\nerror in creating the video file";
            exit(1);
        }
    }

    if(!outAVFormatContext->nb_streams)
    {
        cout<<"\noutput file dose not contain any stream";
        exit(1);
    }

    /* imp: mp4 container or some advanced container file required header information*/
    //value = avformat_write_header(outAVFormatContext , &options);
    //if(value < 0)
    //{
    //    cout<<"\nerror in writing the header context";
    //    exit(value);
    //}

    /*
    // uncomment here to view the complete video file informations
    cout<<"\n\nOutput file information :\n\n";
    av_dump_format(outAVFormatContext , 0 ,output_file ,1);
    */
    return 0;
}