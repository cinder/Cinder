


#ifndef __GST_MPEGTS_ENUM_TYPES_H__
#define __GST_MPEGTS_ENUM_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "gstmpegtssection.h" */
GType gst_mpegts_section_type_get_type (void);
#define GST_TYPE_MPEGTS_SECTION_TYPE (gst_mpegts_section_type_get_type())
GType gst_mpegts_section_table_id_get_type (void);
#define GST_TYPE_MPEGTS_SECTION_TABLE_ID (gst_mpegts_section_table_id_get_type())
GType gst_mpegts_stream_type_get_type (void);
#define GST_TYPE_MPEGTS_STREAM_TYPE (gst_mpegts_stream_type_get_type())

/* enumerations from "gstmpegtsdescriptor.h" */
GType gst_mpegts_descriptor_type_get_type (void);
#define GST_TYPE_MPEGTS_DESCRIPTOR_TYPE (gst_mpegts_descriptor_type_get_type())
GType gst_mpegts_misc_descriptor_type_get_type (void);
#define GST_TYPE_MPEGTS_MISC_DESCRIPTOR_TYPE (gst_mpegts_misc_descriptor_type_get_type())
GType gst_mpegts_atsc_descriptor_type_get_type (void);
#define GST_TYPE_MPEGTS_ATSC_DESCRIPTOR_TYPE (gst_mpegts_atsc_descriptor_type_get_type())
GType gst_mpegts_isdb_descriptor_type_get_type (void);
#define GST_TYPE_MPEGTS_ISDB_DESCRIPTOR_TYPE (gst_mpegts_isdb_descriptor_type_get_type())
GType gst_mpegts_iso639_audio_type_get_type (void);
#define GST_TYPE_MPEGTS_ISO639_AUDIO_TYPE (gst_mpegts_iso639_audio_type_get_type())

/* enumerations from "gst-atsc-section.h" */
GType gst_mpegts_section_atsc_table_id_get_type (void);
#define GST_TYPE_MPEGTS_SECTION_ATSC_TABLE_ID (gst_mpegts_section_atsc_table_id_get_type())
GType gst_mpegts_atsc_mgt_table_type_get_type (void);
#define GST_TYPE_MPEGTS_ATSC_MGT_TABLE_TYPE (gst_mpegts_atsc_mgt_table_type_get_type())

/* enumerations from "gst-dvb-section.h" */
GType gst_mpegts_section_dvb_table_id_get_type (void);
#define GST_TYPE_MPEGTS_SECTION_DVB_TABLE_ID (gst_mpegts_section_dvb_table_id_get_type())
GType gst_mpegts_running_status_get_type (void);
#define GST_TYPE_MPEGTS_RUNNING_STATUS (gst_mpegts_running_status_get_type())

/* enumerations from "gst-scte-section.h" */
GType gst_mpegts_scte_stream_type_get_type (void);
#define GST_TYPE_MPEGTS_SCTE_STREAM_TYPE (gst_mpegts_scte_stream_type_get_type())
GType gst_mpegts_section_scte_table_id_get_type (void);
#define GST_TYPE_MPEGTS_SECTION_SCTE_TABLE_ID (gst_mpegts_section_scte_table_id_get_type())

/* enumerations from "gst-dvb-descriptor.h" */
GType gst_mpegts_dvb_descriptor_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_DESCRIPTOR_TYPE (gst_mpegts_dvb_descriptor_type_get_type())
GType gst_mpegts_dvb_extended_descriptor_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_EXTENDED_DESCRIPTOR_TYPE (gst_mpegts_dvb_extended_descriptor_type_get_type())
GType gst_mpegts_modulation_type_get_type (void);
#define GST_TYPE_MPEGTS_MODULATION_TYPE (gst_mpegts_modulation_type_get_type())
GType gst_mpegts_dvb_code_rate_get_type (void);
#define GST_TYPE_MPEGTS_DVB_CODE_RATE (gst_mpegts_dvb_code_rate_get_type())
GType gst_mpegts_satellite_rolloff_get_type (void);
#define GST_TYPE_MPEGTS_SATELLITE_ROLLOFF (gst_mpegts_satellite_rolloff_get_type())
GType gst_mpegts_satellite_polarization_type_get_type (void);
#define GST_TYPE_MPEGTS_SATELLITE_POLARIZATION_TYPE (gst_mpegts_satellite_polarization_type_get_type())
GType gst_mpegts_cable_outer_fec_scheme_get_type (void);
#define GST_TYPE_MPEGTS_CABLE_OUTER_FEC_SCHEME (gst_mpegts_cable_outer_fec_scheme_get_type())
GType gst_mpegts_dvb_service_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_SERVICE_TYPE (gst_mpegts_dvb_service_type_get_type())
GType gst_mpegts_dvb_linkage_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_LINKAGE_TYPE (gst_mpegts_dvb_linkage_type_get_type())
GType gst_mpegts_dvb_linkage_hand_over_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_LINKAGE_HAND_OVER_TYPE (gst_mpegts_dvb_linkage_hand_over_type_get_type())
GType gst_mpegts_component_stream_content_get_type (void);
#define GST_TYPE_MPEGTS_COMPONENT_STREAM_CONTENT (gst_mpegts_component_stream_content_get_type())
GType gst_mpegts_content_nibble_hi_get_type (void);
#define GST_TYPE_MPEGTS_CONTENT_NIBBLE_HI (gst_mpegts_content_nibble_hi_get_type())
GType gst_mpegts_dvb_teletext_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_TELETEXT_TYPE (gst_mpegts_dvb_teletext_type_get_type())
GType gst_mpegts_terrestrial_transmission_mode_get_type (void);
#define GST_TYPE_MPEGTS_TERRESTRIAL_TRANSMISSION_MODE (gst_mpegts_terrestrial_transmission_mode_get_type())
GType gst_mpegts_terrestrial_guard_interval_get_type (void);
#define GST_TYPE_MPEGTS_TERRESTRIAL_GUARD_INTERVAL (gst_mpegts_terrestrial_guard_interval_get_type())
GType gst_mpegts_terrestrial_hierarchy_get_type (void);
#define GST_TYPE_MPEGTS_TERRESTRIAL_HIERARCHY (gst_mpegts_terrestrial_hierarchy_get_type())
GType gst_mpegts_dvb_scrambling_mode_type_get_type (void);
#define GST_TYPE_MPEGTS_DVB_SCRAMBLING_MODE_TYPE (gst_mpegts_dvb_scrambling_mode_type_get_type())
G_END_DECLS

#endif /* __GST_MPEGTS_ENUM_TYPES_H__ */



