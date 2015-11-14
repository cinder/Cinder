/* GStreamer
 *
 * Copyright (C) 2008 Nokia Corporation <multimedia@maemo.org>
 *
 * photography.h: photography interface for digital imaging
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_PHOTOGRAPHY_H__
#define __GST_PHOTOGRAPHY_H__

#ifndef GST_USE_UNSTABLE_API
#warning "The GstPhotography interface is unstable API and may change in future."
#warning "You can define GST_USE_UNSTABLE_API to avoid this warning." 
#endif

#include <gst/gst.h>
#include <gst/interfaces/photography-enumtypes.h>

G_BEGIN_DECLS

#define GST_TYPE_PHOTOGRAPHY \
  (gst_photography_get_type ())
#define GST_PHOTOGRAPHY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_PHOTOGRAPHY, GstPhotography))
#define GST_IS_PHOTOGRAPHY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_PHOTOGRAPHY))
#define GST_PHOTOGRAPHY_GET_INTERFACE(inst) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GST_TYPE_PHOTOGRAPHY, GstPhotographyInterface))


/**
 * GST_PHOTOGRAPHY_AUTOFOCUS_DONE:
 *
 * Name of custom GstMessage that will be posted to #GstBus when autofocusing
 * is complete.
 * This message contains following fields:
 * <itemizedlist>
 * <listitem>
 *   <para>
 *   #GstPhotographyFocusStatus
 *   <classname>&quot;status&quot;</classname>:
 *   Tells if focusing succeeded or failed.
 *   </para>
 * </listitem>
 * <listitem>
 *   <para>
 *   #G_TYPE_INT
 *   <classname>&quot;focus-window-rows&quot;</classname>:
 *   Tells number of focus matrix rows.
 *   </para>
 * </listitem>
 * <listitem>
 *   <para>
 *   #G_TYPE_INT
 *   <classname>&quot;focus-window-columns&quot;</classname>:
 *   Tells number of focus matrix columns.
 *   </para>
 * </listitem>
 * <listitem>
 *   <para>
 *   #G_TYPE_INT
 *   <classname>&quot;focus-window-mask&quot;</classname>:
 *   Bitmask containing rows x columns bits which mark the focus points in the
 *   focus matrix. Lowest bit (LSB) always represents the top-left corner of the
 *   focus matrix. This field is only valid when focusing status is SUCCESS.
 *   </para>
 * </listitem>
 * </itemizedlist>
 */
#define GST_PHOTOGRAPHY_AUTOFOCUS_DONE "autofocus-done"

/**
 * GST_PHOTOGRAPHY_SHAKE_RISK:
 *
 * Name of custom GstMessage that is posted to #GstBus during autofocusing
 * process. It is posted if there is change in the risk of captured image
 * becoming "shaken" due to camera movement and too long exposure time.
 *
 * This message contains following fields:
 * <itemizedlist>
 * <listitem>
 *   <para>
 *   #GstPhotographyShakeRisk
 *   <classname>&quot;status&quot;</classname>:
 *   Tells risk level of capturing shaken image.
 *   </para>
 * </listitem>
 * </itemizedlist>
 */
#define GST_PHOTOGRAPHY_SHAKE_RISK "shake-risk"

/* Maximum white point values used in #GstPhotographySettings */
#define MAX_WHITE_POINT_VALUES 4

/* Interface property names */
#define GST_PHOTOGRAPHY_PROP_WB_MODE      "white-balance-mode"
#define GST_PHOTOGRAPHY_PROP_COLOR_TONE   "color-tone-mode"
#define GST_PHOTOGRAPHY_PROP_SCENE_MODE   "scene-mode"
#define GST_PHOTOGRAPHY_PROP_FLASH_MODE   "flash-mode"
#define GST_PHOTOGRAPHY_PROP_NOISE_REDUCTION   "noise-reduction"
#define GST_PHOTOGRAPHY_PROP_FOCUS_STATUS "focus-status"
#define GST_PHOTOGRAPHY_PROP_CAPABILITIES "capabilities"
#define GST_PHOTOGRAPHY_PROP_SHAKE_RISK   "shake-risk"
#define GST_PHOTOGRAPHY_PROP_EV_COMP      "ev-compensation"
#define GST_PHOTOGRAPHY_PROP_ISO_SPEED    "iso-speed"
#define GST_PHOTOGRAPHY_PROP_APERTURE     "aperture"
#define GST_PHOTOGRAPHY_PROP_EXPOSURE_TIME     "exposure-time"
#define GST_PHOTOGRAPHY_PROP_IMAGE_CAPTURE_SUPPORTED_CAPS \
    "image-capture-supported-caps"
