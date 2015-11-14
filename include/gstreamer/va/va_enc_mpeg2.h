/*
 * Copyright (c) 2012 Intel Corporation. All Rights Reserved.
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

/**
 * \file va_enc_mpeg2.h
 * \brief The MPEG-2 encoding API
 *
 * This file contains the \ref api_enc_mpeg2 "MPEG-2 encoding API".
 */

#ifndef _VA_ENC_MPEG2_H_
#define _VA_ENC_MPEG2_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup api_enc_mpeg2 MPEG-2 encoding API
 *
 * @{
 */

/**
 * \brief MPEG-2 Quantization Matrix Buffer
 *
 */
typedef VAIQMatrixBufferMPEG2 VAQMatrixBufferMPEG2;

/**
 * \brief Packed header types specific to MPEG-2 encoding.
 *
 * Types of packed headers generally used for MPEG-2 encoding.
 */
typedef enum {
    /**
     * \brief Packed Sequence Parameter Set (SPS).
     *
     */
    VAEncPackedHeaderMPEG2_SPS = VAEncPackedHeaderSequence,
    /**
     * \brief Packed Picture Parameter Set (PPS).
     *
     */
    VAEncPackedHeaderMPEG2_PPS = VAEncPackedHeaderPicture,
    /**
     * \brief Packed slice header.
     *
     */
    VAEncPackedHeaderMPEG2_Slice = VAEncPackedHeaderSlice,
} VAEncPackedHeaderTypeMPEG2;

/**
 * \brief Sequence parameter for MPEG-2 encoding
 *
 * This structure holds information for \c sequence_header() and
 * sequence_extension().
 *
 * If packed sequence headers mode is used, i.e. if the encoding
 * pipeline was configured with the #VA_ENC_PACKED_HEADER_SEQUENCE
 * flag, then the driver expects two more buffers to be provided to
 * the same \c vaRenderPicture() as this buffer:
 * - a #VAEncPackedHeaderParameterBuffer with type set to
 *   VAEncPackedHeaderType::VAEncPackedHeaderSequence ;
 * - a #VAEncPackedHeaderDataBuffer which holds the actual packed
 *   header data.
 *
 */
typedef struct _VAEncSequenceParameterBufferMPEG2 {
    /** \brief Period between I frames. */
    unsigned int intra_period;
    /** \brief Period between I/P frames. */
    unsigned int ip_period;
    /** \brief Picture width.
     *
     * A 14bits unsigned inter, the lower 12bits 
     * is horizontal_size_value, and the upper 
     * 2bits is \c horizontal_size_extension
     *
     */
    unsigned short picture_width;
    /** \brief Picture height.
     *
     * A 14bits unsigned inter, the lower 12bits
     * is vertical_size_value, and the upper 2bits is 
     * vertical_size_size_extension
     *
     */
    unsigned short picture_height;
    /**
     * \brief Initial bitrate set for this sequence in CBR or VBR modes.
     *
     * This field represents the initial bitrate value for this
     * sequence if CBR or VBR mode is used, i.e. if the encoder
     * pipeline was created with a #VAConfigAttribRateControl
     * attribute set to either \ref VA_RC_CBR or \ref VA_RC_VBR.
     *
     * bits_per_second may be derived from bit_rate.
     *
     */
    unsigned int bits_per_second;
    /**
     * \brief Frame rate
     * 
     * Derived from frame_rate_value, frame_rate_extension_n and 
     * frame_rate_extension_d
     *
     */
    float frame_rate;
    /** \brief Same as the element in sequence_header() */
    unsigned short aspect_ratio_information;
    /** \brief Define the size of VBV */
    unsigned int vbv_buffer_size;

    union {
        struct {
            /** \brief Same as the element in Sequence extension() */
            unsigned int profile_and_level_indication   : 8;
            /** \brief Same as the element in Sequence extension() */
            unsigned int progressive_sequence           : 1;
            /** \brief Same as the element in Sequence extension() */
            unsigned int chroma_format                  : 2;
            /** \brief Same as the element in Sequence extension() */
            unsigned int low_delay                      : 1;
            /** \brief Same as the element in Sequence extension() */
            unsigned int frame_rate_extension_n         : 2;
            /** \brief Same as the element in Sequence extension() */
            unsigned int frame_rate_extension_d         : 5;
        } bits;
        unsigned int value;
    } sequence_extension;

    /** \brief Flag to indicate the following GOP header are being updated */
    unsigned int new_gop_header;

    union {
        struct {
            /** \brief Time code */
            unsigned int time_code                      : 25;
            /** \brief Same as the element in GOP header */
            unsigned int closed_gop                     : 1;
            /** \brief SAme as the element in GOP header */
            unsigned int broken_link                    : 1;
        } bits;
        unsigned int value;
    } gop_header;
} VAEncSequenceParameterBufferMPEG2;

/**
 * \brief Picture parameter for MPEG-2 encoding
 *
 * This structure holds information for picture_header() and 
 * picture_coding_extension()
 *
 * If packed picture headers mode is used, i.e. if the encoding
 * pipeline was configured with the #VA_ENC_PACKED_HEADER_PICTURE
 * flag, then the driver expects two more buffers to be provided to
 * the same \c vaRenderPicture() as this buffer:
 * - a #VAEncPackedHeaderParameterBuffer with type set to
 *   VAEncPackedHeaderType::VAEncPackedHeaderPicture ;
 * - a #VAEncPackedHeaderDataBuffer which holds the actual packed
 *   header data.
 *
 */
typedef struct _VAEncPictureParameterBufferMPEG2 {
    /** \brief Forward reference picture */
    VASurfaceID forward_reference_picture;
    /** \brief Backward reference picture */
    VASurfaceID backward_reference_picture;
    /** \brief Reconstructed(decoded) picture */
    VASurfaceID reconstructed_picture;
    /**
     * \brief Output encoded bitstream.
     *
     * \ref coded_buf has type #VAEncCodedBufferType. It should be
     * large enough to hold the compressed NAL slice and possibly SPS
     * and PPS NAL units.
     */
    VABufferID coded_buf;
    /**
     * \brief Flag to indicate the picture is the last one or not.
     *
     * This fields holds 0 if the picture to be encoded is not 
     * the last one in the stream. Otherwise, it 
     * is \ref MPEG2_LAST_PICTURE_EOSTREAM.
     */
    unsigned char last_picture;
    /** \brief Picture type */
    VAEncPictureType picture_type;
    /** \brief Same as the element in picture_header() */
    unsigned int temporal_reference;
    /** \brief Same as the element in picture_header() */
    unsigned int vbv_delay;
    /** \brief Same as the element in Picture coding extension */
    unsigned char f_code[2][2];
    union {
        struct {
            /** \brief Same as the element in Picture coding extension */
            unsigned int intra_dc_precision             : 2; 
            /** \brief Same as the element in Picture coding extension */
            unsigned int picture_structure              : 2; 
            /** \brief Same as the element in Picture coding extension */
            unsigned int top_field_first                : 1; 
            /** \brief Same as the element in Picture coding extension */
            unsigned int frame_pred_frame_dct           : 1; 
            /** \brief Same as the element in Picture coding extension */
            unsigned int concealment_motion_vectors     : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int q_scale_type                   : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int intra_vlc_format               : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int alternate_scan                 : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int repeat_first_field             : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int progressive_frame              : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int composite_display_flag         : 1;
        } bits;
        unsigned int value;
    } picture_coding_extension;

    /* \brief Parameters for composite display
     *
     * Valid only when omposite_display_flag is 1
     */
    union {
        struct {
            /** \brief Same as the element in Picture coding extension */            
            unsigned int v_axis                         : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int field_sequence                 : 3;
            /** \brief Same as the element in Picture coding extension */
            unsigned int sub_carrier                    : 1;
            /** \brief Same as the element in Picture coding extension */
            unsigned int burst_amplitude                : 7;
            /** \brief Same as the element in Picture coding extension */
            unsigned int sub_carrier_phase              : 8;
        } bits;
        unsigned int value;
    } composite_display;
} VAEncPictureParameterBufferMPEG2;

/**
 * \brief Slice parameter for MPEG-2 encoding
 *
 */
typedef struct _VAEncSliceParameterBufferMPEG2 {
    /** \brief Starting MB address for this slice. */
    unsigned int macroblock_address;
    /** \brief Number of macroblocks in this slice. */
    unsigned int num_macroblocks;
    /** \brief Same as the element in slice() */
    int quantiser_scale_code;
    /** \brief Flag to indicate intra slice */
    int is_intra_slice;
} VAEncSliceParameterBufferMPEG2;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VA_ENC_MPEG2_H_ */
