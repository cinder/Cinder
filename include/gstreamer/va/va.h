/*
 * Copyright (c) 2007-2009 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL INTEL AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * Video Acceleration (VA) API Specification
 *
 * Rev. 0.30
 * <jonathan.bian@intel.com>
 *
 * Revision History:
 * rev 0.10 (12/10/2006 Jonathan Bian) - Initial draft
 * rev 0.11 (12/15/2006 Jonathan Bian) - Fixed some errors
 * rev 0.12 (02/05/2007 Jonathan Bian) - Added VC-1 data structures for slice level decode
 * rev 0.13 (02/28/2007 Jonathan Bian) - Added GetDisplay()
 * rev 0.14 (04/13/2007 Jonathan Bian) - Fixed MPEG-2 PictureParameter structure, cleaned up a few funcs.
 * rev 0.15 (04/20/2007 Jonathan Bian) - Overhauled buffer management
 * rev 0.16 (05/02/2007 Jonathan Bian) - Added error codes and fixed some issues with configuration
 * rev 0.17 (05/07/2007 Jonathan Bian) - Added H.264/AVC data structures for slice level decode.
 * rev 0.18 (05/14/2007 Jonathan Bian) - Added data structures for MPEG-4 slice level decode 
 *                                       and MPEG-2 motion compensation.
 * rev 0.19 (08/06/2007 Jonathan Bian) - Removed extra type for bitplane data.
 * rev 0.20 (08/08/2007 Jonathan Bian) - Added missing fields to VC-1 PictureParameter structure.
 * rev 0.21 (08/20/2007 Jonathan Bian) - Added image and subpicture support.
 * rev 0.22 (08/27/2007 Jonathan Bian) - Added support for chroma-keying and global alpha.
 * rev 0.23 (09/11/2007 Jonathan Bian) - Fixed some issues with images and subpictures.
 * rev 0.24 (09/18/2007 Jonathan Bian) - Added display attributes.
 * rev 0.25 (10/18/2007 Jonathan Bian) - Changed to use IDs only for some types.
 * rev 0.26 (11/07/2007 Waldo Bastian) - Change vaCreateBuffer semantics
 * rev 0.27 (11/19/2007 Matt Sottek)   - Added DeriveImage
 * rev 0.28 (12/06/2007 Jonathan Bian) - Added new versions of PutImage and AssociateSubpicture 
 *                                       to enable scaling
 * rev 0.29 (02/07/2008 Jonathan Bian) - VC1 parameter fixes,
 *                                       added VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED
 * rev 0.30 (03/01/2009 Jonathan Bian) - Added encoding support for H.264 BP and MPEG-4 SP and fixes
 *                                       for ISO C conformance.
 * rev 0.31 (09/02/2009 Gwenole Beauchesne) - VC-1/H264 fields change for VDPAU and XvBA backend
 *                                       Application needs to relink with the new library.
 *
 * rev 0.31.1 (03/29/2009)              - Data structure for JPEG encode
 * rev 0.31.2 (01/13/2011 Anthony Pabon)- Added a flag to indicate Subpicture coordinates are screen
 *                                        screen relative rather than source video relative.
 * rev 0.32.0 (01/13/2011 Xiang Haihao) - Add profile into VAPictureParameterBufferVC1
 *                                        update VAAPI to 0.32.0
 *
 * Acknowledgements:
 *  Some concepts borrowed from XvMC and XvImage.
 *  Waldo Bastian (Intel), Matt Sottek (Intel),  Austin Yuan (Intel), and Gwenole Beauchesne (SDS)
 *  contributed to various aspects of the API.
 */

/**
 * \file va.h
 * \brief The Core API
 *
 * This file contains the \ref api_core "Core API".
 */

#ifndef _VA_H_
#define _VA_H_

#include <stddef.h>
#include <stdint.h>
#include <va/va_version.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \mainpage Video Acceleration (VA) API
 *
 * \section intro Introduction
 *
 * The main motivation for VA-API (Video Acceleration API) is to
 * enable hardware accelerated video decode and encode at various
 * entry-points (VLD, IDCT, Motion Compensation etc.) for the
 * prevailing coding standards today (MPEG-2, MPEG-4 ASP/H.263, MPEG-4
 * AVC/H.264, VC-1/VMW3, and JPEG).
 *
 * VA-API is split into several modules:
 * - \ref api_core
 * - \ref api_enc_h264
 * - \ref api_vpp
 */

/**
 * \defgroup api_core Core API
 *
 * @{
 */

/**
Overview 

The VA API is intended to provide an interface between a video decode/encode/display
application (client) and a hardware accelerator (server), to off-load 
video decode/encode/display operations from the host to the hardware accelerator at various 
entry-points.

The basic operation steps are:

- Negotiate a mutually acceptable configuration with the server to lock
  down profile, entrypoints, and other attributes that will not change on 
  a frame-by-frame basis.
- Create a decode context which represents a "virtualized" hardware decode 
  device
- Get and fill decode buffers with picture level, slice level and macroblock 
  level data (depending on entrypoints)
- Pass the decode buffers to the server to decode the current frame

Initialization & Configuration Management 

- Find out supported profiles
- Find out entrypoints for a given profile
- Find out configuration attributes for a given profile/entrypoint pair
- Create a configuration for use by the decoder

*/

typedef void* VADisplay;	/* window system dependent */

typedef int VAStatus;	/** Return status type from functions */
/** Values for the return status */
#define VA_STATUS_SUCCESS			0x00000000
#define VA_STATUS_ERROR_OPERATION_FAILED	0x00000001
#define VA_STATUS_ERROR_ALLOCATION_FAILED	0x00000002
#define VA_STATUS_ERROR_INVALID_DISPLAY		0x00000003
#define VA_STATUS_ERROR_INVALID_CONFIG		0x00000004
#define VA_STATUS_ERROR_INVALID_CONTEXT		0x00000005
#define VA_STATUS_ERROR_INVALID_SURFACE		0x00000006
#define VA_STATUS_ERROR_INVALID_BUFFER		0x00000007
#define VA_STATUS_ERROR_INVALID_IMAGE		0x00000008
#define VA_STATUS_ERROR_INVALID_SUBPICTURE	0x00000009
#define VA_STATUS_ERROR_ATTR_NOT_SUPPORTED	0x0000000a
#define VA_STATUS_ERROR_MAX_NUM_EXCEEDED	0x0000000b
#define VA_STATUS_ERROR_UNSUPPORTED_PROFILE	0x0000000c
#define VA_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT	0x0000000d
#define VA_STATUS_ERROR_UNSUPPORTED_RT_FORMAT	0x0000000e
#define VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE	0x0000000f
#define VA_STATUS_ERROR_SURFACE_BUSY		0x00000010
#define VA_STATUS_ERROR_FLAG_NOT_SUPPORTED      0x00000011
#define VA_STATUS_ERROR_INVALID_PARAMETER	0x00000012
#define VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED 0x00000013
#define VA_STATUS_ERROR_UNIMPLEMENTED           0x00000014
#define VA_STATUS_ERROR_SURFACE_IN_DISPLAYING   0x00000015
#define VA_STATUS_ERROR_INVALID_IMAGE_FORMAT    0x00000016
#define VA_STATUS_ERROR_DECODING_ERROR          0x00000017
#define VA_STATUS_ERROR_ENCODING_ERROR          0x00000018
/**
 * \brief An invalid/unsupported value was supplied.
 *
 * This is a catch-all error code for invalid or unsupported values.
 * e.g. value exceeding the valid range, invalid type in the context
 * of generic attribute values.
 */
#define VA_STATUS_ERROR_INVALID_VALUE           0x00000019
/** \brief An unsupported filter was supplied. */
#define VA_STATUS_ERROR_UNSUPPORTED_FILTER      0x00000020
/** \brief An invalid filter chain was supplied. */
#define VA_STATUS_ERROR_INVALID_FILTER_CHAIN    0x00000021
/** \brief Indicate HW busy (e.g. run multiple encoding simultaneously). */
#define VA_STATUS_ERROR_HW_BUSY	                0x00000022
/** \brief An unsupported memory type was supplied. */
#define VA_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE 0x00000024
#define VA_STATUS_ERROR_UNKNOWN			0xFFFFFFFF

/** De-interlacing flags for vaPutSurface() */
#define VA_FRAME_PICTURE        0x00000000 
#define VA_TOP_FIELD            0x00000001
#define VA_BOTTOM_FIELD         0x00000002

/**
 * Enabled the positioning/cropping/blending feature:
 * 1, specify the video playback position in the isurface
 * 2, specify the cropping info for video playback
 * 3, encoded video will blend with background color
 */
#define VA_ENABLE_BLEND         0x00000004 /* video area blend with the constant color */ 
    
/**
 * Clears the drawable with background color.
 * for hardware overlay based implementation this flag
 * can be used to turn off the overlay
 */
#define VA_CLEAR_DRAWABLE       0x00000008

/** Color space conversion flags for vaPutSurface() */
#define VA_SRC_COLOR_MASK       0x000000f0
#define VA_SRC_BT601            0x00000010
#define VA_SRC_BT709            0x00000020
#define VA_SRC_SMPTE_240        0x00000040

/** Scaling flags for vaPutSurface() */
#define VA_FILTER_SCALING_DEFAULT       0x00000000
#define VA_FILTER_SCALING_FAST          0x00000100
#define VA_FILTER_SCALING_HQ            0x00000200
#define VA_FILTER_SCALING_NL_ANAMORPHIC 0x00000300
#define VA_FILTER_SCALING_MASK          0x00000f00

/**
 * Returns a short english description of error_status
 */
const char *vaErrorStr(VAStatus error_status);

/**
 * Initialization:
 * A display must be obtained by calling vaGetDisplay() before calling
 * vaInitialize() and other functions. This connects the API to the 
 * native window system.
 * For X Windows, native_dpy would be from XOpenDisplay()
 */
typedef void* VANativeDisplay;	/* window system dependent */

int vaDisplayIsValid(VADisplay dpy);

/**
 *  Set the override driver name instead of queried driver driver.
 */
VAStatus vaSetDriverName(VADisplay dpy,
                         char *driver_name
);

/**
 * Initialize the library 
 */
VAStatus vaInitialize (
    VADisplay dpy,
    int *major_version,	 /* out */
    int *minor_version 	 /* out */
);

/**
 * After this call, all library internal resources will be cleaned up
 */ 
VAStatus vaTerminate (
    VADisplay dpy
);

/**
 * vaQueryVendorString returns a pointer to a zero-terminated string
 * describing some aspects of the VA implemenation on a specific    
 * hardware accelerator. The format of the returned string is vendor
 * specific and at the discretion of the implementer.
 * e.g. for the Intel GMA500 implementation, an example would be:
 * "Intel GMA500 - 2.0.0.32L.0005"
 */
const char *vaQueryVendorString (
    VADisplay dpy
);

typedef int (*VAPrivFunc)();

/**
 * Return a function pointer given a function name in the library.
 * This allows private interfaces into the library
 */ 
VAPrivFunc vaGetLibFunc (
    VADisplay dpy,
    const char *func
);

/** Currently defined profiles */
typedef enum
{
    /** \brief Profile ID used for video processing. */
    VAProfileNone                       = -1,
    VAProfileMPEG2Simple		= 0,
    VAProfileMPEG2Main			= 1,
    VAProfileMPEG4Simple		= 2,
    VAProfileMPEG4AdvancedSimple	= 3,
    VAProfileMPEG4Main			= 4,
    VAProfileH264Baseline		= 5,
    VAProfileH264Main			= 6,
    VAProfileH264High			= 7,
    VAProfileVC1Simple			= 8,
    VAProfileVC1Main			= 9,
    VAProfileVC1Advanced		= 10,
    VAProfileH263Baseline		= 11,
    VAProfileJPEGBaseline               = 12,
    VAProfileH264ConstrainedBaseline    = 13,
    VAProfileVP8Version0_3              = 14,
    VAProfileH264MultiviewHigh          = 15,
    VAProfileH264StereoHigh             = 16,
    VAProfileHEVCMain                   = 17,
    VAProfileHEVCMain10                 = 18,
    VAProfileVP9Profile0                = 19
} VAProfile;

/**
 *  Currently defined entrypoints 
 */
typedef enum
{
    VAEntrypointVLD		= 1,
    VAEntrypointIZZ		= 2,
    VAEntrypointIDCT		= 3,
    VAEntrypointMoComp		= 4,
    VAEntrypointDeblocking	= 5,
    VAEntrypointEncSlice	= 6,	/* slice level encode */
    VAEntrypointEncPicture 	= 7,	/* pictuer encode, JPEG, etc */
    VAEntrypointVideoProc       = 10,   /**< Video pre/post-processing. */
} VAEntrypoint;