#define GST_PHOTOGRAPHY_PROP_IMAGE_PREVIEW_SUPPORTED_CAPS \
    "image-preview-supported-caps"
#define GST_PHOTOGRAPHY_PROP_FLICKER_MODE "flicker-mode"
#define GST_PHOTOGRAPHY_PROP_FOCUS_MODE   "focus-mode"
#define GST_PHOTOGRAPHY_PROP_ZOOM   "zoom"
#define GST_PHOTOGRAPHY_PROP_COLOR_TEMPERATURE "color-temperature"
#define GST_PHOTOGRAPHY_PROP_WHITE_POINT  "white-point"
#define GST_PHOTOGRAPHY_PROP_ANALOG_GAIN  "analog-gain"
#define GST_PHOTOGRAPHY_PROP_EXPOSURE_MODE     "exposure-mode"
#define GST_PHOTOGRAPHY_PROP_LENS_FOCUS   "lens-focus"
#define GST_PHOTOGRAPHY_PROP_MIN_EXPOSURE_TIME "min-exposure-time"
#define GST_PHOTOGRAPHY_PROP_MAX_EXPOSURE_TIME "max-exposure-time"

/**
 * GstPhotography:
 *
 * Opaque #GstPhotography data structure.
 */
typedef struct _GstPhotography GstPhotography;

/**
 * GstPhotographyNoiseReduction:
 * @GST_PHOTOGRAPHY_NOISE_REDUCTION_BAYER: Adaptive noise reduction on Bayer
 * format
 * @GST_PHOTOGRAPHY_NOISE_REDUCTION_YCC: reduces the noise on Y and 2-chroma
 * images.
 * @GST_PHOTOGRAPHY_NOISE_REDUCTION_TEMPORAL: Multi-frame adaptive NR,
 * provided for the video mode
 * @GST_PHOTOGRAPHY_NOISE_REDUCTION_FIXED: Fixed Pattern Noise refers to noise
 * that does not change between frames. The noise is removed from the sensor
 * image, by subtracting a previously-captured black image in memory.
 * @GST_PHOTOGRAPHY_NOISE_REDUCTION_EXTRA: Extra Noise Reduction. In the case
 * of high-ISO capturing, some noise remains after YCC NR. XNR reduces this
 * remaining noise.
 *
 * Noise Reduction features of a photography capture or filter element.
 *
 * Since: 0.10.21
 */
typedef enum
{
  GST_PHOTOGRAPHY_NOISE_REDUCTION_BAYER = ( 1<<0 ),
  GST_PHOTOGRAPHY_NOISE_REDUCTION_YCC = ( 1<<1 ),
  GST_PHOTOGRAPHY_NOISE_REDUCTION_TEMPORAL= ( 1<< 2),
  GST_PHOTOGRAPHY_NOISE_REDUCTION_FIXED = (1 << 3),
  GST_PHOTOGRAPHY_NOISE_REDUCTION_EXTRA = (1 << 4)
} GstPhotographyNoiseReduction;

