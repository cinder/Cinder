


#ifndef __GST_VIDEO_ENUM_TYPES_H__
#define __GST_VIDEO_ENUM_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "video-format.h" */
GType gst_video_format_get_type (void);
#define GST_TYPE_VIDEO_FORMAT (gst_video_format_get_type())
GType gst_video_format_flags_get_type (void);
#define GST_TYPE_VIDEO_FORMAT_FLAGS (gst_video_format_flags_get_type())
GType gst_video_pack_flags_get_type (void);
#define GST_TYPE_VIDEO_PACK_FLAGS (gst_video_pack_flags_get_type())

/* enumerations from "video-color.h" */
GType gst_video_color_range_get_type (void);
#define GST_TYPE_VIDEO_COLOR_RANGE (gst_video_color_range_get_type())
GType gst_video_color_matrix_get_type (void);
#define GST_TYPE_VIDEO_COLOR_MATRIX (gst_video_color_matrix_get_type())
GType gst_video_transfer_function_get_type (void);
#define GST_TYPE_VIDEO_TRANSFER_FUNCTION (gst_video_transfer_function_get_type())
GType gst_video_color_primaries_get_type (void);
#define GST_TYPE_VIDEO_COLOR_PRIMARIES (gst_video_color_primaries_get_type())

/* enumerations from "video-info.h" */
GType gst_video_interlace_mode_get_type (void);
#define GST_TYPE_VIDEO_INTERLACE_MODE (gst_video_interlace_mode_get_type())
GType gst_video_multiview_mode_get_type (void);
#define GST_TYPE_VIDEO_MULTIVIEW_MODE (gst_video_multiview_mode_get_type())
GType gst_video_multiview_frame_packing_get_type (void);
#define GST_TYPE_VIDEO_MULTIVIEW_FRAME_PACKING (gst_video_multiview_frame_packing_get_type())
GType gst_video_multiview_flags_get_type (void);
#define GST_TYPE_VIDEO_MULTIVIEW_FLAGS (gst_video_multiview_flags_get_type())
GType gst_video_flags_get_type (void);
#define GST_TYPE_VIDEO_FLAGS (gst_video_flags_get_type())

/* enumerations from "video-dither.h" */
GType gst_video_dither_method_get_type (void);
#define GST_TYPE_VIDEO_DITHER_METHOD (gst_video_dither_method_get_type())
GType gst_video_dither_flags_get_type (void);
#define GST_TYPE_VIDEO_DITHER_FLAGS (gst_video_dither_flags_get_type())

/* enumerations from "colorbalance.h" */
GType gst_color_balance_type_get_type (void);
#define GST_TYPE_COLOR_BALANCE_TYPE (gst_color_balance_type_get_type())

/* enumerations from "navigation.h" */
GType gst_navigation_command_get_type (void);
#define GST_TYPE_NAVIGATION_COMMAND (gst_navigation_command_get_type())
GType gst_navigation_query_type_get_type (void);
#define GST_TYPE_NAVIGATION_QUERY_TYPE (gst_navigation_query_type_get_type())
GType gst_navigation_message_type_get_type (void);
#define GST_TYPE_NAVIGATION_MESSAGE_TYPE (gst_navigation_message_type_get_type())
GType gst_navigation_event_type_get_type (void);
#define GST_TYPE_NAVIGATION_EVENT_TYPE (gst_navigation_event_type_get_type())

/* enumerations from "video-chroma.h" */
GType gst_video_chroma_site_get_type (void);
#define GST_TYPE_VIDEO_CHROMA_SITE (gst_video_chroma_site_get_type())
GType gst_video_chroma_method_get_type (void);
#define GST_TYPE_VIDEO_CHROMA_METHOD (gst_video_chroma_method_get_type())
GType gst_video_chroma_flags_get_type (void);
#define GST_TYPE_VIDEO_CHROMA_FLAGS (gst_video_chroma_flags_get_type())

/* enumerations from "video-tile.h" */
GType gst_video_tile_type_get_type (void);
#define GST_TYPE_VIDEO_TILE_TYPE (gst_video_tile_type_get_type())
GType gst_video_tile_mode_get_type (void);
#define GST_TYPE_VIDEO_TILE_MODE (gst_video_tile_mode_get_type())

/* enumerations from "video-converter.h" */
GType gst_video_alpha_mode_get_type (void);
#define GST_TYPE_VIDEO_ALPHA_MODE (gst_video_alpha_mode_get_type())
GType gst_video_chroma_mode_get_type (void);
#define GST_TYPE_VIDEO_CHROMA_MODE (gst_video_chroma_mode_get_type())
GType gst_video_matrix_mode_get_type (void);
#define GST_TYPE_VIDEO_MATRIX_MODE (gst_video_matrix_mode_get_type())
GType gst_video_gamma_mode_get_type (void);
#define GST_TYPE_VIDEO_GAMMA_MODE (gst_video_gamma_mode_get_type())
GType gst_video_primaries_mode_get_type (void);
#define GST_TYPE_VIDEO_PRIMARIES_MODE (gst_video_primaries_mode_get_type())

/* enumerations from "video-resampler.h" */
GType gst_video_resampler_method_get_type (void);
#define GST_TYPE_VIDEO_RESAMPLER_METHOD (gst_video_resampler_method_get_type())
GType gst_video_resampler_flags_get_type (void);
#define GST_TYPE_VIDEO_RESAMPLER_FLAGS (gst_video_resampler_flags_get_type())

/* enumerations from "video-frame.h" */
GType gst_video_frame_flags_get_type (void);
#define GST_TYPE_VIDEO_FRAME_FLAGS (gst_video_frame_flags_get_type())
GType gst_video_buffer_flags_get_type (void);
#define GST_TYPE_VIDEO_BUFFER_FLAGS (gst_video_buffer_flags_get_type())
GType gst_video_frame_map_flags_get_type (void);
#define GST_TYPE_VIDEO_FRAME_MAP_FLAGS (gst_video_frame_map_flags_get_type())

/* enumerations from "video-scaler.h" */
GType gst_video_scaler_flags_get_type (void);
#define GST_TYPE_VIDEO_SCALER_FLAGS (gst_video_scaler_flags_get_type())
G_END_DECLS

#endif /* __GST_VIDEO_ENUM_TYPES_H__ */



