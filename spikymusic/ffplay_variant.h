#pragma once

#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>

extern "C"
{
	#include "libavutil/avstring.h"
	#include "libavutil/eval.h"
	#include "libavutil/mathematics.h"
	#include "libavutil/pixdesc.h"
	#include "libavutil/imgutils.h"
	#include "libavutil/dict.h"
	#include "libavutil/parseutils.h"
	#include "libavutil/samplefmt.h"
	#include "libavutil/avassert.h"
	#include "libavutil/time.h"
	#include "libavformat/avformat.h"
	#include "libavdevice/avdevice.h"
	#include "libswscale/swscale.h"
	#include "libavutil/opt.h"
	#include "libavcodec/avfft.h"
	#include "libswresample/swresample.h"
}
#if CONFIG_AVFILTER
# include "libavfilter/avfilter.h"
# include "libavfilter/buffersink.h"
# include "libavfilter/buffersrc.h"
#endif

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_thread.h>
#include <assert.h>
#include "logger.h"
#include "dump_variant.h"
#include <strsafe.h>


#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")
//sdl
#pragma comment(lib,"SDL2.lib")
//#pragma comment(lib,"SDL2main.lib")
//#pragma comment(lib,"SDL2test.lib")




#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* Step size for volume control in dB */
#define SDL_VOLUME_STEP (0.75)

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000

#define USE_ONEPASS_SUBTITLE_RENDER 1

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))








using std::string;
using std::wstring;
using std::array;
typedef struct MyAVPacketList {
    AVPacket pkt;
    struct MyAVPacketList* next;
    int serial;
} MyAVPacketList;

typedef struct PacketQueue {
    MyAVPacketList* first_pkt, * last_pkt;
    int nb_packets;
    int size;
    int64_t duration;
    int abort_request;
    int serial;
    SDL_mutex* mutex;
    SDL_cond* cond;
} PacketQueue;

typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

typedef struct Clock {
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int* queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
    AVFrame* frame;
    AVSubtitle sub;
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
    int width;
    int height;
    int format;
    AVRational sar;
    int uploaded;
    int flip_v;
} Frame;

typedef struct FrameQueue {
    Frame queue[FRAME_QUEUE_SIZE];
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int rindex_shown;
    SDL_mutex* mutex;
    SDL_cond* cond;
    PacketQueue* pktq;
} FrameQueue;
typedef struct Decoder {
    AVPacket pkt;
    PacketQueue* queue;
    AVCodecContext* avctx;
    int pkt_serial;
    int finished;
    int packet_pending;
    SDL_cond* empty_queue_cond;
    int64_t start_pts;
    AVRational start_pts_tb;
    int64_t next_pts;
    AVRational next_pts_tb;
    SDL_Thread* decoder_tid;
} Decoder;