/**
 * GstPhotographyWhiteBalanceMode:
 * @GST_PHOTOGRAPHY_WB_MODE_AUTO: Choose white balance mode automatically
 * @GST_PHOTOGRAPHY_WB_MODE_DAYLIGHT: Mode for daylight conditions
 * @GST_PHOTOGRAPHY_WB_MODE_CLOUDY: Mode for cloudy conditions
 * @GST_PHOTOGRAPHY_WB_MODE_SUNSET: Mode for sunset conditions
 * @GST_PHOTOGRAPHY_WB_MODE_TUNGSTEN: Mode for tungsten lighting
 * @GST_PHOTOGRAPHY_WB_MODE_FLUORESCENT: Mode for fluorescent lighting
 * @GST_PHOTOGRAPHY_WB_MODE_MANUAL: Disable automatic white balance adjustment
 * and keep current values.
 * @GST_PHOTOGRAPHY_WB_MODE_WARM_FLUORESCENT: Mode for warm fluorescent lighting (Since 1.2)
 * @GST_PHOTOGRAPHY_WB_MODE_SHADE: Mode for shade lighting (Since 1.2)
 *
 * Modes for white balance control.
 */
typedef enum
{
  GST_PHOTOGRAPHY_WB_MODE_AUTO = 0,
  GST_PHOTOGRAPHY_WB_MODE_DAYLIGHT,
  GST_PHOTOGRAPHY_WB_MODE_CLOUDY,
  GST_PHOTOGRAPHY_WB_MODE_SUNSET,
  GST_PHOTOGRAPHY_WB_MODE_TUNGSTEN,
  GST_PHOTOGRAPHY_WB_MODE_FLUORESCENT,
  GST_PHOTOGRAPHY_WB_MODE_MANUAL,
  GST_PHOTOGRAPHY_WB_MODE_WARM_FLUORESCENT,
  GST_PHOTOGRAPHY_WB_MODE_SHADE
} GstPhotographyWhiteBalanceMode;

/**
 * GstPhotographyColorToneMode:
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_NORMAL: No effects
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_SEPIA: Sepia
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_NEGATIVE: Negative
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_GRAYSCALE: Grayscale
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_NATURAL: Natural
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_VIVID: Vivid
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_COLORSWAP: Colorswap
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_SOLARIZE: Solarize
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_OUT_OF_FOCUS: Out of focus
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_SKY_BLUE: Sky blue
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_GRASS_GREEN: Grass green
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_SKIN_WHITEN: Skin whiten
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_POSTERIZE: Posterize (Since 1.2)
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_WHITEBOARD: Whiteboard (Since 1.2)
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_BLACKBOARD: Blackboard (Since 1.2)
 * @GST_PHOTOGRAPHY_COLOR_TONE_MODE_AQUA: Aqua (Since 1.2)
 *
 * Modes for special color effects.
 */
typedef enum
{
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_NORMAL = 0,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_SEPIA,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_NEGATIVE,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_GRAYSCALE,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_NATURAL,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_VIVID,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_COLORSWAP,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_SOLARIZE,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_OUT_OF_FOCUS,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_SKY_BLUE,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_GRASS_GREEN,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_SKIN_WHITEN,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_POSTERIZE,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_WHITEBOARD,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_BLACKBOARD,
  GST_PHOTOGRAPHY_COLOR_TONE_MODE_AQUA
} GstPhotographyColorToneMode;

/**
 * GstPhotographySceneMode:
 * @GST_PHOTOGRAPHY_SCENE_MODE_MANUAL: Set individual options manually
 * @GST_PHOTOGRAPHY_SCENE_MODE_CLOSEUP: Mode for close objects
 * @GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT: Mode for portraits
 * @GST_PHOTOGRAPHY_SCENE_MODE_LANDSCAPE: Mode for landscapes
 * @GST_PHOTOGRAPHY_SCENE_MODE_SPORT: Mode for scene with fast motion
 * @GST_PHOTOGRAPHY_SCENE_MODE_NIGHT: Mode for night conditions
 * @GST_PHOTOGRAPHY_SCENE_MODE_AUTO: Choose scene mode automatically
 * @GST_PHOTOGRAPHY_SCENE_MODE_ACTION: Take photos of fast moving
 *     objects (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_NIGHT_PORTRAIT: Take people pictures
 *     at night (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_THEATRE: Take photos in a theater (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_BEACH: Take pictures on the beach (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_SNOW: Take pictures on the snow (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_SUNSET: Take sunset photos (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_STEADY_PHOTO: Avoid blurry pictures
 *     (for example, due to hand shake) (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_FIREWORKS: For shooting firework
 *     displays (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_PARTY: Take indoor low-light shot (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_CANDLELIGHT: Capture the naturally warm color
 *     of scenes lit by candles (Since 1.2)
 * @GST_PHOTOGRAPHY_SCENE_MODE_BARCODE: Applications are looking for
 *     a barcode (Since 1.2)
 *
 * Each mode contains preset #GstPhotography options in order to produce
 * good capturing result in certain scene.
 */
