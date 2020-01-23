#pragma once
extern "C"
{
	#include <libavformat\avformat.h>
	#include <libavutil\avstring.h>
	#include <libavutil\opt.h>
	#include "libavutil/channel_layout.h"
	#include "libavutil/display.h"
	#include "libavutil/intreadwrite.h"
	#include "libavutil/log.h"
	#include "libavutil/mastering_display_metadata.h"
	#include "libavutil/mathematics.h"
	#include "libavutil/avstring.h"
	#include "libavutil/replaygain.h"
	#include "libavutil/spherical.h"
	#include "libavutil/stereo3d.h"
	#include <inttypes.h>
}
#include "logger.h"



class Dump
{
public:
	Dump();
	void dump_format(AVFormatContext* ic, int index, const char* url, int is_output);
	void dump_metadata(void* ctx, AVDictionary* m);
	void dump_stream_format(AVFormatContext* ic, int i, int index, int is_output);
	void print_fps(double d, const char* postfix);
	void dump_sidedata(void* ctx, AVStream* st, const char* indent);
	void dump_paramchange(void* ctx, AVPacketSideData* sd);
	void dump_replaygain(void* ctx, AVPacketSideData* sd);
	void dump_stereo3d(void* ctx, AVPacketSideData* sd);
	void dump_audioservicetype(void* ctx, AVPacketSideData* sd);
	void dump_cpb(void* ctx, AVPacketSideData* sd);
	void dump_mastering_display_metadata(void* ctx, AVPacketSideData* sd);
	void print_gain(void* ctx, const char* str, int32_t gain);
	void print_peak(void* ctx, const char* str, uint32_t peak);
	void dump_content_light_metadata(void* ctx, AVPacketSideData* sd);
	void dump_spherical(void* ctx, AVCodecParameters* par, AVPacketSideData* sd);
};

