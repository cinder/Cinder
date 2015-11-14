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

#ifndef GST_DVB_SECTION_H
#define GST_DVB_SECTION_H

#include <gst/gst.h>
#include <gst/mpegts/gstmpegtssection.h>
#include <gst/mpegts/gstmpegtsdescriptor.h>

G_BEGIN_DECLS

/**
 * GstMpegtsSectionDVBTableID:
 *
 * Values for a #GstMpegtsSection table_id.
 *
 * These are the registered DVB table_id variants.
 *
 * see also: #GstMpegtsSectionTableID
 */
typedef enum {
  /* EN 300 468 (DVB) v 1.12.1 */
  GST_MTS_TABLE_ID_NETWORK_INFORMATION_ACTUAL_NETWORK	= 0x40,
  GST_MTS_TABLE_ID_NETWORK_INFORMATION_OTHER_NETWORK	= 0x41,
  GST_MTS_TABLE_ID_SERVICE_DESCRIPTION_ACTUAL_TS	= 0x42,
  GST_MTS_TABLE_ID_SERVICE_DESCRIPTION_OTHER_TS		= 0x46,
  GST_MTS_TABLE_ID_BOUQUET_ASSOCIATION			= 0x4A,
  GST_MTS_TABLE_ID_EVENT_INFORMATION_ACTUAL_TS_PRESENT	= 0x4E,
  GST_MTS_TABLE_ID_EVENT_INFORMATION_OTHER_TS_PRESENT	= 0x4F,
  GST_MTS_TABLE_ID_EVENT_INFORMATION_ACTUAL_TS_SCHEDULE_1	= 0x50,
  GST_MTS_TABLE_ID_EVENT_INFORMATION_ACTUAL_TS_SCHEDULE_N	= 0x5F,
  GST_MTS_TABLE_ID_EVENT_INFORMATION_OTHER_TS_SCHEDULE_1	= 0x60,
  GST_MTS_TABLE_ID_EVENT_INFORMATION_OTHER_TS_SCHEDULE_N	= 0x6F,
  GST_MTS_TABLE_ID_TIME_DATE				= 0x70,
  GST_MTS_TABLE_ID_RUNNING_STATUS			= 0x71,
  GST_MTS_TABLE_ID_STUFFING				= 0x72,
  GST_MTS_TABLE_ID_TIME_OFFSET				= 0x73,

  /* TS 102 812 (MHP v1.1.3) */
  GST_MTS_TABLE_ID_APPLICATION_INFORMATION_TABLE	= 0x74,

  /* TS 102 323 (DVB TV Anytime v1.5.1) */
  GST_MTS_TABLE_ID_CONTAINER				= 0x75,
  GST_MTS_TABLE_ID_RELATED_CONTENT			= 0x76,
  GST_MTS_TABLE_ID_CONTENT_IDENTIFIER			= 0x77,
  
  /* EN 301 192 (DVB specification for data broadcasting) */
  GST_MTS_TABLE_ID_MPE_FEC				= 0x78,

  /* TS 102 323 (DVB TV Anytime v1.5.1) */
  GST_MTS_TABLE_ID_RESOLUTION_NOTIFICATION		= 0x79,

  /* TS 102 772 (DVB-SH Multi-Protocol Encapsulation) */
  GST_MTS_TABLE_ID_MPE_IFEC				= 0x7A,
  
  /* EN 300 468 (DVB) v 1.12.1 */
  GST_MTS_TABLE_ID_DISCONTINUITY_INFORMATION		= 0x7E,
  GST_MTS_TABLE_ID_SELECTION_INFORMATION		= 0x7F,

  /* ETR 289 (DVB Support for use of scrambling and CA) */
  GST_MTS_TABLE_ID_CA_MESSAGE_ECM_0			= 0x80,
  GST_MTS_TABLE_ID_CA_MESSAGE_ECM_1			= 0x81,
  GST_MTS_TABLE_ID_CA_MESSAGE_SYSTEM_PRIVATE_1		= 0x82,
  GST_MTS_TABLE_ID_CA_MESSAGE_SYSTEM_PRIVATE_N		= 0x8F,

  /* ... */

  /* EN 301 790 (DVB interaction channel for satellite distribution channels) */
  GST_MTS_TABLE_ID_SCT					= 0xA0,
  GST_MTS_TABLE_ID_FCT					= 0xA1,
  GST_MTS_TABLE_ID_TCT					= 0xA2,
  GST_MTS_TABLE_ID_SPT					= 0xA3,
  GST_MTS_TABLE_ID_CMT					= 0xA4,
  GST_MTS_TABLE_ID_TBTP					= 0xA5,
  GST_MTS_TABLE_ID_PCR_PACKET_PAYLOAD			= 0xA6,
  GST_MTS_TABLE_ID_TRANSMISSION_MODE_SUPPORT_PAYLOAD	= 0xAA,
  GST_MTS_TABLE_ID_TIM					= 0xB0,
  GST_MTS_TABLE_ID_LL_FEC_PARITY_DATA_TABLE		= 0xB1

} GstMpegtsSectionDVBTableID;

