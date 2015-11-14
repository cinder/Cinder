/*
 *  gstvaapiprofile.h - VA profile abstraction
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2012-2013 Intel Corporation
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@intel.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 */

#ifndef GST_VAAPI_PROFILE_H
#define GST_VAAPI_PROFILE_H

#include <gst/gstvalue.h>

G_BEGIN_DECLS

/**
 * GstVaapiCodec:
 * @GST_VAAPI_CODEC_MPEG1: MPEG-1 (ISO/IEC 11172)
 * @GST_VAAPI_CODEC_MPEG2: MPEG-2 (ISO/IEC 13818-2)
 * @GST_VAAPI_CODEC_MPEG4: MPEG-4 Part 2 (ISO/IEC 14496-2)
 * @GST_VAAPI_CODEC_H263: H.263
 * @GST_VAAPI_CODEC_H264: H.264 aka MPEG-4 Part 10 (ISO/IEC 14496-10)
 * @GST_VAAPI_CODEC_WMV3: Windows Media Video 9. VC-1 Simple or Main profile (SMPTE 421M)
 * @GST_VAAPI_CODEC_VC1: VC-1 Advanced profile (SMPTE 421M)
 * @GST_VAAPI_CODEC_JPEG: JPEG (ITU-T 81)
 * @GST_VAAPI_CODEC_H265: H.265 aka MPEG-H Part 2 (ITU-T H.265)
 *
 * The set of all codecs for #GstVaapiCodec.
 */
typedef enum {
    GST_VAAPI_CODEC_MPEG1       = GST_MAKE_FOURCC('M','P','1',0),
    GST_VAAPI_CODEC_MPEG2       = GST_MAKE_FOURCC('M','P','2',0),
    GST_VAAPI_CODEC_MPEG4       = GST_MAKE_FOURCC('M','P','4',0),
    GST_VAAPI_CODEC_H263        = GST_MAKE_FOURCC('2','6','3',0),
    GST_VAAPI_CODEC_H264        = GST_MAKE_FOURCC('2','6','4',0),
    GST_VAAPI_CODEC_WMV3        = GST_MAKE_FOURCC('W','M','V',0),
    GST_VAAPI_CODEC_VC1         = GST_MAKE_FOURCC('V','C','1',0),
    GST_VAAPI_CODEC_JPEG        = GST_MAKE_FOURCC('J','P','G',0),
    GST_VAAPI_CODEC_VP8         = GST_MAKE_FOURCC('V','P','8',0),
    GST_VAAPI_CODEC_H265        = GST_MAKE_FOURCC('2','6','5',0),
} GstVaapiCodec;

/**
 * GST_VAAPI_MAKE_PROFILE:
 * @codec: the #GstVaapiCodec without the GST_VAAPI_CODEC_ prefix
 * @sub_id: a non-zero sub-codec id
 *
 * Macro that evaluates to the profile composed from @codec and
 * @sub_id.
 */
