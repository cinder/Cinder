/*
 * gstmpegtsdescriptor.h - 
 * Copyright (C) 2013 Edward Hervey
 * 
 * Authors:
 *   Edward Hervey <edward@collabora.com>
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
 *
 * Some parts of this code come from the Fluendo MPEG Demuxer plugin.
 *
 * The Initial Developer of the Original Code is Fluendo, S.L.
 * Portions created by Fluendo, S.L. are Copyright (C) 2005
 * Fluendo, S.L. All Rights Reserved.
 *
 * Contributor(s): Wim Taymans <wim@fluendo.com>
 */

#ifndef GST_DVB_DESCRIPTOR_H
#define GST_DVB_DESCRIPTOR_H

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * GstMpegtsDVBDescriptorType:
 *
 * The type of #GstMpegtsDescriptor
 *
 * These values correspond to the registered descriptor type from
 * the various DVB specifications.
 *
 * Consult the relevant specifications for more details.
 */
typedef enum {
  /* 64-127 DVB tags ETSI EN 300 468
   * (Specification for Service Information (SI) in DVB systems)
   */
  GST_MTS_DESC_DVB_NETWORK_NAME                 = 0x40,
  GST_MTS_DESC_DVB_SERVICE_LIST                 = 0x41,
  GST_MTS_DESC_DVB_STUFFING                     = 0x42,
  GST_MTS_DESC_DVB_SATELLITE_DELIVERY_SYSTEM    = 0x43,
  GST_MTS_DESC_DVB_CABLE_DELIVERY_SYSTEM        = 0x44,
  GST_MTS_DESC_DVB_VBI_DATA                     = 0x45,
  GST_MTS_DESC_DVB_VBI_TELETEXT                 = 0x46,
  GST_MTS_DESC_DVB_BOUQUET_NAME                 = 0x47,
  GST_MTS_DESC_DVB_SERVICE                      = 0x48,
  GST_MTS_DESC_DVB_COUNTRY_AVAILABILITY         = 0x49,
  GST_MTS_DESC_DVB_LINKAGE                      = 0x4A,
  GST_MTS_DESC_DVB_NVOD_REFERENCE               = 0x4B,
  GST_MTS_DESC_DVB_TIME_SHIFTED_SERVICE         = 0x4C,
  GST_MTS_DESC_DVB_SHORT_EVENT                  = 0x4D,
  GST_MTS_DESC_DVB_EXTENDED_EVENT               = 0x4E,
  GST_MTS_DESC_DVB_TIME_SHIFTED_EVENT           = 0x4F,
  GST_MTS_DESC_DVB_COMPONENT                    = 0x50,
  GST_MTS_DESC_DVB_MOSAIC                       = 0x51,
  GST_MTS_DESC_DVB_STREAM_IDENTIFIER            = 0x52,
  GST_MTS_DESC_DVB_CA_IDENTIFIER                = 0x53,
  GST_MTS_DESC_DVB_CONTENT                      = 0x54,
  GST_MTS_DESC_DVB_PARENTAL_RATING              = 0x55,
  GST_MTS_DESC_DVB_TELETEXT                     = 0x56,
  GST_MTS_DESC_DVB_TELEPHONE                    = 0x57,
  GST_MTS_DESC_DVB_LOCAL_TIME_OFFSET            = 0x58,
  GST_MTS_DESC_DVB_SUBTITLING                   = 0x59,
  GST_MTS_DESC_DVB_TERRESTRIAL_DELIVERY_SYSTEM  = 0x5A,
  GST_MTS_DESC_DVB_MULTILINGUAL_NETWORK_NAME    = 0x5B,
  GST_MTS_DESC_DVB_MULTILINGUAL_BOUQUET_NAME    = 0x5C,
  GST_MTS_DESC_DVB_MULTILINGUAL_SERVICE_NAME    = 0x5D,
  GST_MTS_DESC_DVB_MULTILINGUAL_COMPONENT       = 0x5E,
  GST_MTS_DESC_DVB_PRIVATE_DATA_SPECIFIER       = 0x5F,
  GST_MTS_DESC_DVB_SERVICE_MOVE                 = 0x60,
  GST_MTS_DESC_DVB_SHORT_SMOOTHING_BUFFER       = 0x61,
  GST_MTS_DESC_DVB_FREQUENCY_LIST               = 0x62,
  GST_MTS_DESC_DVB_PARTIAL_TRANSPORT_STREAM     = 0x63,
  GST_MTS_DESC_DVB_DATA_BROADCAST               = 0x64,
  GST_MTS_DESC_DVB_SCRAMBLING                   = 0x65,
  GST_MTS_DESC_DVB_DATA_BROADCAST_ID            = 0x66,
  GST_MTS_DESC_DVB_TRANSPORT_STREAM             = 0x67,
  GST_MTS_DESC_DVB_DSNG                         = 0x68,
  GST_MTS_DESC_DVB_PDC                          = 0x69,
  GST_MTS_DESC_DVB_AC3                          = 0x6A,
  GST_MTS_DESC_DVB_ANCILLARY_DATA               = 0x6B,
  GST_MTS_DESC_DVB_CELL_LIST                    = 0x6C,
  GST_MTS_DESC_DVB_CELL_FREQUENCY_LINK          = 0x6D,
  GST_MTS_DESC_DVB_ANNOUNCEMENT_SUPPORT         = 0x6E,
  GST_MTS_DESC_DVB_APPLICATION_SIGNALLING       = 0x6F,
  GST_MTS_DESC_DVB_ADAPTATION_FIELD_DATA        = 0x70,
  GST_MTS_DESC_DVB_SERVICE_IDENTIFIER           = 0x71,
  GST_MTS_DESC_DVB_SERVICE_AVAILABILITY         = 0x72,
  GST_MTS_DESC_DVB_DEFAULT_AUTHORITY            = 0x73,
  GST_MTS_DESC_DVB_RELATED_CONTENT              = 0x74,
  GST_MTS_DESC_DVB_TVA_ID                       = 0x75,
  GST_MTS_DESC_DVB_CONTENT_IDENTIFIER           = 0x76,
  GST_MTS_DESC_DVB_TIMESLICE_FEC_IDENTIFIER     = 0x77,
  GST_MTS_DESC_DVB_ECM_REPETITION_RATE          = 0x78,
  GST_MTS_DESC_DVB_S2_SATELLITE_DELIVERY_SYSTEM = 0x79,
  GST_MTS_DESC_DVB_ENHANCED_AC3                 = 0x7A,
  GST_MTS_DESC_DVB_DTS                          = 0x7B,
  GST_MTS_DESC_DVB_AAC                          = 0x7C,
  GST_MTS_DESC_DVB_XAIT_LOCATION                = 0x7D,
  GST_MTS_DESC_DVB_FTA_CONTENT_MANAGEMENT       = 0x7E,
  GST_MTS_DESC_DVB_EXTENSION                    = 0x7F,
} GstMpegtsDVBDescriptorType;

