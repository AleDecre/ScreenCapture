//
// Created by Andre on 25/10/2021.
//

#include "ScreenRecorder.h"

using namespace std;

ScreenRecorder::ScreenRecorder()
{

    //av_register_all();
    //avcodec_register_all();
    avdevice_register_all();
    cout<<"\nall required functions are registered successfully";
}

int ScreenRecorder::openCamera(){

    avdevice_register_all();

    value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();//Allocate an AVFormatContext

    pAVInputFormat = av_find_input_format("gdigrab");

    value = avformat_open_input(&pAVFormatContext, "desktop", pAVInputFormat, NULL);
    if(value != 0)
    {
        cout<<"\nerror in opening input device";
        exit(1);
    }
    value = av_dict_set(&options, "framrate","30",0);
    if(value < 0)
    {
        cout<<"\nerror in setting dictionary value";
        exit(1);
    }
    value = av_dict_set(&options, "preset","medium",0);
    if(value < 0)
    {
        cout<<"\nerror in setting preset values";
        exit(1);
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

    pAVCodec = avcodec_find_decoder(pAVFormatContext->streams[VideoStreamIndx]->codecpar->codec_id);
    if( pAVCodec == NULL )
    {
        cout<<"\nunable to find the decoder";
        exit(1);
    }

    pAVCodecContext = avcodec_alloc_context3(pAVCodec);
    value = avcodec_open2(pAVCodecContext , pAVCodec , NULL);//Initialize the AVCodecContext to use the given AVCodec.
    if( value < 0 )
    {
        cout<<"\nunable to open the av codec";
        exit(1);
    }
    return 0;
}

int ScreenRecorder::init_outputfile()
{
    int prova;
    outAVFormatContext = NULL;
    value = 0;
    output_file = "C:/Users/Andre/OneDrive/Desktop/output.mp4";

    avformat_alloc_output_context2(&outAVFormatContext, NULL, NULL, output_file);
    if (!outAVFormatContext)
    {
        cout<<"\nerror in allocating av format output context";
        exit(1);
    }

    //Returns the output format in the list of registered output formats which best matches the provided parameters, or returns NULL if there is no match.
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
/*
    outAVCodecContext = avcodec_alloc_context3(outAVCodec);
    if( !outAVCodecContext )
    {
        cout<<"\nerror in allocating the codec contexts";
        exit(1);
    }

    // set property of the video file
    outAVCodecContext = video_st->codec;
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

    // Some container formats (like MP4) require global headers to be present
    //  Mark the encoder so that it behaves accordingly.

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

    // create empty video file
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

    // imp: mp4 container or some advanced container file required header information
    value = avformat_write_header(outAVFormatContext , &options);
    if(value < 0)
    {
        cout<<"\nerror in writing the header context";
        exit(1);
    }
*/
    /*
    // uncomment here to view the complete video file informations
    cout<<"\n\nOutput file information :\n\n";
    av_dump_format(outAVFormatContext , 0 ,output_file ,1);
    */
    return 0;
}