#define GST_VAAPI_MAKE_PROFILE(codec, sub_id) \
    (GST_VAAPI_CODEC_##codec | GST_MAKE_FOURCC(0,0,0,sub_id))

/**
 * GstVaapiProfile:
 * @GST_VAAPI_PROFILE_UNKNOWN:
 *   Unknown profile, used for initializers
 * @GST_VAAPI_PROFILE_MPEG1:
 *   MPEG-1
 * @GST_VAAPI_PROFILE_MPEG2_SIMPLE:
 *   MPEG-2 simple profile
 * @GST_VAAPI_PROFILE_MPEG2_MAIN:
 *   MPEG-2 main profile
 * @GST_VAAPI_PROFILE_MPEG2_HIGH:
 *   MPEG-2 high profile
 * @GST_VAAPI_PROFILE_MPEG4_SIMPLE:
 *   MPEG-4 Part-2 simple profile
 * @GST_VAAPI_PROFILE_MPEG4_ADVANCED_SIMPLE:
 *   MPEG-4 Part-2 advanced simple profile
 * @GST_VAAPI_PROFILE_MPEG4_MAIN:
 *   MPEG-4 Part-2 main profile
 * @GST_VAAPI_PROFILE_H263_BASELINE:
 *   H.263 baseline profile
 * @GST_VAAPI_PROFILE_H264_BASELINE:
 *   H.264 (MPEG-4 Part-10) baseline profile [A.2.1]
 * @GST_VAAPI_PROFILE_H264_CONSTRAINED_BASELINE:
 *   H.264 (MPEG-4 Part-10) constrained baseline profile [A.2.1.1]
 * @GST_VAAPI_PROFILE_H264_MAIN:
 *   H.264 (MPEG-4 Part-10) main profile [A.2.2]
 * @GST_VAAPI_PROFILE_H264_EXTENDED:
 *   H.264 (MPEG-4 Part 10) extended profile [A.2.3]
 * @GST_VAAPI_PROFILE_H264_HIGH:
 *   H.264 (MPEG-4 Part-10) high profile [A.2.4]
 * @GST_VAAPI_PROFILE_H264_HIGH10:
 *   H.264 (MPEG-4 Part-10) high 10 profile [A.2.5], or high 10 intra
 *   profile [A.2.8], depending on constraint_set3_flag
 * @GST_VAAPI_PROFILE_H264_HIGH_422:
 *   H.264 (MPEG-4 Part-10) high 4:2:2 profile [A.2.6], or high 4:2:2
 *   intra profile [A.2.9], depending on constraint_set3_flag
 * @GST_VAAPI_PROFILE_H264_HIGH_444:
 *   H.264 (MPEG-4 Part-10) high 4:4:4 predictive profile [A.2.7], or
 *   high 4:4:4 intra profile [A.2.10], depending on constraint_set3_flag
 * @GST_VAAPI_PROFILE_H264_SCALABLE_BASELINE:
 *   H.264 (MPEG-4 Part-10) scalable baseline profile [G.10.1.1]
 * @GST_VAAPI_PROFILE_H264_SCALABLE_HIGH:
 *   H.264 (MPEG-4 Part-10) scalable high profile [G.10.1.2], or scalable
 *   high intra profile [G.10.1.3], depending on constraint_set3_flag
 * @GST_VAAPI_PROFILE_H264_MULTIVIEW_HIGH:
 *   H.264 (MPEG-4 Part-10) multiview high profile [H.10.1.1]
 * @GST_VAAPI_PROFILE_H264_STEREO_HIGH:
 *   H.264 (MPEG-4 Part-10) stereo high profile [H.10.1.2]
 * @GST_VAAPI_PROFILE_VC1_SIMPLE:
 *   VC-1 simple profile
 * @GST_VAAPI_PROFILE_VC1_MAIN:
 *   VC-1 main profile
 * @GST_VAAPI_PROFILE_VC1_ADVANCED:
 *   VC-1 advanced profile
 * @GST_VAAPI_PROFILE_JPEG_BASELINE:
 *   JPEG baseline profile
 * @GST_VAAPI_PROFILE_H265_MAIN:
 *   H.265 main profile [A.3.2]
 * @GST_VAAPI_PROFILE_H265_MAIN10:
 *   H.265 main 10 profile [A.3.3]
 * @GST_VAAPI_PROFILE_H265_MAIN_STILL_PICTURE:
 *   H.265 main still picture profile [A.3.4]
 *
 * The set of all profiles for #GstVaapiProfile.
 */
typedef enum {
    GST_VAAPI_PROFILE_UNKNOWN               = 0,
    GST_VAAPI_PROFILE_MPEG1                 = GST_VAAPI_MAKE_PROFILE(MPEG1,1),
    GST_VAAPI_PROFILE_MPEG2_SIMPLE          = GST_VAAPI_MAKE_PROFILE(MPEG2,1),
    GST_VAAPI_PROFILE_MPEG2_MAIN            = GST_VAAPI_MAKE_PROFILE(MPEG2,2),
    GST_VAAPI_PROFILE_MPEG2_HIGH            = GST_VAAPI_MAKE_PROFILE(MPEG2,3),
    GST_VAAPI_PROFILE_MPEG4_SIMPLE          = GST_VAAPI_MAKE_PROFILE(MPEG4,1),
    GST_VAAPI_PROFILE_MPEG4_ADVANCED_SIMPLE = GST_VAAPI_MAKE_PROFILE(MPEG4,2),
    GST_VAAPI_PROFILE_MPEG4_MAIN            = GST_VAAPI_MAKE_PROFILE(MPEG4,3),
    GST_VAAPI_PROFILE_H263_BASELINE         = GST_VAAPI_MAKE_PROFILE(H263,1),
    GST_VAAPI_PROFILE_H264_BASELINE         = GST_VAAPI_MAKE_PROFILE(H264,1),
    GST_VAAPI_PROFILE_H264_CONSTRAINED_BASELINE =
                                              GST_VAAPI_MAKE_PROFILE(H264,9),
    GST_VAAPI_PROFILE_H264_MAIN             = GST_VAAPI_MAKE_PROFILE(H264,2),
    GST_VAAPI_PROFILE_H264_EXTENDED         = GST_VAAPI_MAKE_PROFILE(H264,10),
    GST_VAAPI_PROFILE_H264_HIGH             = GST_VAAPI_MAKE_PROFILE(H264,3),
    GST_VAAPI_PROFILE_H264_HIGH10           = GST_VAAPI_MAKE_PROFILE(H264,7),
    GST_VAAPI_PROFILE_H264_HIGH_422         = GST_VAAPI_MAKE_PROFILE(H264,4),
    GST_VAAPI_PROFILE_H264_HIGH_444         = GST_VAAPI_MAKE_PROFILE(H264,8),
    GST_VAAPI_PROFILE_H264_SCALABLE_BASELINE =
                                              GST_VAAPI_MAKE_PROFILE(H264,5),
    GST_VAAPI_PROFILE_H264_SCALABLE_HIGH    = GST_VAAPI_MAKE_PROFILE(H264,6),
    GST_VAAPI_PROFILE_H264_MULTIVIEW_HIGH   = GST_VAAPI_MAKE_PROFILE(H264,11),
    GST_VAAPI_PROFILE_H264_STEREO_HIGH      = GST_VAAPI_MAKE_PROFILE(H264,15),
    GST_VAAPI_PROFILE_VC1_SIMPLE            = GST_VAAPI_MAKE_PROFILE(VC1,1),
    GST_VAAPI_PROFILE_VC1_MAIN              = GST_VAAPI_MAKE_PROFILE(VC1,2),
    GST_VAAPI_PROFILE_VC1_ADVANCED          = GST_VAAPI_MAKE_PROFILE(VC1,3),
    GST_VAAPI_PROFILE_JPEG_BASELINE         = GST_VAAPI_MAKE_PROFILE(JPEG,1),
    GST_VAAPI_PROFILE_VP8                   = GST_VAAPI_MAKE_PROFILE(VP8,1),
    GST_VAAPI_PROFILE_H265_MAIN             = GST_VAAPI_MAKE_PROFILE(H265,1),
    GST_VAAPI_PROFILE_H265_MAIN10           = GST_VAAPI_MAKE_PROFILE(H265,2),
    GST_VAAPI_PROFILE_H265_MAIN_STILL_PICTURE =
                                               GST_VAAPI_MAKE_PROFILE(H265,3),
} GstVaapiProfile;

/**
 * GstVaapiEntrypoint:
 * @GST_VAAPI_ENTRYPOINT_VLD: Variable Length Decoding
 * @GST_VAAPI_ENTRYPOINT_IDCT: Inverse Decrete Cosine Transform
 * @GST_VAAPI_ENTRYPOINT_MOCO: Motion Compensation
 * @GST_VAAPI_ENTRYPOINT_SLICE_ENCODE: Encode Slice
 * @GST_VAAPI_ENTRYPOINT_PICTURE_ENCODE: Encode Picture
 *
 * The set of all entrypoints for #GstVaapiEntrypoint
 */
typedef enum {
    GST_VAAPI_ENTRYPOINT_VLD = 1,
    GST_VAAPI_ENTRYPOINT_IDCT,
    GST_VAAPI_ENTRYPOINT_MOCO,
    GST_VAAPI_ENTRYPOINT_SLICE_ENCODE,
    GST_VAAPI_ENTRYPOINT_PICTURE_ENCODE
} GstVaapiEntrypoint;

const gchar *
gst_vaapi_codec_get_name(GstVaapiCodec codec);

GstVaapiProfile
gst_vaapi_profile(VAProfile profile);

GstVaapiProfile
gst_vaapi_profile_from_caps(const GstCaps *caps);

const gchar *
gst_vaapi_profile_get_name(GstVaapiProfile profile);

const gchar *
gst_vaapi_profile_get_media_type_name(GstVaapiProfile profile);

VAProfile
gst_vaapi_profile_get_va_profile(GstVaapiProfile profile);

GstCaps *
gst_vaapi_profile_get_caps(GstVaapiProfile profile);

GstVaapiCodec
gst_vaapi_profile_get_codec(GstVaapiProfile profile);

GstVaapiEntrypoint
gst_vaapi_entrypoint(VAEntrypoint entrypoint);

VAEntrypoint
gst_vaapi_entrypoint_get_va_entrypoint(GstVaapiEntrypoint entrypoint);

G_END_DECLS

#endif /* GST_GST_VAAPI_IMAGE_H */