/**
 * GstMpegtsDVBExtendedDescriptorType:
 *
 * The type of #GstMpegtsDescriptor
 *
 * These values correspond to the registered extended descriptor
 * type from the various DVB specifications.
 *
 * Consult the relevant specifications for more details.
 */
typedef enum {
  /* 00 - 0x7F DVB extended tags ETSI EN 300 468
   * (Specification for Service Information (SI) in DVB systems)
   */
  GST_MTS_DESC_EXT_DVB_IMAGE_ICON               = 0x00,
  /* TS/TR 102 825 */
  GST_MTS_DESC_EXT_DVB_CPCM_DELIVERY_SIGNALLING = 0x01,
  GST_MTS_DESC_EXT_DVB_CP                       = 0x02,
  GST_MTS_DESC_EXT_DVB_CP_IDENTIFIER            = 0x03,
  GST_MTS_DESC_EXT_DVB_T2_DELIVERY_SYSTEM       = 0x04,
  GST_MTS_DESC_EXT_DVB_SH_DELIVERY_SYSTEM       = 0x05,
  GST_MTS_DESC_EXT_DVB_SUPPLEMENTARY_AUDIO      = 0x06,
  GST_MTS_DESC_EXT_DVB_NETWORK_CHANGE_NOTIFY    = 0x07,
  GST_MTS_DESC_EXT_DVB_MESSAGE                  = 0x08,
  GST_MTS_DESC_EXT_DVB_TARGET_REGION            = 0x09,
  GST_MTS_DESC_EXT_DVB_TARGET_REGION_NAME       = 0x0A,
  GST_MTS_DESC_EXT_DVB_SERVICE_RELOCATED        = 0x0B,
  GST_MTS_DESC_EXT_DVB_XAIT_PID                 = 0x0C,
  GST_MTS_DESC_EXT_DVB_C2_DELIVERY_SYSTEM       = 0x0D,
  GST_MTS_DESC_EXT_DVB_DTS_HD_AUDIO_STREAM      = 0x0E,
  GST_MTS_DESC_EXT_DVB_DTS_NEUTRAL              = 0x0F,
  GST_MTS_DESC_EXT_DVB_VIDEO_DEPTH_RANGE        = 0x10,
  GST_MTS_DESC_EXT_DVB_T2MI                     = 0x11,
  GST_MTS_DESC_EXT_DVB_URI_LINKAGE              = 0x13,
} GstMpegtsDVBExtendedDescriptorType;

/* GST_MTS_DESC_DVB_CAROUSEL_IDENTIFIER (0x13) */
/* FIXME : Implement */

/* GST_MTS_DESC_DVB_NETWORK_NAME (0x40) */
gboolean gst_mpegts_descriptor_parse_dvb_network_name (const GstMpegtsDescriptor *descriptor,
						       gchar **name);

GstMpegtsDescriptor *gst_mpegts_descriptor_from_dvb_network_name (const gchar * name);

/* GST_MTS_DESC_DVB_STUFFING (0x42) */
gboolean gst_mpegts_descriptor_parse_dvb_stuffing (const GstMpegtsDescriptor * descriptor,
                                                   guint8 ** stuffing_bytes);


/* GST_MTS_DESC_DVB_SATELLITE_DELIVERY_SYSTEM (0x43) */
typedef struct _GstMpegtsSatelliteDeliverySystemDescriptor GstMpegtsSatelliteDeliverySystemDescriptor;

typedef enum {
  GST_MPEGTS_MODULATION_QPSK    = 0,
  GST_MPEGTS_MODULATION_QAM_16,
  GST_MPEGTS_MODULATION_QAM_32,
  GST_MPEGTS_MODULATION_QAM_64,
  GST_MPEGTS_MODULATION_QAM_128,
  GST_MPEGTS_MODULATION_QAM_256,
  GST_MPEGTS_MODULATION_QAM_AUTO,
  GST_MPEGTS_MODULATION_VSB_8,
  GST_MPEGTS_MODULATION_VSB_16,
  GST_MPEGTS_MODULATION_PSK_8,
  GST_MPEGTS_MODULATION_APSK_16,
  GST_MPEGTS_MODULATION_APSK_32,
  GST_MPEGTS_MODULATION_DQPSK,
  GST_MPEGTS_MODULATION_QAM_4_NR_,
  GST_MPEGTS_MODULATION_NONE
} GstMpegtsModulationType;