typedef struct VideoState
{
    SDL_Thread* read_tid;
    AVInputFormat* iformat;
    int abort_request;
    int force_refresh;
    int paused;
    int last_paused;
    int queue_attachments_req;
    int seek_req;
    int seek_flags;
    int64_t seek_pos;
    int64_t seek_rel;
    int read_pause_return;
    AVFormatContext* ic;
    int realtime;

    Clock audclk;
    Clock vidclk;
    Clock extclk;

    FrameQueue pictq;
    FrameQueue subpq;
    FrameQueue sampq;

    Decoder auddec;
    Decoder viddec;
    Decoder subdec;

    int audio_stream;

    int av_sync_type;

    double audio_clock;
    int audio_clock_serial;
    double audio_diff_cum; /* used for AV difference average computation */
    double audio_diff_avg_coef;
    double audio_diff_threshold;
    int audio_diff_avg_count;
    AVStream* audio_st;
    PacketQueue audioq;
    int audio_hw_buf_size;
    uint8_t* audio_buf;
    uint8_t* audio_buf1;
    unsigned int audio_buf_size; /* in bytes */
    unsigned int audio_buf1_size;
    int audio_buf_index; /* in bytes */
    int audio_write_buf_size;
    int audio_volume;
    int muted;
    struct AudioParams audio_src;

#if CONFIG_AVFILTER
    struct AudioParams audio_filter_src;
#endif
    struct AudioParams audio_tgt;
    struct SwrContext* swr_ctx;
    int frame_drops_early;
    int frame_drops_late;

    enum ShowMode
    {
        SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
    } show_mode;
    int16_t sample_array[SAMPLE_ARRAY_SIZE];
    int sample_array_index;
    int last_i_start;
    RDFTContext* rdft;
    int rdft_bits;
    FFTSample* rdft_data;
    int xpos;
    double last_vis_time;
    SDL_Texture* vis_texture;
    SDL_Texture* sub_texture;
    SDL_Texture* vid_texture;

    int subtitle_stream;
    AVStream* subtitle_st;
    PacketQueue subtitleq;

    double frame_timer;
    double frame_last_returned_time;
    double frame_last_filter_delay;
    int video_stream;
    AVStream* video_st;
    PacketQueue videoq;
    double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
    struct SwsContext* img_convert_ctx;
    struct SwsContext* sub_convert_ctx;
    int eof;

    char* filename;
    int width, height, xleft, ytop;
    int step;
#if CONFIG_AVFILTER
    int vfilter_idx;
    AVFilterContext* in_video_filter;   // the first filter in the video chain
    AVFilterContext* out_video_filter;  // the last filter in the video chain
    AVFilterContext* in_audio_filter;   // the first filter in the audio chain
    AVFilterContext* out_audio_filter;  // the last filter in the audio chain
    AVFilterGraph* agraph;              // audio filter graph
#endif

    int last_video_stream, last_audio_stream, last_subtitle_stream;

    SDL_cond* continue_read_thread;
} VideoState;

