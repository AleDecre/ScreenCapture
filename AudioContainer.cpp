//
// Created by Andre on 11/02/2022.
//

#include "AudioContainer.h"
#include <iostream>

using namespace std;

static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

static int select_channel_layout(const AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}



AudioContainer::AudioContainer()
{

}

AudioContainer::~AudioContainer()
{

}



int AudioContainer::setAudioDecoder(AVFormatContext *pAVFormatContext) {
    int value = 0;
    audioStreamIndx = -1;

    for(int i = 0; i < pAVFormatContext->nb_streams; i++ ) // find audio stream posistion/index.
    {
        if( pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO ){
            audioStreamIndx = i;
        }
    }

    if( audioStreamIndx == -1){
        cout<<"\nunable to find the audio stream index. (-1)";
        exit(1);
    }

    audioDecoder = avcodec_find_decoder(pAVFormatContext->streams[audioStreamIndx]->codecpar->codec_id);
    if( audioDecoder == NULL ){
        cout<<"\nunable to find the decoder";
        exit(1);
    }

    audioDecoderContext = avcodec_alloc_context3(audioDecoder);
    if( !audioDecoderContext ){
        cout<<"\nerror in allocating the codec contexts audio";
        exit(1);
    }

    value = avcodec_parameters_to_context(audioDecoderContext , pAVFormatContext->streams[audioStreamIndx]->codecpar);
    if( value < 0 )
    {
        cout<<"\nunable to create avcodec context audio.";
        exit(value);
    }

    value = avcodec_open2(audioDecoderContext , audioDecoder , NULL);//Initialize the AVCodecContext to use the given AVCodec.
    if( value < 0 )
    {
        cout<<"\nunable to open the av codec audio";
        exit(1) ;
    }

    return 0;
}