typedef enum {
  GST_MPEGTS_FEC_NONE = 0,
  GST_MPEGTS_FEC_1_2,
  GST_MPEGTS_FEC_2_3,
  GST_MPEGTS_FEC_3_4,
  GST_MPEGTS_FEC_4_5,
  GST_MPEGTS_FEC_5_6,
  GST_MPEGTS_FEC_6_7,
  GST_MPEGTS_FEC_7_8,
  GST_MPEGTS_FEC_8_9,
  GST_MPEGTS_FEC_AUTO,
  GST_MPEGTS_FEC_3_5,
  GST_MPEGTS_FEC_9_10,
  GST_MPEGTS_FEC_2_5
} GstMpegtsDVBCodeRate;

typedef enum {
  GST_MPEGTS_ROLLOFF_35 = 0,
  GST_MPEGTS_ROLLOFF_20,
  GST_MPEGTS_ROLLOFF_25,
  GST_MPEGTS_ROLLOFF_RESERVED,
  GST_MPEGTS_ROLLOFF_AUTO
} GstMpegtsSatelliteRolloff;

typedef enum {
  GST_MPEGTS_POLARIZATION_LINEAR_HORIZONTAL = 0,
  GST_MPEGTS_POLARIZATION_LINEAR_VERTICAL,
  GST_MPEGTS_POLARIZATION_CIRCULAR_LEFT,
  GST_MPEGTS_POLARIZATION_CIRCULAR_RIGHT
} GstMpegtsSatellitePolarizationType;

/**
 * GstMpegtsSatelliteDeliverySystemDescriptor:
 * @frequency: the frequency in kHz (kiloHertz)
 * @orbital_position: the orbital position in degrees
 * @west_east: If %TRUE, the satellite is in the eastern part of the orbit,
 * else in the western part.
 * @polarization: The polarization of the transmitted signal
 * @roll_off: Roll-off factor used in DVB-S2
 * @modulation_system: modulation system, %TRUE if DVB-S2, else DVB-S
 * @modulation_type: Modulation scheme used
 * @symbol_rate: Symbol rate (in symbols per second)
 * @fec_inner: inner FEC scheme used
 *
 * Satellite Delivery System Descriptor (EN 300 468 v.1.13.1)
 */
struct _GstMpegtsSatelliteDeliverySystemDescriptor
{
  guint32                            frequency;
  gfloat                             orbital_position;
  gboolean                           west_east; 
  GstMpegtsSatellitePolarizationType polarization;

  GstMpegtsSatelliteRolloff          roll_off;
  gboolean                           modulation_system;
  GstMpegtsModulationType            modulation_type;

  guint32                            symbol_rate;
  GstMpegtsDVBCodeRate               fec_inner;
};

gboolean gst_mpegts_descriptor_parse_satellite_delivery_system (const GstMpegtsDescriptor *descriptor,
								GstMpegtsSatelliteDeliverySystemDescriptor *res);


/* GST_MTS_DESC_DVB_CABLE_DELIVERY_SYSTEM (0x44) */
typedef enum {
  GST_MPEGTS_CABLE_OUTER_FEC_UNDEFINED = 0,
  GST_MPEGTS_CABLE_OUTER_FEC_NONE,
  GST_MPEGTS_CABLE_OUTER_FEC_RS_204_188,
} GstMpegtsCableOuterFECScheme;

typedef struct _GstMpegtsCableDeliverySystemDescriptor GstMpegtsCableDeliverySystemDescriptor;
/**
 * GstMpegtsCableDeliverySystemDescriptor:
 * @frequency: the frequency in Hz (Hertz)
 * @outer_fec: the outer FEC scheme used
 * @modulation: Modulation scheme used
 * @symbol_rate: Symbol rate (in symbols per second)
 * @fec_inner: inner FEC scheme used
 *
 * Cable Delivery System Descriptor (EN 300 468 v.1.13.1)
 */
struct _GstMpegtsCableDeliverySystemDescriptor
{
  guint32                            frequency;
  GstMpegtsCableOuterFECScheme       outer_fec;
  GstMpegtsModulationType            modulation;

  guint32                            symbol_rate;
  GstMpegtsDVBCodeRate               fec_inner;
};

gboolean gst_mpegts_descriptor_parse_cable_delivery_system (const GstMpegtsDescriptor *descriptor,
							    GstMpegtsCableDeliverySystemDescriptor *res);

/* GST_MTS_DESC_DVB_BOUQUET_NAME (0x47) */
gboolean gst_mpegts_descriptor_parse_dvb_bouquet_name (const GstMpegtsDescriptor * descriptor,
                                                       gchar ** bouquet_name);

/* GST_MTS_DESC_DVB_SERVICE (0x48) */
/**
 * GstMpegtsDVBServiceType:
 * 
 * The type of service of a channel.
 *
 * As specified in Table 87 of ETSI EN 300 468 v1.13.1 
 */
typedef enum {
  GST_DVB_SERVICE_RESERVED_00               = 0x00,
  GST_DVB_SERVICE_DIGITAL_TELEVISION,
  GST_DVB_SERVICE_DIGITAL_RADIO_SOUND,
  GST_DVB_SERVICE_TELETEXT,
  GST_DVB_SERVICE_NVOD_REFERENCE,
  GST_DVB_SERVICE_NVOD_TIME_SHIFTED,
  GST_DVB_SERVICE_MOSAIC,
  GST_DVB_SERVICE_FM_RADIO,
  GST_DVB_SERVICE_DVB_SRM,
  GST_DVB_SERVICE_RESERVED_09,
  GST_DVB_SERVICE_ADVANCED_CODEC_DIGITAL_RADIO_SOUND,
  GST_DVB_SERVICE_ADVANCED_CODEC_MOSAIC,
  GST_DVB_SERVICE_DATA_BROADCAST,
  GST_DVB_SERVICE_RESERVED_0D_COMMON_INTERFACE,
  GST_DVB_SERVICE_RCS_MAP,
  GST_DVB_SERVICE_RCS_FLS,
  GST_DVB_SERVICE_DVB_MHP,
  GST_DVB_SERVICE_MPEG2_HD_DIGITAL_TELEVISION,
  /* 0x12 - 015 Reserved for future use */
  GST_DVB_SERVICE_ADVANCED_CODEC_SD_DIGITAL_TELEVISION = 0x16,
  GST_DVB_SERVICE_ADVANCED_CODEC_SD_NVOD_TIME_SHIFTED,
  GST_DVB_SERVICE_ADVANCED_CODEC_SD_NVOD_REFERENCE,
  GST_DVB_SERVICE_ADVANCED_CODEC_HD_DIGITAL_TELEVISION,
  GST_DVB_SERVICE_ADVANCED_CODEC_HD_NVOD_TIME_SHIFTED,
  GST_DVB_SERVICE_ADVANCED_CODEC_HD_NVOD_REFERENCE,
  GST_DVB_SERVICE_ADVANCED_CODEC_STEREO_HD_DIGITAL_TELEVISION,
  GST_DVB_SERVICE_ADVANCED_CODEC_STEREO_HD_NVOD_TIME_SHIFTED,
  GST_DVB_SERVICE_ADVANCED_CODEC_STEREO_HD_NVOD_REFERENCE,
  /* 0x1F - 0x7f Reserved for future use */
  /* 0x80 - 0xfe user defined */
  /* 0xff Reserved for future use */
  GST_DVB_SERVICE_RESERVED_FF
} GstMpegtsDVBServiceType;