/** Currently defined configuration attribute types */
typedef enum
{
    VAConfigAttribRTFormat		= 0,
    VAConfigAttribSpatialResidual	= 1,
    VAConfigAttribSpatialClipping	= 2,
    VAConfigAttribIntraResidual		= 3,
    VAConfigAttribEncryption		= 4,
    VAConfigAttribRateControl		= 5,

    /** @name Attributes for decoding */
    /**@{*/
    /**
     * \brief Slice Decoding mode. Read/write.
     *
     * This attribute determines what mode the driver supports for slice
     * decoding, through vaGetConfigAttributes(); and what mode the user
     * will be providing to the driver, through vaCreateConfig(), if the
     * driver supports those. If this attribute is not set by the user then
     * it is assumed that VA_DEC_SLICE_MODE_NORMAL mode is used.
     *
     * See \c VA_DEC_SLICE_MODE_xxx for the list of slice decoding modes.
     */
    VAConfigAttribDecSliceMode		= 6,

    /** @name Attributes for encoding */
    /**@{*/
    /**
     * \brief Packed headers mode. Read/write.
     *
     * This attribute determines what packed headers the driver supports,
     * through vaGetConfigAttributes(); and what packed headers the user
     * will be providing to the driver, through vaCreateConfig(), if the
     * driver supports those.
     *
     * See \c VA_ENC_PACKED_HEADER_xxx for the list of packed headers.
     */
    VAConfigAttribEncPackedHeaders      = 10,
    /**
     * \brief Interlaced mode. Read/write.
     *
     * This attribute determines what kind of interlaced encoding mode
     * the driver supports.
     *
     * See \c VA_ENC_INTERLACED_xxx for the list of interlaced modes.
     */
    VAConfigAttribEncInterlaced         = 11,
    /**
     * \brief Maximum number of reference frames. Read-only.
     *
     * This attribute determines the maximum number of reference
     * frames supported for encoding.
     *
     * Note: for H.264 encoding, the value represents the maximum number
     * of reference frames for both the reference picture list 0 (bottom
     * 16 bits) and the reference picture list 1 (top 16 bits).
     */
    VAConfigAttribEncMaxRefFrames       = 13,
    /**
     * \brief Maximum number of slices per frame. Read-only.
     *
     * This attribute determines the maximum number of slices the
     * driver can support to encode a single frame.
     */
    VAConfigAttribEncMaxSlices          = 14,
    /**
     * \brief Slice structure. Read-only.
     *
     * This attribute determines slice structures supported by the
     * driver for encoding. This attribute is a hint to the user so
     * that he can choose a suitable surface size and how to arrange
     * the encoding process of multiple slices per frame.
     *
     * More specifically, for H.264 encoding, this attribute
     * determines the range of accepted values to
     * VAEncSliceParameterBufferH264::macroblock_address and
     * VAEncSliceParameterBufferH264::num_macroblocks.
     *
     * See \c VA_ENC_SLICE_STRUCTURE_xxx for the supported slice
     * structure types.
     */
    VAConfigAttribEncSliceStructure     = 15,
    /**
     * \brief Macroblock information. Read-only.
     *
     * This attribute determines whether the driver supports extra
     * encoding information per-macroblock. e.g. QP.
     *
     * More specifically, for H.264 encoding, if the driver returns a non-zero
     * value for this attribute, this means the application can create
     * additional #VAEncMacroblockParameterBufferH264 buffers referenced
     * through VAEncSliceParameterBufferH264::macroblock_info.
     */
    VAConfigAttribEncMacroblockInfo     = 16,
    /**
     * \brief JPEG encoding attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the underlying
     * JPEG implementation. The attribute value is partitioned into fields as defined in the 
     * VAConfigAttribValEncJPEG union.
     */
    VAConfigAttribEncJPEG             = 20,
    /**
     * \brief Encoding quality range attribute. Read-only.
     *
     * This attribute conveys whether the driver supports different quality level settings
     * for encoding. A value less than or equal to 1 means that the encoder only has a single
     * quality setting, and a value greater than 1 represents the number of quality levels 
     * that can be configured. e.g. a value of 2 means there are two distinct quality levels. 
     */
    VAConfigAttribEncQualityRange     = 21,
    /**
     * \brief Encoding skip frame attribute. Read-only.
     *
     * This attribute conveys whether the driver supports sending skip frame parameters 
     * (VAEncMiscParameterTypeSkipFrame) to the encoder's rate control, when the user has 
     * externally skipped frames. 
     */
    VAConfigAttribEncSkipFrame        = 24,
    /**@}*/
    VAConfigAttribTypeMax
} VAConfigAttribType;

/**
 * Configuration attributes
 * If there is more than one value for an attribute, a default
 * value will be assigned to the attribute if the client does not
 * specify the attribute when creating a configuration
 */
typedef struct _VAConfigAttrib {
    VAConfigAttribType type;
    unsigned int value; /* OR'd flags (bits) for this attribute */
} VAConfigAttrib;

/** attribute value for VAConfigAttribRTFormat */
#define VA_RT_FORMAT_YUV420	0x00000001	
#define VA_RT_FORMAT_YUV422	0x00000002
#define VA_RT_FORMAT_YUV444	0x00000004
#define VA_RT_FORMAT_YUV411	0x00000008
#define VA_RT_FORMAT_YUV400	0x00000010
#define VA_RT_FORMAT_RGB16	0x00010000
#define VA_RT_FORMAT_RGB32	0x00020000
/* RGBP covers RGBP and BGRP fourcc */ 
#define VA_RT_FORMAT_RGBP	0x00100000
#define VA_RT_FORMAT_PROTECTED	0x80000000

/** @name Attribute values for VAConfigAttribRateControl */
/**@{*/
/** \brief Driver does not support any form of rate control. */
#define VA_RC_NONE                      0x00000001
/** \brief Constant bitrate. */
#define VA_RC_CBR                       0x00000002
/** \brief Variable bitrate. */
#define VA_RC_VBR                       0x00000004
/** \brief Video conference mode. */
#define VA_RC_VCM                       0x00000008
/** \brief Constant QP. */
#define VA_RC_CQP                       0x00000010
/** \brief Variable bitrate with peak rate higher than average bitrate. */
#define VA_RC_VBR_CONSTRAINED           0x00000020
/**@}*/

/** @name Attribute values for VAConfigAttribDecSliceMode */
/**@{*/
/** \brief Driver supports normal mode for slice decoding */
#define VA_DEC_SLICE_MODE_NORMAL       0x00000001
/** \brief Driver supports base mode for slice decoding */
#define VA_DEC_SLICE_MODE_BASE         0x00000002
/**@}*/

/** @name Attribute values for VAConfigAttribEncPackedHeaders */
/**@{*/
/** \brief Driver does not support any packed headers mode. */
#define VA_ENC_PACKED_HEADER_NONE       0x00000000
/** \brief Driver supports packed sequence headers. e.g. SPS for H.264. */
#define VA_ENC_PACKED_HEADER_SEQUENCE   0x00000001
/** \brief Driver supports packed picture headers. e.g. PPS for H.264. */
#define VA_ENC_PACKED_HEADER_PICTURE    0x00000002
/** \brief Driver supports packed slice headers. e.g. \c slice_header() for H.264. */
#define VA_ENC_PACKED_HEADER_SLICE      0x00000004
/** \brief Driver supports misc packed headers. e.g. SEI for H.264. */
#define VA_ENC_PACKED_HEADER_MISC       0x00000008
/** \brief Driver supports raw packed header, see VAEncPackedHeaderRawData */
#define VA_ENC_PACKED_HEADER_RAW_DATA   0x00000010
/**@}*/

/** @name Attribute values for VAConfigAttribEncInterlaced */
/**@{*/
/** \brief Driver does not support interlaced coding. */
#define VA_ENC_INTERLACED_NONE          0x00000000
/** \brief Driver supports interlaced frame coding. */
#define VA_ENC_INTERLACED_FRAME         0x00000001
/** \brief Driver supports interlaced field coding. */
#define VA_ENC_INTERLACED_FIELD         0x00000002
/** \brief Driver supports macroblock adaptive frame field coding. */
#define VA_ENC_INTERLACED_MBAFF         0x00000004
/** \brief Driver supports picture adaptive frame field coding. */
#define VA_ENC_INTERLACED_PAFF          0x00000008
/**@}*/

/** @name Attribute values for VAConfigAttribEncSliceStructure */
/**@{*/
/** \brief Driver supports an arbitrary number of rows per slice. */
#define VA_ENC_SLICE_STRUCTURE_ARBITRARY_ROWS           0x00000000
/** \brief Driver supports a power-of-two number of rows per slice. */
#define VA_ENC_SLICE_STRUCTURE_POWER_OF_TWO_ROWS        0x00000001
/** \brief Driver supports an arbitrary number of rows per slice. */
#define VA_ENC_SLICE_STRUCTURE_ARBITRARY_MACROBLOCKS    0x00000002
/**@}*/

/** \brief Attribute value for VAConfigAttribEncJPEG */
typedef union _VAConfigAttribValEncJPEG {
    struct {
        /** \brief set to 1 for arithmatic coding. */
        unsigned int arithmatic_coding_mode : 1;
        /** \brief set to 1 for progressive dct. */
        unsigned int progressive_dct_mode : 1;
        /** \brief set to 1 for non-interleaved. */
        unsigned int non_interleaved_mode : 1;
        /** \brief set to 1 for differential. */
        unsigned int differential_mode : 1;
        unsigned int max_num_components : 3;
        unsigned int max_num_scans : 4;
        unsigned int max_num_huffman_tables : 3;
        unsigned int max_num_quantization_tables : 3;
    } bits;
    unsigned int value;
} VAConfigAttribValEncJPEG;

/**
 * if an attribute is not applicable for a given
 * profile/entrypoint pair, then set the value to the following 
 */
#define VA_ATTRIB_NOT_SUPPORTED 0x80000000

/** Get maximum number of profiles supported by the implementation */
int vaMaxNumProfiles (
    VADisplay dpy
);

/** Get maximum number of entrypoints supported by the implementation */
int vaMaxNumEntrypoints (
    VADisplay dpy
);

/** Get maximum number of attributs supported by the implementation */
int vaMaxNumConfigAttributes (
    VADisplay dpy
);

/**
 * Query supported profiles 
 * The caller must provide a "profile_list" array that can hold at
 * least vaMaxNumProfile() entries. The actual number of profiles
 * returned in "profile_list" is returned in "num_profile".
 */
VAStatus vaQueryConfigProfiles (
    VADisplay dpy,
    VAProfile *profile_list,	/* out */
    int *num_profiles		/* out */
);

/**
 * Query supported entrypoints for a given profile 
 * The caller must provide an "entrypoint_list" array that can hold at
 * least vaMaxNumEntrypoints() entries. The actual number of entrypoints 
 * returned in "entrypoint_list" is returned in "num_entrypoints".
 */
VAStatus vaQueryConfigEntrypoints (
    VADisplay dpy,
    VAProfile profile,
    VAEntrypoint *entrypoint_list,	/* out */
    int *num_entrypoints		/* out */
);

/**
 * Get attributes for a given profile/entrypoint pair 
 * The caller must provide an "attrib_list" with all attributes to be 
 * retrieved.  Upon return, the attributes in "attrib_list" have been 
 * updated with their value.  Unknown attributes or attributes that are 
 * not supported for the given profile/entrypoint pair will have their 
 * value set to VA_ATTRIB_NOT_SUPPORTED
 */
VAStatus vaGetConfigAttributes (
    VADisplay dpy,
    VAProfile profile,
    VAEntrypoint entrypoint,
    VAConfigAttrib *attrib_list, /* in/out */
    int num_attribs
);

/** Generic ID type, can be re-typed for specific implementation */
typedef unsigned int VAGenericID;

typedef VAGenericID VAConfigID;

/**
 * Create a configuration for the decode pipeline 
 * it passes in the attribute list that specifies the attributes it cares 
 * about, with the rest taking default values.  
 */
VAStatus vaCreateConfig (
    VADisplay dpy,
    VAProfile profile, 
    VAEntrypoint entrypoint, 
    VAConfigAttrib *attrib_list,
    int num_attribs,
    VAConfigID *config_id /* out */
);

/**
 * Free resources associdated with a given config 
 */
VAStatus vaDestroyConfig (
    VADisplay dpy,
    VAConfigID config_id
);

/**
 * Query all attributes for a given configuration 
 * The profile of the configuration is returned in "profile"
 * The entrypoint of the configuration is returned in "entrypoint"
 * The caller must provide an "attrib_list" array that can hold at least 
 * vaMaxNumConfigAttributes() entries. The actual number of attributes 
 * returned in "attrib_list" is returned in "num_attribs"
 */
VAStatus vaQueryConfigAttributes (
    VADisplay dpy,
    VAConfigID config_id, 
    VAProfile *profile, 	/* out */
    VAEntrypoint *entrypoint, 	/* out */
    VAConfigAttrib *attrib_list,/* out */
    int *num_attribs 		/* out */
);


/**
 * Contexts and Surfaces
 *
 * Context represents a "virtual" video decode pipeline. Surfaces are render 
 * targets for a given context. The data in the surfaces are not accessible  
 * to the client and the internal data format of the surface is implementatin 
 * specific. 
 *
 * Surfaces will be bound to a context when the context is created. Once
 * a surface is bound to a given context, it can not be used to create  
 * another context. The association is removed when the context is destroyed
 * 
 * Both contexts and surfaces are identified by unique IDs and its
 * implementation specific internals are kept opaque to the clients
 */

typedef VAGenericID VAContextID;

typedef VAGenericID VASurfaceID;

#define VA_INVALID_ID		0xffffffff
#define VA_INVALID_SURFACE	VA_INVALID_ID

/** \brief Generic value types. */
typedef enum  {
    VAGenericValueTypeInteger = 1,      /**< 32-bit signed integer. */
    VAGenericValueTypeFloat,            /**< 32-bit floating-point value. */
    VAGenericValueTypePointer,          /**< Generic pointer type */
    VAGenericValueTypeFunc              /**< Pointer to function */
} VAGenericValueType;

/** \brief Generic function type. */
typedef void (*VAGenericFunc)(void);

/** \brief Generic value. */
typedef struct _VAGenericValue {
    /** \brief Value type. See #VAGenericValueType. */
    VAGenericValueType  type;
    /** \brief Value holder. */
    union {
        /** \brief 32-bit signed integer. */
        int             i;
        /** \brief 32-bit float. */
        float           f;
        /** \brief Generic pointer. */
        void           *p;
        /** \brief Pointer to function. */
        VAGenericFunc   fn;
    }                   value;
} VAGenericValue;

/** @name Surface attribute flags */
/**@{*/
/** \brief Surface attribute is not supported. */
#define VA_SURFACE_ATTRIB_NOT_SUPPORTED 0x00000000
/** \brief Surface attribute can be got through vaQuerySurfaceAttributes(). */
#define VA_SURFACE_ATTRIB_GETTABLE      0x00000001
/** \brief Surface attribute can be set through vaCreateSurfaces(). */
#define VA_SURFACE_ATTRIB_SETTABLE      0x00000002
/**@}*/