class Ffplay
{
public:
    Ffplay();
    void sdl_push_event();
    void play_song(string file, HWND parent);
    void close_song(VideoState* video_state);
    void displayLastErrorDebug(LPTSTR lpSzFunction);
    VideoState* stream_open(const char* filename, AVInputFormat* iformat);
    int frame_queue_init(FrameQueue* f, PacketQueue* pktq, int max_size, int keep_last);
    void frame_queue_signal(FrameQueue* f);
    Frame* frame_queue_peek(FrameQueue* f);
    Frame* frame_queue_peek_next(FrameQueue* f);
    Frame* frame_queue_peek_last(FrameQueue* f);
    Frame* frame_queue_peek_writable(FrameQueue* f);
    Frame* frame_queue_peek_readable(FrameQueue* f);
    void frame_queue_push(FrameQueue* f);
    void frame_queue_next(FrameQueue* f);
    int packet_queue_init(PacketQueue* q);
    void packet_queue_destroy(PacketQueue* q);
    void packet_queue_flush(PacketQueue* q);
    int packet_queue_put_private(PacketQueue* q, AVPacket* pkt);
    int packet_queue_put(PacketQueue* q, AVPacket* pkt);
    int packet_queue_put_nullpacket(PacketQueue* q, int stream_index);
    void packet_queue_abort(PacketQueue* q);
    void packet_queue_start(PacketQueue* q);
    int packet_queue_get( PacketQueue* q, AVPacket* pkt, int block, int* serial);
    void init_clock(Clock* c, int* queue_serial);
    void set_clock(Clock* c, double pts, int serial);
    void set_clock_at(Clock* c, double pts, int serial, double time);
    double get_clock(Clock* c);
    void set_clock_speed(Clock* c, double speed);
    void sync_clock_to_slave(Clock* c, Clock* slave);
    int get_master_sync_type(VideoState* is);
    int is_realtime(AVFormatContext* s);
    double get_master_clock(VideoState* is);
    void check_external_clock_speed(VideoState* is);
    void stream_close(VideoState* is);
    void stream_component_close(VideoState* is, int stream_index);
    static int static_read_thread(void* arg);
    int read_thread(void* arg);
    void event_loop(VideoState* cur_stream);
    void refresh_loop_wait_event(VideoState* is, SDL_Event* event);
    static int static_decode_interrupt_cb(void* ctx);
    int decode_interrupt_cb(void* ctx);
    void frame_queue_destory(FrameQueue* f);
    void frame_queue_unref_item(Frame* vp);
    int frame_queue_nb_remaining(FrameQueue* f);
    int64_t frame_queue_last_pos(FrameQueue* f);
    void do_exit(VideoState* is);
    void toggle_full_screen(VideoState* is);
    void toggle_audio_display(VideoState* is);
    void stream_toggle_pause(VideoState* is);
    void toggle_pause(VideoState* is);
    void toggle_mute(VideoState* is);
    void update_volume(VideoState* is, int sign, double step);
    void step_to_next_frame(VideoState* is);
    void seek_chapter(VideoState* is, int incr);
    double compute_target_delay(double delay, VideoState* is);
    double vp_duration(VideoState* is, Frame* vp, Frame* nextvp);
    void update_video_pts(VideoState* is, double pts, int64_t pos, int serial);
    void decoder_init(Decoder* d, AVCodecContext* avctx, PacketQueue* queue, SDL_cond* empty_queue_cond);
    int decoder_start(Decoder* d, int(*fn)(void*), const char* thread_name, void* arg);
    int decoder_decode_frame(Decoder* d, AVFrame* frame, AVSubtitle* sub);
    void decoder_abort(Decoder* d, FrameQueue* fq);
    void decoder_destroy(Decoder* d);
    void set_default_window_size(int width, int height, AVRational sar);
    void calculate_display_rect(SDL_Rect* rect, int scr_xleft, int scr_ytop, int scr_width, int scr_height, int pic_width, int pic_height, AVRational pic_sar);
    void stream_seek(VideoState* is, int64_t pos, int64_t rel, int seek_by_bytes);
    int stream_has_enough_packets(AVStream* st, int stream_id, PacketQueue* queue);
    void stream_cycle_channel(VideoState* is, int codec_type);
    int stream_component_open(VideoState* is, int stream_index);
    int audio_open(void* opaque, int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, AudioParams* audio_hw_params);
    static void static_sdl_audio_callback(void* opaque, Uint8* stream, int len);
    static void sdl_audio_callback(void* opaque, Uint8* stream, int len);
    int audio_decode_frame(VideoState* is);
    int synchronize_audio(VideoState* is, int nb_samples);
    void update_sample_display(VideoState* is, short* samples, int samples_size);
    void video_refresh(void* opaque, double* remaining_time);
    void video_display(VideoState* is);
    int video_open(VideoState* is);
    void video_audio_display(VideoState* s);
    void video_image_display(VideoState* is);
    int get_video_frame(VideoState* is, AVFrame* frame);
    int queue_picture(VideoState* is, AVFrame* src_frame, double pts, double duration, int64_t pos, int serial);
    inline void fill_rectangle(int x, int y, int w, int h);
    inline int compute_mod(int a, int b);
    AVDictionary* filter_codec_opts(AVDictionary* opts, AVCodecID codec_id, AVFormatContext* s, AVStream* st, AVCodec* codec);
    int check_stream_specifier(AVFormatContext* s, AVStream* st, const char* spec);
    int realloc_texture(SDL_Texture** texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture);
    void get_sdl_pix_fmt_and_blendmode(int format, Uint32* sdl_pix_fmt, SDL_BlendMode* sdl_blendmode);
    int upload_texture(SDL_Texture** tex, AVFrame* frame, SwsContext** img_convert_ctx);
    void set_sdl_yuv_conversion_mode(AVFrame* frame);
    static int static_audio_thread(void* arg);
    int audio_thread(void* arg);
    static int static_video_thread(void* arg);
    int video_thread(void* arg);
    static int static_subtitle_thread(void* arg);
    int subtitle_thread(void* arg);
    AVDictionary** setup_find_stream_info_opts(AVFormatContext* s, AVDictionary* codec_opts);
    void print_error(const char* filename, int err);
    void outputDebugStr(string str);
    int opt_format( const char* arg);
    static bool is_song_duration_set();
    static int64_t get_song_duration();
    static double get_time_played_in_secs();
    ~Ffplay();
private:
    const char program_name[7] = "ffplay";
    const int program_birth_year = 2003;
    unsigned sws_flags = SWS_BICUBIC;