gboolean gst_mpegts_descriptor_parse_dvb_service (const GstMpegtsDescriptor *descriptor,
						  GstMpegtsDVBServiceType *service_type,
						  gchar **service_name,
						  gchar **provider_name);

GstMpegtsDescriptor *gst_mpegts_descriptor_from_dvb_service (GstMpegtsDVBServiceType service_type,
							     const gchar * service_name,
							     const gchar * service_provider);

/* GST_MTS_DESC_DVB_SERVICE_LIST (0x41) */
typedef struct _GstMpegtsDVBServiceListItem GstMpegtsDVBServiceListItem;

/**
 * GstMpegtsDVBServiceListItem:
 * @service_id: the id of a service
 * @type: the type of a service
 */
struct _GstMpegtsDVBServiceListItem
{
  guint16                 service_id;
  GstMpegtsDVBServiceType type;
};

gboolean gst_mpegts_descriptor_parse_dvb_service_list (const GstMpegtsDescriptor * descriptor,
    GPtrArray ** list);

/* GST_MTS_DESC_DVB_LINKAGE (0x4A) */
/**
 * GstMpegtsDVBLinkageType:
 *
 * Linkage Type (EN 300 468 v.1.13.1)
 */
typedef enum {
  /* 0x00, 0x0F-0x7F reserved for future use */
  GST_MPEGTS_DVB_LINKAGE_RESERVED_00               = 0x00,
  GST_MPEGTS_DVB_LINKAGE_INFORMATION               = 0x01,
  GST_MPEGTS_DVB_LINKAGE_EPG                       = 0x02,
  GST_MPEGTS_DVB_LINKAGE_CA_REPLACEMENT            = 0x03,
  GST_MPEGTS_DVB_LINKAGE_TS_CONTAINING_COMPLETE_SI = 0x04,
  GST_MPEGTS_DVB_LINKAGE_SERVICE_REPLACEMENT       = 0x05,
  GST_MPEGTS_DVB_LINKAGE_DATA_BROADCAST            = 0x06,
  GST_MPEGTS_DVB_LINKAGE_RCS_MAP                   = 0x07,
  GST_MPEGTS_DVB_LINKAGE_MOBILE_HAND_OVER          = 0x08,
  GST_MPEGTS_DVB_LINKAGE_SYSTEM_SOFTWARE_UPDATE    = 0x09,
  GST_MPEGTS_DVB_LINKAGE_TS_CONTAINING_SSU         = 0x0A,
  GST_MPEGTS_DVB_LINKAGE_IP_MAC_NOTIFICATION       = 0x0B,
  GST_MPEGTS_DVB_LINKAGE_TS_CONTAINING_INT         = 0x0C,
  GST_MPEGTS_DVB_LINKAGE_EVENT                     = 0x0D,
  GST_MPEGTS_DVB_LINKAGE_EXTENDED_EVENT            = 0x0E,
} GstMpegtsDVBLinkageType;

typedef enum {
  GST_MPEGTS_DVB_LINKAGE_HAND_OVER_RESERVED        = 0x00,
  GST_MPEGTS_DVB_LINKAGE_HAND_OVER_IDENTICAL       = 0x01,
  GST_MPEGTS_DVB_LINKAGE_HAND_OVER_LOCAL_VARIATION = 0x02,
  GST_MPEGTS_DVB_LINKAGE_HAND_OVER_ASSOCIATED      = 0x03,
} GstMpegtsDVBLinkageHandOverType;

typedef struct _GstMpegtsDVBLinkageMobileHandOver GstMpegtsDVBLinkageMobileHandOver;
typedef struct _GstMpegtsDVBLinkageEvent GstMpegtsDVBLinkageEvent;
typedef struct _GstMpegtsDVBLinkageExtendedEvent GstMpegtsDVBLinkageExtendedEvent;
typedef struct _GstMpegtsDVBLinkageDescriptor GstMpegtsDVBLinkageDescriptor;

struct _GstMpegtsDVBLinkageMobileHandOver
{
  GstMpegtsDVBLinkageHandOverType hand_over_type;
  /* 0 = NIT, 1 = SDT */
  gboolean                        origin_type;
  guint16                         network_id;
  guint16                         initial_service_id;
};

struct _GstMpegtsDVBLinkageEvent
{
  guint16  target_event_id;
  gboolean target_listed;
  gboolean event_simulcast;
};

