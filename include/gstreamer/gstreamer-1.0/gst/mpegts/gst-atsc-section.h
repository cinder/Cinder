/*
 * mpegtspacketizer.h - 
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
 */

#ifndef GST_ATSC_SECTION_H
#define GST_ATSC_SECTION_H

#include <gst/gst.h>
#include <gst/mpegts/gstmpegtssection.h>
#include <gst/mpegts/gstmpegtsdescriptor.h>

G_BEGIN_DECLS

/**
 * GstMpegtsSectionATSCTableID:
 *
 * Values for a #GstMpegtsSection table_id.
 *
 * These are the registered ATSC table_id variants.
 *
 * see also: #GstMpegtsSectionTableID
 */
typedef enum {


  /* ATSC (A/65) */
  GST_MTS_TABLE_ID_ATSC_MASTER_GUIDE                    = 0xC7,
  GST_MTS_TABLE_ID_ATSC_TERRESTRIAL_VIRTUAL_CHANNEL     = 0xC8,
  GST_MTS_TABLE_ID_ATSC_CABLE_VIRTUAL_CHANNEL           = 0xC9,
  GST_MTS_TABLE_ID_ATSC_RATING_REGION                   = 0xCA,
  GST_MTS_TABLE_ID_ATSC_EVENT_INFORMATION               = 0xCB,
  GST_MTS_TABLE_ID_ATSC_CHANNEL_OR_EVENT_EXTENDED_TEXT  = 0xCC,
  GST_MTS_TABLE_ID_ATSC_SYSTEM_TIME                     = 0xCD,
  /* ATSC (A/90) */
  GST_MTS_TABLE_ID_ATSC_DATA_EVENT                      = 0xCE,
  GST_MTS_TABLE_ID_ATSC_DATA_SERVICE                    = 0xCF,
  /* 0xD0 ?? */
  GST_MTS_TABLE_ID_ATSC_NETWORK_RESOURCE                = 0xD1,
  GST_MTS_TABLE_ID_ATSC_LONG_TERM_SERVICE               = 0xD2,
  GST_MTS_TABLE_ID_ATSC_DIRECTED_CHANNEL_CHANGE         = 0xD3,
  GST_MTS_TABLE_ID_ATSC_DIRECTED_CHANNEL_CHANGE_SECTION_CODE = 0xD4,
  /* 0xD5 ?? */
  GST_MTS_TABLE_ID_ATSC_AGGREGATE_EVENT_INFORMATION     = 0xD6,
  GST_MTS_TABLE_ID_ATSC_AGGREGATE_EXTENDED_TEXT         = 0xD7,
  /* 0xD8 ?? */
  GST_MTS_TABLE_ID_ATSC_AGGREGATE_DATA_EVENT            = 0xD9,
  GST_MTS_TABLE_ID_ATSC_SATELLITE_VIRTUAL_CHANNEL       = 0xDA,
} GstMpegtsSectionATSCTableID;

/* TVCT/CVCT */
#define GST_TYPE_MPEGTS_ATSC_VCT (gst_mpegts_atsc_vct_get_type ())
#define GST_TYPE_MPEGTS_ATSC_VCT_SOURCE (gst_mpegts_atsc_vct_source_get_type ())

typedef struct _GstMpegtsAtscVCTSource GstMpegtsAtscVCTSource;
typedef struct _GstMpegtsAtscVCT GstMpegtsAtscVCT;

/**
 * GstMpegtsAtscVCTSource:
 * @descriptors: (element-type GstMpegtsDescriptor): descriptors
 *
 * Source from a @GstMpegtsAtscVCT, can be used both for TVCT and CVCT tables
 */
