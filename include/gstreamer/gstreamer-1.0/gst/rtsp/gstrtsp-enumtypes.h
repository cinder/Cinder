


#ifndef __gst_rtsp_ENUM_TYPES_H__
#define __gst_rtsp_ENUM_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "gstrtsptransport.h" */
GType gst_rtsp_trans_mode_get_type (void);
#define GST_TYPE_RTSP_TRANS_MODE (gst_rtsp_trans_mode_get_type())
GType gst_rtsp_profile_get_type (void);
#define GST_TYPE_RTSP_PROFILE (gst_rtsp_profile_get_type())
GType gst_rtsp_lower_trans_get_type (void);
#define GST_TYPE_RTSP_LOWER_TRANS (gst_rtsp_lower_trans_get_type())

/* enumerations from "gstrtspmessage.h" */
GType gst_rtsp_msg_type_get_type (void);
#define GST_TYPE_RTSP_MSG_TYPE (gst_rtsp_msg_type_get_type())

/* enumerations from "gstrtspdefs.h" */
GType gst_rtsp_result_get_type (void);
#define GST_TYPE_RTSP_RESULT (gst_rtsp_result_get_type())
GType gst_rtsp_event_get_type (void);
#define GST_TYPE_RTSP_EVENT (gst_rtsp_event_get_type())
GType gst_rtsp_family_get_type (void);
#define GST_TYPE_RTSP_FAMILY (gst_rtsp_family_get_type())
GType gst_rtsp_state_get_type (void);
#define GST_TYPE_RTSP_STATE (gst_rtsp_state_get_type())
GType gst_rtsp_version_get_type (void);
#define GST_TYPE_RTSP_VERSION (gst_rtsp_version_get_type())
GType gst_rtsp_method_get_type (void);
#define GST_TYPE_RTSP_METHOD (gst_rtsp_method_get_type())
GType gst_rtsp_auth_method_get_type (void);
#define GST_TYPE_RTSP_AUTH_METHOD (gst_rtsp_auth_method_get_type())
GType gst_rtsp_header_field_get_type (void);
#define GST_TYPE_RTSP_HEADER_FIELD (gst_rtsp_header_field_get_type())
GType gst_rtsp_status_code_get_type (void);
#define GST_TYPE_RTSP_STATUS_CODE (gst_rtsp_status_code_get_type())

/* enumerations from "gstrtsprange.h" */
GType gst_rtsp_range_unit_get_type (void);
#define GST_TYPE_RTSP_RANGE_UNIT (gst_rtsp_range_unit_get_type())
GType gst_rtsp_time_type_get_type (void);
#define GST_TYPE_RTSP_TIME_TYPE (gst_rtsp_time_type_get_type())
G_END_DECLS

#endif /* __gst_rtsp_ENUM_TYPES_H__ */