int AudioContainer::setAudioEncoder(AVFormatContext *outAVFormatContext){
    int value = 0;
    audioEncoder = avcodec_find_encoder(outAVFormatContext->oformat->audio_codec);
    if( !audioEncoder ){
        cout<<"\nerror in finding the av codecs. try again with correct codec audio";
        exit(1);
    }

    audio_st = avformat_new_stream(outAVFormatContext , NULL);
    if( !audio_st ){
        cout<<"\nerror in creating a av format new stream (audio)";
        exit(1);
    }

    audioEncoderContext = avcodec_alloc_context3(audioEncoder);
    if( !audioEncoderContext ){
        cout<<"\nerror in allocating the codec contexts audio";
        exit(1);
    }


    audioEncoderContext->channels       = av_get_channel_layout_nb_channels(audioEncoderContext->channel_layout);
    audioEncoderContext->channel_layout = select_channel_layout(audioEncoder);
    audioEncoderContext->sample_rate = audioDecoderContext->sample_rate;//44100;
    //audioEncoderContext->sample_rate    = select_sample_rate(audioEncoder);
    //audioEncoderContext->sample_fmt   = AV_SAMPLE_FMT_FLTP;
    //audioEncoderContext->sample_fmt = AV_SAMPLE_FMT_FLTP; //audioEncoder->sample_fmts[0];
    audioEncoderContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    audioEncoderContext->bit_rate = 64000;

    audioEncoderContext->extradata = audioDecoderContext->extradata;
    audioEncoderContext->extradata_size = audioDecoderContext->extradata_size;

    audio_st->time_base.num = 1;
    audio_st->time_base.den = audioDecoderContext->sample_rate;


/*
    audioEncoderContext->sample_rate = audioDecoderContext->sample_rate;
    audioEncoderContext->codec_id = AV_CODEC_ID_AAC;
    audioEncoderContext->channels = audioDecoderContext->channels;
    audioEncoderContext->channel_layout = av_get_default_channel_layout(audioEncoderContext->channels);
    audioEncoderContext->bit_rate = audioDecoderContext->bit_rate;
    audioEncoderContext->time_base.num = 1;
    audioEncoderContext->time_base.den = audioDecoderContext->sample_rate;
    audioEncoderContext->sample_fmt = audioEncoder->sample_fmts? audioEncoder->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
    audioEncoderContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
*/

    if (!check_sample_fmt(audioEncoder, audioEncoderContext->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(audioEncoderContext->sample_fmt));
        exit(1);
    }

    value = avcodec_parameters_from_context(outAVFormatContext->streams[audioStreamIndx]->codecpar , audioEncoderContext);
    if( value < 0 ){
        cout<<"\nunable to modify format context stream audio.";
        exit(value);
    }



    value = avcodec_open2(audioEncoderContext, audioEncoder, NULL);
    if( value < 0){
        cout<<"\nerror in opening the avcodec audio";
        exit(1);
    }

    if ( outAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER){
        audioEncoderContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    return 0;
}



int AudioContainer::init_input_audio_frame()
{
    if (!(inAudioFrame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate input frame\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

int AudioContainer::init_output_audio_frame(){
    int value;
    outAudioFrame = av_frame_alloc();
    if( !outAudioFrame )
    {
        cout<<"\nunable to release the avframe resources for outframe";
        exit(1);
    }
    outAudioFrame->nb_samples     = audioEncoderContext->frame_size;
    outAudioFrame->channel_layout = audioEncoderContext->channel_layout;
    outAudioFrame->format         = audioEncoderContext->sample_fmt;
    outAudioFrame->sample_rate    = audioEncoderContext->sample_rate;

    value = av_frame_get_buffer(outAudioFrame, 0);
    if (value < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    //value = av_frame_make_writable(outFrameAudio);
    //if (value < 0)
    // exit(1);
    return 0;
}



int AudioContainer::init_fifo(){
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(fifo = av_audio_fifo_alloc(audioEncoderContext->sample_fmt,
                                     audioEncoderContext->channels, 1))) {
        fprintf(stderr, "Could not allocate FIFO\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}//TODO: Ricorda di deallocare la fifo e le altre cose

int AudioContainer::add_samples_to_fifo(uint8_t **converted_input_samples, const int frame_size){
    int error;

    /* Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples. */
    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
        fprintf(stderr, "Could not reallocate FIFO\n");
        return error;
    }

    /* Store the new samples in the FIFO buffer. */
    if ((error = av_audio_fifo_write(fifo, (void **)converted_input_samples, frame_size)) < frame_size) {
        fprintf(stderr, "Could not write data to FIFO\n");
        return AVERROR_EXIT;
    }
    return 0;
}

int AudioContainer::init_resampler(){
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
    if(audioEncoderContext->sample_rate != audioDecoderContext->sample_rate){
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

int AudioContainer::init_converted_samples(uint8_t ***converted_input_samples, int frame_size)
{
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

int AudioContainer::convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size){
    int error;
    /* Convert the samples using the resampler. */
    if ((error = swr_convert(resample_context,
                             converted_data, frame_size,
                             input_data    , frame_size)) < 0) {
        fprintf(stderr, "Could not convert input samples\n");
        return error;
    }
    return 0;
}




int AudioContainer::recordAudio(AVFormatContext *pAVFormatContext, AVFormatContext *outAVFormatContext){
    int value = 0;
    if (init_resampler()){
        cout<<"\nunable to init resampler";
        exit(1);
    }
    if(init_fifo()){
        cout<<"\nunable to init fifo";
        exit(1);
    }

    init_input_audio_frame();
    init_output_audio_frame();

    inPacketAudio = av_packet_alloc();
    if( !inPacketAudio ){
        cout<<"\nunable to allocate the avpacket";
        exit(1);
    }

    outPacketAudio = av_packet_alloc();
    if( !outPacketAudio ){
        cout<<"\nunable to allocate the avpacket";
        exit(1);
    }

    int ii = 0;
    int no_sec = 5;
    //int no_frames = no_sec * videoEncoderContext->time_base.den;//numero di frames = secondi * fps
    int no_frames = 100;
    int64_t pts = 0;

    while( av_read_frame( pAVFormatContext , inPacketAudio ) >= 0 )
    {
        if( ii++ == no_frames )break;
        cout << endl<< endl << ii << endl << endl;
        if(inPacketAudio->stream_index == audioStreamIndx ){
            cout << "audio packet" << endl;
            //---------------------------------------------------------------------------------------------------
            //DECODING
            value = avcodec_send_packet(audioDecoderContext, inPacketAudio);
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
            if (init_converted_samples(&converted_input_samples, inAudioFrame->nb_samples)){
                exit(1);
            }

            if (convert_samples((const uint8_t**)inAudioFrame->extended_data, converted_input_samples, inAudioFrame->nb_samples)){
                exit(1);
            }

            audioDecoderContext->frame_size;
            inAudioFrame->pkt_size;
            if (add_samples_to_fifo(converted_input_samples, inAudioFrame->nb_samples)){
                exit(1);
            }

            //Passa a fare encoding solo se ho abbastanza dati nella fifo per 1 frame di output
            if(av_audio_fifo_size(fifo) < audioEncoderContext->frame_size)
                continue;

            //---------------------------------------------------------------------------------------------------
            //ENCODING
            const int frame_size = FFMIN(av_audio_fifo_size(fifo),audioEncoderContext->frame_size);

            init_output_audio_frame();

            //cout << "dimensione fifo: " << av_audio_fifo_size(fifo) << endl;
            while(av_audio_fifo_size(fifo) > audioEncoderContext->frame_size) {
                if (av_audio_fifo_read(fifo, (void **) outAudioFrame->data, frame_size) < frame_size) {
                    fprintf(stderr, "Could not read data from FIFO\n");
                    //av_frame_free(&output_frame);
                    //return AVERROR_EXIT;
                }
                //cout << "dimensione fifo: " << av_audio_fifo_size(fifo) << endl;

                outAudioFrame->pts = pts;
                pts += outAudioFrame->nb_samples;
                cout << "audio frame pts: " << outAudioFrame->pts << endl;

                value = avcodec_send_frame(audioEncoderContext,
                                           outAudioFrame); //invia il frame da codificare all'encoder
                if (value == AVERROR(EAGAIN) || value == AVERROR_EOF || value == AVERROR(EINVAL)) {
                    return 1;//break;
                } else if (value < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                    return 1;//break;
                }

                AVPacket *outPacketAudio;
                outPacketAudio = av_packet_alloc();

                int error = avcodec_receive_packet(audioEncoderContext, outPacketAudio);
                //* If the encoder asks for more data to be able to provide an
                //* encoded frame, return indicating that no data is present.
                if (error == AVERROR(EAGAIN)) {
                    error = 0;
                    continue;
                    // If the last frame has been encoded, stop encoding.
                } else if (error == AVERROR_EOF) {
                    cout << " mette da parte e non scrivo ancora";
                    error = 0;
                    exit(1);
                } else if (error < 0) {
                    fprintf(stderr, "Could not encode frame (error)\n");
                    // Default case: Return encoded data.
                    exit(1);
                } else {
                    cout << "audio pts: " << outPacketAudio->pts << " audio dts: " << outPacketAudio->dts;
                    error = av_interleaved_write_frame(outAVFormatContext, outPacketAudio);
                    if (error < 0) {
                        cout << " noooo" << endl;
                    } else {
                        cout << " ok" << endl;
                    }
                }
                av_packet_unref(outPacketAudio);
            }
        }
    }// End of while-loop

    return 0;
}
