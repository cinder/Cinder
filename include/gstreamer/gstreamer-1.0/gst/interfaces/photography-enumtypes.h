


#ifndef __GST_PHOTOGRAPHY_ENUM_TYPES_H__
#define __GST_PHOTOGRAPHY_ENUM_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "photography.h" */
GType gst_photography_noise_reduction_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_NOISE_REDUCTION (gst_photography_noise_reduction_get_type())
GType gst_photography_white_balance_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_WHITE_BALANCE_MODE (gst_photography_white_balance_mode_get_type())
GType gst_photography_color_tone_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_COLOR_TONE_MODE (gst_photography_color_tone_mode_get_type())
GType gst_photography_scene_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_SCENE_MODE (gst_photography_scene_mode_get_type())
GType gst_photography_flash_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_FLASH_MODE (gst_photography_flash_mode_get_type())
GType gst_photography_focus_status_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_FOCUS_STATUS (gst_photography_focus_status_get_type())
GType gst_photography_caps_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_CAPS (gst_photography_caps_get_type())
GType gst_photography_shake_risk_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_SHAKE_RISK (gst_photography_shake_risk_get_type())
GType gst_photography_flicker_reduction_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_FLICKER_REDUCTION_MODE (gst_photography_flicker_reduction_mode_get_type())
GType gst_photography_focus_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_FOCUS_MODE (gst_photography_focus_mode_get_type())
GType gst_photography_exposure_mode_get_type (void);
#define GST_TYPE_PHOTOGRAPHY_EXPOSURE_MODE (gst_photography_exposure_mode_get_type())
G_END_DECLS

#endif /* __GST_PHOTOGRAPHY_ENUM_TYPES_H__ */