typedef enum
{
  GST_PHOTOGRAPHY_SCENE_MODE_MANUAL = 0,
  GST_PHOTOGRAPHY_SCENE_MODE_CLOSEUP,
  GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT,
  GST_PHOTOGRAPHY_SCENE_MODE_LANDSCAPE,
  GST_PHOTOGRAPHY_SCENE_MODE_SPORT,
  GST_PHOTOGRAPHY_SCENE_MODE_NIGHT,
  GST_PHOTOGRAPHY_SCENE_MODE_AUTO,
  GST_PHOTOGRAPHY_SCENE_MODE_ACTION,
  GST_PHOTOGRAPHY_SCENE_MODE_NIGHT_PORTRAIT,
  GST_PHOTOGRAPHY_SCENE_MODE_THEATRE,
  GST_PHOTOGRAPHY_SCENE_MODE_BEACH,
  GST_PHOTOGRAPHY_SCENE_MODE_SNOW,
  GST_PHOTOGRAPHY_SCENE_MODE_SUNSET,
  GST_PHOTOGRAPHY_SCENE_MODE_STEADY_PHOTO,
  GST_PHOTOGRAPHY_SCENE_MODE_FIREWORKS,
  GST_PHOTOGRAPHY_SCENE_MODE_PARTY,
  GST_PHOTOGRAPHY_SCENE_MODE_CANDLELIGHT,
  GST_PHOTOGRAPHY_SCENE_MODE_BARCODE
} GstPhotographySceneMode;

/**
 * GstPhotographyFlashMode:
 * @GST_PHOTOGRAPHY_FLASH_MODE_AUTO: Fire flash automatically according to
 * lighting conditions.
 * @GST_PHOTOGRAPHY_FLASH_MODE_OFF: Never fire flash
 * @GST_PHOTOGRAPHY_FLASH_MODE_ON: Always fire flash
 * @GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN: Fill in flash
 * @GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE: Flash mode for reducing chance of
 * capturing red eyes
 *
 * Modes for flash control.
 */
typedef enum
{
  GST_PHOTOGRAPHY_FLASH_MODE_AUTO = 0,
  GST_PHOTOGRAPHY_FLASH_MODE_OFF,
  GST_PHOTOGRAPHY_FLASH_MODE_ON,
  GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN,
  GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE
} GstPhotographyFlashMode;

/**
 * GstPhotographyFocusStatus:
 * @GST_PHOTOGRAPHY_FOCUS_STATUS_NONE: No status available
 * @GST_PHOTOGRAPHY_FOCUS_STATUS_RUNNING: Focusing is ongoing
 * @GST_PHOTOGRAPHY_FOCUS_STATUS_FAIL: Focusing failed
 * @GST_PHOTOGRAPHY_FOCUS_STATUS_SUCCESS: Focusing succeeded
 *
 * Status of the focusing operation, used in #GST_PHOTOGRAPHY_AUTOFOCUS_DONE
 * message.
 */
typedef enum
{
  GST_PHOTOGRAPHY_FOCUS_STATUS_NONE = 0,
  GST_PHOTOGRAPHY_FOCUS_STATUS_RUNNING,
  GST_PHOTOGRAPHY_FOCUS_STATUS_FAIL,
  GST_PHOTOGRAPHY_FOCUS_STATUS_SUCCESS
} GstPhotographyFocusStatus;

/**
 * GstPhotographyCaps:
 *
 * Bitmask that indicates which #GstPhotography interface features an instance
 * supports.
 */
