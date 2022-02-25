#include <vector>
#include "ScreenRecorder.h"

using namespace std;

ScreenRecorder::ScreenRecorder(int audio) {
    avdevice_register_all();
    this->audio = audio;
    cout << "\nall required functions are registered successfully";
}

ScreenRecorder::~ScreenRecorder() {

    if(audio){
        avcodec_free_context(&audioDecoderContext);
        avcodec_free_context(&audioEncoderContext);
        av_frame_free(&inAudioFrame);
        av_frame_free(&outFrameAudio);
        av_packet_free(&inPacket_audio);
        av_packet_free(&outPacket_audio);
        swr_free(&resample_context);
        av_audio_fifo_free(fifo);
        closeAudio();
    }
    avformat_free_context(outAVFormatContext_video);
    avcodec_free_context(&videoDecoderContext);
    avcodec_free_context(&videoEncoderContext);
    av_frame_free(&inVideoFrame);
    av_frame_free(&outVideoFrame);
    av_packet_free(&inPacket_video);
    av_packet_free(&outPacket_video);
    sws_freeContext(swsCtx_);
    av_dict_free(&options);
    closeVideo();
}


//METODI PER ENCODER AUDIO

static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

static int select_channel_layout(const AVCodec *codec) {
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

//METODI PER ENCODER VIDEO

//CREAZIONE FILE

int ScreenRecorder::init_outputfile_AV(string file) {
    this->output_file_video = std::string(file);

    avformat_alloc_output_context2(&outAVFormatContext_video, NULL, NULL, output_file_video.c_str());
    if (!outAVFormatContext_video) {
        cout << "\nerror in allocating av format output context";
        return 1;
    }
    return 0;
}

int ScreenRecorder::create_outputfile() {

    int value;
    if (outAVFormatContext_video->oformat->flags & AVFMT_GLOBALHEADER) {
        videoEncoderContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    /* create empty video file */
    if (!(outAVFormatContext_video->flags & AVFMT_NOFILE)) {
        if (avio_open2(&outAVFormatContext_video->pb, output_file_video.c_str(), AVIO_FLAG_WRITE, NULL, NULL) < 0) {
            cout << "\nerror in creating the video file";
            return 1;
        }
    }

    value = avformat_write_header(outAVFormatContext_video, NULL);
    if (value < 0) {
        cout << "\nerror in writing the header context";
        return 1;
    }
    if(audio){
        if (outAVFormatContext_video->nb_streams != 2) {
            cout << "\noutput file dose not contain any stream, number is: " << outAVFormatContext_video->nb_streams
            << endl;;
            return -1;
        }
    }
    else{
        if (outAVFormatContext_video->nb_streams != 1) {
            cout << "\noutput file dose not contain any stream, number is: " << outAVFormatContext_video->nb_streams
                 << endl;;
            return -1;
        }
    }

    return 0;

}

int ScreenRecorder::close_outputfile() {
    if (av_write_trailer(outAVFormatContext_video) < 0) {
        cout << "\nerror in writing av trailer";
        return 1;
    }
    return 0;
}


//REGISTRATORE VOCALE
int ScreenRecorder::openMic() {
    int value = 0;
    options = NULL;
    pAVFormatContext_audio = NULL;

    pAVFormatContext_audio = avformat_alloc_context();

#if defined(_WIN32)
    pAVInputFormat_audio = av_find_input_format("dshow");

    value = avformat_open_input(&pAVFormatContext_audio, "audio=virtual-audio-capturer", pAVInputFormat_audio,
                                NULL);
#endif

#if defined(__unix__)
    pAVInputFormat_audio = av_find_input_format("alsa");
    //pAVInputFormat_audio = av_find_input_format("pulse");

    value = avformat_open_input(&pAVFormatContext_audio, "hw:1", pAVInputFormat_audio,NULL);
    //value = avformat_open_input(&pAVFormatContext_audio, "alsa_output.pci-0000_00_05.0.analog-stereo.monitor", pAVInputFormat_audio,NULL);
#endif

    if (value != 0) {
        cout << "\nerror in opening input device";
        return 1;
    }
    value = avformat_find_stream_info(pAVFormatContext_audio, NULL);
    if (value < 0) {
        cout << "\nunable to find the stream information";
        return -1;
    }

    return 0;
}

int ScreenRecorder::setAudioDecoder() {
    int value = 0;
    audioStreamIndx = -1;

    for (unsigned i = 0; i < pAVFormatContext_audio->nb_streams; i++) // find audio stream posistion/index.
    {
        if (pAVFormatContext_audio->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndx = i;
        }
    }

    if (audioStreamIndx == -1) {
        cout << "\nunable to find the audio stream index. (-1)";
        exit(1);
    }

    audioDecoder = avcodec_find_decoder(pAVFormatContext_audio->streams[audioStreamIndx]->codecpar->codec_id);
    if (audioDecoder == NULL) {
        cout << "\nunable to find the decoder";
        exit(1);
    }

    audioDecoderContext = avcodec_alloc_context3(audioDecoder);
    if (!audioDecoderContext) {
        cout << "\nerror in allocating the codec contexts audio";
        exit(1);
    }

    value = avcodec_parameters_to_context(audioDecoderContext,
                                          pAVFormatContext_audio->streams[audioStreamIndx]->codecpar);
    if (value < 0) {
        cout << "\nunable to create avcodec context audio.";
        exit(value);
    }

    value = avcodec_open2(audioDecoderContext, audioDecoder,
                          NULL);//Initialize the AVCodecContext to use the given AVCodec.
    if (value < 0) {
        cout << "\nunable to open the av codec audio";
        exit(1);
    }

    return 0;
}

int ScreenRecorder::setAudioEncoder() {
    int value = 0;
    audioEncoder = avcodec_find_encoder(outAVFormatContext_video->oformat->audio_codec);
    if (!audioEncoder) {
        cout << "\nerror in finding the av codecs. try again with correct codec audio";
        exit(1);
    }

    audio_st = avformat_new_stream(outAVFormatContext_video, NULL);
    if (!audio_st) {
        cout << "\nerror in creating a av format new stream (audio)";
        exit(1);
    }


    audioEncoderContext = avcodec_alloc_context3(audioEncoder);
    if (!audioEncoderContext) {
        cout << "\nerror in allocating the codec contexts audio";
        exit(1);
    }


    audioEncoderContext->channels = av_get_channel_layout_nb_channels(audioEncoderContext->channel_layout);
    audioEncoderContext->channel_layout = select_channel_layout(audioEncoder);
    audioEncoderContext->sample_rate = audioDecoderContext->sample_rate;
    audioEncoderContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    audioEncoderContext->bit_rate = 64000;
    audioEncoderContext->extradata = audioDecoderContext->extradata;
    audioEncoderContext->extradata_size = audioDecoderContext->extradata_size;
    audio_st->time_base.num = 1;
    audio_st->time_base.den = audioDecoderContext->sample_rate;


    if (!check_sample_fmt(audioEncoder, audioEncoderContext->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(audioEncoderContext->sample_fmt));
        exit(1);
    }
    value = avcodec_parameters_from_context(outAVFormatContext_video->streams[1]->codecpar, audioEncoderContext);
    if (value < 0) {
        cout << "\nunable to modify format context stream audio.";
        exit(value);
    }


    value = avcodec_open2(audioEncoderContext, audioEncoder, NULL);
    if (value < 0) {
        cout << "\nerror in opening the avcodec audio";
        exit(1);
    }

    return 0;
}

int ScreenRecorder::init_input_audio_frame() {
    if (!(inAudioFrame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate input frame\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

int ScreenRecorder::init_output_audio_frame() {
    int value;
    outFrameAudio = av_frame_alloc();
    if (!outFrameAudio) {
        cout << "\nunable to release the avframe resources for outframe";
        exit(1);
    }
    outFrameAudio->nb_samples = audioEncoderContext->frame_size;
    outFrameAudio->channel_layout = audioEncoderContext->channel_layout;
    outFrameAudio->format = audioEncoderContext->sample_fmt;
    outFrameAudio->sample_rate = audioEncoderContext->sample_rate;

    value = av_frame_get_buffer(outFrameAudio, 0);
    if (value < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    //value = av_frame_make_writable(outFrameAudio);
    //if (value < 0)
    // exit(1);
    return 0;
}

int ScreenRecorder::init_resampler() {
    int error;
    /*
     * Create a resampler context for the conversion.
     * Set the conversion parameters.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity (they are sometimes not detected
     * properly by the demuxer and/or decoder).
     */
    resample_context = swr_alloc_set_opts(NULL,
                                          av_get_default_channel_layout(audioEncoderContext->channels),
                                          audioEncoderContext->sample_fmt,
                                          audioEncoderContext->sample_rate,
                                          av_get_default_channel_layout(audioDecoderContext->channels),
                                          audioDecoderContext->sample_fmt,
                                          audioDecoderContext->sample_rate,
                                          0, NULL);
    if (!resample_context) {
        fprintf(stderr, "Could not allocate resample context\n");
        return AVERROR(ENOMEM);
    }
    /*
    * Perform a sanity check so that the number of converted samples is
    * not greater than the number of samples to be converted.
    * If the sample rates differ, this case has to be handled differently
    */
    if (audioEncoderContext->sample_rate != audioDecoderContext->sample_rate) {
        exit(1);
    }

    /* Open the resampler with the specified parameters. */
    if ((error = swr_init(resample_context)) < 0) {
        fprintf(stderr, "Could not open resample context\n");
        swr_free(&resample_context);
        return error;
    }
    return 0;
}

int ScreenRecorder::init_fifo() {
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(fifo = av_audio_fifo_alloc(audioEncoderContext->sample_fmt,
                                     audioEncoderContext->channels, 1))) {
        fprintf(stderr, "Could not allocate FIFO\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

int ScreenRecorder::init_converted_samples(uint8_t ***converted_input_samples, int frame_size) {
    int error;

    /* Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */
    if (!(*converted_input_samples = static_cast<uint8_t **>(calloc(audioEncoderContext->channels,
                                                                    sizeof(**converted_input_samples))))) {
        fprintf(stderr, "Could not allocate converted input sample pointers\n");
        return AVERROR(ENOMEM);
    }

    /* Allocate memory for the samples of all channels in one consecutive
     * block for convenience. */
    if ((error = av_samples_alloc(*converted_input_samples, NULL,
                                  audioEncoderContext->channels,
                                  frame_size,
                                  audioEncoderContext->sample_fmt, 0)) < 0) {
        fprintf(stderr,
                "Could not allocate converted input samples\n");
        av_freep(&(*converted_input_samples)[0]);
        free(*converted_input_samples);
        return error;
    }
    return 0;
}

int ScreenRecorder::convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size) {
    int error;
    /* Convert the samples using the resampler. */
    if ((error = swr_convert(resample_context,
                             converted_data, frame_size,
                             input_data, frame_size)) < 0) {
        fprintf(stderr, "Could not convert input samples\n");
        return error;
    }
    return 0;
}

int ScreenRecorder::add_samples_to_fifo(uint8_t **converted_input_samples, const int frame_size) {
    int error;

    /* Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples. */
    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
        fprintf(stderr, "Could not reallocate FIFO\n");
        return error;
    }

    /* Store the new samples in the FIFO buffer. */
    if ((av_audio_fifo_write(fifo, (void **) converted_input_samples, frame_size)) < frame_size) {
        fprintf(stderr, "Could not write data to FIFO\n");
        return AVERROR_EXIT;
    }
    return 0;
}

int ScreenRecorder::recordAudio() {
    int value = 0;
    if (init_resampler()) {
        cout << "\nunable to init resampler";
        exit(1);
    }
    if (init_fifo()) {
        cout << "\nunable to init fifo";
        exit(1);
    }

    init_input_audio_frame();
    init_output_audio_frame();

    inPacket_audio = av_packet_alloc();
    if (!inPacket_audio) {
        cout << "\nunable to allocate the avpacket";
        exit(1);
    }

    outPacket_audio = av_packet_alloc();
    if (!outPacket_audio) {
        cout << "\nunable to allocate the avpacket";
        exit(1);
    }

    while (av_read_frame(pAVFormatContext_audio, inPacket_audio) >= 0) {
        stop_m.lock();
        if (stop == 1) {
            stop_m.unlock();
            break;
        }
        stop_m.unlock();
        if (inPacket_audio->stream_index == audioStreamIndx) {
            //---------------------------------------------------------------------------------------------------
            //DECODING
            value = avcodec_send_packet(audioDecoderContext, inPacket_audio);
            if (value < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                continue;
            }

            value = avcodec_receive_frame(audioDecoderContext, inAudioFrame);
            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                break;
            } else if (value < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                break;
            }

            uint8_t **converted_input_samples = NULL;
            if (init_converted_samples(&converted_input_samples, inAudioFrame->nb_samples)) {
                exit(1);
            }

            if (convert_samples((const uint8_t **) inAudioFrame->extended_data, converted_input_samples,
                                inAudioFrame->nb_samples)) {
                exit(1);
            }

            if (add_samples_to_fifo(converted_input_samples, inAudioFrame->nb_samples)) {
                exit(1);
            }

            //Passa a fare encoding solo se ho abbastanza dati nella fifo per 1 frame di output
            if (av_audio_fifo_size(fifo) < audioEncoderContext->frame_size)
                continue;

            //---------------------------------------------------------------------------------------------------
            //ENCODING
            const int frame_size = FFMIN(av_audio_fifo_size(fifo), audioEncoderContext->frame_size);

            init_output_audio_frame();

            //cout << "dimensione fifo: " << av_audio_fifo_size(fifo) << endl;
            while (av_audio_fifo_size(fifo) > audioEncoderContext->frame_size) {
                if (av_audio_fifo_read(fifo, (void **) outFrameAudio->data, frame_size) < frame_size) {
                    fprintf(stderr, "Could not read data from FIFO\n");
                    //av_frame_free(&output_frame);
                    //return AVERROR_EXIT;
                }
                //cout << "dimensione fifo: " << av_audio_fifo_size(fifo) << endl;

                outFrameAudio->pts = pts;
                outFrameAudio->pkt_dts = pts;
                pts += outFrameAudio->nb_samples;


                value = avcodec_send_frame(audioEncoderContext,
                                           outFrameAudio); //invia il frame da codificare all'encoder
                if (value == AVERROR(EAGAIN) || value == AVERROR_EOF || value == AVERROR(EINVAL)) {
                    return 1;//break;
                } else if (value < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                    return 1;//break;
                }

                //AVPacket *outPacketAudio;
                //outPacketAudio = av_packet_alloc();

                int error = avcodec_receive_packet(audioEncoderContext, outPacket_audio);
                //* If the encoder asks for more data to be able to provide an
                //* encoded frame, return indicating that no data is present.
                if (error == AVERROR(EAGAIN)) {
                    error = 0;
                    continue;
                    // If the last frame has been encoded, stop encoding.
                } else if (error == AVERROR_EOF) {
                    cout << " mette da parte e non scrivo ancora";
                    exit(1);
                } else if (error < 0) {
                    fprintf(stderr, "Could not encode frame (error)\n");
                    // Default case: Return encoded data.
                    exit(1);
                } else {
                    outPacket_audio->stream_index = 1;
                    outPacket_audio->time_base = audio_st->time_base;

                    recording.lock();
                    value = av_interleaved_write_frame(outAVFormatContext_video,
                                                       outPacket_audio);//scrivi il pacchetto su file
                    recording.unlock();
                    if (value < 0) {
                        cout << " noooo" << endl;
                    } else {
                    }
                }
            }
            av_packet_unref(inPacket_audio);
            av_packet_unref(inPacket_audio);
        }
    }// End of while-loop


    return 0;
}

int ScreenRecorder::closeAudio() {
    avformat_close_input(&pAVFormatContext_audio);
    if (!pAVFormatContext_audio) {
        cout << "\nfile closed sucessfully";
    } else {
        cout << "\nunable to close the file";
        exit(1);
    }
    avformat_free_context(pAVFormatContext_audio);
    if (!pAVFormatContext_audio) {
        cout << "\navformat free successfully";
    } else {
        cout << "\nunable to free avformat context";
        exit(1);
    }
    return 0;
}


//REGISTRATORE VIDEO
int ScreenRecorder::openScreen() {
    int value = 0;
    options = NULL;
    pAVFormatContext_video = NULL;

    //av_dict_set(&options, "video_size", "1280x720", 0);

    pAVFormatContext_video = avformat_alloc_context();

    #if defined(_WIN32)
    pAVInputFormat_video = av_find_input_format("dshow");
    value = avformat_open_input(&pAVFormatContext_video, "video=screen-capture-recorder", pAVInputFormat_video,&options);
    #endif
    #if defined(__unix__)
    pAVInputFormat_video = av_find_input_format("x11grab");

    av_dict_set(&options, "grab_x", std::to_string(this->origin_x).c_str(), 0);
    av_dict_set(&options, "grab_y", std::to_string(this->origin_y).c_str(), 0);
    av_dict_set(&options, "video_size", this->videoSize.c_str(), 0);
    av_dict_set(&options, "framerate", "15", 0);

    value = avformat_open_input(&pAVFormatContext_video, "", pAVInputFormat_video,&options);

    #endif


    if (value != 0) {
        cout << "\nerror in opening input device";
        return 1;
    }

    value = avformat_find_stream_info(pAVFormatContext_video, NULL);
    if (value < 0) {
        cout << "\nunable to find the stream information";
        return -1;
    }

    return 0;
}

int ScreenRecorder::setVideoDecoder() {
    int value = 0;
    videoStreamIndx = -1;

    for (unsigned i = 0; i < pAVFormatContext_video->nb_streams; i++) // find video stream posistion/index.
    {
        if (pAVFormatContext_video->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndx = i;
        }
    }

    if (videoStreamIndx == -1) {
        cout << "\nunable to find the video stream index. (-1)";
        exit(1);
    }

    videoDecoder = avcodec_find_decoder(pAVFormatContext_video->streams[videoStreamIndx]->codecpar->codec_id);
    if (videoDecoder == NULL) {
        cout << "\nunable to find the decoder";
        exit(1);
    }

    videoDecoderContext = avcodec_alloc_context3(videoDecoder);
    if (!videoDecoderContext) {
        cout << "\nerror in allocating the codec contexts video";
        exit(1);
    }

    value = avcodec_parameters_to_context(videoDecoderContext,
                                          pAVFormatContext_video->streams[videoStreamIndx]->codecpar);
    if (value < 0) {
        cout << "\nunable to create avcodec context video.";
        exit(value);
    }


    value = avcodec_open2(videoDecoderContext, videoDecoder,
                          &options);//Initialize the AVCodecContext to use the given AVCodec.
    if (value < 0) {
        cout << "\nunable to open the av codec";
        exit(1);
    }

    return 0;
}

int ScreenRecorder::setVideoEncoder() {
    int value = 0;
    outAVFormatContext_video->video_codec_id = AV_CODEC_ID_MPEG4;
    videoEncoder = avcodec_find_encoder(outAVFormatContext_video->video_codec_id);
    if (!videoEncoder) {
        cout << "\nerror in finding the av codecs. try again with correct codec video";
        exit(1);
    }

    video_st = avformat_new_stream(outAVFormatContext_video, NULL);
    if (!video_st) {
        cout << "\nerror in creating a av format new stream (video)";
        exit(1);
    }

    videoEncoderContext = avcodec_alloc_context3(videoEncoder);
    if (!videoEncoderContext) {
        cout << "\nerror in allocating the codec contexts video";
        exit(1);
    }

    /* set property of the video file */
    videoEncoderContext->codec_id = AV_CODEC_ID_MPEG4;// AV_CODEC_ID_MPEG4; // AV_CODEC_ID_H264 // AV_CODEC_ID_MPEG1VIDEO
    videoEncoderContext->codec_type = AVMEDIA_TYPE_VIDEO;
    videoEncoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
    videoEncoderContext->bit_rate = 40000000; // 2500000
    videoEncoderContext->width = videoDecoderContext->width;
    videoEncoderContext->height = videoDecoderContext->height;
    videoEncoderContext->gop_size = 12;
    videoEncoderContext->max_b_frames = videoDecoderContext->max_b_frames;
    videoEncoderContext->time_base.num = 1;
    videoEncoderContext->time_base.den = 15; // 15fps

    value = avcodec_parameters_from_context(outAVFormatContext_video->streams[0]->codecpar, videoEncoderContext);
    if (value < 0) {
        cout << "\nunable to modify format context stream video.";
        exit(value);
    }

    value = avcodec_open2(videoEncoderContext, videoEncoder, NULL);
    if (value < 0) {
        cout << "\nerror in opening the avcodec video";
        exit(1);
    }



    /* Some container formats (like MP4) require global headers to be present
   Mark the encoder so that it behaves accordingly. */



    return 0;
}

int ScreenRecorder::init_input_video_frame() {
    inVideoFrame = av_frame_alloc();
    if (!inVideoFrame) {
        cout << "\nunable to release the avframe resources";
        exit(1);
    }
    return 0;
}

int ScreenRecorder::init_output_video_frame() {
    int value = 0;

    outVideoFrame = av_frame_alloc();
    if (!outVideoFrame) {
        cout << "\nunable to release the avframe resources for outframe";
        exit(1);
    }

    outVideoFrame->format = videoEncoderContext->pix_fmt;
    outVideoFrame->width = videoEncoderContext->width;
    outVideoFrame->height = videoEncoderContext->height;

    value = av_frame_get_buffer(outVideoFrame, 0);
    if (value < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    return 0;
}

int ScreenRecorder::recordVideo() {
    int value = 0;


    init_input_video_frame();
    init_output_video_frame();

    inPacket_video = av_packet_alloc();
    if (!inPacket_video) {
        cout << "\nunable to allocate the avpacket";
        exit(1);
    }

    outPacket_video = av_packet_alloc();
    if (!outPacket_video) {
        cout << "\nunable to allocate the avpacket";
        exit(1);
    }

    int nbytes = av_image_get_buffer_size(videoEncoderContext->pix_fmt, videoEncoderContext->width,
                                          videoEncoderContext->height, 1);
    uint8_t *video_outbuf = (uint8_t *) av_malloc(nbytes);
    if (video_outbuf == NULL) {
        cout << "\nunable to allocate memory";
        exit(1);
    }

    // Setup the data pointers and linesizes based on the specified image parameters and the provided array.
    value = av_image_fill_arrays(outVideoFrame->data, outVideoFrame->linesize, video_outbuf, AV_PIX_FMT_YUV420P,
                                 videoEncoderContext->width, videoEncoderContext->height,
                                 1); // returns : the size in bytes required for src
    if (value < 0) {
        cout << "\nerror in filling image array";
    }
    // Allocate and return swsContext.
    // a pointer to an allocated context, or NULL in case of error
    // Deprecated : Use sws_getCachedContext() instead.
    swsCtx_ = sws_getContext(videoDecoderContext->width,
                             videoDecoderContext->height,
                             videoDecoderContext->pix_fmt,
                             videoEncoderContext->width,
                             videoEncoderContext->height,
                             videoEncoderContext->pix_fmt,
                             SWS_BICUBIC, NULL, NULL, NULL);

    while (av_read_frame(pAVFormatContext_video, inPacket_video) >= 0) {
        stop_m.lock();
        if (stop == 1) {
            stop_m.unlock();
            break;
        }
        stop_m.unlock();

        if (inPacket_video->stream_index == videoStreamIndx) {
            value = avcodec_send_packet(videoDecoderContext, inPacket_video); //inviamo il pacchetto al decoder
            if (value < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                return 1;//break;
            }

            value = avcodec_receive_frame(videoDecoderContext,
                                          inVideoFrame); //riceviamo il frame decodificato dal decoder
            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                return 1;//break;
            } else if (value < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                return 1;//break;
            }


            sws_scale(swsCtx_, inVideoFrame->data, inVideoFrame->linesize, 0, videoDecoderContext->height,
                      outVideoFrame->data, outVideoFrame->linesize);
            outPacket_video->data = NULL;    // packet data will be allocated by the encoder
            outPacket_video->size = 0;


            //outVideoFrame = crop_frame(outVideoFrame, 100, 100, 300, 400);



            value = avcodec_send_frame(videoEncoderContext, outVideoFrame); //invia il frame da codificare all'encoder
            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF || value == AVERROR(EINVAL)) {
                return 1;//break;
            } else if (value < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                return 1;//break;
            }

            value = avcodec_receive_packet(videoEncoderContext,
                                           outPacket_video); //ricevi dall'encoder il pacchetto codificato
            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                return 2;//continue;
            } else if (value < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                return 2;//continue;
            }


            //dove ts viene scalato così:
            //ts += av_rescale_q( 1, outAVCodecContext->time_base, video_st->time_base);


            outPacket_video->pts = av_rescale_q(outPacket_video->pts, videoEncoderContext->time_base,
                                                video_st->time_base);
            outPacket_video->dts = av_rescale_q(outPacket_video->dts, videoEncoderContext->time_base,
                                                video_st->time_base);



            outPacket_video->time_base = video_st->time_base;

            recording.lock();
            value = av_interleaved_write_frame(outAVFormatContext_video, outPacket_video);//scrivi il pacchetto su file
            recording.unlock();

            if (value != 0) {
                cout << "error in writing video frame\n";
            }


            av_packet_unref(inPacket_video);
            av_packet_unref(outPacket_video);
        }
    }// End of while-loop

    return 0;
}

int ScreenRecorder::closeVideo() {
    avformat_close_input(&pAVFormatContext_video);
    if (!pAVFormatContext_video) {
        cout << "\nfile closed sucessfully";
    } else {
        cout << "\nunable to close the file";
        exit(1);
    }
    avformat_free_context(pAVFormatContext_video);
    if (!pAVFormatContext_video) {
        cout << "\navformat free successfully";
    } else {
        cout << "\nunable to free avformat context";
        exit(1);
    }
    return 0;
}