/** \brief Surface attribute types. */
typedef enum {
    VASurfaceAttribNone = 0,
    /**
     * \brief Pixel format (fourcc).
     *
     * The value is meaningful as input to vaQuerySurfaceAttributes().
     * If zero, the driver returns the optimal pixel format for the
     * specified config. Otherwise, if non-zero, the value represents
     * a pixel format (FOURCC) that is kept as is on output, if the
     * driver supports it. Otherwise, the driver sets the value to
     * zero and drops the \c VA_SURFACE_ATTRIB_SETTABLE flag.
     */
    VASurfaceAttribPixelFormat,
    /** \brief Minimal width in pixels (int, read-only). */
    VASurfaceAttribMinWidth,
    /** \brief Maximal width in pixels (int, read-only). */
    VASurfaceAttribMaxWidth,
    /** \brief Minimal height in pixels (int, read-only). */
    VASurfaceAttribMinHeight,
    /** \brief Maximal height in pixels (int, read-only). */
    VASurfaceAttribMaxHeight,
    /** \brief Surface memory type expressed in bit fields (int, read/write). */
    VASurfaceAttribMemoryType,
    /** \brief External buffer descriptor (pointer, write). */
    VASurfaceAttribExternalBufferDescriptor,
    /** \brief Surface usage hint, gives the driver a hint of intended usage 
     *  to optimize allocation (e.g. tiling) (int, read/write). */
    VASurfaceAttribUsageHint,
    /** \brief Number of surface attributes. */
    VASurfaceAttribCount
} VASurfaceAttribType;

/** \brief Surface attribute. */
typedef struct _VASurfaceAttrib {
    /** \brief Type. */
    VASurfaceAttribType type;
    /** \brief Flags. See "Surface attribute flags". */
    unsigned int        flags;
    /** \brief Value. See "Surface attribute types" for the expected types. */
    VAGenericValue      value;
} VASurfaceAttrib;

/** 
 * @name VASurfaceAttribMemoryType values in bit fields. 
 * Bit 0:7 are reserved for generic types, Bit 31:28 are reserved for 
 * Linux DRM, Bit 23:20 are reserved for Android. DRM and Android specific
 * types are defined in DRM and Android header files.
 */
/**@{*/
/** \brief VA memory type (default) is supported. */
#define VA_SURFACE_ATTRIB_MEM_TYPE_VA			0x00000001
/** \brief V4L2 buffer memory type is supported. */
#define VA_SURFACE_ATTRIB_MEM_TYPE_V4L2			0x00000002
/** \brief User pointer memory type is supported. */
#define VA_SURFACE_ATTRIB_MEM_TYPE_USER_PTR		0x00000004
/**@}*/

/** 
 * \brief VASurfaceAttribExternalBuffers structure for 
 * the VASurfaceAttribExternalBufferDescriptor attribute.
 */
typedef struct _VASurfaceAttribExternalBuffers {
    /** \brief pixel format in fourcc. */
    unsigned int pixel_format;
    /** \brief width in pixels. */
    unsigned int width;
    /** \brief height in pixels. */
    unsigned int height;
    /** \brief total size of the buffer in bytes. */
    unsigned int data_size;
    /** \brief number of planes for planar layout */
    unsigned int num_planes;
    /** \brief pitch for each plane in bytes */
    unsigned int pitches[4];
    /** \brief offset for each plane in bytes */
    unsigned int offsets[4];
    /** \brief buffer handles or user pointers */
    unsigned long *buffers;
    /** \brief number of elements in the "buffers" array */
    unsigned int num_buffers;
    /** \brief flags. See "Surface external buffer descriptor flags". */
    unsigned int flags;
    /** \brief reserved for passing private data */
    void *private_data;
} VASurfaceAttribExternalBuffers;

/** @name VASurfaceAttribExternalBuffers flags */
/**@{*/
/** \brief Enable memory tiling */
#define VA_SURFACE_EXTBUF_DESC_ENABLE_TILING	0x00000001
/** \brief Memory is cacheable */
#define VA_SURFACE_EXTBUF_DESC_CACHED		0x00000002
/** \brief Memory is non-cacheable */
#define VA_SURFACE_EXTBUF_DESC_UNCACHED		0x00000004
/** \brief Memory is write-combined */
#define VA_SURFACE_EXTBUF_DESC_WC		0x00000008
/** \brief Memory is protected */
#define VA_SURFACE_EXTBUF_DESC_PROTECTED        0x80000000

/** @name VASurfaceAttribUsageHint attribute usage hint flags */
/**@{*/
/** \brief Surface usage not indicated. */
#define VA_SURFACE_ATTRIB_USAGE_HINT_GENERIC 	0x00000000
/** \brief Surface used by video decoder. */
#define VA_SURFACE_ATTRIB_USAGE_HINT_DECODER 	0x00000001
/** \brief Surface used by video encoder. */
#define VA_SURFACE_ATTRIB_USAGE_HINT_ENCODER 	0x00000002
/** \brief Surface read by video post-processing. */
#define VA_SURFACE_ATTRIB_USAGE_HINT_VPP_READ 	0x00000004
/** \brief Surface written by video post-processing. */
#define VA_SURFACE_ATTRIB_USAGE_HINT_VPP_WRITE 	0x00000008
/** \brief Surface used for display. */
#define VA_SURFACE_ATTRIB_USAGE_HINT_DISPLAY 	0x00000010

/**@}*/

/**
 * \brief Queries surface attributes for the supplied config.
 *
 * Unlike vaGetSurfaceAttributes(), this function queries for all
 * supported attributes for the supplied VA @config. In particular, if
 * the underlying hardware supports the creation of VA surfaces in
 * various formats, then this function will enumerate all pixel
 * formats that are supported.
 *
 * The \c attrib_list array is allocated by the user and \c
 * num_attribs shall be initialized to the number of allocated
 * elements in that array. Upon successful return, the actual number
 * of attributes will be overwritten into \c num_attribs. Otherwise,
 * \c VA_STATUS_ERROR_MAX_NUM_EXCEEDED is returned and \c num_attribs
 * is adjusted to the number of elements that would be returned if
 * enough space was available.
 *
 * Note: it is perfectly valid to pass NULL to the \c attrib_list
 * argument when vaQuerySurfaceAttributes() is used to determine the
 * actual number of elements that need to be allocated.
 *
 * @param[in] dpy               the VA display
 * @param[in] config            the config identifying a codec or a video
 *     processing pipeline
 * @param[out] attrib_list      the output array of #VASurfaceAttrib elements
 * @param[in,out] num_attribs   the number of elements allocated on
 *      input, the number of elements actually filled in output
 */
VAStatus
vaQuerySurfaceAttributes(
    VADisplay           dpy,
    VAConfigID          config,
    VASurfaceAttrib    *attrib_list,
    unsigned int       *num_attribs
);

/**
 * \brief Creates an array of surfaces
 *
 * Creates an array of surfaces. The optional list of attributes shall
 * be constructed and validated through vaGetSurfaceAttributes() or
 * constructed based based on what the underlying hardware could
 * expose through vaQuerySurfaceAttributes().
 *
 * @param[in] dpy               the VA display
 * @param[in] format            the desired surface format. See \c VA_RT_FORMAT_*
 * @param[in] width             the surface width
 * @param[in] height            the surface height
 * @param[out] surfaces         the array of newly created surfaces
 * @param[in] num_surfaces      the number of surfaces to create
 * @param[in] attrib_list       the list of (optional) attributes, or \c NULL
 * @param[in] num_attribs       the number of attributes supplied in
 *     \c attrib_list, or zero
 */
VAStatus
vaCreateSurfaces(
    VADisplay           dpy,
    unsigned int        format,
    unsigned int        width,
    unsigned int        height,
    VASurfaceID        *surfaces,
    unsigned int        num_surfaces,
    VASurfaceAttrib    *attrib_list,
    unsigned int        num_attribs
);
    
/**
 * vaDestroySurfaces - Destroy resources associated with surfaces. 
 *  Surfaces can only be destroyed after the context associated has been 
 *  destroyed.  
 *  dpy: display
 *  surfaces: array of surfaces to destroy
 *  num_surfaces: number of surfaces in the array to be destroyed.
 */
VAStatus vaDestroySurfaces (
    VADisplay dpy,
    VASurfaceID *surfaces,
    int num_surfaces
);

#define VA_PROGRESSIVE 0x1
/**
 * vaCreateContext - Create a context
 *  dpy: display
 *  config_id: configuration for the context
 *  picture_width: coded picture width
 *  picture_height: coded picture height
 *  flag: any combination of the following:
 *    VA_PROGRESSIVE (only progressive frame pictures in the sequence when set)
 *  render_targets: render targets (surfaces) tied to the context
 *  num_render_targets: number of render targets in the above array
 *  context: created context id upon return
 */
VAStatus vaCreateContext (
    VADisplay dpy,
    VAConfigID config_id,
    int picture_width,
    int picture_height,
    int flag,
    VASurfaceID *render_targets,
    int num_render_targets,
    VAContextID *context		/* out */
);

/**
 * vaDestroyContext - Destroy a context 
 *  dpy: display
 *  context: context to be destroyed
 */
VAStatus vaDestroyContext (
    VADisplay dpy,
    VAContextID context
);

/**
 * Buffers 
 * Buffers are used to pass various types of data from the
 * client to the server. The server maintains a data store
 * for each buffer created, and the client idenfies a buffer
 * through a unique buffer id assigned by the server.
 */

typedef VAGenericID VABufferID;

typedef enum
{
    VAPictureParameterBufferType	= 0,
    VAIQMatrixBufferType		= 1,
    VABitPlaneBufferType		= 2,
    VASliceGroupMapBufferType		= 3,
    VASliceParameterBufferType		= 4,
    VASliceDataBufferType		= 5,
    VAMacroblockParameterBufferType	= 6,
    VAResidualDataBufferType		= 7,
    VADeblockingParameterBufferType	= 8,
    VAImageBufferType			= 9,
    VAProtectedSliceDataBufferType	= 10,
    VAQMatrixBufferType                 = 11,
    VAHuffmanTableBufferType            = 12,
    VAProbabilityBufferType             = 13,

/* Following are encode buffer types */
    VAEncCodedBufferType		= 21,
    VAEncSequenceParameterBufferType	= 22,
    VAEncPictureParameterBufferType	= 23,
    VAEncSliceParameterBufferType	= 24,
    VAEncPackedHeaderParameterBufferType = 25,
    VAEncPackedHeaderDataBufferType     = 26,
    VAEncMiscParameterBufferType	= 27,
    VAEncMacroblockParameterBufferType	= 28,
    VAEncMacroblockMapBufferType        = 29,
/* Following are video processing buffer types */
    /**
     * \brief Video processing pipeline parameter buffer.
     *
     * This buffer describes the video processing pipeline. See
     * #VAProcPipelineParameterBuffer for details.
     */
    VAProcPipelineParameterBufferType   = 41,
    /**
     * \brief Video filter parameter buffer.
     *
     * This buffer describes the video filter parameters. All buffers
     * inherit from #VAProcFilterParameterBufferBase, thus including
     * a unique filter buffer type.
     *
     * The default buffer used by most filters is #VAProcFilterParameterBuffer.
     * Filters requiring advanced parameters include, but are not limited to,
     * deinterlacing (#VAProcFilterParameterBufferDeinterlacing),
     * color balance (#VAProcFilterParameterBufferColorBalance), etc.
     */
    VAProcFilterParameterBufferType     = 42,
    VABufferTypeMax
} VABufferType;

typedef enum
{
    VAEncMiscParameterTypeFrameRate 	= 0,
    VAEncMiscParameterTypeRateControl  	= 1,
    VAEncMiscParameterTypeMaxSliceSize	= 2,
    VAEncMiscParameterTypeAIR    	= 3,
    /** \brief Buffer type used to express a maximum frame size (in bits). */
    VAEncMiscParameterTypeMaxFrameSize  = 4,
    /** \brief Buffer type used for HRD parameters. */
    VAEncMiscParameterTypeHRD           = 5,
    VAEncMiscParameterTypeQualityLevel  = 6,
    /** \brief Buffer type used for sending skip frame parameters to the encoder's
      * rate control, when the user has externally skipped frames. */
    VAEncMiscParameterTypeSkipFrame     = 9
} VAEncMiscParameterType;

/** \brief Packed header type. */
typedef enum {
    /** \brief Packed sequence header. */
    VAEncPackedHeaderSequence   = 1,
    /** \brief Packed picture header. */
    VAEncPackedHeaderPicture    = 2,
    /** \brief Packed slice header. */
    VAEncPackedHeaderSlice      = 3,
    /** 
     * \brief Packed raw header. 
     * 
     * Packed raw data header can be used by the client to insert a header  
     * into the bitstream data buffer at the point it is passed, the driver 
     * will handle the raw packed header based on "has_emulation_bytes" field
     * in the packed header parameter structure.
     */
    VAEncPackedHeaderRawData    = 4,
    /** \brief Misc packed header. See codec-specific definitions. */
    VAEncPackedHeaderMiscMask   = 0x80000000,
} VAEncPackedHeaderType;

/** \brief Packed header parameter. */
typedef struct _VAEncPackedHeaderParameterBuffer {
    /** Type of the packed header buffer. See #VAEncPackedHeaderType. */
    unsigned int                type;
    /** \brief Size of the #VAEncPackedHeaderDataBuffer in bits. */
    unsigned int                bit_length;
    /** \brief Flag: buffer contains start code emulation prevention bytes? */
    unsigned char               has_emulation_bytes;
} VAEncPackedHeaderParameterBuffer;