typedef enum
{
  GST_PHOTOGRAPHY_CAPS_NONE = (0 << 0),
  GST_PHOTOGRAPHY_CAPS_EV_COMP = (1 << 0),
  GST_PHOTOGRAPHY_CAPS_ISO_SPEED = (1 << 1),
  GST_PHOTOGRAPHY_CAPS_WB_MODE = (1 << 2),
  GST_PHOTOGRAPHY_CAPS_TONE = (1 << 3),
  GST_PHOTOGRAPHY_CAPS_SCENE = (1 << 4),
  GST_PHOTOGRAPHY_CAPS_FLASH = (1 << 5),
  GST_PHOTOGRAPHY_CAPS_ZOOM = (1 << 6),
  GST_PHOTOGRAPHY_CAPS_FOCUS = (1 << 7),
  GST_PHOTOGRAPHY_CAPS_APERTURE = (1 << 8),
  GST_PHOTOGRAPHY_CAPS_EXPOSURE = (1 << 9),
  GST_PHOTOGRAPHY_CAPS_SHAKE = (1 << 10),
  GST_PHOTOGRAPHY_CAPS_WHITE_BALANCE = (1 << 11),
  GST_PHOTOGRAPHY_CAPS_NOISE_REDUCTION = (1 << 12),
  GST_PHOTOGRAPHY_CAPS_FLICKER_REDUCTION = (1 << 13),
  GST_PHOTOGRAPHY_CAPS_ALL = (~0)
} GstPhotographyCaps;

/**
 * GstPhotographyShakeRisk:
 * @GST_PHOTOGRAPHY_SHAKE_RISK_LOW: Low risk
 * @GST_PHOTOGRAPHY_SHAKE_RISK_MEDIUM: Medium risk
 * @GST_PHOTOGRAPHY_SHAKE_RISK_HIGH: High risk
 *
 * Risk level of captured image becoming "shaken" due to camera movement and
 * too long exposure time. Used in #GST_PHOTOGRAPHY_SHAKE_RISK #GstMessage.
 */
typedef enum
{
  GST_PHOTOGRAPHY_SHAKE_RISK_LOW = 0,
  GST_PHOTOGRAPHY_SHAKE_RISK_MEDIUM,
  GST_PHOTOGRAPHY_SHAKE_RISK_HIGH,
} GstPhotographyShakeRisk;

/**
 * GstPhotographyFlickerReductionMode:
 * @GST_PHOTOGRAPHY_FLICKER_REDUCTION_OFF: Disable flicker reduction
 * @GST_PHOTOGRAPHY_FLICKER_REDUCTION_50HZ: 50Hz flicker reduction
 * @GST_PHOTOGRAPHY_FLICKER_REDUCTION_60HZ: 60Hz flicker reduction
 * @GST_PHOTOGRAPHY_FLICKER_REDUCTION_AUTO: Choose mode automatically
 *
 * Reduce flicker in video caused by light source fluctuation.
 */
typedef enum
{
  GST_PHOTOGRAPHY_FLICKER_REDUCTION_OFF = 0,
  GST_PHOTOGRAPHY_FLICKER_REDUCTION_50HZ,
  GST_PHOTOGRAPHY_FLICKER_REDUCTION_60HZ,
  GST_PHOTOGRAPHY_FLICKER_REDUCTION_AUTO,
} GstPhotographyFlickerReductionMode;

/**
 * GstPhotographyFocusMode:
 * @GST_PHOTOGRAPHY_FOCUS_MODE_AUTO: Choose focus mode automatically
 * @GST_PHOTOGRAPHY_FOCUS_MODE_MACRO: Mode for focusing objects close to lens
 * @GST_PHOTOGRAPHY_FOCUS_MODE_PORTRAIT: Mode for portraits
 * @GST_PHOTOGRAPHY_FOCUS_MODE_INFINITY: Mode for landscapes and far away objects
 * @GST_PHOTOGRAPHY_FOCUS_MODE_HYPERFOCAL: Mode for maximum depth of field, keeping
 * focus acceptable both in infinify and as close objects as possible
 * @GST_PHOTOGRAPHY_FOCUS_MODE_EXTENDED: Extended focus mode
 * @GST_PHOTOGRAPHY_FOCUS_MODE_CONTINUOUS_NORMAL: Continuous autofocus mode
 * @GST_PHOTOGRAPHY_FOCUS_MODE_CONTINUOUS_EXTENDED: Extended continuous
 * autofocus mode
 * @GST_PHOTOGRAPHY_FOCUS_MODE_MANUAL: Disable automatic focusing
 * and keep current value. #GstPhotography:lens-focus property can
 * be used to change focus manually.
 *
 * Choose mode for focusing algorithm.
 */