struct _GstMpegtsDVBLinkageExtendedEvent
{
  guint16        target_event_id;
  gboolean       target_listed;
  gboolean       event_simulcast;
  /* FIXME: */
  guint8         link_type;
  /* FIXME: */
  guint8         target_id_type;
  gboolean       original_network_id_flag;
  gboolean       service_id_flag;
  /* if (target_id_type == 3) */
  guint16        user_defined_id;
  /* else */
  guint16        target_transport_stream_id;
  guint16        target_original_network_id;
  guint16        target_service_id;
};

/**
 * GstMpegtsDVBLinkageDescriptor:
 * @transport_stream_id: the transport id
 * @original_network_id: the original network id
 * @service_id: the service id
 * @linkage_type: the type which %linkage_data has
 * @private_data_length: the length for %private_data_bytes
 * @private_data_bytes: additional data bytes
 */
struct _GstMpegtsDVBLinkageDescriptor
{
  guint16                           transport_stream_id;
  guint16                           original_network_id;
  guint16                           service_id;
  GstMpegtsDVBLinkageType           linkage_type;
  /*< private >*/
  gpointer                          linkage_data;
  /*< public >*/
  guint8                            private_data_length;
  guint8                            *private_data_bytes;
};

#define GST_TYPE_MPEGTS_DVB_LINKAGE_DESCRIPTOR ( gst_mpegts_dvb_linkage_descriptor_get_type())

GType gst_mpegts_dvb_linkage_descriptor_get_type (void);

void gst_mpegts_dvb_linkage_descriptor_free (GstMpegtsDVBLinkageDescriptor * source);
const GstMpegtsDVBLinkageMobileHandOver * gst_mpegts_dvb_linkage_descriptor_get_mobile_hand_over
    (const GstMpegtsDVBLinkageDescriptor * desc);
const GstMpegtsDVBLinkageEvent * gst_mpegts_dvb_linkage_descriptor_get_event
    (const GstMpegtsDVBLinkageDescriptor * desc);
const GPtrArray * gst_mpegts_dvb_linkage_descriptor_get_extended_event
    (const GstMpegtsDVBLinkageDescriptor * desc);
gboolean gst_mpegts_descriptor_parse_dvb_linkage (const GstMpegtsDescriptor * descriptor,
                                                  GstMpegtsDVBLinkageDescriptor ** res);

/* GST_MTS_DESC_DVB_SHORT_EVENT (0x4D) */
gboolean gst_mpegts_descriptor_parse_dvb_short_event (const GstMpegtsDescriptor *descriptor,
						       gchar **language_code,
						       gchar **event_name,
						       gchar **text);

/* GST_MTS_DESC_DVB_EXTENDED_EVENT (0x4E) */
typedef struct _GstMpegtsExtendedEventDescriptor GstMpegtsExtendedEventDescriptor;
typedef struct _GstMpegtsExtendedEventItem GstMpegtsExtendedEventItem;

/* FIXME : Maybe make a separate method for getting a specific item entry ? */
struct _GstMpegtsExtendedEventItem
{
  gchar *item_description;
  gchar *item;
};

/**
 * GstMpegtsExtendedEventDescriptor:
 * @descriptor_number:
 * @last_descriptor_number:
 * @language_code: NULL terminated language code.
 * @items: (element-type GstMpegtsExtendedEventItem): the #GstMpegtsExtendedEventItem
 * @text:
 *
 * Extended Event Descriptor (EN 300 468 v.1.13.1)
 */
struct _GstMpegtsExtendedEventDescriptor
{
  guint8 descriptor_number;
  guint8 last_descriptor_number;
  gchar  *language_code;
  GPtrArray *items;
  gchar *text;
};

#define GST_TYPE_MPEGTS_EXTENDED_EVENT_DESCRIPTOR (gst_mpegts_extended_event_descriptor_get_type())

GType gst_mpegts_extended_event_descriptor_get_type (void);
void gst_mpegts_extended_event_descriptor_free (GstMpegtsExtendedEventDescriptor * source);
gboolean gst_mpegts_descriptor_parse_dvb_extended_event (const GstMpegtsDescriptor *descriptor,
							  GstMpegtsExtendedEventDescriptor **res);

/* GST_MTS_DESC_DVB_COMPONENT (0x50) */
#define GST_TYPE_MPEGTS_COMPONENT_DESCRIPTOR (gst_mpegts_component_descriptor_get_type ())
typedef enum {
  GST_MPEGTS_STREAM_CONTENT_MPEG2_VIDEO          = 0x01,
  GST_MPEGTS_STREAM_CONTENT_MPEG1_LAYER2_AUDIO   = 0x02,
  GST_MPEGTS_STREAM_CONTENT_TELETEXT_OR_SUBTITLE = 0x03,
  GST_MPEGTS_STREAM_CONTENT_AC_3                 = 0x04,
  GST_MPEGTS_STREAM_CONTENT_AVC                  = 0x05,
  GST_MPEGTS_STREAM_CONTENT_AAC                  = 0x06,
  GST_MPEGTS_STREAM_CONTENT_DTS                  = 0x07,
  GST_MPEGTS_STREAM_CONTENT_SRM_CPCM             = 0x08
} GstMpegtsComponentStreamContent;

typedef struct _GstMpegtsComponentDescriptor GstMpegtsComponentDescriptor;
struct _GstMpegtsComponentDescriptor
{
  guint8 stream_content;
  guint8 component_type;
  guint8 component_tag;
  gchar  *language_code;
  gchar  *text;
};

GType gst_mpegts_component_descriptor_get_type (void);
void gst_mpegts_dvb_component_descriptor_free (GstMpegtsComponentDescriptor * source);
gboolean gst_mpegts_descriptor_parse_dvb_component (const GstMpegtsDescriptor *descriptor,
						    GstMpegtsComponentDescriptor **res);

/* GST_MTS_DESC_DVB_STREAM_IDENTIFIER (0x52) */
gboolean gst_mpegts_descriptor_parse_dvb_stream_identifier (const GstMpegtsDescriptor *descriptor,
							    guint8 *component_tag);