struct _GstMpegtsAtscVCTSource
{
  gchar    *short_name;
  guint16   major_channel_number;
  guint16   minor_channel_number;
  guint8    modulation_mode;
  guint32   carrier_frequency;
  guint16   channel_TSID;
  guint16   program_number;
  /* FIXME: */
  guint8    ETM_location;
  gboolean  access_controlled;
  gboolean  hidden;
  gboolean  path_select; /* CVCT only - reserved bit in TVCT */
  gboolean  out_of_band; /* CVCT only - reserved bit in TVCT */
  gboolean  hide_guide;
  /* FIXME: */
  guint8    service_type;
  guint16   source_id;
  GPtrArray *descriptors;
};

/**
 * GstMpegtsAtscVCT:
 * @sources: (element-type GstMpegtsAtscVCTSource): sources
 * @descriptors: (element-type GstMpegtsDescriptor): descriptors
 *
 * Represents both:
 *   Terrestrial Virtual Channel Table (A65)
 *   Cable Virtual Channel Table (A65)
 *
 */
struct _GstMpegtsAtscVCT
{
  guint16   transport_stream_id;
  guint8    protocol_version;
  GPtrArray *sources;
  GPtrArray *descriptors;
};

GType gst_mpegts_atsc_vct_get_type (void);
GType gst_mpegts_atsc_vct_source_get_type (void);

const GstMpegtsAtscVCT * gst_mpegts_section_get_atsc_tvct (GstMpegtsSection * section);
const GstMpegtsAtscVCT * gst_mpegts_section_get_atsc_cvct (GstMpegtsSection * section);

/* MGT */
#define GST_TYPE_MPEGTS_ATSC_MGT (gst_mpegts_atsc_mgt_get_type ())
#define GST_TYPE_MPEGTS_ATSC_MGT_TABLE (gst_mpegts_atsc_mgt_table_get_type ())

typedef struct _GstMpegtsAtscMGTTable GstMpegtsAtscMGTTable;
typedef struct _GstMpegtsAtscMGT GstMpegtsAtscMGT;

typedef enum {
  GST_MPEGTS_ATSC_MGT_TABLE_TYPE_EIT0 = 0x0100,
  GST_MPEGTS_ATSC_MGT_TABLE_TYPE_EIT127 = 0x017F,
  GST_MPEGTS_ATSC_MGT_TABLE_TYPE_ETT0 = 0x0200,
  GST_MPEGTS_ATSC_MGT_TABLE_TYPE_ETT127 = 0x027F
} GstMpegtsAtscMGTTableType;

/**
 * GstMpegtsAtscMGTTable:
 * @descriptors: (element-type GstMpegtsDescriptor): descriptors
 *
 * Source from a @GstMpegtsAtscMGT
 */
struct _GstMpegtsAtscMGTTable
{
  guint16 table_type;
  guint16 pid;
  guint8  version_number;
  guint32 number_bytes;
  GPtrArray *descriptors;
};

/**
 * GstMpegtsAtscMGT:
 * @tables: (element-type GstMpegtsAtscMGTTable): the tables
 * @descriptors: (element-type GstMpegtsDescriptor): descriptors
 *
 * Master Guide Table (A65)
 *
 */
struct _GstMpegtsAtscMGT
{
  guint8  protocol_version;
  guint16 tables_defined;
  GPtrArray *tables;
  GPtrArray *descriptors;
};

GType gst_mpegts_atsc_mgt_get_type (void);
GType gst_mpegts_atsc_mgt_table_get_type (void);

const GstMpegtsAtscMGT * gst_mpegts_section_get_atsc_mgt (GstMpegtsSection * section);

/* Multiple string structure (used in ETT and EIT */

#define GST_TYPE_MPEGTS_ATSC_STRING_SEGMENT (gst_mpegts_atsc_string_segment_get_type())
#define GST_TYPE_MPEGTS_ATSC_MULT_STRING (gst_mpegts_atsc_mult_string_get_type())

typedef struct _GstMpegtsAtscStringSegment GstMpegtsAtscStringSegment;
typedef struct _GstMpegtsAtscMultString GstMpegtsAtscMultString;

struct _GstMpegtsAtscStringSegment {
  guint8 compression_type;
  guint8 mode;
  guint8 compressed_data_size;
  guint8 *compressed_data;

