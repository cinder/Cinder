/*
 * gst-scte-section.h - 
 * Copyright (C) 2013, CableLabs, Louisville, CO 80027
 * 
 * Authors:
 *   RUIH Team <ruih@cablelabs.com>
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

#ifndef GST_SCTE_SECTION_H
#define GST_SCTE_SECTION_H

#include <gst/gst.h>
#include <gst/mpegts/gstmpegtssection.h>
#include <gst/mpegts/gstmpegtsdescriptor.h>

G_BEGIN_DECLS

/**
 * GstMpegtsScteStreamType:
 * @GST_MPEGTS_STREAM_TYPE_SCTE_SUBTITLING:  SCTE-27 Subtitling
 * @GST_MPEGTS_STREAM_TYPE_SCTE_ISOCH_DATA:  SCTE-19 Isochronous data
 * @GST_MPEGTS_STREAM_TYPE_SCTE_DST_NRT:     SCTE-07 Data Service or
 * Network Resource Table
 * @GST_MPEGTS_STREAM_TYPE_SCTE_DSMCC_DCB:   Type B - DSM-CC Data Carousel
 * [IEC 13818-6])
 * @GST_MPEGTS_STREAM_TYPE_SCTE_SIGNALING:   Enhanced Television Application
 * Signaling (OC-SP-ETV-AM1.0.1-120614)
 * @GST_MPEGTS_STREAM_TYPE_SCTE_SYNC_DATA:   SCTE-07 Synchronous data
 * @GST_MPEGTS_STREAM_TYPE_SCTE_ASYNC_DATA:  SCTE-53 Asynchronous data
 *
 * Type of mpeg-ts streams for SCTE
 *
 */
typedef enum {

  /* 0x01 - 0x82 : defined in other specs */
  GST_MPEGTS_STREAM_TYPE_SCTE_SUBTITLING = 0x82,   /* Subtitling data */
  GST_MPEGTS_STREAM_TYPE_SCTE_ISOCH_DATA = 0x83,   /* Isochronous data */
  /* 0x84 - 0x94 : defined in other specs */
  GST_MPEGTS_STREAM_TYPE_SCTE_DST_NRT    = 0x95,   /* DST / NRT data */
  /* 0x96 - 0xaf : defined in other specs */
  GST_MPEGTS_STREAM_TYPE_SCTE_DSMCC_DCB  = 0xb0,   /* Data Carousel Type B */
  /* 0xb1 - 0xbf : User Private (or defined in other specs) */
  GST_MPEGTS_STREAM_TYPE_SCTE_SIGNALING  = 0xc0,   /* EBIF Signaling */
  GST_MPEGTS_STREAM_TYPE_SCTE_SYNC_DATA  = 0xc2,   /* Synchronous data */
  GST_MPEGTS_STREAM_TYPE_SCTE_ASYNC_DATA = 0xc3,   /* Asynchronous data */
  /* 0xc4 - 0xff : User Private (or defined in other specs) */

} GstMpegtsScteStreamType;


/**
 * GstMpegtsSectionSCTETableID:
 * @GST_MTS_TABLE_ID_SCTE_EAS:    SCTE-18 Emergency Alert System
 * @GST_MTS_TABLE_ID_SCTE_EBIF:   CL-SP-ETV-AM 1.0.1 EBIF message
 * @GST_MTS_TABLE_ID_SCTE_EISS:   CL-SP-ETV-AM 1.0.1 EBIF Int. Signaling Sect.
 * @GST_MTS_TABLE_ID_SCTE_DII:    CL-SP-ETV-AM 1.0.1 DSMCC DII message
 * @GST_MTS_TABLE_ID_SCTE_DDB:    CL-SP-ETV-AM 1.0.1 DSMCC Data Download Block
 * @GST_MTS_TABLE_ID_SCTE_SPLICE: SCTE-35 splice information is carried in a
 * section stream on a separate PID in the program’s Map Table (PMT) allowing
 * Splice Event notifications to remain associated with the program and pass
 * through multiplexers.
 *
 * Values for a #GstMpegtsSection table_id.
 *
 * These are the registered SCTE table_id variants.
 *
 * see also: #GstMpegtsSectionTableID
 */
typedef enum {

  /* 0x01 - 0xd7 : defined in other specs */
  GST_MTS_TABLE_ID_SCTE_EAS      = 0xd8,     /* emergency alert information */
  /* 0xd8 - 0xdf : defined in other specs */
  GST_MTS_TABLE_ID_SCTE_EBIF     = 0xE0,     /* EBIF message */
  GST_MTS_TABLE_ID_SCTE_RESERVED = 0xE1,
  GST_MTS_TABLE_ID_SCTE_EISS     = 0xE2,     /* EBIF Int. Signaling Sect. */
  GST_MTS_TABLE_ID_SCTE_DII      = 0xE3,     /* DSMCC DII message */
  GST_MTS_TABLE_ID_SCTE_DDB      = 0xE4,     /* DSMCC Data Download Block */
  /* 0xe5 - 0xfb : defined in other specs */
  GST_MTS_TABLE_ID_SCTE_SPLICE   = 0xfc,     /* splice information table */

} GstMpegtsSectionSCTETableID;

G_END_DECLS

#endif  /* GST_SCTE_SECTION_H */