/**
 *  For application, e.g. set a new bitrate
 *    VABufferID buf_id;
 *    VAEncMiscParameterBuffer *misc_param;
 *    VAEncMiscParameterRateControl *misc_rate_ctrl;
 * 
 *    vaCreateBuffer(dpy, context, VAEncMiscParameterBufferType,
 *              sizeof(VAEncMiscParameterBuffer) + sizeof(VAEncMiscParameterRateControl),
 *              1, NULL, &buf_id);
 *
 *    vaMapBuffer(dpy,buf_id,(void **)&misc_param);
 *    misc_param->type = VAEncMiscParameterTypeRateControl;
 *    misc_rate_ctrl= (VAEncMiscParameterRateControl *)misc_param->data;
 *    misc_rate_ctrl->bits_per_second = 6400000;
 *    vaUnmapBuffer(dpy, buf_id);
 *    vaRenderPicture(dpy, context, &buf_id, 1);
 */
typedef struct _VAEncMiscParameterBuffer
{
    VAEncMiscParameterType type;
    unsigned int data[0];
} VAEncMiscParameterBuffer;


/** \brief Rate control parameters */
typedef struct _VAEncMiscParameterRateControl
{
    /* this is the maximum bit-rate to be constrained by the rate control implementation */
    unsigned int bits_per_second;
    /* this is the bit-rate the rate control is targeting, as a percentage of the maximum
     * bit-rate for example if target_percentage is 95 then the rate control will target
     * a bit-rate that is 95% of the maximum bit-rate
     */
    unsigned int target_percentage;
    /* windows size in milliseconds. For example if this is set to 500,
     * then the rate control will guarantee the target bit-rate over a 500 ms window
     */
    unsigned int window_size;
    /* initial QP at I frames */
    unsigned int initial_qp;
    unsigned int min_qp;
    unsigned int basic_unit_size;
    union
    {
        struct
        {
            unsigned int reset : 1;
            unsigned int disable_frame_skip : 1; /* Disable frame skip in rate control mode */
            unsigned int disable_bit_stuffing : 1; /* Disable bit stuffing in rate control mode */
        } bits;
        unsigned int value;
    } rc_flags;
} VAEncMiscParameterRateControl;

typedef struct _VAEncMiscParameterFrameRate
{
    unsigned int framerate;
} VAEncMiscParameterFrameRate;

/**
 * Allow a maximum slice size to be specified (in bits).
 * The encoder will attempt to make sure that individual slices do not exceed this size
 * Or to signal applicate if the slice size exceed this size, see "status" of VACodedBufferSegment
 */
typedef struct _VAEncMiscParameterMaxSliceSize
{
    unsigned int max_slice_size;
} VAEncMiscParameterMaxSliceSize;

typedef struct _VAEncMiscParameterAIR
{
    unsigned int air_num_mbs;
    unsigned int air_threshold;
    unsigned int air_auto; /* if set to 1 then hardware auto-tune the AIR threshold */
} VAEncMiscParameterAIR;

typedef struct _VAEncMiscParameterHRD
{
    unsigned int initial_buffer_fullness;       /* in bits */
    unsigned int buffer_size;                   /* in bits */
} VAEncMiscParameterHRD;

/**
 * \brief Defines a maximum frame size (in bits).
 *
 * This misc parameter buffer defines the maximum size of a frame (in
 * bits). The encoder will try to make sure that each frame does not
 * exceed this size. Otherwise, if the frame size exceeds this size,
 * the \c status flag of #VACodedBufferSegment will contain
 * #VA_CODED_BUF_STATUS_FRAME_SIZE_OVERFLOW.
 */
typedef struct _VAEncMiscParameterBufferMaxFrameSize {
    /** \brief Type. Shall be set to #VAEncMiscParameterTypeMaxFrameSize. */
    VAEncMiscParameterType      type;
    /** \brief Maximum size of a frame (in bits). */
    unsigned int                max_frame_size;
} VAEncMiscParameterBufferMaxFrameSize;

/**
 * \brief Encoding quality level.
 *
 * The encoding quality could be set through this structure, if the implementation  
 * supports multiple quality levels. The quality level set through this structure is 
 * persistent over the entire coded sequence, or until a new structure is being sent.
 * The quality level range can be queried through the VAConfigAttribEncQualityRange 
 * attribute. A lower value means higher quality, and a value of 1 represents the highest 
 * quality. The quality level setting is used as a trade-off between quality and speed/power 
 * consumption, with higher quality corresponds to lower speed and higher power consumption. 
 */
typedef struct _VAEncMiscParameterBufferQualityLevel {
    /** \brief Encoding quality level setting. When set to 0, default quality
     * level is used.
     */
    unsigned int                quality_level;
} VAEncMiscParameterBufferQualityLevel;

/**
 * \brief Encoding skip frame.
 *
 * The application may choose to skip frames externally to the encoder (e.g. drop completely or 
 * code as all skip's). For rate control purposes the encoder will need to know the size and number 
 * of skipped frames.  Skip frame(s) indicated through this structure is applicable only to the 
 * current frame.  It is allowed for the application to still send in packed headers for the driver to 
 * pack, although no frame will be encoded (e.g. for HW to encrypt the frame).  
 */
typedef struct _VAEncMiscParameterSkipFrame {
    /** \brief Indicates skip frames as below.
      * 0: Encode as normal, no skip.
      * 1: One or more frames were skipped prior to the current frame, encode the current frame as normal.  
      * 2: The current frame is to be skipped, do not encode it but pack/encrypt the packed header contents
      *    (all except VAEncPackedHeaderSlice) which could contain actual frame contents (e.g. pack the frame 
      *    in VAEncPackedHeaderPicture).  */
    unsigned char               skip_frame_flag;
    /** \brief The number of frames skipped prior to the current frame.  Valid when skip_frame_flag = 1. */
    unsigned char               num_skip_frames;
    /** \brief When skip_frame_flag = 1, the size of the skipped frames in bits.   When skip_frame_flag = 2, 
      * the size of the current skipped frame that is to be packed/encrypted in bits. */
    unsigned int                size_skip_frames;
} VAEncMiscParameterSkipFrame;

/* 
 * There will be cases where the bitstream buffer will not have enough room to hold
 * the data for the entire slice, and the following flags will be used in the slice
 * parameter to signal to the server for the possible cases.
 * If a slice parameter buffer and slice data buffer pair is sent to the server with 
 * the slice data partially in the slice data buffer (BEGIN and MIDDLE cases below), 
 * then a slice parameter and data buffer needs to be sent again to complete this slice. 
 */
#define VA_SLICE_DATA_FLAG_ALL		0x00	/* whole slice is in the buffer */
#define VA_SLICE_DATA_FLAG_BEGIN	0x01	/* The beginning of the slice is in the buffer but the end if not */
#define VA_SLICE_DATA_FLAG_MIDDLE	0x02	/* Neither beginning nor end of the slice is in the buffer */
#define VA_SLICE_DATA_FLAG_END		0x04	/* end of the slice is in the buffer */

/* Codec-independent Slice Parameter Buffer base */
typedef struct _VASliceParameterBufferBase
{
    unsigned int slice_data_size;	/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;	/* the offset to the first byte of slice data */
    unsigned int slice_data_flag;	/* see VA_SLICE_DATA_FLAG_XXX definitions */
} VASliceParameterBufferBase;

/**********************************
 * JPEG common  data structures
 **********************************/
/**
 * \brief Huffman table for JPEG decoding.
 *
 * This structure holds the complete Huffman tables. This is an
 * aggregation of all Huffman table (DHT) segments maintained by the
 * application. i.e. up to 2 Huffman tables are stored in there for
 * baseline profile.
 *
 * The #load_huffman_table array can be used as a hint to notify the
 * VA driver implementation about which table(s) actually changed
 * since the last submission of this buffer.
 */
typedef struct _VAHuffmanTableBufferJPEGBaseline {
    /** \brief Specifies which #huffman_table is valid. */
    unsigned char       load_huffman_table[2];
    /** \brief Huffman tables indexed by table identifier (Th). */
    struct {
        /** @name DC table (up to 12 categories) */
        /**@{*/
        /** \brief Number of Huffman codes of length i + 1 (Li). */
        unsigned char   num_dc_codes[16];
        /** \brief Value associated with each Huffman code (Vij). */
        unsigned char   dc_values[12];
        /**@}*/
        /** @name AC table (2 special codes + up to 16 * 10 codes) */
        /**@{*/
        /** \brief Number of Huffman codes of length i + 1 (Li). */
        unsigned char   num_ac_codes[16];
        /** \brief Value associated with each Huffman code (Vij). */
        unsigned char   ac_values[162];
        /** \brief Padding to 4-byte boundaries. Must be set to zero. */
        unsigned char   pad[2];
        /**@}*/
    }                   huffman_table[2];
} VAHuffmanTableBufferJPEGBaseline;

/****************************
 * MPEG-2 data structures
 ****************************/
 
/* MPEG-2 Picture Parameter Buffer */
/* 
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VAPictureParameterBufferMPEG2
{
    unsigned short horizontal_size;
    unsigned short vertical_size;
    VASurfaceID forward_reference_picture;
    VASurfaceID backward_reference_picture;
    /* meanings of the following fields are the same as in the standard */
    int picture_coding_type;
    int f_code; /* pack all four fcode into this */
    union {
        struct {
            unsigned int intra_dc_precision		: 2; 
            unsigned int picture_structure		: 2; 
            unsigned int top_field_first		: 1; 
            unsigned int frame_pred_frame_dct		: 1; 
            unsigned int concealment_motion_vectors	: 1;
            unsigned int q_scale_type			: 1;
            unsigned int intra_vlc_format		: 1;
            unsigned int alternate_scan			: 1;
            unsigned int repeat_first_field		: 1;
            unsigned int progressive_frame		: 1;
            unsigned int is_first_field			: 1; /* indicate whether the current field
                                                              * is the first field for field picture
                                                              */
        } bits;
        unsigned int value;
    } picture_coding_extension;
} VAPictureParameterBufferMPEG2;

/** MPEG-2 Inverse Quantization Matrix Buffer */
typedef struct _VAIQMatrixBufferMPEG2
{
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int load_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int load_non_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int load_chroma_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int load_chroma_non_intra_quantiser_matrix;
    /** \brief Luminance intra matrix, in zig-zag scan order. */
    unsigned char intra_quantiser_matrix[64];
    /** \brief Luminance non-intra matrix, in zig-zag scan order. */
    unsigned char non_intra_quantiser_matrix[64];
    /** \brief Chroma intra matrix, in zig-zag scan order. */
    unsigned char chroma_intra_quantiser_matrix[64];
    /** \brief Chroma non-intra matrix, in zig-zag scan order. */
    unsigned char chroma_non_intra_quantiser_matrix[64];
} VAIQMatrixBufferMPEG2;

/** MPEG-2 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferMPEG2
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the first byte of slice data */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned int macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    unsigned int slice_horizontal_position;
    unsigned int slice_vertical_position;
    int quantiser_scale_code;
    int intra_slice_flag;
} VASliceParameterBufferMPEG2;

/** MPEG-2 Macroblock Parameter Buffer */
typedef struct _VAMacroblockParameterBufferMPEG2
{
    unsigned short macroblock_address;
    /* 
     * macroblock_address (in raster scan order)
     * top-left: 0
     * bottom-right: picture-height-in-mb*picture-width-in-mb - 1
     */
    unsigned char macroblock_type;  /* see definition below */
    union {
        struct {
            unsigned int frame_motion_type		: 2; 
            unsigned int field_motion_type		: 2; 
            unsigned int dct_type			: 1; 
        } bits;
        unsigned int value;
    } macroblock_modes;
    unsigned char motion_vertical_field_select; 
    /* 
     * motion_vertical_field_select:
     * see section 6.3.17.2 in the spec
     * only the lower 4 bits are used
     * bit 0: first vector forward
     * bit 1: first vector backward
     * bit 2: second vector forward
     * bit 3: second vector backward
     */
    short PMV[2][2][2]; /* see Table 7-7 in the spec */
    unsigned short coded_block_pattern;
    /* 
     * The bitplanes for coded_block_pattern are described 
     * in Figure 6.10-12 in the spec
     */
     
    /* Number of skipped macroblocks after this macroblock */
    unsigned short num_skipped_macroblocks;
} VAMacroblockParameterBufferMPEG2;

/* 
 * OR'd flags for macroblock_type (section 6.3.17.1 in the spec)
 */
#define VA_MB_TYPE_MOTION_FORWARD	0x02
#define VA_MB_TYPE_MOTION_BACKWARD	0x04
#define VA_MB_TYPE_MOTION_PATTERN	0x08
#define VA_MB_TYPE_MOTION_INTRA		0x10

/**
 * MPEG-2 Residual Data Buffer 
 * For each macroblock, there wil be 64 shorts (16-bit) in the 
 * residual data buffer
 */

/****************************
 * MPEG-4 Part 2 data structures
 ****************************/
 
/* MPEG-4 Picture Parameter Buffer */
/* 
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VAPictureParameterBufferMPEG4
{
    unsigned short vop_width;
    unsigned short vop_height;
    VASurfaceID forward_reference_picture;
    VASurfaceID backward_reference_picture;
    union {
        struct {
            unsigned int short_video_header		: 1; 
            unsigned int chroma_format			: 2; 
            unsigned int interlaced			: 1; 
            unsigned int obmc_disable			: 1; 
            unsigned int sprite_enable			: 2; 
            unsigned int sprite_warping_accuracy	: 2; 
            unsigned int quant_type			: 1; 
            unsigned int quarter_sample			: 1; 
            unsigned int data_partitioned		: 1; 
            unsigned int reversible_vlc			: 1; 
            unsigned int resync_marker_disable		: 1; 
        } bits;
        unsigned int value;
    } vol_fields;
    unsigned char no_of_sprite_warping_points;
    short sprite_trajectory_du[3];
    short sprite_trajectory_dv[3];
    unsigned char quant_precision;
    union {
        struct {
            unsigned int vop_coding_type		: 2; 
            unsigned int backward_reference_vop_coding_type	: 2; 
            unsigned int vop_rounding_type		: 1; 
            unsigned int intra_dc_vlc_thr		: 3; 
            unsigned int top_field_first		: 1; 
            unsigned int alternate_vertical_scan_flag	: 1; 
        } bits;
        unsigned int value;
    } vop_fields;
    unsigned char vop_fcode_forward;
    unsigned char vop_fcode_backward;
    unsigned short vop_time_increment_resolution;
    /* short header related */
    unsigned char num_gobs_in_vop;
    unsigned char num_macroblocks_in_gob;
    /* for direct mode prediction */
    short TRB;
    short TRD;
} VAPictureParameterBufferMPEG4;