/**
 * GstMpegtsRunningStatus:
 *
 * Running status of a service.
 *
 * Corresponds to table 6 of ETSI EN 300 468 (v1.13.0)
 */
typedef enum
{
  GST_MPEGTS_RUNNING_STATUS_UNDEFINED = 0,
  GST_MPEGTS_RUNNING_STATUS_NOT_RUNNING,
  GST_MPEGTS_RUNNING_STATUS_STARTS_IN_FEW_SECONDS,
  GST_MPEGTS_RUNNING_STATUS_PAUSING,
  GST_MPEGTS_RUNNING_STATUS_RUNNING,
  GST_MPEGTS_RUNNING_STATUS_OFF_AIR
} GstMpegtsRunningStatus;



/* NIT */

typedef struct _GstMpegtsNITStream GstMpegtsNITStream;
typedef struct _GstMpegtsNIT GstMpegtsNIT;

#define GST_TYPE_MPEGTS_NIT (gst_mpegts_nit_get_type())
#define GST_TYPE_MPEGTS_NIT_STREAM (gst_mpegts_nit_stream_get_type())

/**
 * GstMpegtsNITStream:
 * @transport_stream_id:
 * @original_network_id:
 * @descriptors: (element-type GstMpegtsDescriptor):
 *
 */
struct _GstMpegtsNITStream
{
  guint16  transport_stream_id;
  guint16  original_network_id;

  GPtrArray  *descriptors;
};

/**
 * GstMpegtsNIT:
 * @actual_network: Whether this NIT corresponds to the actual stream
 * @network_id: ID of the network that this NIT describes
 * @descriptors: (element-type GstMpegtsDescriptor): the global descriptors
 * @streams: (element-type GstMpegtsNITStream): the streams
 *
 * Network Information Table (ISO/IEC 13818-1 / EN 300 468)
 *
 */
struct _GstMpegtsNIT
{
  gboolean   actual_network;
  guint16    network_id;

  GPtrArray  *descriptors;

  GPtrArray *streams;
};

GType gst_mpegts_nit_get_type (void);
GType gst_mpegts_nit_stream_get_type (void);

const GstMpegtsNIT *gst_mpegts_section_get_nit (GstMpegtsSection *section);
GstMpegtsSection *gst_mpegts_section_from_nit (GstMpegtsNIT *nit);

GstMpegtsNIT *gst_mpegts_nit_new (void);
GstMpegtsNITStream *gst_mpegts_nit_stream_new (void);


/* BAT */

typedef struct _GstMpegtsBATStream GstMpegtsBATStream;
typedef struct _GstMpegtsBAT GstMpegtsBAT;

#define GST_TYPE_MPEGTS_BAT (gst_mpegts_bat_get_type())
#define GST_TYPE_MPEGTS_BAT_STREAM (gst_mpegts_bat_get_type())

struct _GstMpegtsBATStream
{
  guint16   transport_stream_id;
  guint16   original_network_id;

  GPtrArray   *descriptors;
};

/**
 * GstMpegtsBAT:
 * @descriptors: (element-type GstMpegtsDescriptor):
 * @streams: (element-type GstMpegtsBATStream):
 *
 * DVB Bouquet Association Table (EN 300 468)
 */
struct _GstMpegtsBAT
{
  GPtrArray  *descriptors;

  GPtrArray  *streams;
};

GType gst_mpegts_bat_get_type (void);
GType gst_mpegts_bat_stream_get_type (void);

const GstMpegtsBAT *gst_mpegts_section_get_bat (GstMpegtsSection *section);