    enum {
        AV_SYNC_AUDIO_MASTER, /* default choice */
        AV_SYNC_VIDEO_MASTER,
        AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
    };

    enum ShowMode {
        SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
    };
  

    /* options specified by the user */
    AVInputFormat* file_iformat;
    string input_filename;
    const char* window_title;
    int default_width = 640;
    int default_height = 480;
    int screen_width = 0;
    int screen_height = 0;
    int screen_left = SDL_WINDOWPOS_CENTERED;
    int screen_top = SDL_WINDOWPOS_CENTERED;
    int audio_disable = false;
    int video_disable = false;
    int subtitle_disable = false;
    const char* wanted_stream_spec[AVMEDIA_TYPE_NB] =
    { "v", // video
        "a", // audio 
        "s", // subtitle
        "d", // data
        "t" // attachment
    };
    int seek_by_bytes = -1;
    float seek_interval = 10;
    int display_disable;
    int borderless;
    int alwaysontop;
    int startup_volume = 100;
    int show_status = 1;
    int av_sync_type = AV_SYNC_AUDIO_MASTER;
    int64_t start_time = AV_NOPTS_VALUE;
    int64_t duration = AV_NOPTS_VALUE;
    int fast = 0;
    int genpts = 0;
    int lowres = 0;
    int decoder_reorder_pts = -1;
    int autoexit;
    int exit_on_keydown;
    int exit_on_mousedown=1;
    int loop = 1;
    int framedrop = -1;
    int infinite_buffer = -1;
    enum ShowMode show_mode=SHOW_MODE_NONE; //the default initializer fro is->show_mode
    const char* audio_codec_name;
    const char* subtitle_codec_name;
    const char* video_codec_name;
    double rdftspeed = 0.02;
    int64_t cursor_last_shown;
    int cursor_hidden = 0;
   

#if CONFIG_AVFILTER
    const char** vfilters_list = NULL;
    int nb_vfilters = 0;
    char* afilters = NULL;
#endif
    int autorotate = 1;
    int find_stream_info = 1;
    int filter_nbthreads = 0;

    /* current context */
    int is_full_screen;
    int64_t audio_callback_time;

    AVPacket flush_pkt;

#define FF_QUIT_EVENT    (SDL_USEREVENT + 2)

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_RendererInfo renderer_info = { 0 };
    static SDL_AudioDeviceID audio_dev;

    const struct TextureFormatEntry
    {
        enum AVPixelFormat format;
        int texture_fmt;
    };
    TextureFormatEntry sdl_texture_format_map[20] ={
        { AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
        { AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
        { AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
        { AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
        { AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
        { AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
        { AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
        { AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
        { AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
        { AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
        { AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
        { AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
        { AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
        { AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
        { AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
        { AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
        { AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
        { AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
        { AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
        { AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
    };

#if CONFIG_AVFILTER
    int opt_add_vfilter(void* optctx, const char* opt, const char* arg)
    {
        GROW_ARRAY(vfilters_list, nb_vfilters);
        vfilters_list[nb_vfilters - 1] = arg;
        return 0;
    }
#endif
    AVDictionary* format_opts, * codec_opts, * resample_opts;
    Dump dump;
    static bool abort;
    static bool song_duration_set;
    static int64_t song_duration;
    static double d_time_played_s;
   
};