/** MPEG-4 Inverse Quantization Matrix Buffer */
typedef struct _VAIQMatrixBufferMPEG4
{
    /** Same as the MPEG-4:2 bitstream syntax element. */
    int load_intra_quant_mat;
    /** Same as the MPEG-4:2 bitstream syntax element. */
    int load_non_intra_quant_mat;
    /** The matrix for intra blocks, in zig-zag scan order. */
    unsigned char intra_quant_mat[64];
    /** The matrix for non-intra blocks, in zig-zag scan order. */
    unsigned char non_intra_quant_mat[64];
} VAIQMatrixBufferMPEG4;

/** MPEG-4 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferMPEG4
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the first byte of slice data */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned int macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    unsigned int macroblock_number;
    int quant_scale;
} VASliceParameterBufferMPEG4;

/**
 VC-1 data structures
*/

typedef enum   /* see 7.1.1.32 */
{
    VAMvMode1Mv                        = 0,
    VAMvMode1MvHalfPel                 = 1,
    VAMvMode1MvHalfPelBilinear         = 2,
    VAMvModeMixedMv                    = 3,
    VAMvModeIntensityCompensation      = 4 
} VAMvModeVC1;

/** VC-1 Picture Parameter Buffer */
/* 
 * For each picture, and before any slice data, a picture parameter
 * buffer must be send. Multiple picture parameter buffers may be
 * sent for a single picture. In that case picture parameters will
 * apply to all slice data that follow it until a new picture
 * parameter buffer is sent.
 *
 * Notes:
 *   pic_quantizer_type should be set to the applicable quantizer
 *   type as defined by QUANTIZER (J.1.19) and either
 *   PQUANTIZER (7.1.1.8) or PQINDEX (7.1.1.6)
 */
typedef struct _VAPictureParameterBufferVC1
{
    VASurfaceID forward_reference_picture;
    VASurfaceID backward_reference_picture;
    /* if out-of-loop post-processing is done on the render
       target, then we need to keep the in-loop decoded 
       picture as a reference picture */
    VASurfaceID inloop_decoded_picture;

    /* sequence layer for AP or meta data for SP and MP */
    union {
        struct {
            unsigned int pulldown	: 1; /* SEQUENCE_LAYER::PULLDOWN */
            unsigned int interlace	: 1; /* SEQUENCE_LAYER::INTERLACE */
            unsigned int tfcntrflag	: 1; /* SEQUENCE_LAYER::TFCNTRFLAG */
            unsigned int finterpflag	: 1; /* SEQUENCE_LAYER::FINTERPFLAG */
            unsigned int psf		: 1; /* SEQUENCE_LAYER::PSF */
            unsigned int multires	: 1; /* METADATA::MULTIRES */
            unsigned int overlap	: 1; /* METADATA::OVERLAP */
            unsigned int syncmarker	: 1; /* METADATA::SYNCMARKER */
            unsigned int rangered	: 1; /* METADATA::RANGERED */
            unsigned int max_b_frames	: 3; /* METADATA::MAXBFRAMES */
            unsigned int profile	: 2; /* SEQUENCE_LAYER::PROFILE or The MSB of METADATA::PROFILE */
        } bits;
        unsigned int value;
    } sequence_fields;

    unsigned short coded_width;		/* ENTRY_POINT_LAYER::CODED_WIDTH */
    unsigned short coded_height;	/* ENTRY_POINT_LAYER::CODED_HEIGHT */
    union {
	struct {
            unsigned int broken_link	: 1; /* ENTRY_POINT_LAYER::BROKEN_LINK */
            unsigned int closed_entry	: 1; /* ENTRY_POINT_LAYER::CLOSED_ENTRY */
            unsigned int panscan_flag	: 1; /* ENTRY_POINT_LAYER::PANSCAN_FLAG */
            unsigned int loopfilter	: 1; /* ENTRY_POINT_LAYER::LOOPFILTER */
	} bits;
	unsigned int value;
    } entrypoint_fields;
    unsigned char conditional_overlap_flag; /* ENTRY_POINT_LAYER::CONDOVER */
    unsigned char fast_uvmc_flag;	/* ENTRY_POINT_LAYER::FASTUVMC */
    union {
        struct {
            unsigned int luma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPY_FLAG */
            unsigned int luma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPY */
            unsigned int chroma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPUV_FLAG */
            unsigned int chroma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPUV */
        } bits;
        unsigned int value;
    } range_mapping_fields;

    unsigned char b_picture_fraction;	/* PICTURE_LAYER::BFRACTION */
    unsigned char cbp_table;		/* PICTURE_LAYER::CBPTAB/ICBPTAB */
    unsigned char mb_mode_table;	/* PICTURE_LAYER::MBMODETAB */
    unsigned char range_reduction_frame;/* PICTURE_LAYER::RANGEREDFRM */
    unsigned char rounding_control;	/* PICTURE_LAYER::RNDCTRL */
    unsigned char post_processing;	/* PICTURE_LAYER::POSTPROC */
    unsigned char picture_resolution_index;	/* PICTURE_LAYER::RESPIC */
    unsigned char luma_scale;		/* PICTURE_LAYER::LUMSCALE */
    unsigned char luma_shift;		/* PICTURE_LAYER::LUMSHIFT */
    union {
        struct {
            unsigned int picture_type		: 3; /* PICTURE_LAYER::PTYPE */
            unsigned int frame_coding_mode	: 3; /* PICTURE_LAYER::FCM */
            unsigned int top_field_first	: 1; /* PICTURE_LAYER::TFF */
            unsigned int is_first_field		: 1; /* set to 1 if it is the first field */
            unsigned int intensity_compensation	: 1; /* PICTURE_LAYER::INTCOMP */
        } bits;
        unsigned int value;
    } picture_fields;
    union {
        struct {
            unsigned int mv_type_mb	: 1; 	/* PICTURE::MVTYPEMB */
            unsigned int direct_mb	: 1; 	/* PICTURE::DIRECTMB */
            unsigned int skip_mb	: 1; 	/* PICTURE::SKIPMB */
            unsigned int field_tx	: 1; 	/* PICTURE::FIELDTX */
            unsigned int forward_mb	: 1;	/* PICTURE::FORWARDMB */
            unsigned int ac_pred	: 1;	/* PICTURE::ACPRED */
            unsigned int overflags	: 1;	/* PICTURE::OVERFLAGS */
        } flags;
        unsigned int value;
    } raw_coding;
    union {
        struct {
            unsigned int bp_mv_type_mb   : 1;    /* PICTURE::MVTYPEMB */
            unsigned int bp_direct_mb    : 1;    /* PICTURE::DIRECTMB */
            unsigned int bp_skip_mb      : 1;    /* PICTURE::SKIPMB */  
            unsigned int bp_field_tx     : 1;    /* PICTURE::FIELDTX */ 
            unsigned int bp_forward_mb   : 1;    /* PICTURE::FORWARDMB */
            unsigned int bp_ac_pred      : 1;    /* PICTURE::ACPRED */   
            unsigned int bp_overflags    : 1;    /* PICTURE::OVERFLAGS */
        } flags;
        unsigned int value;
    } bitplane_present; /* signal what bitplane is being passed via the bitplane buffer */
    union {
        struct {
            unsigned int reference_distance_flag : 1;/* PICTURE_LAYER::REFDIST_FLAG */
            unsigned int reference_distance	: 5;/* PICTURE_LAYER::REFDIST */
            unsigned int num_reference_pictures: 1;/* PICTURE_LAYER::NUMREF */
            unsigned int reference_field_pic_indicator	: 1;/* PICTURE_LAYER::REFFIELD */
        } bits;
        unsigned int value;
    } reference_fields;
    union {
        struct {
            unsigned int mv_mode		: 3; /* PICTURE_LAYER::MVMODE */
            unsigned int mv_mode2		: 3; /* PICTURE_LAYER::MVMODE2 */
            unsigned int mv_table		: 3; /* PICTURE_LAYER::MVTAB/IMVTAB */
            unsigned int two_mv_block_pattern_table: 2; /* PICTURE_LAYER::2MVBPTAB */
            unsigned int four_mv_switch		: 1; /* PICTURE_LAYER::4MVSWITCH */
            unsigned int four_mv_block_pattern_table : 2; /* PICTURE_LAYER::4MVBPTAB */
            unsigned int extended_mv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_MV */
            unsigned int extended_mv_range	: 2; /* PICTURE_LAYER::MVRANGE */
            unsigned int extended_dmv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_DMV */
            unsigned int extended_dmv_range	: 2; /* PICTURE_LAYER::DMVRANGE */
        } bits;
        unsigned int value;
    } mv_fields;
    union {
        struct {
            unsigned int dquant	: 2; 	/* ENTRY_POINT_LAYER::DQUANT */
            unsigned int quantizer     : 2; 	/* ENTRY_POINT_LAYER::QUANTIZER */
            unsigned int half_qp	: 1; 	/* PICTURE_LAYER::HALFQP */
            unsigned int pic_quantizer_scale : 5;/* PICTURE_LAYER::PQUANT */
            unsigned int pic_quantizer_type : 1;/* PICTURE_LAYER::PQUANTIZER */
            unsigned int dq_frame	: 1; 	/* VOPDQUANT::DQUANTFRM */
            unsigned int dq_profile	: 2; 	/* VOPDQUANT::DQPROFILE */
            unsigned int dq_sb_edge	: 2; 	/* VOPDQUANT::DQSBEDGE */
            unsigned int dq_db_edge 	: 2; 	/* VOPDQUANT::DQDBEDGE */
            unsigned int dq_binary_level : 1; 	/* VOPDQUANT::DQBILEVEL */
            unsigned int alt_pic_quantizer : 5;/* VOPDQUANT::ALTPQUANT */
        } bits;
        unsigned int value;
    } pic_quantizer_fields;
    union {
        struct {
            unsigned int variable_sized_transform_flag	: 1;/* ENTRY_POINT_LAYER::VSTRANSFORM */
            unsigned int mb_level_transform_type_flag	: 1;/* PICTURE_LAYER::TTMBF */
            unsigned int frame_level_transform_type	: 2;/* PICTURE_LAYER::TTFRM */
            unsigned int transform_ac_codingset_idx1	: 2;/* PICTURE_LAYER::TRANSACFRM */
            unsigned int transform_ac_codingset_idx2	: 2;/* PICTURE_LAYER::TRANSACFRM2 */
            unsigned int intra_transform_dc_table	: 1;/* PICTURE_LAYER::TRANSDCTAB */
        } bits;
        unsigned int value;
    } transform_fields;
} VAPictureParameterBufferVC1;

/** VC-1 Bitplane Buffer
There will be at most three bitplanes coded in any picture header. To send 
the bitplane data more efficiently, each byte is divided in two nibbles, with
each nibble carrying three bitplanes for one macroblock.  The following table
shows the bitplane data arrangement within each nibble based on the picture
type.

Picture Type	Bit3		Bit2		Bit1		Bit0
I or BI				OVERFLAGS	ACPRED		FIELDTX
P				MYTYPEMB	SKIPMB		DIRECTMB
B				FORWARDMB	SKIPMB		DIRECTMB

Within each byte, the lower nibble is for the first MB and the upper nibble is 
for the second MB.  E.g. the lower nibble of the first byte in the bitplane
buffer is for Macroblock #1 and the upper nibble of the first byte is for 
Macroblock #2 in the first row.
*/

/* VC-1 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferVC1
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    unsigned int slice_data_offset;/* the offset to the first byte of slice data */
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    unsigned int macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    unsigned int slice_vertical_position;
} VASliceParameterBufferVC1;

/* VC-1 Slice Data Buffer */
/* 
This is simplely a buffer containing raw bit-stream bytes 
*/

/****************************
 * H.264/AVC data structures
 ****************************/

typedef struct _VAPictureH264
{
    VASurfaceID picture_id;
    unsigned int frame_idx;
    unsigned int flags;
    signed int TopFieldOrderCnt;
    signed int BottomFieldOrderCnt;
} VAPictureH264;
/* flags in VAPictureH264 could be OR of the following */
#define VA_PICTURE_H264_INVALID			0x00000001
#define VA_PICTURE_H264_TOP_FIELD		0x00000002
#define VA_PICTURE_H264_BOTTOM_FIELD		0x00000004
#define VA_PICTURE_H264_SHORT_TERM_REFERENCE	0x00000008
#define VA_PICTURE_H264_LONG_TERM_REFERENCE	0x00000010

