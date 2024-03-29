#include "dump_variant.h"


Dump::Dump()
{
    //theres no need to intialize the logger since its already been done in ffplay_variant.cpp
    //logger.init();
}

void Dump::dump_format(AVFormatContext* ic, int index,    const char* url, int is_output)
{
    int i;
    uint8_t* printed = static_cast<uint8_t*>( ic->nb_streams ? av_mallocz(ic->nb_streams) : NULL); 
    if (ic->nb_streams && !printed)
        return;

    logger.log(logger.LEVEL_INFO, "%s #%d, %s, %s '%s':\n",
        is_output ? "Output" : "Input",
        index,
        is_output ? ic->oformat->name : ic->iformat->name,
        is_output ? "to" : "from", url);
    dump_metadata(NULL, ic->metadata);

    if (!is_output) {
        logger.log(logger.LEVEL_INFO, "  Duration: ");
        if (ic->duration != AV_NOPTS_VALUE) {
            int hours, mins, secs, us;
            int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
            secs = duration / AV_TIME_BASE;
            us = duration % AV_TIME_BASE;
            mins = secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;
            logger.log(logger.LEVEL_INFO, "%02d:%02d:%02d.%02d", hours, mins, secs,
                (100 * us) / AV_TIME_BASE);
        }
        else {
            logger.log(logger.LEVEL_INFO, "N/A");
        }
        if (ic->start_time != AV_NOPTS_VALUE) {
            int secs, us;
            logger.log(logger.LEVEL_INFO, ", start: ");
            secs = llabs(ic->start_time / AV_TIME_BASE);
            us = llabs(ic->start_time % AV_TIME_BASE);
            logger.log(logger.LEVEL_INFO, "%s%d.%06d",
                ic->start_time >= 0 ? "" : "-",
                secs,
                (int)av_rescale(us, 1000000, AV_TIME_BASE));
        }
        logger.log(logger.LEVEL_INFO, ", bitrate: ");
        if (ic->bit_rate)
            logger.log(logger.LEVEL_INFO, "%d kb/s", ic->bit_rate / 1000);
        else
            logger.log(logger.LEVEL_INFO, "N/A");
        logger.log(logger.LEVEL_INFO, "\n");
    }

    for (i = 0; i < ic->nb_chapters; i++) {
        AVChapter* ch = ic->chapters[i];
        logger.log(logger.LEVEL_INFO, "    Chapter #%d:%d: ", index, i);
        logger.log(logger.LEVEL_INFO,
            "start %f, ", ch->start * av_q2d(ch->time_base));
        logger.log(logger.LEVEL_INFO,
            "end %f\n", ch->end * av_q2d(ch->time_base));

        dump_metadata(NULL, ch->metadata);
    }

    if (ic->nb_programs) {
        int j, k, total = 0;
        for (j = 0; j < ic->nb_programs; j++) {
            AVDictionaryEntry* name = av_dict_get(ic->programs[j]->metadata,
                "name", NULL, 0);
            logger.log(logger.LEVEL_INFO, "  Program %d %s\n", ic->programs[j]->id,
                name ? name->value : "");
            dump_metadata(NULL, ic->programs[j]->metadata);
            for (k = 0; k < ic->programs[j]->nb_stream_indexes; k++) {
                dump_stream_format(ic, ic->programs[j]->stream_index[k],
                    index, is_output);
                printed[ic->programs[j]->stream_index[k]] = 1;
            }
            total += ic->programs[j]->nb_stream_indexes;
        }
        if (total < ic->nb_streams)
            logger.log(logger.LEVEL_INFO, "  No Program\n");
    }

    for (i = 0; i < ic->nb_streams; i++)
        if (!printed[i])
            dump_stream_format(ic, i, index, is_output);

    av_free(printed);
}
void Dump::dump_metadata(void* ctx, AVDictionary* m)
{
    if (m && !(av_dict_count(m) == 1 && av_dict_get(m, "language", NULL, 0))) {
        AVDictionaryEntry* tag = NULL;

        logger.log(logger.LEVEL_INFO, "Metadata:\n");
        while ((tag = av_dict_get(m, "", tag, AV_DICT_IGNORE_SUFFIX)))
            if (strcmp("language", tag->key))
            {
                const char* p = tag->value;
                logger.log(logger.LEVEL_INFO,    "%-1s: ", tag->key);
                while (*p) 
                {
                    char tmp[256];
                    size_t len = strcspn(p, "\x8\xa\xb\xc\xd");
                    av_strlcpy(tmp, p, FFMIN(sizeof(tmp), len + 1));
                    logger.log(logger.LEVEL_INFO, "%s", tmp);
                    p += len;
                    if (*p == 0xd) logger.log(logger.LEVEL_INFO, " ");
                    if (*p == 0xa) logger.log(logger.LEVEL_INFO, "%-16s: ", "");
                    if (*p) p++;
                }
            }
    }
}
/* "user interface" functions */
void Dump::dump_stream_format(AVFormatContext* ic, int i,
    int index, int is_output)
{
    char buf[256];
    int flags = (is_output ? ic->oformat->flags : ic->iformat->flags);
    AVStream* st = ic->streams[i];
    AVDictionaryEntry* lang = av_dict_get(st->metadata, "language", NULL, 0);
    char* separator = (char*) ic->dump_separator;
    AVCodecContext* avctx;
    int ret;

    avctx = avcodec_alloc_context3(NULL);
    if (!avctx)
        return;

    ret = avcodec_parameters_to_context(avctx, st->codecpar);
    if (ret < 0) {
        avcodec_free_context(&avctx);
        return;
    }

    // Fields which are missing from AVCodecParameters need to be taken from the AVCodecContext
   

    /*avctx->properties = st->codec->properties; st->
    avctx->codec = st->codecpar->codec_type;
    avctx->qmin = st->qmin;
    avctx->qmax = st->codec->qmax;
    avctx->coded_width = st->codec->coded_width;
    avctx->coded_height = st->codec->coded_height;*/

    if (separator)
        av_opt_set(avctx, "dump_separator", separator, 0);
    avcodec_string(buf, sizeof(buf), avctx, is_output);
    avcodec_free_context(&avctx);

    logger.log(logger.LEVEL_INFO, "    Stream #%d:%d", index, i);

    /* the pid is an important information, so we display it */
    /* XXX: add a generic system */
    if (flags & AVFMT_SHOW_IDS)
        logger.log(logger.LEVEL_INFO, "[0x%x]", st->id);
    if (lang)
        logger.log(logger.LEVEL_INFO, "(%s)", lang->value);
    logger.log(logger.LEVEL_INFO, ", %d, %d/%d", st->codec_info_nb_frames,
        st->time_base.num, st->time_base.den);
    logger.log(logger.LEVEL_INFO, ": %s", buf);

    if (st->sample_aspect_ratio.num &&
        av_cmp_q(st->sample_aspect_ratio, st->codecpar->sample_aspect_ratio)) {
        AVRational display_aspect_ratio;
        av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
            st->codecpar->width * (int64_t)st->sample_aspect_ratio.num,
            st->codecpar->height * (int64_t)st->sample_aspect_ratio.den,
            1024 * 1024);
        logger.log(logger.LEVEL_INFO, ", SAR %d:%d DAR %d:%d",
            st->sample_aspect_ratio.num, st->sample_aspect_ratio.den,
            display_aspect_ratio.num, display_aspect_ratio.den);
    }

    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        int fps = st->avg_frame_rate.den && st->avg_frame_rate.num;
        int tbr = st->r_frame_rate.den && st->r_frame_rate.num;
        int tbn = st->time_base.den && st->time_base.num;
        int tbc = st->time_base.den && st->time_base.num;

        if (fps || tbr || tbn || tbc)
            logger.log(logger.LEVEL_INFO, "%s", separator);

        if (fps)
            print_fps(av_q2d(st->avg_frame_rate), tbr || tbn || tbc ? "fps, " : "fps");
        if (tbr)
            print_fps(av_q2d(st->r_frame_rate), tbn || tbc ? "tbr, " : "tbr");
        if (tbn)
            print_fps(1 / av_q2d(st->time_base), tbc ? "tbn, " : "tbn");
        if (tbc)
            print_fps(1 / av_q2d(st->time_base), "tbc");
    }

    if (st->disposition & AV_DISPOSITION_DEFAULT)
        logger.log(logger.LEVEL_INFO, " (default)");
    if (st->disposition & AV_DISPOSITION_DUB)
        logger.log(logger.LEVEL_INFO, " (dub)");
    if (st->disposition & AV_DISPOSITION_ORIGINAL)
        logger.log(logger.LEVEL_INFO, " (original)");
    if (st->disposition & AV_DISPOSITION_COMMENT)
        logger.log(logger.LEVEL_INFO, " (comment)");
    if (st->disposition & AV_DISPOSITION_LYRICS)
        logger.log(logger.LEVEL_INFO, " (lyrics)");
    if (st->disposition & AV_DISPOSITION_KARAOKE)
        logger.log(logger.LEVEL_INFO, " (karaoke)");
    if (st->disposition & AV_DISPOSITION_FORCED)
        logger.log(logger.LEVEL_INFO, " (forced)");
    if (st->disposition & AV_DISPOSITION_HEARING_IMPAIRED)
        logger.log(logger.LEVEL_INFO, " (hearing impaired)");
    if (st->disposition & AV_DISPOSITION_VISUAL_IMPAIRED)
        logger.log(logger.LEVEL_INFO, " (visual impaired)");
    if (st->disposition & AV_DISPOSITION_CLEAN_EFFECTS)
        logger.log(logger.LEVEL_INFO, " (clean effects)");
    if (st->disposition & AV_DISPOSITION_ATTACHED_PIC)
        logger.log(logger.LEVEL_INFO, " (attached pic)");
    if (st->disposition & AV_DISPOSITION_TIMED_THUMBNAILS)
        logger.log(logger.LEVEL_INFO, " (timed thumbnails)");
    if (st->disposition & AV_DISPOSITION_CAPTIONS)
        logger.log(logger.LEVEL_INFO, " (captions)");
    if (st->disposition & AV_DISPOSITION_DESCRIPTIONS)
        logger.log(logger.LEVEL_INFO, " (descriptions)");
    if (st->disposition & AV_DISPOSITION_METADATA)
        logger.log(logger.LEVEL_INFO, " (metadata)");
    if (st->disposition & AV_DISPOSITION_DEPENDENT)
        logger.log(logger.LEVEL_INFO, " (dependent)");
    if (st->disposition & AV_DISPOSITION_STILL_IMAGE)
        logger.log(logger.LEVEL_INFO, " (still image)");
    logger.log(logger.LEVEL_INFO, "\n");

    dump_metadata(NULL, st->metadata);

    dump_sidedata(NULL, st, "    ");
}
void Dump::print_fps(double d, const char* postfix)
{
    uint64_t v = lrintf(d * 100);
    if (!v)
        logger.log(logger.LEVEL_INFO, "%1.4f %s", d, postfix);
    else if (v % 100)
        logger.log(logger.LEVEL_INFO, "%3.2f %s", d, postfix);
    else if (v % (100 * 1000))
        logger.log(logger.LEVEL_INFO, "%1.0f %s", d, postfix);
    else
        logger.log(logger.LEVEL_INFO, "%1.0fk %s", d / 1000, postfix);
}
void Dump::dump_sidedata(void* ctx, AVStream* st, const char* indent)
{
    int i;

    if (st->nb_side_data)
        logger.log(logger.LEVEL_INFO, "%sSide data:\n", indent);

    for (i = 0; i < st->nb_side_data; i++) {
        AVPacketSideData sd = st->side_data[i];
        logger.log(logger.LEVEL_INFO, "%s  ", indent);

        switch (sd.type) {
        case AV_PKT_DATA_PALETTE:
            logger.log(logger.LEVEL_INFO, "palette");
            break;
        case AV_PKT_DATA_NEW_EXTRADATA:
            logger.log(logger.LEVEL_INFO, "new extradata");
            break;
        case AV_PKT_DATA_PARAM_CHANGE:
            logger.log(logger.LEVEL_INFO, "paramchange: ");
            dump_paramchange(ctx, &sd);
            break;
        case AV_PKT_DATA_H263_MB_INFO:
            logger.log(logger.LEVEL_INFO, "H.263 macroblock info");
            break;
        case AV_PKT_DATA_REPLAYGAIN:
            logger.log(logger.LEVEL_INFO, "replaygain: ");
            dump_replaygain(ctx, &sd);
            break;
        case AV_PKT_DATA_DISPLAYMATRIX:
            logger.log(logger.LEVEL_INFO, "displaymatrix: rotation of %.2f degrees",
                av_display_rotation_get((int32_t*)sd.data));
            break;
        case AV_PKT_DATA_STEREO3D:
            logger.log(logger.LEVEL_INFO, "stereo3d: ");
            dump_stereo3d(ctx, &sd);
            break;
        case AV_PKT_DATA_AUDIO_SERVICE_TYPE:
            logger.log(logger.LEVEL_INFO, "audio service type: ");
            dump_audioservicetype(ctx, &sd);
            break;
        case AV_PKT_DATA_QUALITY_STATS:
            logger.log(logger.LEVEL_INFO, "quality factor: %lu, pict_type: %c",
                AV_RL32(sd.data),(unsigned long) av_get_picture_type_char( (AVPictureType)sd.data[4]));
            break;
        case AV_PKT_DATA_CPB_PROPERTIES:
            logger.log(logger.LEVEL_INFO, "cpb: ");
            dump_cpb(ctx, &sd);
            break;
        case AV_PKT_DATA_MASTERING_DISPLAY_METADATA:
            dump_mastering_display_metadata(ctx, &sd);
            break;
        case AV_PKT_DATA_SPHERICAL:
            logger.log(logger.LEVEL_INFO, "spherical: ");
            dump_spherical(ctx, st->codecpar, &sd);
            break;
        case AV_PKT_DATA_CONTENT_LIGHT_LEVEL:
            dump_content_light_metadata(ctx, &sd);
            break;
        default:
            logger.log(logger.LEVEL_INFO,
                "unknown side data type %d (%d bytes)", sd.type, sd.size);
            break;
        }

        logger.log(logger.LEVEL_INFO, "\n");
    }
}
/* param change side data*/
void Dump::dump_paramchange(void* ctx, AVPacketSideData* sd)
{
    int size = sd->size;
    const uint8_t* data = sd->data;
    uint32_t flags, channels, sample_rate, width, height;
    uint64_t layout;

    if (!data || sd->size < 4)
        goto fail;

    flags = AV_RL32(data);
    data += 4;
    size -= 4;

    if (flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT) {
        if (size < 4)
            goto fail;
        channels = AV_RL32(data);
        data += 4;
        size -= 4;
        logger.log(logger.LEVEL_INFO, "channel count %lu, ", (unsigned long)channels);
    }
    if (flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT) {
        if (size < 8)
            goto fail;
        layout = AV_RL64(data);
        data += 8;
        size -= 8;
        logger.log(logger.LEVEL_INFO,
            "channel layout: %s, ", av_get_channel_name(layout));
    }
    if (flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE) {
        if (size < 4)
            goto fail;
        sample_rate = AV_RL32(data);
        data += 4;
        size -= 4;
        logger.log(logger.LEVEL_INFO, "sample_rate %d\n", sample_rate);
    }
    if (flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS) {
        if (size < 8)
            goto fail;
        width = AV_RL32(data);
        data += 4;
        size -= 4;
        height = AV_RL32(data);
        data += 4;
        size -= 4;
        logger.log(logger.LEVEL_INFO, "width %d height %d\n", width, height);
    }

    return;
fail:
    logger.log(logger.LEVEL_ERROR, "unknown param");
}
void Dump::dump_replaygain(void* ctx, AVPacketSideData* sd)
{
    AVReplayGain* rg;

    if (sd->size < sizeof(*rg)) {
        logger.log(logger.LEVEL_ERROR, "invalid data");
        return;
    }
    rg = (AVReplayGain*)sd->data;

    print_gain(ctx, "track gain", rg->track_gain);
    print_peak(ctx, "track peak", rg->track_peak);
    print_gain(ctx, "album gain", rg->album_gain);
    print_peak(ctx, "album peak", rg->album_peak);
}
void Dump::dump_stereo3d(void* ctx, AVPacketSideData* sd)
{
    AVStereo3D* stereo;

    if (sd->size < sizeof(*stereo)) {
        logger.log(logger.LEVEL_ERROR, "invalid data");
        return;
    }

    stereo = (AVStereo3D*)sd->data;

    logger.log(logger.LEVEL_INFO, "%s", av_stereo3d_type_name(stereo->type));

    if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
        logger.log(logger.LEVEL_INFO, " (inverted)");
}
void Dump::dump_audioservicetype(void* ctx, AVPacketSideData* sd)
{
    enum AVAudioServiceType* ast = (enum AVAudioServiceType*)sd->data;

    if (sd->size < sizeof(*ast)) {
        logger.log(logger.LEVEL_ERROR, "invalid data");
        return;
    }

    switch (*ast) {
    case AV_AUDIO_SERVICE_TYPE_MAIN:
        logger.log(logger.LEVEL_INFO, "main");
        break;
    case AV_AUDIO_SERVICE_TYPE_EFFECTS:
        logger.log(logger.LEVEL_INFO, "effects");
        break;
    case AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED:
        logger.log(logger.LEVEL_INFO, "visually impaired");
        break;
    case AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED:
        logger.log(logger.LEVEL_INFO, "hearing impaired");
        break;
    case AV_AUDIO_SERVICE_TYPE_DIALOGUE:
        logger.log(logger.LEVEL_INFO, "dialogue");
        break;
    case AV_AUDIO_SERVICE_TYPE_COMMENTARY:
        logger.log(logger.LEVEL_INFO, "commentary");
        break;
    case AV_AUDIO_SERVICE_TYPE_EMERGENCY:
        logger.log(logger.LEVEL_INFO, "emergency");
        break;
    case AV_AUDIO_SERVICE_TYPE_VOICE_OVER:
        logger.log(logger.LEVEL_INFO, "voice over");
        break;
    case AV_AUDIO_SERVICE_TYPE_KARAOKE:
        logger.log(logger.LEVEL_INFO, "karaoke");
        break;
    default:
        logger.log(logger.LEVEL_WARNING, "unknown");
        break;
    }
}
void Dump::dump_cpb(void* ctx, AVPacketSideData* sd)
{
    AVCPBProperties* cpb = (AVCPBProperties*)sd->data;

    if (sd->size < sizeof(*cpb)) {
        logger.log(logger.LEVEL_ERROR, "invalid data");
        return;
    }

    logger.log(logger.LEVEL_INFO,
#if FF_API_UNSANITIZED_BITRATES
        "bitrate max/min/avg: %d/%d/%d buffer size: %d ",
#else
        "bitrate max/min/avg: %"PRId64"/%"PRId64"/%"PRId64" buffer size: %d ",
#endif
        cpb->max_bitrate, cpb->min_bitrate, cpb->avg_bitrate,
        cpb->buffer_size);
    if (cpb->vbv_delay == UINT64_MAX)
        logger.log(logger.LEVEL_INFO, "vbv_delay: N/A");
    else
        logger.log(logger.LEVEL_INFO, "vbv_delay: %d", cpb->vbv_delay);
}
void Dump::dump_mastering_display_metadata(void* ctx, AVPacketSideData* sd) {
    AVMasteringDisplayMetadata* metadata = (AVMasteringDisplayMetadata*)sd->data;
    logger.log(logger.LEVEL_INFO, "Mastering Display Metadata, "
        "has_primaries:%d has_luminance:%d "
        "r(%5.4f,%5.4f) g(%5.4f,%5.4f) b(%5.4f %5.4f) wp(%5.4f, %5.4f) "
        "min_luminance=%f, max_luminance=%f",
        metadata->has_primaries, metadata->has_luminance,
        av_q2d(metadata->display_primaries[0][0]),
        av_q2d(metadata->display_primaries[0][1]),
        av_q2d(metadata->display_primaries[1][0]),
        av_q2d(metadata->display_primaries[1][1]),
        av_q2d(metadata->display_primaries[2][0]),
        av_q2d(metadata->display_primaries[2][1]),
        av_q2d(metadata->white_point[0]), av_q2d(metadata->white_point[1]),
        av_q2d(metadata->min_luminance), av_q2d(metadata->max_luminance));
}
/* replaygain side data*/
void Dump::print_gain(void* ctx, const char* str, int32_t gain)
{
    logger.log(logger.LEVEL_INFO, "%s - ", str);
    if (gain == INT32_MIN)
        logger.log(logger.LEVEL_INFO, "unknown");
    else
        logger.log(logger.LEVEL_INFO, "%f", gain / 100000.0f);
    logger.log(logger.LEVEL_INFO, ", ");
}
void Dump::print_peak(void* ctx, const char* str, uint32_t peak)
{
    logger.log(logger.LEVEL_INFO, "%s - ", str);
    if (!peak)
        logger.log(logger.LEVEL_INFO, "unknown");
    else
        logger.log(logger.LEVEL_INFO, "%f", (float)peak / UINT32_MAX);
    logger.log(logger.LEVEL_INFO, ", ");
}
void Dump::dump_content_light_metadata(void* ctx, AVPacketSideData* sd)
{
    AVContentLightMetadata* metadata = (AVContentLightMetadata*)sd->data;
    logger.log(logger.LEVEL_INFO, "Content Light Level Metadata, "
        "MaxCLL=%d, MaxFALL=%d",
        metadata->MaxCLL, metadata->MaxFALL);
}
void Dump::dump_spherical(void* ctx, AVCodecParameters* par, AVPacketSideData* sd)
{
    AVSphericalMapping* spherical = (AVSphericalMapping*)sd->data;
    double yaw, pitch, roll;

    if (sd->size < sizeof(*spherical)) {
        logger.log(logger.LEVEL_ERROR, "invalid data");
        return;
    }

    logger.log(logger.LEVEL_INFO, "%s ", av_spherical_projection_name(spherical->projection));

    yaw = ((double)spherical->yaw) / (1 << 16);
    pitch = ((double)spherical->pitch) / (1 << 16);
    roll = ((double)spherical->roll) / (1 << 16);
    logger.log(logger.LEVEL_INFO, "(%f/%f/%f) ", yaw, pitch, roll);

    if (spherical->projection == AV_SPHERICAL_EQUIRECTANGULAR_TILE) {
        size_t l, t, r, b;
        av_spherical_tile_bounds(spherical, par->width, par->height,
            &l, &t, &r, &b);
        logger.log(logger.LEVEL_INFO,
            "[%zu, %zu, %zu, %zu ",
            l, t, r, b);
    }
    else if (spherical->projection == AV_SPHERICAL_CUBEMAP) {
        logger.log(logger.LEVEL_INFO, "[pad %" PRId32, (unsigned long)spherical->padding);
    }
}