/* GST_MTS_DESC_DVB_CA_IDENTIFIER (0x53) */
gboolean gst_mpegts_descriptor_parse_dvb_ca_identifier (const GstMpegtsDescriptor * descriptor,
                                                        GArray ** list);

/* GST_MTS_DESC_DVB_CONTENT (0x54) */
typedef enum {
  GST_MPEGTS_CONTENT_MOVIE_DRAMA                = 0x01,
  GST_MPEGTS_CONTENT_NEWS_CURRENT_AFFAIRS       = 0x02,
  GST_MPEGTS_CONTENT_SHOW_GAME_SHOW             = 0x03,
  GST_MPEGTS_CONTENT_SPORTS                     = 0x04,
  GST_MPEGTS_CONTENT_CHILDREN_YOUTH_PROGRAM     = 0x05,
  GST_MPEGTS_CONTENT_MUSIC_BALLET_DANCE         = 0x06,
  GST_MPEGTS_CONTENT_ARTS_CULTURE               = 0x07,
  GST_MPEGTS_CONTENT_SOCIAL_POLITICAL_ECONOMICS = 0x08,
  GST_MPEGTS_CONTENT_EDUCATION_SCIENCE_FACTUAL  = 0x09,
  GST_MPEGTS_CONTENT_LEISURE_HOBBIES            = 0x0A,
  GST_MPEGTS_CONTENT_SPECIAL_CHARACTERISTICS    = 0x0B
} GstMpegtsContentNibbleHi;

typedef struct _GstMpegtsContent GstMpegtsContent;
struct _GstMpegtsContent
{
  GstMpegtsContentNibbleHi content_nibble_1;
  guint8 content_nibble_2;
  guint8 user_byte;
};

gboolean gst_mpegts_descriptor_parse_dvb_content (const GstMpegtsDescriptor *
	descriptor, GPtrArray ** content);

/* GST_MTS_DESC_DVB_PARENTAL_RATING (0x55) */
typedef struct _GstMpegtsDVBParentalRatingItem GstMpegtsDVBParentalRatingItem;

/**
 * GstMpegtsDVBParentalRating:
 * @country_code: This 24-bit field identifies a country using the 3-character
 * code as specified in ISO 3166
 * @rating: the rating age
 */
struct _GstMpegtsDVBParentalRatingItem
{
  gchar  *country_code;
  guint8 rating;
};

gboolean gst_mpegts_descriptor_parse_dvb_parental_rating (const GstMpegtsDescriptor
        * descriptor, GPtrArray ** rating);

/* GST_MTS_DESC_DVB_TELETEXT (0x56) */
/**
 * GstMpegtsDVBTeletextType:
 *
 * The type of teletext page.
 *
 * As specified in Table 100 of ETSI EN 300 468 v1.13.1
 */
typedef enum {
  INITIAL_PAGE = 0x01,
  SUBTITLE_PAGE,
  ADDITIONAL_INFO_PAGE,
  PROGRAMME_SCHEDULE_PAGE,
  HEARING_IMPAIRED_PAGE
} GstMpegtsDVBTeletextType;

gboolean gst_mpegts_descriptor_parse_dvb_teletext_idx (const GstMpegtsDescriptor *
    descriptor, guint idx, gchar ** language_code,
    GstMpegtsDVBTeletextType * teletext_type, guint8 * magazine_number,
    guint8 * page_number);

guint gst_mpegts_descriptor_parse_dvb_teletext_nb (const GstMpegtsDescriptor *
    descriptor);

/* GST_MTS_DESC_DVB_SUBTITLING (0x59) */
gboolean gst_mpegts_descriptor_parse_dvb_subtitling_idx (const GstMpegtsDescriptor *descriptor,
							 guint idx, gchar ** lang,
							 guint8 *type, guint16 *composition_page_id,
							 guint16 *ancillary_page_id);
guint gst_mpegts_descriptor_parse_dvb_subtitling_nb (const GstMpegtsDescriptor *descriptor);

GstMpegtsDescriptor * gst_mpegts_descriptor_from_dvb_subtitling (const gchar *lang,
    guint8 type, guint16 composition, guint16 ancillary);



/* GST_MTS_DESC_DVB_TERRESTRIAL_DELIVERY_SYSTEM (0x5A) */
typedef struct _GstMpegtsTerrestrialDeliverySystemDescriptor GstMpegtsTerrestrialDeliverySystemDescriptor;

typedef enum {
  GST_MPEGTS_TRANSMISSION_MODE_2K = 0,
  GST_MPEGTS_TRANSMISSION_MODE_8K,
  GST_MPEGTS_TRANSMISSION_MODE_AUTO,
  GST_MPEGTS_TRANSMISSION_MODE_4K,
  GST_MPEGTS_TRANSMISSION_MODE_1K,
  GST_MPEGTS_TRANSMISSION_MODE_16K,
  GST_MPEGTS_TRANSMISSION_MODE_32K,
  GST_MPEGTS_TRANSMISSION_MODE_C1,
  GST_MPEGTS_TRANSMISSION_MODE_C3780
} GstMpegtsTerrestrialTransmissionMode;

typedef enum {
  GST_MPEGTS_GUARD_INTERVAL_1_32 = 0,
  GST_MPEGTS_GUARD_INTERVAL_1_16,
  GST_MPEGTS_GUARD_INTERVAL_1_8,
  GST_MPEGTS_GUARD_INTERVAL_1_4,
  GST_MPEGTS_GUARD_INTERVAL_AUTO,
  GST_MPEGTS_GUARD_INTERVAL_1_128,
  GST_MPEGTS_GUARD_INTERVAL_19_128,
  GST_MPEGTS_GUARD_INTERVAL_19_256,
  GST_MPEGTS_GUARD_INTERVAL_PN420,
  GST_MPEGTS_GUARD_INTERVAL_PN595,
  GST_MPEGTS_GUARD_INTERVAL_PN945
} GstMpegtsTerrestrialGuardInterval;