/** H.264 Picture Parameter Buffer */
/* 
 * For each picture, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VAPictureParameterBufferH264
{
    VAPictureH264 CurrPic;
    VAPictureH264 ReferenceFrames[16];	/* in DPB */
    unsigned short picture_width_in_mbs_minus1;
    unsigned short picture_height_in_mbs_minus1;
    unsigned char bit_depth_luma_minus8;
    unsigned char bit_depth_chroma_minus8;
    unsigned char num_ref_frames;
    union {
        struct {
            unsigned int chroma_format_idc			: 2; 
            unsigned int residual_colour_transform_flag		: 1; 
            unsigned int gaps_in_frame_num_value_allowed_flag	: 1; 
            unsigned int frame_mbs_only_flag			: 1; 
            unsigned int mb_adaptive_frame_field_flag		: 1; 
            unsigned int direct_8x8_inference_flag		: 1; 
            unsigned int MinLumaBiPredSize8x8			: 1; /* see A.3.3.2 */
            unsigned int log2_max_frame_num_minus4		: 4;
            unsigned int pic_order_cnt_type			: 2;
            unsigned int log2_max_pic_order_cnt_lsb_minus4	: 4;
            unsigned int delta_pic_order_always_zero_flag	: 1;
        } bits;
        unsigned int value;
    } seq_fields;
    unsigned char num_slice_groups_minus1;
    unsigned char slice_group_map_type;
    unsigned short slice_group_change_rate_minus1;
    signed char pic_init_qp_minus26;
    signed char pic_init_qs_minus26;
    signed char chroma_qp_index_offset;
    signed char second_chroma_qp_index_offset;
    union {
        struct {
            unsigned int entropy_coding_mode_flag	: 1;
            unsigned int weighted_pred_flag		: 1;
            unsigned int weighted_bipred_idc		: 2;
            unsigned int transform_8x8_mode_flag	: 1;
            unsigned int field_pic_flag			: 1;
            unsigned int constrained_intra_pred_flag	: 1;
            unsigned int pic_order_present_flag			: 1;
            unsigned int deblocking_filter_control_present_flag : 1;
            unsigned int redundant_pic_cnt_present_flag		: 1;
            unsigned int reference_pic_flag			: 1; /* nal_ref_idc != 0 */
        } bits;
        unsigned int value;
    } pic_fields;
    unsigned short frame_num;
} VAPictureParameterBufferH264;

/** H.264 Inverse Quantization Matrix Buffer */
typedef struct _VAIQMatrixBufferH264
{
    /** \brief 4x4 scaling list, in raster scan order. */
    unsigned char ScalingList4x4[6][16];
    /** \brief 8x8 scaling list, in raster scan order. */
    unsigned char ScalingList8x8[2][64];
} VAIQMatrixBufferH264;

/**
 * H.264 Slice Group Map Buffer 
 * When VAPictureParameterBufferH264::num_slice_group_minus1 is not equal to 0,
 * A slice group map buffer should be sent for each picture if required. The buffer
 * is sent only when there is a change in the mapping values.
 * The slice group map buffer map "map units" to slice groups as specified in 
 * section 8.2.2 of the H.264 spec. The buffer will contain one byte for each macroblock 
 * in raster scan order
 */ 

/** H.264 Slice Parameter Buffer */
typedef struct _VASliceParameterBufferH264
{
    unsigned int slice_data_size;/* number of bytes in the slice data buffer for this slice */
    /** \brief Byte offset to the NAL Header Unit for this slice. */
    unsigned int slice_data_offset;
    unsigned int slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    /**
     * \brief Bit offset from NAL Header Unit to the begining of slice_data().
     *
     * This bit offset is relative to and includes the NAL unit byte
     * and represents the number of bits parsed in the slice_header()
     * after the removal of any emulation prevention bytes in
     * there. However, the slice data buffer passed to the hardware is
     * the original bitstream, thus including any emulation prevention
     * bytes.
     */
    unsigned short slice_data_bit_offset;
    unsigned short first_mb_in_slice;
    unsigned char slice_type;
    unsigned char direct_spatial_mv_pred_flag;
    unsigned char num_ref_idx_l0_active_minus1;
    unsigned char num_ref_idx_l1_active_minus1;
    unsigned char cabac_init_idc;
    char slice_qp_delta;
    unsigned char disable_deblocking_filter_idc;
    char slice_alpha_c0_offset_div2;
    char slice_beta_offset_div2;
    VAPictureH264 RefPicList0[32];	/* See 8.2.4.2 */
    VAPictureH264 RefPicList1[32];	/* See 8.2.4.2 */
    unsigned char luma_log2_weight_denom;
    unsigned char chroma_log2_weight_denom;
    unsigned char luma_weight_l0_flag;
    short luma_weight_l0[32];
    short luma_offset_l0[32];
    unsigned char chroma_weight_l0_flag;
    short chroma_weight_l0[32][2];
    short chroma_offset_l0[32][2];
    unsigned char luma_weight_l1_flag;
    short luma_weight_l1[32];
    short luma_offset_l1[32];
    unsigned char chroma_weight_l1_flag;
    short chroma_weight_l1[32][2];
    short chroma_offset_l1[32][2];
} VASliceParameterBufferH264;

/****************************
 * Common encode data structures 
 ****************************/
typedef enum
{
    VAEncPictureTypeIntra		= 0,
    VAEncPictureTypePredictive		= 1,
    VAEncPictureTypeBidirectional	= 2,
} VAEncPictureType;

/* Encode Slice Parameter Buffer */
typedef struct _VAEncSliceParameterBuffer
{
    unsigned int start_row_number;	/* starting MB row number for this slice */
    unsigned int slice_height;	/* slice height measured in MB */
    union {
        struct {
            unsigned int is_intra	: 1;
            unsigned int disable_deblocking_filter_idc : 2;
            unsigned int uses_long_term_ref		:1;
            unsigned int is_long_term_ref		:1;
        } bits;
        unsigned int value;
    } slice_flags;
} VAEncSliceParameterBuffer;


/****************************
 * H.263 specific encode data structures
 ****************************/

typedef struct _VAEncSequenceParameterBufferH263
{
    unsigned int intra_period;
    unsigned int bits_per_second;
    unsigned int frame_rate;
    unsigned int initial_qp;
    unsigned int min_qp;
} VAEncSequenceParameterBufferH263;

typedef struct _VAEncPictureParameterBufferH263
{
    VASurfaceID reference_picture;
    VASurfaceID reconstructed_picture;
    VABufferID coded_buf;
    unsigned short picture_width;
    unsigned short picture_height;
    VAEncPictureType picture_type;
} VAEncPictureParameterBufferH263;

/****************************
 * MPEG-4 specific encode data structures
 ****************************/

typedef struct _VAEncSequenceParameterBufferMPEG4
{
    unsigned char profile_and_level_indication;
    unsigned int intra_period;
    unsigned int video_object_layer_width;
    unsigned int video_object_layer_height;
    unsigned int vop_time_increment_resolution;
    unsigned int fixed_vop_rate;
    unsigned int fixed_vop_time_increment;
    unsigned int bits_per_second;
    unsigned int frame_rate;
    unsigned int initial_qp;
    unsigned int min_qp;
} VAEncSequenceParameterBufferMPEG4;

typedef struct _VAEncPictureParameterBufferMPEG4
{
    VASurfaceID reference_picture;
    VASurfaceID reconstructed_picture;
    VABufferID coded_buf;
    unsigned short picture_width;
    unsigned short picture_height;
    unsigned int modulo_time_base; /* number of 1s */
    unsigned int vop_time_increment;
    VAEncPictureType picture_type;
} VAEncPictureParameterBufferMPEG4;



/** Buffer functions */

/**
 * Creates a buffer for "num_elements" elements of "size" bytes and 
 * initalize with "data".
 * if "data" is null, then the contents of the buffer data store
 * are undefined.
 * Basically there are two ways to get buffer data to the server side. One is 
 * to call vaCreateBuffer() with a non-null "data", which results the data being
 * copied to the data store on the server side.  A different method that 
 * eliminates this copy is to pass null as "data" when calling vaCreateBuffer(),
 * and then use vaMapBuffer() to map the data store from the server side to the
 * client address space for access.
 *  Note: image buffers are created by the library, not the client. Please see 
 *        vaCreateImage on how image buffers are managed.
 */
VAStatus vaCreateBuffer (
    VADisplay dpy,
    VAContextID context,
    VABufferType type,	/* in */
    unsigned int size,	/* in */
    unsigned int num_elements, /* in */
    void *data,		/* in */
    VABufferID *buf_id	/* out */
);

/**
 * Convey to the server how many valid elements are in the buffer. 
 * e.g. if multiple slice parameters are being held in a single buffer,
 * this will communicate to the server the number of slice parameters
 * that are valid in the buffer.
 */
VAStatus vaBufferSetNumElements (
    VADisplay dpy,
    VABufferID buf_id,	/* in */
    unsigned int num_elements /* in */
);


/**
 * device independent data structure for codedbuffer
 */

/* 
 * FICTURE_AVE_QP(bit7-0): The average Qp value used during this frame
 * LARGE_SLICE(bit8):At least one slice in the current frame was large
 *              enough for the encoder to attempt to limit its size.
 * SLICE_OVERFLOW(bit9): At least one slice in the current frame has
 *              exceeded the maximum slice size specified.
 * BITRATE_OVERFLOW(bit10): The peak bitrate was exceeded for this frame.
 * BITRATE_HIGH(bit11): The frame size got within the safety margin of the maximum size (VCM only)
 * AIR_MB_OVER_THRESHOLD: the number of MBs adapted to Intra MB
 */
#define VA_CODED_BUF_STATUS_PICTURE_AVE_QP_MASK         0xff
#define VA_CODED_BUF_STATUS_LARGE_SLICE_MASK            0x100
#define VA_CODED_BUF_STATUS_SLICE_OVERFLOW_MASK         0x200
#define VA_CODED_BUF_STATUS_BITRATE_OVERFLOW		0x400
#define VA_CODED_BUF_STATUS_BITRATE_HIGH		0x800
/**
 * \brief The frame has exceeded the maximum requested size.
 *
 * This flag indicates that the encoded frame size exceeds the value
 * specified through a misc parameter buffer of type
 * #VAEncMiscParameterTypeMaxFrameSize.
 */
#define VA_CODED_BUF_STATUS_FRAME_SIZE_OVERFLOW         0x1000
#define VA_CODED_BUF_STATUS_AIR_MB_OVER_THRESHOLD	0xff0000

/**
 * \brief The coded buffer segment contains a single NAL unit. 
 *
 * This flag indicates that the coded buffer segment contains a
 * single NAL unit. This flag might be useful to the user for 
 * processing the coded buffer.
 */
#define VA_CODED_BUF_STATUS_SINGLE_NALU                 0x10000000	

/**
 * \brief Coded buffer segment.
 *
 * #VACodedBufferSegment is an element of a linked list describing
 * some information on the coded buffer. The coded buffer segment
 * could contain either a single NAL unit, or more than one NAL unit. 
 * It is recommended (but not required) to return a single NAL unit 
 * in a coded buffer segment, and the implementation should set the 
 * VA_CODED_BUF_STATUS_SINGLE_NALU status flag if that is the case.
 */
typedef  struct _VACodedBufferSegment  {
    /**
     * \brief Size of the data buffer in this segment (in bytes).
     */
    unsigned int        size;
    /** \brief Bit offset into the data buffer where the video data starts. */
    unsigned int        bit_offset;
    /** \brief Status set by the driver. See \c VA_CODED_BUF_STATUS_*. */
    unsigned int        status;
    /** \brief Reserved for future use. */
    unsigned int        reserved;
    /** \brief Pointer to the start of the data buffer. */
    void               *buf;
    /**
     * \brief Pointer to the next #VACodedBufferSegment element,
     * or \c NULL if there is none.
     */
    void               *next;
} VACodedBufferSegment;
     
/**
 * Map data store of the buffer into the client's address space
 * vaCreateBuffer() needs to be called with "data" set to NULL before
 * calling vaMapBuffer()
 *
 * if buffer type is VAEncCodedBufferType, pbuf points to link-list of
 * VACodedBufferSegment, and the list is terminated if "next" is NULL
 */
VAStatus vaMapBuffer (
    VADisplay dpy,
    VABufferID buf_id,	/* in */
    void **pbuf 	/* out */
);

/**
 * After client making changes to a mapped data store, it needs to
 * "Unmap" it to let the server know that the data is ready to be
 * consumed by the server
 */
VAStatus vaUnmapBuffer (
    VADisplay dpy,
    VABufferID buf_id	/* in */
);

/**
 * After this call, the buffer is deleted and this buffer_id is no longer valid
 * Only call this if the buffer is not going to be passed to vaRenderBuffer
 */
VAStatus vaDestroyBuffer (
    VADisplay dpy,
    VABufferID buffer_id
);

/** \brief VA buffer information */
typedef struct {
    /** \brief Buffer handle */
    uintptr_t           handle;
    /** \brief Buffer type (See \ref VABufferType). */
    uint32_t            type;
    /**
     * \brief Buffer memory type (See \ref VASurfaceAttribMemoryType).
     *
     * On input to vaAcquireBufferHandle(), this field can serve as a hint
     * to specify the set of memory types the caller is interested in.
     * On successful return from vaAcquireBufferHandle(), the field is
     * updated with the best matching memory type.
     */
    uint32_t            mem_type;
    /** \brief Size of the underlying buffer. */
    size_t              mem_size;
} VABufferInfo;

/**
 * \brief Acquires buffer handle for external API usage
 *
 * Locks the VA buffer object \ref buf_id for external API usage like
 * EGL or OpenCL (OCL). This function is a synchronization point. This
 * means that any pending operation is guaranteed to be completed
 * prior to returning from the function.
 *
 * If the referenced VA buffer object is the backing store of a VA
 * surface, then this function acts as if vaSyncSurface() on the
 * parent surface was called first.
 *
 * The \ref VABufferInfo argument shall be zero'ed on input. On
 * successful output, the data structure is filled in with all the
 * necessary buffer level implementation details like handle, type,
 * memory type and memory size.
 *
 * Note: the external API implementation, or the application, can
 * express the memory types it is interested in by filling in the \ref
 * mem_type field accordingly. On successful output, the memory type
 * that fits best the request and that was used is updated in the \ref
 * VABufferInfo data structure. If none of the supplied memory types
 * is supported, then a \ref VA_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE
 * error is returned.
 *
 * The \ref VABufferInfo data is valid until vaReleaseBufferHandle()
 * is called. Besides, no additional operation is allowed on any of
 * the buffer parent object until vaReleaseBufferHandle() is called.
 * e.g. decoding into a VA surface backed with the supplied VA buffer
 * object \ref buf_id would fail with a \ref VA_STATUS_ERROR_SURFACE_BUSY
 * error.
 *
 * Possible errors:
 * - \ref VA_STATUS_ERROR_UNIMPLEMENTED: the VA driver implementation
 *   does not support this interface
 * - \ref VA_STATUS_ERROR_INVALID_DISPLAY: an invalid display was supplied
 * - \ref VA_STATUS_ERROR_INVALID_BUFFER: an invalid buffer was supplied
 * - \ref VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: the implementation
 *   does not support exporting buffers of the specified type
 * - \ref VA_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE: none of the requested
 *   memory types in \ref VABufferInfo.mem_type was supported
 *
 * @param[in] dpy               the VA display
 * @param[in] buf_id            the VA buffer
 * @param[in,out] buf_info      the associated VA buffer information
 * @return VA_STATUS_SUCCESS if successful
 */