/* SDT */
#define GST_TYPE_MPEGTS_SDT (gst_mpegts_sdt_get_type())
#define GST_TYPE_MPEGTS_SDT_SERVICE (gst_mpegts_sdt_service_get_type())

typedef struct _GstMpegtsSDTService GstMpegtsSDTService;
typedef struct _GstMpegtsSDT GstMpegtsSDT;

/**
 * GstMpegtsSDTService:
 * @service_id: The program number this table belongs to
 * @EIT_schedule_flag: EIT schedule information is present in this transport stream
 * @EIT_present_following_flag: EIT present/following information is present in this transport stream
 * @running_status: Status of this service
 * @free_CA_mode: True if one or more streams is controlled by a CA system
 * @descriptors: (element-type GstMpegtsDescriptor): List of descriptors
 *
 */
struct _GstMpegtsSDTService
{
  guint16    service_id;

  gboolean   EIT_schedule_flag;
  gboolean   EIT_present_following_flag;
  GstMpegtsRunningStatus running_status;
  gboolean   free_CA_mode;

  GPtrArray    *descriptors;
};

/**
 * GstMpegtsSDT:
 * @original_network_id: Network ID of the originating delivery system
 * @actual_ts: True if the table describes this transport stream
 * @transport_stream_id: ID of this transport stream
 * @services: (element-type GstMpegtsSDTService): List of services
 *
 * Service Description Table (EN 300 468)
 *
 */
struct _GstMpegtsSDT
{
  guint16    original_network_id;
  gboolean   actual_ts;
  guint16    transport_stream_id;

  GPtrArray *services;
};

GType gst_mpegts_sdt_get_type (void);
GType gst_mpegts_sdt_service_get_type (void);

const GstMpegtsSDT *gst_mpegts_section_get_sdt (GstMpegtsSection *section);

GstMpegtsSection *gst_mpegts_section_from_sdt (GstMpegtsSDT * sdt);

GstMpegtsSDT *gst_mpegts_sdt_new (void);
GstMpegtsSDTService *gst_mpegts_sdt_service_new (void);

/* EIT */

#define GST_TYPE_MPEGTS_EIT (gst_mpegts_eit_get_type())
#define GST_TYPE_MPEGTS_EIT_EVENT (gst_mpegts_eit_event_get_type())

typedef struct _GstMpegtsEITEvent GstMpegtsEITEvent;
typedef struct _GstMpegtsEIT GstMpegtsEIT;

/**
 * GstMpegtsEITEvent:
 * @descriptors: (element-type GstMpegtsDescriptor): List of descriptors
 *
 * Event from a @GstMpegtsEIT
 */
struct _GstMpegtsEITEvent
{
  guint16      event_id;

  GstDateTime *start_time;
  guint32      duration;

  GstMpegtsRunningStatus running_status;
  gboolean     free_CA_mode;

  GPtrArray   *descriptors;
};

/**
 * GstMpegtsEIT:
 * @events: (element-type GstMpegtsEITEvent): List of events
 *
 * Event Information Table (EN 300 468)
 *
 */
struct _GstMpegtsEIT
{
  guint16        transport_stream_id;
  guint16        original_network_id;
  guint8         segment_last_section_number;
  guint8         last_table_id;

  gboolean       actual_stream;
  gboolean       present_following;

  GPtrArray     *events;
};

GType gst_mpegts_eit_get_type (void);
GType gst_mpegts_eit_event_get_type (void);

const GstMpegtsEIT *gst_mpegts_section_get_eit (GstMpegtsSection *section);

/* TDT */
GstDateTime *gst_mpegts_section_get_tdt (GstMpegtsSection *section);

/* TOT */

typedef struct _GstMpegtsTOT GstMpegtsTOT;
#define GST_TYPE_MPEGTS_TOT (gst_mpegts_tot_get_type())
/**
 * GstMpegtsTOT:
 * @descriptors: (element-type GstMpegtsDescriptor): List of descriptors
 *
 * Time Offset Table (EN 300 468)
 *
 */
struct _GstMpegtsTOT
{
  GstDateTime   *utc_time;

  GPtrArray     *descriptors;
};

GType gst_mpegts_tot_get_type (void);
const GstMpegtsTOT *gst_mpegts_section_get_tot (GstMpegtsSection *section);

G_END_DECLS

#endif				/* GST_MPEGTS_SECTION_H */