  gchar *cached_string;
};

const gchar * gst_mpegts_atsc_string_segment_get_string (GstMpegtsAtscStringSegment * seg);

/**
 * GstMpegtsAtscMultString:
 * @segments: (element-type GstMpegtsAtscStringSegment)
 *
 */
struct _GstMpegtsAtscMultString {
  gchar      iso_639_langcode[4];
  GPtrArray *segments;
};

GType gst_mpegts_atsc_string_segment_get_type (void);
GType gst_mpegts_atsc_mult_string_get_type (void);

/* EIT */

#define GST_TYPE_MPEGTS_ATSC_EIT_EVENT (gst_mpegts_atsc_eit_event_get_type())
#define GST_TYPE_MPEGTS_ATSC_EIT (gst_mpegts_atsc_eit_get_type())

typedef struct _GstMpegtsAtscEITEvent GstMpegtsAtscEITEvent;
typedef struct _GstMpegtsAtscEIT GstMpegtsAtscEIT;

/**
 * GstMpegtsAtscEITEvent:
 * @titles: (element-type GstMpegtsAtscMultString): the titles
 * @descriptors: (element-type GstMpegtsDescriptor): descriptors
 *
 * An ATSC EIT Event
 */
struct _GstMpegtsAtscEITEvent {
  guint16        event_id;
  guint32        start_time;
  guint8         etm_location;
  guint32        length_in_seconds;
  GPtrArray     *titles;

  GPtrArray *descriptors;
};

/**
 * GstMpegtsAtscEIT:
 * @events: (element-type GstMpegtsAtscEITEvent): Events
 *
 * Event Information Table (ATSC)
 *
 */
struct _GstMpegtsAtscEIT
{
  guint16        source_id;
  guint8         protocol_version;

  GPtrArray     *events;
};

GType gst_mpegts_atsc_eit_event_get_type (void);
GType gst_mpegts_atsc_eit_get_type (void);

const GstMpegtsAtscEIT *gst_mpegts_section_get_atsc_eit (GstMpegtsSection *section);

/* ETT */

#define GST_TYPE_MPEGTS_ATSC_ETT (gst_mpegts_atsc_ett_get_type())

typedef struct _GstMpegtsAtscETT GstMpegtsAtscETT;

/**
 * GstMpegtsAtscETT:
 * @messages: (element-type GstMpegtsAtscMultString): List of texts
 *
 * Extended Text Table (ATSC)
 *
 */
struct _GstMpegtsAtscETT
{
  guint16        ett_table_id_extension;
  guint16        protocol_version;
  guint32        etm_id;

  GPtrArray     *messages;
};

GType gst_mpegts_atsc_ett_get_type (void);

const GstMpegtsAtscETT *gst_mpegts_section_get_atsc_ett (GstMpegtsSection *section);

/* STT */
#define GST_TYPE_MPEGTS_ATSC_STT (gst_mpegts_atsc_stt_get_type ())

typedef struct _GstMpegtsAtscSTT GstMpegtsAtscSTT;

/**
 * GstMpegtsAtscSTT:
 * @descriptors: (element-type GstMpegtsDescriptor): descriptors
 *
 * System Time Table (A65)
 *
 */
struct _GstMpegtsAtscSTT
{
  guint8     protocol_version;
  guint32    system_time;
  guint8     gps_utc_offset;
  gboolean   ds_status;
  guint8     ds_dayofmonth;
  guint8     ds_hour;
  GPtrArray *descriptors;

  GstDateTime *utc_datetime;
};

GType gst_mpegts_atsc_stt_get_type (void);

const GstMpegtsAtscSTT * gst_mpegts_section_get_atsc_stt (GstMpegtsSection * section);
/* FIXME receive a non-const parameter but we only provide a const getter */
GstDateTime * gst_mpegts_atsc_stt_get_datetime_utc (GstMpegtsAtscSTT * stt);

G_END_DECLS

#endif				/* GST_MPEGTS_SECTION_H */