VAStatus
vaAcquireBufferHandle(VADisplay dpy, VABufferID buf_id, VABufferInfo *buf_info);

/**
 * \brief Releases buffer after usage from external API
 *
 * Unlocks the VA buffer object \ref buf_id from external API usage like
 * EGL or OpenCL (OCL). This function is a synchronization point. This
 * means that any pending operation is guaranteed to be completed
 * prior to returning from the function.
 *
 * The \ref VABufferInfo argument shall point to the original data
 * structure that was obtained from vaAcquireBufferHandle(), unaltered.
 * This is necessary so that the VA driver implementation could
 * deallocate any resources that were needed.
 *
 * In any case, returning from this function invalidates any contents
 * in \ref VABufferInfo. i.e. the underlyng buffer handle is no longer
 * valid. Therefore, VA driver implementations are free to reset this
 * data structure to safe defaults.
 *
 * Possible errors:
 * - \ref VA_STATUS_ERROR_UNIMPLEMENTED: the VA driver implementation
 *   does not support this interface
 * - \ref VA_STATUS_ERROR_INVALID_DISPLAY: an invalid display was supplied
 * - \ref VA_STATUS_ERROR_INVALID_BUFFER: an invalid buffer was supplied
 * - \ref VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: the implementation
 *   does not support exporting buffers of the specified type
 *
 * @param[in] dpy               the VA display
 * @param[in] buf_id            the VA buffer
 * @return VA_STATUS_SUCCESS if successful
 */
VAStatus
vaReleaseBufferHandle(VADisplay dpy, VABufferID buf_id);

/*
Render (Decode) Pictures

A picture represents either a frame or a field.

The Begin/Render/End sequence sends the decode buffers to the server
*/

/**
 * Get ready to decode a picture to a target surface
 */
VAStatus vaBeginPicture (
    VADisplay dpy,
    VAContextID context,
    VASurfaceID render_target
);

/**
 * Send decode buffers to the server.
 * Buffers are automatically destroyed afterwards
 */
VAStatus vaRenderPicture (
    VADisplay dpy,
    VAContextID context,
    VABufferID *buffers,
    int num_buffers
);

/**
 * Make the end of rendering for a picture. 
 * The server should start processing all pending operations for this 
 * surface. This call is non-blocking. The client can start another 
 * Begin/Render/End sequence on a different render target.
 */
VAStatus vaEndPicture (
    VADisplay dpy,
    VAContextID context
);

/*

Synchronization 

*/

/**
 * This function blocks until all pending operations on the render target
 * have been completed.  Upon return it is safe to use the render target for a 
 * different picture. 
 */
VAStatus vaSyncSurface (
    VADisplay dpy,
    VASurfaceID render_target
);

typedef enum
{
    VASurfaceRendering	= 1, /* Rendering in progress */ 
    VASurfaceDisplaying	= 2, /* Displaying in progress (not safe to render into it) */ 
                             /* this status is useful if surface is used as the source */
                             /* of an overlay */
    VASurfaceReady	= 4, /* not being rendered or displayed */
    VASurfaceSkipped	= 8  /* Indicate a skipped frame during encode */
} VASurfaceStatus;

/**
 * Find out any pending ops on the render target 
 */
VAStatus vaQuerySurfaceStatus (
    VADisplay dpy,
    VASurfaceID render_target,
    VASurfaceStatus *status	/* out */
);

typedef enum
{
    VADecodeSliceMissing            = 0,
    VADecodeMBError                 = 1,
} VADecodeErrorType;

/**
 * Client calls vaQuerySurfaceError with VA_STATUS_ERROR_DECODING_ERROR, server side returns
 * an array of structure VASurfaceDecodeMBErrors, and the array is terminated by setting status=-1
*/
typedef struct _VASurfaceDecodeMBErrors
{
    int status; /* 1 if hardware has returned detailed info below, -1 means this record is invalid */
    unsigned int start_mb; /* start mb address with errors */
    unsigned int end_mb;  /* end mb address with errors */
    VADecodeErrorType decode_error_type;
} VASurfaceDecodeMBErrors;

/**
 * After the application gets VA_STATUS_ERROR_DECODING_ERROR after calling vaSyncSurface(),
 * it can call vaQuerySurfaceError to find out further details on the particular error.
 * VA_STATUS_ERROR_DECODING_ERROR should be passed in as "error_status",
 * upon the return, error_info will point to an array of _VASurfaceDecodeMBErrors structure,
 * which is allocated and filled by libVA with detailed information on the missing or error macroblocks.
 * The array is terminated if "status==-1" is detected.
 */
VAStatus vaQuerySurfaceError(
    VADisplay dpy,
    VASurfaceID surface,
    VAStatus error_status,
    void **error_info
);

/**
 * Images and Subpictures
 * VAImage is used to either get the surface data to client memory, or 
 * to copy image data in client memory to a surface. 
 * Both images, subpictures and surfaces follow the same 2D coordinate system where origin 
 * is at the upper left corner with positive X to the right and positive Y down
 */
#define VA_FOURCC(ch0, ch1, ch2, ch3) \
    ((unsigned long)(unsigned char) (ch0) | ((unsigned long)(unsigned char) (ch1) << 8) | \
    ((unsigned long)(unsigned char) (ch2) << 16) | ((unsigned long)(unsigned char) (ch3) << 24 ))

/* 
 * Pre-defined fourcc codes
 */
#define VA_FOURCC_NV12		0x3231564E
#define VA_FOURCC_AI44		0x34344149
#define VA_FOURCC_RGBA		0x41424752
#define VA_FOURCC_RGBX		0x58424752
#define VA_FOURCC_BGRA		0x41524742
#define VA_FOURCC_BGRX		0x58524742
#define VA_FOURCC_ARGB		0x42475241
#define VA_FOURCC_XRGB		0x42475258
#define VA_FOURCC_ABGR          0x52474241
#define VA_FOURCC_XBGR          0x52474258
#define VA_FOURCC_UYVY          0x59565955
#define VA_FOURCC_YUY2          0x32595559
#define VA_FOURCC_AYUV          0x56555941
#define VA_FOURCC_NV11          0x3131564e
#define VA_FOURCC_YV12          0x32315659
#define VA_FOURCC_P208          0x38303250
#define VA_FOURCC_IYUV          0x56555949
#define VA_FOURCC_YV24          0x34325659
#define VA_FOURCC_YV32          0x32335659
#define VA_FOURCC_Y800          0x30303859
#define VA_FOURCC_IMC3          0x33434D49
#define VA_FOURCC_411P          0x50313134
#define VA_FOURCC_422H          0x48323234
#define VA_FOURCC_422V          0x56323234
#define VA_FOURCC_444P          0x50343434
#define VA_FOURCC_RGBP          0x50424752
#define VA_FOURCC_BGRP          0x50524742
#define VA_FOURCC_411R          0x52313134 /* rotated 411P */

/* byte order */
#define VA_LSB_FIRST		1
#define VA_MSB_FIRST		2

typedef struct _VAImageFormat
{
    unsigned int	fourcc;
    unsigned int	byte_order; /* VA_LSB_FIRST, VA_MSB_FIRST */
    unsigned int	bits_per_pixel;
    /* for RGB formats */
    unsigned int	depth; /* significant bits per pixel */
    unsigned int	red_mask;
    unsigned int	green_mask;
    unsigned int	blue_mask;
    unsigned int	alpha_mask;
} VAImageFormat;

typedef VAGenericID VAImageID;

typedef struct _VAImage
{
    VAImageID		image_id; /* uniquely identify this image */
    VAImageFormat	format;
    VABufferID		buf;	/* image data buffer */
    /*
     * Image data will be stored in a buffer of type VAImageBufferType to facilitate
     * data store on the server side for optimal performance. The buffer will be 
     * created by the CreateImage function, and proper storage allocated based on the image
     * size and format. This buffer is managed by the library implementation, and 
     * accessed by the client through the buffer Map/Unmap functions.
     */
    unsigned short	width; 
    unsigned short	height;
    unsigned int	data_size;
    unsigned int	num_planes;	/* can not be greater than 3 */
    /* 
     * An array indicating the scanline pitch in bytes for each plane.
     * Each plane may have a different pitch. Maximum 3 planes for planar formats
     */
    unsigned int	pitches[3];
    /* 
     * An array indicating the byte offset from the beginning of the image data 
     * to the start of each plane.
     */
    unsigned int	offsets[3];

    /* The following fields are only needed for paletted formats */
    int num_palette_entries;   /* set to zero for non-palette images */
    /* 
     * Each component is one byte and entry_bytes indicates the number of components in 
     * each entry (eg. 3 for YUV palette entries). set to zero for non-palette images   
     */
    int entry_bytes; 
    /*
     * An array of ascii characters describing the order of the components within the bytes.
     * Only entry_bytes characters of the string are used.
     */
    char component_order[4];
} VAImage;

/** Get maximum number of image formats supported by the implementation */
int vaMaxNumImageFormats (
    VADisplay dpy
);

/**
 * Query supported image formats 
 * The caller must provide a "format_list" array that can hold at
 * least vaMaxNumImageFormats() entries. The actual number of formats
 * returned in "format_list" is returned in "num_formats".
 */
VAStatus vaQueryImageFormats (
    VADisplay dpy,
    VAImageFormat *format_list,	/* out */
    int *num_formats		/* out */
);

/**
 * Create a VAImage structure
 * The width and height fields returned in the VAImage structure may get 
 * enlarged for some YUV formats. Upon return from this function, 
 * image->buf has been created and proper storage allocated by the library. 
 * The client can access the image through the Map/Unmap calls.
 */
VAStatus vaCreateImage (
    VADisplay dpy,
    VAImageFormat *format,
    int width,
    int height,
    VAImage *image	/* out */
);

/**
 * Should call DestroyImage before destroying the surface it is bound to
 */
VAStatus vaDestroyImage (
    VADisplay dpy,
    VAImageID image
);

VAStatus vaSetImagePalette (
    VADisplay dpy,
    VAImageID image,
    /* 
     * pointer to an array holding the palette data.  The size of the array is 
     * num_palette_entries * entry_bytes in size.  The order of the components 
     * in the palette is described by the component_order in VAImage struct    
     */
    unsigned char *palette 
);

/**
 * Retrive surface data into a VAImage
 * Image must be in a format supported by the implementation
 */
VAStatus vaGetImage (
    VADisplay dpy,
    VASurfaceID surface,
    int x,	/* coordinates of the upper left source pixel */
    int y,
    unsigned int width, /* width and height of the region */
    unsigned int height,
    VAImageID image
);

/**
 * Copy data from a VAImage to a surface
 * Image must be in a format supported by the implementation
 * Returns a VA_STATUS_ERROR_SURFACE_BUSY if the surface
 * shouldn't be rendered into when this is called
 */
VAStatus vaPutImage (
    VADisplay dpy,
    VASurfaceID surface,
    VAImageID image,
    int src_x,
    int src_y,
    unsigned int src_width,
    unsigned int src_height,
    int dest_x,
    int dest_y,
    unsigned int dest_width,
    unsigned int dest_height
);

/**
 * Derive an VAImage from an existing surface.
 * This interface will derive a VAImage and corresponding image buffer from
 * an existing VA Surface. The image buffer can then be mapped/unmapped for
 * direct CPU access. This operation is only possible on implementations with
 * direct rendering capabilities and internal surface formats that can be
 * represented with a VAImage. When the operation is not possible this interface
 * will return VA_STATUS_ERROR_OPERATION_FAILED. Clients should then fall back
 * to using vaCreateImage + vaPutImage to accomplish the same task in an
 * indirect manner.
 *
 * Implementations should only return success when the resulting image buffer
 * would be useable with vaMap/Unmap.
 *
 * When directly accessing a surface special care must be taken to insure
 * proper synchronization with the graphics hardware. Clients should call
 * vaQuerySurfaceStatus to insure that a surface is not the target of concurrent
 * rendering or currently being displayed by an overlay.
 *
 * Additionally nothing about the contents of a surface should be assumed
 * following a vaPutSurface. Implementations are free to modify the surface for
 * scaling or subpicture blending within a call to vaPutImage.
 *
 * Calls to vaPutImage or vaGetImage using the same surface from which the image
 * has been derived will return VA_STATUS_ERROR_SURFACE_BUSY. vaPutImage or
 * vaGetImage with other surfaces is supported.
 *
 * An image created with vaDeriveImage should be freed with vaDestroyImage. The
 * image and image buffer structures will be destroyed; however, the underlying
 * surface will remain unchanged until freed with vaDestroySurfaces.
 */
VAStatus vaDeriveImage (
    VADisplay dpy,
    VASurfaceID surface,
    VAImage *image	/* out */
);

/**
 * Subpictures 
 * Subpicture is a special type of image that can be blended 
 * with a surface during vaPutSurface(). Subpicture can be used to render
 * DVD sub-titles or closed captioning text etc.  
 */

typedef VAGenericID VASubpictureID;