typedef enum {
    GST_PHOTOGRAPHY_FOCUS_MODE_AUTO = 0,
    GST_PHOTOGRAPHY_FOCUS_MODE_MACRO,
    GST_PHOTOGRAPHY_FOCUS_MODE_PORTRAIT,
    GST_PHOTOGRAPHY_FOCUS_MODE_INFINITY,
    GST_PHOTOGRAPHY_FOCUS_MODE_HYPERFOCAL,
    GST_PHOTOGRAPHY_FOCUS_MODE_EXTENDED,
    GST_PHOTOGRAPHY_FOCUS_MODE_CONTINUOUS_NORMAL,
    GST_PHOTOGRAPHY_FOCUS_MODE_CONTINUOUS_EXTENDED,
    GST_PHOTOGRAPHY_FOCUS_MODE_MANUAL
} GstPhotographyFocusMode;

/**
 * GstPhotographyExposureMode:
 * @GST_PHOTOGRAPHY_EXPOSURE_MODE_AUTO: Adjust exposure automatically
 * @GST_PHOTOGRAPHY_EXPOSURE_MODE_MANUAL: Disable automatic exposure adjustment
 * and keep current values.
 *
 */
typedef enum {
    GST_PHOTOGRAPHY_EXPOSURE_MODE_AUTO = 0,
    GST_PHOTOGRAPHY_EXPOSURE_MODE_MANUAL
} GstPhotographyExposureMode;

/**
 * GstPhotographySettings:
 *
 * Structure containing all #GstPhotography settings, used to set all
 * settings in one call with @gst_photography_set_config().
 */
typedef struct
{
  GstPhotographyWhiteBalanceMode wb_mode;
  GstPhotographyColorToneMode tone_mode;
  GstPhotographySceneMode scene_mode;
  GstPhotographyFlashMode flash_mode;
  guint32 exposure_time;
  guint aperture;
  gfloat ev_compensation;
  guint iso_speed;
  gfloat zoom;
  GstPhotographyFlickerReductionMode flicker_mode;
  GstPhotographyFocusMode focus_mode;
  GstPhotographyNoiseReduction noise_reduction;
  GstPhotographyExposureMode exposure_mode;
  guint color_temperature;
  guint white_point[MAX_WHITE_POINT_VALUES];
  gfloat analog_gain;
  gfloat lens_focus;
  guint min_exposure_time;
  guint max_exposure_time;
} GstPhotographySettings;

/**
 * GstPhotographyCapturePrepared:
 * @data: user data that has been given, when registering the callback
 * @configured_caps: #GstCaps defining the configured capture format.
 *     Ownership of these caps stays in the element.
 *
 * This callback will be called when the element has finished preparations
 * and is ready for image capture. The next buffer that element produces
 * will be of @configured_caps format, so this callback allows the application
 * to e.g. reconfigure capsfilters in pipeline if any.
 */
typedef void (*GstPhotographyCapturePrepared) (gpointer data,
    const GstCaps *configured_caps);

