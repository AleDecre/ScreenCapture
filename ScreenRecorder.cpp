#include <vector>
#include "ScreenRecorder.h"

using namespace std;

ScreenRecorder::ScreenRecorder()
{
    avdevice_register_all();
    videoDecoderContext = new AVCodecContext;
    cout<<"\nall required functions are registered successfully";
}

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



static int select_sample_rate(const AVCodec *codec)
{
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    return best_samplerate;
}

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



int ScreenRecorder::openScreenAndMic() {
    int value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();

    pAVInputFormat = av_find_input_format("dshow");

    value = av_dict_set( &options,"framerate","30",0 );
    if(value < 0){
        cout<<"\nerror in setting dictionary value";
        exit(1);
    }
    //value = avformat_open_input(&pAVFormatContext, "video=screen-capture-recorder:audio=Microphone Array (Tecnologia Intel® Smart Sound)", pAVInputFormat, NULL);

    value = avformat_open_input(&pAVFormatContext, "video=screen-capture-recorder:audio=virtual-audio-capturer", pAVInputFormat, NULL);
    if(value != 0){
        cout<<"\nerror in opening input device";
        exit(value);
    }

    value = avformat_find_stream_info(pAVFormatContext,NULL);
    if(value < 0){
        cout<<"\nunable to find the stream information";
        exit(1);
    }


    return 0;
}

int ScreenRecorder::openScreen(){
    int value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();

    pAVInputFormat = av_find_input_format("dshow");

    value = av_dict_set( &options,"framerate","30",0 );
    if(value < 0){
        cout<<"\nerror in setting dictionary value";
        exit(1);
    }

    value = avformat_open_input(&pAVFormatContext, "video=screen-capture-recorder", pAVInputFormat, NULL);
    if(value != 0){
        cout<<"\nerror in opening input device";
        exit(value);
    }

    value = avformat_find_stream_info(pAVFormatContext,NULL);
    if(value < 0){
        cout<<"\nunable to find the stream information";
        exit(1);
    }

    return 0;
}

int ScreenRecorder::openMic(){
    int value = 0;
    options = NULL;
    pAVFormatContext = NULL;

    pAVFormatContext = avformat_alloc_context();

    pAVInputFormat = av_find_input_format("dshow");
//C:/Users/Andre/OneDrive/Desktop/q.m4a
//audio=Microphone Array (Tecnologia Intel® Smart Sound)
    value = avformat_open_input(&pAVFormatContext, "audio=virtual-audio-capturer", pAVInputFormat, NULL);
    if(value != 0){
        cout<<"\nerror in opening input device";
        exit(value);
    }

    value = avformat_find_stream_info(pAVFormatContext,NULL);
    if(value < 0){
        cout<<"\nunable to find the stream information";
        exit(1);
    }

    return 0;
}