typedef enum {
  GST_MPEGTS_HIERARCHY_NONE = 0,
  GST_MPEGTS_HIERARCHY_1,
  GST_MPEGTS_HIERARCHY_2,
  GST_MPEGTS_HIERARCHY_4,
  GST_MPEGTS_HIERARCHY_AUTO
} GstMpegtsTerrestrialHierarchy;

/**
 * GstMpegtsTerrestrialDeliverySystemDescriptor:
 * @frequency: the frequency in Hz (Hertz)
 * @bandwidth: the bandwidth in Hz (Hertz)
 * @priority: %TRUE High Priority %FALSE Low Priority
 * @time_slicing: %TRUE no time slicing %FALSE time slicing
 * @mpe_fec: %TRUE no mpe-fec is used %FALSE mpe-fec is use
 * @constellation: the constallation
 * @hierarchy: the hierarchy
 * @code_rate_hp:
 * @code_rate_lp:
 * @guard_interval:
 * @transmission_mode:
 * @other_frequency: %TRUE more frequency are use, else not
 *
 * Terrestrial Delivery System Descriptor (EN 300 468 v.1.13.1)
 */

struct _GstMpegtsTerrestrialDeliverySystemDescriptor
{
  guint32				frequency;
  guint32				bandwidth;
  gboolean				priority;
  gboolean				time_slicing;
  gboolean				mpe_fec;
  GstMpegtsModulationType		constellation;
  GstMpegtsTerrestrialHierarchy		hierarchy;
  GstMpegtsDVBCodeRate			code_rate_hp;
  GstMpegtsDVBCodeRate			code_rate_lp;
  GstMpegtsTerrestrialGuardInterval	guard_interval;
  GstMpegtsTerrestrialTransmissionMode	transmission_mode;
  gboolean				other_frequency;
};

gboolean gst_mpegts_descriptor_parse_terrestrial_delivery_system (const GstMpegtsDescriptor
              *descriptor, GstMpegtsTerrestrialDeliverySystemDescriptor * res);

/* GST_MTS_DESC_DVB_MULTILINGUAL_NETWORK_NAME (0x5B) */
typedef struct _GstMpegtsDvbMultilingualNetworkNameItem GstMpegtsDvbMultilingualNetworkNameItem;

/**
 * GstMpegtsDvbMultilingualNetworkNameItem:
 * @language_code: the ISO 639 language code
 * @network_name: the network name
 *
 * a multilingual network name entry
 */
struct _GstMpegtsDvbMultilingualNetworkNameItem
{
  gchar *language_code;
  gchar *network_name;
};

gboolean gst_mpegts_descriptor_parse_dvb_multilingual_network_name (const GstMpegtsDescriptor
              *descriptor, GPtrArray ** network_name_items);

/* GST_MTS_DESC_DVB_MULTILINGUAL_BOUQUET_NAME (0x5C) */
typedef struct _GstMpegtsDvbMultilingualBouquetNameItem GstMpegtsDvbMultilingualBouquetNameItem;

/**
 * GstMpegtsDvbMultilingualBouquetNameItem:
 * @language_code: the ISO 639 language code
 * @bouquet_name: the bouquet name
 *
 * a multilingual bouquet name entry
 */
struct _GstMpegtsDvbMultilingualBouquetNameItem
{
  gchar *language_code;
  gchar *bouquet_name;
};

gboolean gst_mpegts_descriptor_parse_dvb_multilingual_bouquet_name (const GstMpegtsDescriptor
              *descriptor, GPtrArray ** bouquet_name_items);

/* GST_MTS_DESC_DVB_MULTILINGUAL_SERVICE_NAME (0x5D) */
typedef struct _GstMpegtsDvbMultilingualServiceNameItem GstMpegtsDvbMultilingualServiceNameItem;

/**
 * GstMpegtsDvbMultilingualServiceNameItem:
 * @language_code: the ISO 639 language code
 * @provider_name: the provider name
 * @service_name: the service name
 *
 * a multilingual service name entry
 */
struct _GstMpegtsDvbMultilingualServiceNameItem
{
  gchar *language_code;
  gchar *provider_name;
  gchar *service_name;
};

gboolean gst_mpegts_descriptor_parse_dvb_multilingual_service_name (const GstMpegtsDescriptor
              *descriptor, GPtrArray ** service_name_items);

/* GST_MTS_DESC_DVB_MULTILINGUAL_COMPONENT (0x5E) */
typedef struct _GstMpegtsDvbMultilingualComponentItem GstMpegtsDvbMultilingualComponentItem;

/**
 * GstMpegtsDvbMultilingualComponentItem:
 * @language_code: the ISO 639 language code
 * @description: the component description
 */
struct _GstMpegtsDvbMultilingualComponentItem
{
  gchar *language_code;
  gchar *description;
};

gboolean gst_mpegts_descriptor_parse_dvb_multilingual_component (const GstMpegtsDescriptor
              *descriptor, guint8 * component_tag, GPtrArray ** component_description_items);

/* GST_MTS_DESC_DVB_PRIVATE_DATA_SPECIFIER (0x5F) */
gboolean gst_mpegts_descriptor_parse_dvb_private_data_specifier (const GstMpegtsDescriptor
              * descriptor, guint32 * private_data_specifier, guint8 ** private_data,
              guint8 * length);

/* GST_MTS_DESC_DVB_FREQUENCY_LIST (0x62) */
gboolean gst_mpegts_descriptor_parse_dvb_frequency_list (const GstMpegtsDescriptor
    * descriptor, gboolean * offset, GArray ** list);

/* GST_MTS_DESC_DVB_DATA_BROADCAST (0x64) */
typedef struct _GstMpegtsDataBroadcastDescriptor GstMpegtsDataBroadcastDescriptor;