/**
 * GstPhotographyInterface:
 * @parent: parent interface type.
 * @get_ev_compensation: vmethod to get ev exposure compensation value
 * @get_iso_speed: vmethod to get iso speed (light sensitivity) value
 * @get_aperture: vmethod to get aperture value
 * @get_exposure: vmethod to get exposure time value
 * @get_white_balance_mode: vmethod to get white balance mode value
 * @get_color_tone_mode: vmethod to get color tone mode value
 * @get_scene_mode: vmethod to get scene mode value
 * @get_flash_mode: vmethod to get flash mode value
 * @get_noise_reduction: vmethod to get noise reduction mode value
 * @get_zoom: vmethod to get zoom factor value
 * @set_ev_compensation: vmethod to set ev exposure compensation value
 * @set_iso_speed: vmethod to set iso speed (light sensitivity) value
 * @set_aperture: vmethod to set aperture value
 * @set_exposure: vmethod to set exposure time value
 * @set_white_balance_mode: vmethod to set white balance mode value
 * @set_color_tone_mode: vmethod to set color tone mode value
 * @set_scene_mode: vmethod to set scene mode value
 * @set_flash_mode: vmethod to set flash mode value
 * @set_noise_reduction: vmethod to set noise reduction mode value
 * @set_zoom: vmethod to set zoom factor value
 * @get_capabilities: vmethod to get supported capabilities of the interface
 * @prepare_for_capture: vmethod to tell the element to prepare for capturing
 * @set_autofocus: vmethod to set autofocus on/off
 * @set_config: vmethod to set all configuration parameters at once
 * @get_config: vmethod to get all configuration parameters at once
 * @get_image_capture_supported_caps: vmethod to get caps describing supported image capture formats
 *
 * #GstPhotographyInterface interface.
 */
typedef struct _GstPhotographyInterface
{
  GTypeInterface parent;

  /* virtual functions */
  gboolean (*get_ev_compensation) (GstPhotography * photo, gfloat * ev_comp);
  gboolean (*get_iso_speed) (GstPhotography * photo, guint * iso_speed);
  gboolean (*get_aperture) (GstPhotography * photo, guint * aperture);
  gboolean (*get_exposure) (GstPhotography * photo, guint32 * exposure);
  gboolean (*get_white_balance_mode) (GstPhotography * photo,
    GstPhotographyWhiteBalanceMode * wb_mode);
  gboolean (*get_color_tone_mode) (GstPhotography * photo,
    GstPhotographyColorToneMode * tone_mode);
  gboolean (*get_scene_mode) (GstPhotography * photo,
    GstPhotographySceneMode * scene_mode);
  gboolean (*get_flash_mode) (GstPhotography * photo,
    GstPhotographyFlashMode * flash_mode);
  gboolean (*get_zoom) (GstPhotography * photo, gfloat * zoom);
    gboolean (*get_flicker_mode) (GstPhotography * photo,
    GstPhotographyFlickerReductionMode * flicker_mode);
  gboolean (*get_focus_mode) (GstPhotography * photo,
    GstPhotographyFocusMode * focus_mode);

  gboolean (*set_ev_compensation) (GstPhotography * photo, gfloat ev_comp);
  gboolean (*set_iso_speed) (GstPhotography * photo, guint iso_speed);
  gboolean (*set_aperture) (GstPhotography * photo, guint aperture);
  gboolean (*set_exposure) (GstPhotography * photo, guint32 exposure);
  gboolean (*set_white_balance_mode) (GstPhotography * photo,
    GstPhotographyWhiteBalanceMode wb_mode);
  gboolean (*set_color_tone_mode) (GstPhotography * photo,
    GstPhotographyColorToneMode tone_mode);
  gboolean (*set_scene_mode) (GstPhotography * photo,
    GstPhotographySceneMode scene_mode);
  gboolean (*set_flash_mode) (GstPhotography * photo,
    GstPhotographyFlashMode flash_mode);
  gboolean (*set_zoom) (GstPhotography * photo, gfloat zoom);
  gboolean (*set_flicker_mode) (GstPhotography * photo,
    GstPhotographyFlickerReductionMode flicker_mode);
  gboolean (*set_focus_mode) (GstPhotography * photo,
    GstPhotographyFocusMode focus_mode);

  GstPhotographyCaps (*get_capabilities) (GstPhotography * photo);
  gboolean (*prepare_for_capture) (GstPhotography * photo,
    GstPhotographyCapturePrepared func, GstCaps *capture_caps, gpointer user_data);
  void (*set_autofocus) (GstPhotography * photo, gboolean on);
  gboolean (*set_config) (GstPhotography * photo, GstPhotographySettings * config);
  gboolean (*get_config) (GstPhotography * photo, GstPhotographySettings * config);

  gboolean (*get_noise_reduction) (GstPhotography * photo,
    GstPhotographyNoiseReduction * noise_reduction);
  gboolean (*set_noise_reduction) (GstPhotography * photo,
    GstPhotographyNoiseReduction noise_reduction);

  /*< private > */
  gpointer _gst_reserved[GST_PADDING];
} GstPhotographyInterface;

