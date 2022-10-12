/******************************************************************************
    Copyright (C) 2016 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include <libavutil/avutil.h>

#define do_log(level, format, ...)                               \
	blog(level, "[FFMPEG VAAPI HEVC encoder: '%s'] " format, \
	     obs_encoder_get_name(enc->encoder), ##__VA_ARGS__)

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(55, 27, 100)

#include "obs-ffmpeg-vaapi.h"
#include <obs-hevc.h>

static const char *vaapi_getname_hevc(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "FFMPEG VAAPI HEVC";
}

static inline bool valid_format_hevc(enum video_format format)
{
	return (format == VIDEO_FORMAT_P010) || (format == VIDEO_FORMAT_I010);
}

static void vaapi_video_info_hevc(void *data, struct video_scale_info *info)
{
	vaapi_video_info(data, info, valid_format_hevc, VIDEO_FORMAT_P010);
}

static bool vaapi_init_codec_hevc(struct vaapi_encoder *enc, const char *path)
{
	return vaapi_init_codec(enc, path, AV_PIX_FMT_P010);
}

static bool vaapi_update_hevc(void *data, obs_data_t *settings)
{
	return vaapi_update(data, settings, vaapi_init_codec_hevc,
			    valid_format_hevc, VIDEO_FORMAT_P010);
}

static void *vaapi_create_hevc(obs_data_t *settings, obs_encoder_t *encoder)
{
	return vaapi_create(settings, encoder, "hevc_vaapi", vaapi_update_hevc);
}

static bool vaapi_encode_hevc(void *data, struct encoder_frame *frame,
			      struct encoder_packet *packet,
			      bool *received_packet)
{
	return vaapi_encode(data, frame, packet, received_packet,
			    obs_extract_hevc_headers, obs_hevc_keyframe);
}

static void vaapi_defaults_hevc(obs_data_t *settings)
{
	obs_data_set_default_string(settings, "vaapi_device",
				    "/dev/dri/renderD128");
	obs_data_set_default_int(settings, "profile", FF_PROFILE_HEVC_MAIN_10);
	obs_data_set_default_int(settings, "level", 51);
	obs_data_set_default_int(settings, "bitrate", 2500);
	obs_data_set_default_int(settings, "keyint_sec", 0);
	obs_data_set_default_int(settings, "bf", 0); // 2?
	obs_data_set_default_int(settings, "rendermode", 0);
	obs_data_set_default_string(settings, "rate_control", "CBR");
	obs_data_set_default_int(settings, "qp", 25);
	obs_data_set_default_int(settings, "maxrate", 0);
}

static void add_profile_property(obs_property_t *list)
{
	obs_property_list_add_int(list, "Main", FF_PROFILE_HEVC_MAIN);
	obs_property_list_add_int(list, "Main 10", FF_PROFILE_HEVC_MAIN_10);
}

static obs_properties_t *vaapi_properties_hevc(void *unused)
{
	return vaapi_properties(unused, add_profile_property);
}

struct obs_encoder_info vaapi_encoder_hevc_info = {
	.id = "ffmpeg_vaapi_hevc",
	.type = OBS_ENCODER_VIDEO,
	.codec = "hevc",
	.get_name = vaapi_getname_hevc,
	.create = vaapi_create_hevc,
	.destroy = vaapi_destroy,
	.encode = vaapi_encode_hevc,
	.get_defaults = vaapi_defaults_hevc,
	.get_properties = vaapi_properties_hevc,
	.get_extra_data = vaapi_extra_data,
	.get_sei_data = vaapi_sei_data,
	.get_video_info = vaapi_video_info_hevc,
};

#endif