int ScreenRecorder::setAudioDecoder() {
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

int ScreenRecorder::setVideoDecoder(){
    int value = 0;
    videoStreamIndx = -1;

    for(int i = 0; i < pAVFormatContext->nb_streams; i++ ) // find video stream posistion/index.
    {
        if( pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ){
            videoStreamIndx = i;
        }
    }

    if( videoStreamIndx == -1){
        cout<<"\nunable to find the video stream index. (-1)";
        exit(1);
    }

    videoDecoder = avcodec_find_decoder(pAVFormatContext->streams[videoStreamIndx]->codecpar->codec_id);
    if( videoDecoder == NULL ){
        cout<<"\nunable to find the decoder";
        exit(1);
    }

    videoDecoderContext = avcodec_alloc_context3(videoDecoder);
    if( !videoDecoderContext ){
        cout<<"\nerror in allocating the codec contexts video";
        exit(1);
    }

    value = avcodec_parameters_to_context(videoDecoderContext , pAVFormatContext->streams[videoStreamIndx]->codecpar);
    if( value < 0 )
    {
        cout<<"\nunable to create avcodec context video.";
        exit(value);
    }


    value = avcodec_open2(videoDecoderContext , videoDecoder , &options);//Initialize the AVCodecContext to use the given AVCodec.
    if( value < 0 )
    {
        cout<<"\nunable to open the av codec";
        exit(1) ;
    }

    return 0;
}

int ScreenRecorder::setVideoAudioDecoders(){
    setVideoDecoder();
    setAudioDecoder();
    return 0;
}



int ScreenRecorder::init_outputfile_audio(string file){
    int value = 0;
    outAVFormatContext_audio = NULL;

    output_file_audio = "C:/Users/aless/Desktop/audio.mp4";

    avformat_alloc_output_context2(&outAVFormatContext_audio, NULL, NULL, output_file_audio);
    if (!outAVFormatContext_audio){
        cout<<"\nerror in allocating av format output context";
        exit(1);
    }

    return 0;
}
int ScreenRecorder::init_outputfile_video(string file){
    int value = 0;
    outAVFormatContext_video = NULL;
    output_file_video = file.data();//conversione string a char*

    output_file_video = "C:/Users/aless/Desktop/video.mp4";

    avformat_alloc_output_context2(&outAVFormatContext_video, NULL, NULL, output_file_video);
    if (!outAVFormatContext_video){
        cout<<"\nerror in allocating av format output context";
        exit(1);
    }

    return 0;
}


int ScreenRecorder::setAudioEncoder(){
    int value = 0;
    audioEncoder = avcodec_find_encoder(outAVFormatContext_audio->oformat->audio_codec);
    if( !audioEncoder ){
        cout<<"\nerror in finding the av codecs. try again with correct codec audio";
        exit(1);
    }

    audio_st = avformat_new_stream(outAVFormatContext_audio , NULL);
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
    audioEncoderContext->sample_rate = audioDecoderContext->sample_rate;
    //audioEncoderContext->sample_rate    = select_sample_rate(audioEncoder);
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

    value = avcodec_parameters_from_context(outAVFormatContext_audio->streams[0]->codecpar , audioEncoderContext);
    if( value < 0 ){
        cout<<"\nunable to modify format context stream audio.";
        exit(value);
    }



    value = avcodec_open2(audioEncoderContext, audioEncoder, NULL);
    if( value < 0){
        cout<<"\nerror in opening the avcodec audio";
        exit(1);
    }

    if ( outAVFormatContext_audio->oformat->flags & AVFMT_GLOBALHEADER){
        videoEncoderContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    /* create empty video file */
    if ( !(outAVFormatContext_audio->flags & AVFMT_NOFILE) ){
        if( avio_open2(&outAVFormatContext_audio->pb , output_file_audio , AVIO_FLAG_WRITE ,NULL, NULL) < 0 ){
            cout<<"\nerror in creating the video file";
            exit(1);
        }
    }

    value = avformat_write_header(outAVFormatContext_audio , NULL);
    if(value < 0){
        cout<<"\nerror in writing the header context";
        exit(value);
    }
    if(outAVFormatContext_audio->nb_streams != 1){
        cout<<"\noutput file dose not contain any stream, number is: "<< outAVFormatContext_audio->nb_streams << endl; ;
        exit(1);
    }

    return 0;
}

int ScreenRecorder::setVideoEncoder(){
    int value = 0;
    videoEncoder = avcodec_find_encoder(outAVFormatContext_video->oformat->video_codec);
    if( !videoEncoder ){
        cout<<"\nerror in finding the av codecs. try again with correct codec video";
        exit(1);
    }

    video_st = avformat_new_stream(outAVFormatContext_video , NULL);
    if( !video_st ){
        cout<<"\nerror in creating a av format new stream (video)";
        exit(1);
    }

    videoEncoderContext = avcodec_alloc_context3(videoEncoder);
    if( !videoEncoderContext ){
        cout<<"\nerror in allocating the codec contexts video";
        exit(1);
    }

    /* set property of the video file */
    videoEncoderContext->codec_id = AV_CODEC_ID_MPEG4;// AV_CODEC_ID_MPEG4; // AV_CODEC_ID_H264 // AV_CODEC_ID_MPEG1VIDEO
    videoEncoderContext->codec_type = AVMEDIA_TYPE_VIDEO;
    videoEncoderContext->pix_fmt  = AV_PIX_FMT_YUV420P;
    videoEncoderContext->bit_rate = 40000000; // 2500000
    videoEncoderContext->width = videoDecoderContext->width;
    videoEncoderContext->height = videoDecoderContext->height;
    videoEncoderContext->gop_size = videoDecoderContext->gop_size;
    videoEncoderContext->max_b_frames = videoDecoderContext->max_b_frames;
    videoEncoderContext->time_base.num = 1;
    videoEncoderContext->time_base.den = 30 ; // 15fps
    videoDecoderContext->time_base.den;
    videoEncoderContext->sample_rate = videoDecoderContext->sample_rate;

    value = avcodec_parameters_from_context(outAVFormatContext_video->streams[0]->codecpar , videoEncoderContext);
    if( value < 0 ){
        cout<<"\nunable to modify format context stream video.";
        exit(value);
    }

    value = avcodec_open2(videoEncoderContext, videoEncoder, NULL);
    if( value < 0){
        cout<<"\nerror in opening the avcodec video";
        exit(1);
    }

    /* Some container formats (like MP4) require global headers to be present
   Mark the encoder so that it behaves accordingly. */
    if ( outAVFormatContext_video->oformat->flags & AVFMT_GLOBALHEADER){
        videoEncoderContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    /* create empty video file */
    if ( !(outAVFormatContext_video->flags & AVFMT_NOFILE) ){
        if( avio_open2(&outAVFormatContext_video->pb , output_file_video , AVIO_FLAG_WRITE ,NULL, NULL) < 0 ){
            cout<<"\nerror in creating the video file";
            exit(1);
        }
    }

    value = avformat_write_header(outAVFormatContext_video , NULL);
    if(value < 0){
        cout<<"\nerror in writing the header context";
        exit(value);
    }
    if(outAVFormatContext_video->nb_streams != 1){
        cout<<"\noutput file dose not contain any stream, number is: "<< outAVFormatContext_video->nb_streams << endl; ;
        exit(1);
    }


    return 0;
}

int ScreenRecorder::setVideoAudioEncoders(){
    setVideoEncoder();
    setAudioEncoder();

    return 0;
}


int ScreenRecorder::init_input_video_frame(){
    inFrame = av_frame_alloc();
    if( !inFrame ){
        cout<<"\nunable to release the avframe resources";
        exit(1);
    }
    return 0;
}

int ScreenRecorder::init_output_video_frame(){
    int value = 0;

    outFrame = av_frame_alloc();
    if( !outFrame ){
        cout<<"\nunable to release the avframe resources for outframe";
        exit(1);
    }

    outFrame->format = videoEncoderContext->pix_fmt;
    outFrame->width = videoEncoderContext->width;
    outFrame->height = videoEncoderContext->height;

    value = av_frame_get_buffer(outFrame, 0);
    if (value < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    return 0;
}

int ScreenRecorder::init_input_audio_frame()
{
    if (!(inAudioFrame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate input frame\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

int ScreenRecorder::init_output_audio_frame(){
    int value;
    outFrameAudio = av_frame_alloc();
    if( !outFrameAudio )
    {
        cout<<"\nunable to release the avframe resources for outframe";
        exit(1);
    }
    outFrameAudio->nb_samples     = audioEncoderContext->frame_size;
    outFrameAudio->channel_layout = audioEncoderContext->channel_layout;
    outFrameAudio->format         = audioEncoderContext->sample_fmt;
    outFrameAudio->sample_rate    = audioEncoderContext->sample_rate;

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


int ScreenRecorder::init_resampler(){
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

int ScreenRecorder::init_fifo(){
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(fifo = av_audio_fifo_alloc(audioEncoderContext->sample_fmt,
                                     audioEncoderContext->channels, 1))) {
        fprintf(stderr, "Could not allocate FIFO\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}//TODO: Ricorda di deallocare la fifo e le altre cose

int ScreenRecorder::init_converted_samples(uint8_t ***converted_input_samples, int frame_size)
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

int ScreenRecorder::convert_samples(const uint8_t **input_data, uint8_t **converted_data, const int frame_size){
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

int ScreenRecorder::add_samples_to_fifo(uint8_t **converted_input_samples, const int frame_size){
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

int ScreenRecorder::decode_encode_write_video(int j){
    int value;

    value = avcodec_send_packet(videoDecoderContext, inPacket); //inviamo il pacchetto al decoder
    if (value < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
        return 1;//break;
    }

    value = avcodec_receive_frame(videoDecoderContext, inFrame); //riceviamo il frame decodificato dal decoder
    if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
        return 1;//break;
    } else if (value < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
        return 1;//break;
    }

    //lista a puntatori di pacchetti decodificati.............
    //lista a puntattoi di audio decodificati....
    //audio e video con pts ok in una struttra dati.....
    //1 processo salva tutti i frame video in un video
    //1 provesso salva tutti i frame audio in un audio
    //processo padre muxa il tutto
    //elimina vidoe e audio iniziali e salva solo muxato

    sws_scale(swsCtx_, inFrame->data, inFrame->linesize,0, videoDecoderContext->height, outFrame->data,outFrame->linesize);
    outPacket->data = NULL;    // packet data will be allocated by the encoder
    outPacket->size = 0;

    value = avcodec_send_frame(videoEncoderContext, outFrame); //invia il frame da codificare all'encoder
    if (value == AVERROR(EAGAIN) || value == AVERROR_EOF || value == AVERROR(EINVAL)) {
        return 1;//break;
    } else if (value < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
        return 1;//break;
    }

    value = avcodec_receive_packet(videoEncoderContext, outPacket); //ricevi dall'encoder il pacchetto codificato
    if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
        int x=0;
        return 2;//continue;
    } else if (value < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
        return 2;//continue;
    }


    //dove ts viene scalato così:
    //ts += av_rescale_q( 1, outAVCodecContext->time_base, video_st->time_base);


    outPacket->pts = av_rescale_q(outPacket->pts, videoEncoderContext->time_base, video_st->time_base);
    outPacket->dts = av_rescale_q(outPacket->dts, videoEncoderContext->time_base, video_st->time_base);

    cout<< "video pts: " << outPacket->pts;
    cout<< "\nvideo dts: " << outPacket->dts;
    printf("\nWrite frame video %3d %3d (size= %2d)\n", j++, outPacket->size/1000);


    if(av_interleaved_write_frame(outAVFormatContext_video , outPacket) != 0){//scrivi il pacchetto su file
        cout<<"error in writing video frame\n";
    }else{
        cout<<"ok write video" << endl;
    }


    av_packet_unref(inPacket);
    av_packet_unref(outPacket);
    return 0;
}

/* function to capture and store data in frames by allocating required memory and auto deallocating the memory.   */
int ScreenRecorder::recordVideoAudio()
{
    int value = 0;
    if (init_resampler()){
        cout<<"\nunable to init resampler";
        exit(1);
    }
    if(init_fifo()){
        cout<<"\nunable to init fifo";
        exit(1);
    }

    init_input_video_frame();//probabilmente poi bisogna allocare e deallocare per ogni lettura/scrittura, per evitare memory leak
    init_output_video_frame();

    init_input_audio_frame();
    //init_output_audio_frame();

    inPacket = av_packet_alloc();
    if( !inPacket ){
        cout<<"\nunable to allocate the avpacket";
        exit(1);
    }
    outPacket = av_packet_alloc();
    if( !outPacket ){
        cout<<"\nunable to allocate the avpacket";
        exit(1);
    }

    int nbytes = av_image_get_buffer_size(videoEncoderContext->pix_fmt,videoEncoderContext->width,videoEncoderContext->height,1);
    uint8_t *video_outbuf = (uint8_t*)av_malloc(nbytes);
    if( video_outbuf == NULL ){
        cout<<"\nunable to allocate memory";
        exit(1);
    }

    // Setup the data pointers and linesizes based on the specified image parameters and the provided array.
    value = av_image_fill_arrays( outFrame->data, outFrame->linesize, video_outbuf , AV_PIX_FMT_YUV420P, videoEncoderContext->width,videoEncoderContext->height,1 ); // returns : the size in bytes required for src
    if(value < 0){
        cout<<"\nerror in filling image array";
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

    int ii = 0;
    int no_sec = 5;
    //int no_frames = no_sec * videoEncoderContext->time_base.den;//numero di frames = secondi * fps
    int no_frames = 2000;

    int j = 0;//buggato ma è inutile
    //int64_t pts = 0;
    int64_t dts = 0;
    int got_picture;
    int64_t pts = 0;

    while( av_read_frame( pAVFormatContext , inPacket ) >= 0 )
    {
        if( ii++ == no_frames )break;

        if(inPacket->stream_index == videoStreamIndx){
            cout <<  endl << "video packet: " << endl;
            value = decode_encode_write_video(j);
            if(value == 1){
                break;
            }
            if(value == 2){
                continue;
            }
        }

        if(inPacket->stream_index == audioStreamIndx){
            cout << "audio packet" << endl;
            //---------------------------------------------------------------------------------------------------
            //DECODING
            value = avcodec_send_packet(audioDecoderContext, inPacket);
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
                if (av_audio_fifo_read(fifo, (void **) outFrameAudio->data, frame_size) < frame_size) {
                    fprintf(stderr, "Could not read data from FIFO\n");
                    //av_frame_free(&output_frame);
                    //return AVERROR_EXIT;
                }
                //cout << "dimensione fifo: " << av_audio_fifo_size(fifo) << endl;

                outFrameAudio->pts = pts;
                pts += outFrameAudio->nb_samples;
                cout << "audio frame pts: " << outFrameAudio->pts << endl;

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

                int error = avcodec_receive_packet(audioEncoderContext, outPacket);
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
                    cout << "audio pts: " << outPacket->pts << " audio dts: " << outPacket->dts;
                    error = av_interleaved_write_frame(outAVFormatContext_audio, outPacket);
                    if (error < 0) {
                        cout << " noooo" << endl;
                    } else {
                        cout << " ok" << endl;
                    }
                }
            }
            av_packet_unref(inPacket);
            av_packet_unref(outPacket);
        }
    }// End of while-loop

    value = av_write_trailer(outAVFormatContext_video);

    if( value < 0)
    {
        cout<<"\nerror in writing av trailer";
        exit(1);
    }

    value = av_write_trailer(outAVFormatContext_audio);

    if( value < 0)
    {
        cout<<"\nerror in writing av trailer";
        exit(1);
    }



//THIS WAS ADDED LATER
    av_free(video_outbuf);

    return 0;
}

int ScreenRecorder::recordAudio(){
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

    inPacket = av_packet_alloc();
    if( !inPacket ){
        cout<<"\nunable to allocate the avpacket";
        exit(1);
    }

    outPacket = av_packet_alloc();
    if( !outPacket ){
        cout<<"\nunable to allocate the avpacket";
        exit(1);
    }

    int ii = 0;
    int no_sec = 5;
    //int no_frames = no_sec * videoEncoderContext->time_base.den;//numero di frames = secondi * fps
    int no_frames = 300;
    int64_t pts = 0;

    while( av_read_frame( pAVFormatContext , inPacket ) >= 0 )
    {
        if( ii++ == no_frames )break;
        cout << endl<< endl << ii << endl << endl;
        if(inPacket->stream_index == audioStreamIndx){
            cout << "audio packet" << endl;
            //---------------------------------------------------------------------------------------------------
            //DECODING
            value = avcodec_send_packet(audioDecoderContext, inPacket);
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
                if (av_audio_fifo_read(fifo, (void **) outFrameAudio->data, frame_size) < frame_size) {
                    fprintf(stderr, "Could not read data from FIFO\n");
                    //av_frame_free(&output_frame);
                    //return AVERROR_EXIT;
                }
                //cout << "dimensione fifo: " << av_audio_fifo_size(fifo) << endl;

                outFrameAudio->pts = pts;
                pts += outFrameAudio->nb_samples;
                cout << "audio frame pts: " << outFrameAudio->pts << endl;

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

                int error = avcodec_receive_packet(audioEncoderContext, outPacket);
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
                    cout << "audio pts: " << outPacket->pts << " audio dts: " << outPacket->dts;
                    error = av_interleaved_write_frame(outAVFormatContext_audio, outPacket);
                    if (error < 0) {
                        cout << " noooo" << endl;
                    } else {
                        cout << " ok" << endl;
                    }
                }
            }
            av_packet_unref(inPacket);
            av_packet_unref(outPacket);
        }
    }// End of while-loop

    if( av_write_trailer(outAVFormatContext_audio) < 0)
    {
        cout<<"\nerror in writing av trailer";
        exit(1);
    }

    return 0;
}