GType gst_photography_get_type (void);

/* virtual class function wrappers */
gboolean gst_photography_get_ev_compensation (GstPhotography * photo,
    gfloat * ev_comp);
gboolean gst_photography_get_iso_speed (GstPhotography * photo,
    guint * iso_speed);
gboolean gst_photography_get_aperture (GstPhotography * photo,
    guint * aperture);
gboolean gst_photography_get_exposure (GstPhotography * photo,
    guint32 * exposure);
gboolean gst_photography_get_white_balance_mode (GstPhotography * photo,
    GstPhotographyWhiteBalanceMode * wb_mode);
gboolean gst_photography_get_color_tone_mode (GstPhotography * photo,
    GstPhotographyColorToneMode * tone_mode);
gboolean gst_photography_get_scene_mode (GstPhotography * photo,
    GstPhotographySceneMode * scene_mode);
gboolean gst_photography_get_flash_mode (GstPhotography * photo,
    GstPhotographyFlashMode * flash_mode);
gboolean gst_photography_get_noise_reduction (GstPhotography * photo,
    GstPhotographyNoiseReduction * noise_reduction);
gboolean gst_photography_get_zoom (GstPhotography * photo, gfloat * zoom);
gboolean gst_photography_get_flicker_mode (GstPhotography * photo,
    GstPhotographyFlickerReductionMode *mode);
gboolean gst_photography_get_focus_mode (GstPhotography * photo,
    GstPhotographyFocusMode *mode);

gboolean gst_photography_set_ev_compensation (GstPhotography * photo,
    gfloat ev_comp);
gboolean gst_photography_set_iso_speed (GstPhotography * photo,
    guint iso_speed);
gboolean gst_photography_set_aperture (GstPhotography * photo, guint aperture);
gboolean gst_photography_set_exposure (GstPhotography * photo, guint exposure);
gboolean gst_photography_set_white_balance_mode (GstPhotography * photo,
    GstPhotographyWhiteBalanceMode wb_mode);
gboolean gst_photography_set_color_tone_mode (GstPhotography * photo,
    GstPhotographyColorToneMode tone_mode);
gboolean gst_photography_set_scene_mode (GstPhotography * photo,
    GstPhotographySceneMode scene_mode);
gboolean gst_photography_set_flash_mode (GstPhotography * photo,
    GstPhotographyFlashMode flash_mode);
gboolean gst_photography_set_noise_reduction (GstPhotography * photo,
    GstPhotographyNoiseReduction noise_reduction);
gboolean gst_photography_set_zoom (GstPhotography * photo, gfloat zoom);
gboolean gst_photography_set_flicker_mode (GstPhotography * photo,
    GstPhotographyFlickerReductionMode mode);
gboolean gst_photography_set_focus_mode (GstPhotography * photo,
    GstPhotographyFocusMode mode);

GstPhotographyCaps gst_photography_get_capabilities (GstPhotography * photo);

gboolean gst_photography_prepare_for_capture (GstPhotography * photo,
    GstPhotographyCapturePrepared func, GstCaps *capture_caps, gpointer user_data);

void gst_photography_set_autofocus (GstPhotography * photo, gboolean on);

gboolean gst_photography_set_config (GstPhotography * photo,
    GstPhotographySettings * config);
gboolean gst_photography_get_config (GstPhotography * photo,
    GstPhotographySettings * config);

G_END_DECLS

#endif /* __GST_PHOTOGRAPHY_H__ */