/** Get maximum number of subpicture formats supported by the implementation */
int vaMaxNumSubpictureFormats (
    VADisplay dpy
);

/** flags for subpictures */
#define VA_SUBPICTURE_CHROMA_KEYING			0x0001
#define VA_SUBPICTURE_GLOBAL_ALPHA			0x0002
#define VA_SUBPICTURE_DESTINATION_IS_SCREEN_COORD	0x0004
/**
 * Query supported subpicture formats 
 * The caller must provide a "format_list" array that can hold at
 * least vaMaxNumSubpictureFormats() entries. The flags arrary holds the flag 
 * for each format to indicate additional capabilities for that format. The actual 
 * number of formats returned in "format_list" is returned in "num_formats".
 *  flags: returned value to indicate addtional capabilities
 *         VA_SUBPICTURE_CHROMA_KEYING - supports chroma-keying
 *         VA_SUBPICTURE_GLOBAL_ALPHA - supports global alpha
 * 	   VA_SUBPICTURE_DESTINATION_IS_SCREEN_COORD - supports unscaled screen relative subpictures for On Screen Display
 */

VAStatus vaQuerySubpictureFormats (
    VADisplay dpy,
    VAImageFormat *format_list,	/* out */
    unsigned int *flags,	/* out */
    unsigned int *num_formats	/* out */
);

/**
 * Subpictures are created with an image associated. 
 */
VAStatus vaCreateSubpicture (
    VADisplay dpy,
    VAImageID image,
    VASubpictureID *subpicture	/* out */
);

/**
 * Destroy the subpicture before destroying the image it is assocated to
 */
VAStatus vaDestroySubpicture (
    VADisplay dpy,
    VASubpictureID subpicture
);

/**
 * Bind an image to the subpicture. This image will now be associated with 
 * the subpicture instead of the one at creation.
 */
VAStatus vaSetSubpictureImage (
    VADisplay dpy,
    VASubpictureID subpicture,
    VAImageID image
);

/**
 * If chromakey is enabled, then the area where the source value falls within
 * the chromakey [min, max] range is transparent
 * The chromakey component format is the following:
 *  For RGB: [0:7] Red [8:15] Blue [16:23] Green   
 *  For YUV: [0:7] V [8:15] U [16:23] Y
 * The chromakey mask can be used to mask out certain components for chromakey
 * comparision
 */
VAStatus vaSetSubpictureChromakey (
    VADisplay dpy,
    VASubpictureID subpicture,
    unsigned int chromakey_min,
    unsigned int chromakey_max,
    unsigned int chromakey_mask
);

/**
 * Global alpha value is between 0 and 1. A value of 1 means fully opaque and 
 * a value of 0 means fully transparent. If per-pixel alpha is also specified then
 * the overall alpha is per-pixel alpha multiplied by the global alpha
 */
VAStatus vaSetSubpictureGlobalAlpha (
    VADisplay dpy,
    VASubpictureID subpicture,
    float global_alpha 
);

/**
 * vaAssociateSubpicture associates the subpicture with target_surfaces.
 * It defines the region mapping between the subpicture and the target  
 * surfaces through source and destination rectangles (with the same width and height).
 * Both will be displayed at the next call to vaPutSurface.  Additional
 * associations before the call to vaPutSurface simply overrides the association.
 */
VAStatus vaAssociateSubpicture (
    VADisplay dpy,
    VASubpictureID subpicture,
    VASurfaceID *target_surfaces,
    int num_surfaces,
    short src_x, /* upper left offset in subpicture */
    short src_y,
    unsigned short src_width,
    unsigned short src_height,
    short dest_x, /* upper left offset in surface */
    short dest_y,
    unsigned short dest_width,
    unsigned short dest_height,
    /*
     * whether to enable chroma-keying, global-alpha, or screen relative mode
     * see VA_SUBPICTURE_XXX values
     */
    unsigned int flags
);

/**
 * vaDeassociateSubpicture removes the association of the subpicture with target_surfaces.
 */
VAStatus vaDeassociateSubpicture (
    VADisplay dpy,
    VASubpictureID subpicture,
    VASurfaceID *target_surfaces,
    int num_surfaces
);

typedef struct _VARectangle
{
    short x;
    short y;
    unsigned short width;
    unsigned short height;
} VARectangle;

/**
 * Display attributes
 * Display attributes are used to control things such as contrast, hue, saturation,
 * brightness etc. in the rendering process.  The application can query what
 * attributes are supported by the driver, and then set the appropriate attributes
 * before calling vaPutSurface()
 */
/* PowerVR IEP Lite attributes */
typedef enum
{
    VADISPLAYATTRIB_BLE_OFF              = 0x00,
    VADISPLAYATTRIB_BLE_LOW,
    VADISPLAYATTRIB_BLE_MEDIUM,
    VADISPLAYATTRIB_BLE_HIGH,
    VADISPLAYATTRIB_BLE_NONE,
} VADisplayAttribBLEMode;

/** attribute value for VADisplayAttribRotation   */
#define VA_ROTATION_NONE        0x00000000
#define VA_ROTATION_90          0x00000001
#define VA_ROTATION_180         0x00000002
#define VA_ROTATION_270         0x00000003

/** attribute value for VADisplayAttribOutOfLoopDeblock */
#define VA_OOL_DEBLOCKING_FALSE 0x00000000
#define VA_OOL_DEBLOCKING_TRUE  0x00000001

/** Render mode */
#define VA_RENDER_MODE_UNDEFINED           0
#define VA_RENDER_MODE_LOCAL_OVERLAY       1
#define VA_RENDER_MODE_LOCAL_GPU           2
#define VA_RENDER_MODE_EXTERNAL_OVERLAY    4
#define VA_RENDER_MODE_EXTERNAL_GPU        8

/** Render device */
#define VA_RENDER_DEVICE_UNDEFINED  0
#define VA_RENDER_DEVICE_LOCAL      1
#define VA_RENDER_DEVICE_EXTERNAL   2

/** Currently defined display attribute types */
typedef enum
{
    VADisplayAttribBrightness		= 0,
    VADisplayAttribContrast		= 1,
    VADisplayAttribHue			= 2,
    VADisplayAttribSaturation		= 3,
    /* client can specifiy a background color for the target window
     * the new feature of video conference,
     * the uncovered area of the surface is filled by this color
     * also it will blend with the decoded video color
     */
    VADisplayAttribBackgroundColor      = 4,
    /*
     * this is a gettable only attribute. For some implementations that use the
     * hardware overlay, after PutSurface is called, the surface can not be    
     * re-used until after the subsequent PutSurface call. If this is the case 
     * then the value for this attribute will be set to 1 so that the client   
     * will not attempt to re-use the surface right after returning from a call
     * to PutSurface.
     *
     * Don't use it, use flag VASurfaceDisplaying of vaQuerySurfaceStatus since
     * driver may use overlay or GPU alternatively
     */
    VADisplayAttribDirectSurface       = 5,
    VADisplayAttribRotation            = 6,	
    VADisplayAttribOutofLoopDeblock    = 7,

    /* PowerVR IEP Lite specific attributes */
    VADisplayAttribBLEBlackMode        = 8,
    VADisplayAttribBLEWhiteMode        = 9,
    VADisplayAttribBlueStretch         = 10,
    VADisplayAttribSkinColorCorrection = 11,
    /*
     * For type VADisplayAttribCSCMatrix, "value" field is a pointer to the color
     * conversion matrix. Each element in the matrix is float-point
     */
    VADisplayAttribCSCMatrix           = 12,
    /* specify the constant color used to blend with video surface
     * Cd = Cv*Cc*Ac + Cb *(1 - Ac) C means the constant RGB
     *      d: the final color to overwrite into the frame buffer 
     *      v: decoded video after color conversion, 
     *      c: video color specified by VADisplayAttribBlendColor
     *      b: background color of the drawable
     */
    VADisplayAttribBlendColor          = 13,
    /*
     * Indicate driver to skip painting color key or not.
     * only applicable if the render is overlay
     */
    VADisplayAttribOverlayAutoPaintColorKey   = 14,
    /*
     * customized overlay color key, the format is RGB888
     * [23:16] = Red, [15:08] = Green, [07:00] = Blue.
     */
    VADisplayAttribOverlayColorKey	= 15,
    /*
     * The hint for the implementation of vaPutSurface
     * normally, the driver could use an overlay or GPU to render the surface on the screen
     * this flag provides APP the flexibity to switch the render dynamically
     */
    VADisplayAttribRenderMode           = 16,
    /*
     * specify if vaPutSurface needs to render into specified monitors
     * one example is that one external monitor (e.g. HDMI) is enabled, 
     * but the window manager is not aware of it, and there is no associated drawable
     */
    VADisplayAttribRenderDevice        = 17,
    /*
     * specify vaPutSurface render area if there is no drawable on the monitor
     */
    VADisplayAttribRenderRect          = 18,
} VADisplayAttribType;

/* flags for VADisplayAttribute */
#define VA_DISPLAY_ATTRIB_NOT_SUPPORTED	0x0000
#define VA_DISPLAY_ATTRIB_GETTABLE	0x0001
#define VA_DISPLAY_ATTRIB_SETTABLE	0x0002

typedef struct _VADisplayAttribute
{
    VADisplayAttribType type;
    int min_value;
    int max_value;
    int value;	/* used by the set/get attribute functions */
/* flags can be VA_DISPLAY_ATTRIB_GETTABLE or VA_DISPLAY_ATTRIB_SETTABLE or OR'd together */
    unsigned int flags;
} VADisplayAttribute;

/** Get maximum number of display attributs supported by the implementation */
int vaMaxNumDisplayAttributes (
    VADisplay dpy
);

/**
 * Query display attributes 
 * The caller must provide a "attr_list" array that can hold at
 * least vaMaxNumDisplayAttributes() entries. The actual number of attributes
 * returned in "attr_list" is returned in "num_attributes".
 */
VAStatus vaQueryDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,	/* out */
    int *num_attributes			/* out */
);

/**
 * Get display attributes 
 * This function returns the current attribute values in "attr_list".
 * Only attributes returned with VA_DISPLAY_ATTRIB_GETTABLE set in the "flags" field
 * from vaQueryDisplayAttributes() can have their values retrieved.  
 */
VAStatus vaGetDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,	/* in/out */
    int num_attributes
);

/**
 * Set display attributes 
 * Only attributes returned with VA_DISPLAY_ATTRIB_SETTABLE set in the "flags" field
 * from vaQueryDisplayAttributes() can be set.  If the attribute is not settable or 
 * the value is out of range, the function returns VA_STATUS_ERROR_ATTR_NOT_SUPPORTED
 */
VAStatus vaSetDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,
    int num_attributes
);

/****************************
 * HEVC data structures
 ****************************/
/** 
 * \brief Description of picture properties of those in DPB surfaces.
 *
 * If only progressive scan is supported, each surface contains one whole 
 * frame picture.
 * Otherwise, each surface contains two fields of whole picture.
 * In this case, two entries of ReferenceFrames[] may share same picture_id
 * value.
 */
typedef struct _VAPictureHEVC
{
    /** \brief reconstructed picture buffer surface index 
     * invalid when taking value VA_INVALID_SURFACE.
     */
    VASurfaceID             picture_id;
    /** \brief picture order count. 
     * in HEVC, POCs for top and bottom fields of same picture should
     * take different values.
     */
    int32_t                 pic_order_cnt;
    /* described below */
    uint32_t                flags;
} VAPictureHEVC;

/* flags in VAPictureHEVC could be OR of the following */
#define VA_PICTURE_HEVC_INVALID                 0x00000001
/** \brief indication of interlace scan picture. 
 * should take same value for all the pictures in sequence.
 */ 
#define VA_PICTURE_HEVC_FIELD_PIC               0x00000002
/** \brief polarity of the field picture.
 * top field takes even lines of buffer surface.
 * bottom field takes odd lines of buffer surface.
 */
#define VA_PICTURE_HEVC_BOTTOM_FIELD            0x00000004
/** \brief Long term reference picture */
#define VA_PICTURE_HEVC_LONG_TERM_REFERENCE     0x00000008
/**
 * VA_PICTURE_HEVC_RPS_ST_CURR_BEFORE, VA_PICTURE_HEVC_RPS_ST_CURR_AFTER
 * and VA_PICTURE_HEVC_RPS_LT_CURR of any picture in ReferenceFrames[] should 
 * be exclusive. No more than one of them can be set for any picture.
 * Sum of NumPocStCurrBefore, NumPocStCurrAfter and NumPocLtCurr
 * equals NumPocTotalCurr, which should be equal to or smaller than 8.
 * Application should provide valid values for both short format and long format.
 * The pictures in DPB with any of these three flags turned on are referred by
 * the current picture.
 */
/** \brief RefPicSetStCurrBefore of HEVC spec variable 
 * Number of ReferenceFrames[] entries with this bit set equals 
 * NumPocStCurrBefore.
 */
#define VA_PICTURE_HEVC_RPS_ST_CURR_BEFORE      0x00000010
/** \brief RefPicSetStCurrAfter of HEVC spec variable
 * Number of ReferenceFrames[] entries with this bit set equals 
 * NumPocStCurrAfter.
 */
#define VA_PICTURE_HEVC_RPS_ST_CURR_AFTER       0x00000020
/** \brief RefPicSetLtCurr of HEVC spec variable
 * Number of ReferenceFrames[] entries with this bit set equals 
 * NumPocLtCurr.
 */
#define VA_PICTURE_HEVC_RPS_LT_CURR             0x00000040

#include <va/va_dec_hevc.h>
#include <va/va_dec_jpeg.h>
#include <va/va_dec_vp8.h>
#include <va/va_dec_vp9.h>
#include <va/va_enc_hevc.h>
#include <va/va_enc_h264.h>
#include <va/va_enc_jpeg.h>
#include <va/va_enc_mpeg2.h>
#include <va/va_enc_vp8.h>
#include <va/va_vpp.h>

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VA_H_ */