/**
 * GstMpegtsDataBroadcastDescriptor:
 * @data_broadcast_id: the data broadcast id
 * @component_tag: the component tag
 * @selector_bytes: the selector byte field
 * @language_code: language of @text
 * @text: description of data broadcast
 */
struct _GstMpegtsDataBroadcastDescriptor
{
  guint16     data_broadcast_id;
  guint8      component_tag;
  guint8      length;
  guint8      *selector_bytes;
  gchar       *language_code;
  gchar       *text;
};


#define GST_TYPE_MPEGTS_DVB_DATA_BROADCAST_DESCRIPTOR (gst_mpegts_dvb_data_broadcast_descriptor_get_type ())
GType gst_mpegts_dvb_data_broadcast_descriptor_get_type (void);
void gst_mpegts_dvb_data_broadcast_descriptor_free (GstMpegtsDataBroadcastDescriptor * source);
gboolean gst_mpegts_descriptor_parse_dvb_data_broadcast (const GstMpegtsDescriptor
              *descriptor, GstMpegtsDataBroadcastDescriptor ** res);

/* GST_MTS_DESC_DVB_SCRAMBLING (0x65) */
typedef enum
{
  GST_MPEGTS_DVB_SCRAMBLING_MODE_RESERVED              = 0x00,
  GST_MPEGTS_DVB_SCRAMBLING_MODE_CSA1                  = 0x01,
  GST_MPEGTS_DVB_SCRAMBLING_MODE_CSA2                  = 0x02,
  GST_MPEGTS_DVB_SCRAMBLING_MODE_CSA3_STANDARD         = 0x03,
  GST_MPEGTS_DVB_SCRAMBLING_MODE_CSA3_MINIMAL_ENHANCED = 0x04,
  GST_MPEGTS_DVB_SCRAMBLING_MODE_CSA3_FULL_ENHANCED    = 0x05,
  /* 0x06 - 0x0f reserved for future use */
  GST_MPEGTS_DVB_SCRAMBLING_MODE_CISSA                 = 0x10,
  /* 0x11 - 0x1f reserved for future DVB-CISSA versions */
  GST_MPEGTS_DVB_SCRAMBLING_MODE_ATIS_0                = 0x70,
  GST_MPEGTS_DVB_SCRAMBLING_MODE_ATIS_F                = 0x7f,
} GstMpegtsDVBScramblingModeType;

gboolean gst_mpegts_descriptor_parse_dvb_scrambling (const GstMpegtsDescriptor * descriptor,
       GstMpegtsDVBScramblingModeType * scrambling_mode);

/* GST_MTS_DESC_DVB_DATA_BROADCAST_ID (0x66) */
gboolean gst_mpegts_descriptor_parse_dvb_data_broadcast_id (const GstMpegtsDescriptor
       * descriptor, guint16 * data_broadcast_id, guint8 ** id_selector_bytes, guint8 * len);

/* GST_MTS_DESC_DVB_AC3 (0x6a) */
/* FIXME : Implement */

/* GST_MTS_DESC_EXT_DVB_T2_DELIVERY_SYSTEM (0x7F && 0x04) */
typedef struct _GstMpegtsT2DeliverySystemCellExtension GstMpegtsT2DeliverySystemCellExtension;

/**
 * GstMpegtsT2DeliverySystemCellExtension:
 * @cell_id_extension: id of the sub cell
 * @transposer_frequency: centre frequency of the sub cell in Hz
 */
struct _GstMpegtsT2DeliverySystemCellExtension
{
  guint8  cell_id_extension;
  guint32 transposer_frequency;
};

typedef struct _GstMpegtsT2DeliverySystemCell GstMpegtsT2DeliverySystemCell;

/**
 * GstMpegtsT2DeliverySystemCell:
 * @cell_id: id of the cell
 * @centre_frequencies: (element-type guint32): centre frequencies in Hz
 * @sub_cells: (element-type GstMpegtsT2DeliverySystemCellExtension):
 */
struct _GstMpegtsT2DeliverySystemCell
{
  guint16      cell_id;
  GArray       *centre_frequencies;
  GPtrArray    *sub_cells;
};

typedef struct _GstMpegtsT2DeliverySystemDescriptor GstMpegtsT2DeliverySystemDescriptor;

/**
 * GstMpegtsT2DeliverySystemDescriptor:
 * @plp_id:
 * @t2_system_id:
 * @siso_miso:
 * @bandwidth:
 * @guard_interval:
 * @transmission_mode:
 * @other_frequency:
 * @tfs:
 * @cells: (element-type GstMpegtsT2DeliverySystemCell):
 *
 * describe DVB-T2 transmissions according to EN 302 755
 */
struct _GstMpegtsT2DeliverySystemDescriptor
{
  guint8                                plp_id;
  guint16                               t2_system_id;
  /* FIXME: */
  guint8                                siso_miso;
  guint32                               bandwidth;
  GstMpegtsTerrestrialGuardInterval     guard_interval;
  GstMpegtsTerrestrialTransmissionMode  transmission_mode;
  gboolean                              other_frequency;
  gboolean                              tfs;
  GPtrArray                             *cells;
};

#define GST_TYPE_MPEGTS_T2_DELIVERY_SYSTEM_DESCRIPTOR (gst_mpegts_t2_delivery_system_descriptor_get_type ())
GType gst_mpegts_t2_delivery_system_descriptor_get_type (void);
void gst_mpegts_t2_delivery_system_descriptor_free (GstMpegtsT2DeliverySystemDescriptor * source);
gboolean gst_mpegts_descriptor_parse_dvb_t2_delivery_system (const GstMpegtsDescriptor
              *descriptor, GstMpegtsT2DeliverySystemDescriptor ** res);

G_END_DECLS

#endif				/* GST_MPEGTS_DESCRIPTOR_H */
