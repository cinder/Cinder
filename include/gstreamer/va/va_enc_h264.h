/*
 * Copyright (c) 2007-2011 Intel Corporation. All Rights Reserved.
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
 * \file va_enc_h264.h
 * \brief The H.264 encoding API
 *
 * This file contains the \ref api_enc_h264 "H.264 encoding API".
 */

#ifndef VA_ENC_H264_H
#define VA_ENC_H264_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup api_enc_h264 H.264 encoding API
 *
 * @{
 */

/**
 * @name Picture flags
 *
 * Those flags flags are meant to signal when a picture marks the end
 * of a sequence, a stream, or even both at once.
 *
 * @{
 */
/**
 * \brief Marks the last picture in the sequence.
 *
 * i.e. the driver appends \c end_of_seq() NAL unit to the encoded frame.
 */
#define H264_LAST_PICTURE_EOSEQ     0x01
/**
 * \brief Marks the last picture in the stream.
 *
 * i.e. the driver appends \c end_of_stream() NAL unit to the encoded frame.
 */
#define H264_LAST_PICTURE_EOSTREAM  0x02
/**@}*/

/**
 * \brief Packed header types specific to H.264 encoding.
 *
 * Types of packed headers generally used for H.264 encoding. Each
 * associated packed header data buffer shall contain the start code
 * prefix 0x000001 followed by the complete NAL unit, thus also
 * including the \c nal_unit_type.
 *
 * Note: the start code prefix can contain an arbitrary number of leading
 * zeros. The driver will skip them for emulation prevention bytes insertion,
 * if necessary.
 */
typedef enum {
    /**
     * \brief Packed Sequence Parameter Set (SPS).
     *
     * The corresponding packed header data buffer shall contain the
     * complete seq_parameter_set_rbsp() syntax element.
     *
     * Note: packed \c nal_unit_type shall be equal to 7.
     */
    VAEncPackedHeaderH264_SPS   = VAEncPackedHeaderSequence,
    /**
     * \brief Packed Picture Parameter Set (PPS).
     *
     * The corresponding packed header data buffer shall contain the
     * complete pic_parameter_set_rbsp() syntax element.
     *
     * Note: packed \c nal_unit_type shall be equal to 8.
     */
    VAEncPackedHeaderH264_PPS   = VAEncPackedHeaderPicture,
    /**
     * \brief Packed slice header.
     *
     * The corresponding packed header data buffer shall contain the
     * \c slice_header() syntax element only, along with any start
     * code prefix and NAL unit type preceeding it. i.e. this means
     * that the buffer does not contain any of the \c slice_data() or
     * the \c rbsp_slice_trailing_bits().
     *
     * Note: packed \c nal_unit_type shall be equal to 1 (non-IDR
     * picture), or 5 (IDR picture).
     */
    VAEncPackedHeaderH264_Slice = VAEncPackedHeaderSlice,
    /**
     * \brief Packed Supplemental Enhancement Information (SEI).
     *
     * The corresponding packed header data buffer shall contain the
     * complete sei_rbsp() syntax element, thus including several
     * sei_message() elements if necessary.
     *
     * Note: packed \c nal_unit_type shall be equal to 6.
     */
    VAEncPackedHeaderH264_SEI   = (VAEncPackedHeaderMiscMask | 1),
} VAEncPackedHeaderTypeH264;

/**
 * \brief Sequence parameter for H.264 encoding in baseline, main & high 
 * profiles.
 *
 * This structure holds information for \c seq_parameter_set_data() as
 * defined by the H.264 specification.
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
 * If \c seq_scaling_matrix_present_flag is set to \c 1, then a
 * #VAIQMatrixBufferH264 buffer shall also be provided within the same
 * \c vaRenderPicture() call as this sequence parameter buffer.
 */
typedef struct _VAEncSequenceParameterBufferH264 {
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   seq_parameter_set_id;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   level_idc;
    /** \brief Period between I frames. */
    unsigned int    intra_period;
    /** \brief Period between IDR frames. */
    unsigned int    intra_idr_period;
    /** \brief Period between I/P frames. */
    unsigned int    ip_period;
    /**
     * \brief Initial bitrate set for this sequence in CBR or VBR modes.
     *
     * This field represents the initial bitrate value for this
     * sequence if CBR or VBR mode is used, i.e. if the encoder
     * pipeline was created with a #VAConfigAttribRateControl
     * attribute set to either \ref VA_RC_CBR or \ref VA_RC_VBR.
     *
     * The bitrate can be modified later on through
     * #VAEncMiscParameterRateControl buffers.
     */
    unsigned int    bits_per_second;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    max_num_ref_frames;
    /** \brief Picture width in macroblocks. */
    unsigned short  picture_width_in_mbs;
    /** \brief Picture height in macroblocks. */
    unsigned short  picture_height_in_mbs;

    union {
        struct {
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int chroma_format_idc                      : 2;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int frame_mbs_only_flag                    : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int mb_adaptive_frame_field_flag           : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int seq_scaling_matrix_present_flag        : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int direct_8x8_inference_flag              : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int log2_max_frame_num_minus4              : 4;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int pic_order_cnt_type                     : 2;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int log2_max_pic_order_cnt_lsb_minus4      : 4;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int delta_pic_order_always_zero_flag       : 1;
        } bits;
        unsigned int value;
    } seq_fields;

    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   bit_depth_luma_minus8;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   bit_depth_chroma_minus8;

    /** if pic_order_cnt_type == 1 */
    /**@{*/
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   num_ref_frames_in_pic_order_cnt_cycle;
    /** \brief Same as the H.264 bitstream syntax element. */
    int             offset_for_non_ref_pic;
    /** \brief Same as the H.264 bitstream syntax element. */
    int             offset_for_top_to_bottom_field;
    /** \brief Same as the H.264 bitstream syntax element. */
    int             offset_for_ref_frame[256];
    /**@}*/

    /** @name Cropping (optional) */
    /**@{*/
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   frame_cropping_flag;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    frame_crop_left_offset;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    frame_crop_right_offset;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    frame_crop_top_offset;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    frame_crop_bottom_offset;
    /**@}*/

    /** @name VUI parameters (optional) */
    /**@{*/
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   vui_parameters_present_flag;
    union {
        struct {
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int aspect_ratio_info_present_flag         : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int timing_info_present_flag               : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int bitstream_restriction_flag             : 1;
            /** \brief Range: 0 to 16, inclusive. */
            unsigned int log2_max_mv_length_horizontal          : 5;
            /** \brief Range: 0 to 16, inclusive. */
            unsigned int log2_max_mv_length_vertical            : 5;
        } bits;
        unsigned int value;
    } vui_fields;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   aspect_ratio_idc;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    sar_width;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    sar_height;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    num_units_in_tick;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned int    time_scale;
    /**@}*/
} VAEncSequenceParameterBufferH264;

/**
 * \brief Picture parameter for H.264 encoding in baseline, main & high 
 * profiles.
 *
 * This structure holds information for \c pic_parameter_set_rbsp() as
 * defined by the H.264 specification.
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
 * If \c pic_scaling_matrix_present_flag is set to \c 1, then a
 * #VAIQMatrixBufferH264 buffer shall also be provided within the same
 * \c vaRenderPicture() call as this picture parameter buffer.
 */
typedef struct _VAEncPictureParameterBufferH264 {
    /**
     * \brief Information about the picture to be encoded.
     *
     * See #VAPictureH264 for further description of each field.
     * Note that CurrPic.picture_id represents the reconstructed
     * (decoded) picture. User provides a scratch VA surface ID here.
     */
    VAPictureH264   CurrPic;
    /**
     * \brief Decoded Picture Buffer (DPB).
     *
     * This array represents the list of reconstructed (decoded)
     * frames used as reference. It is important to keep track of
     * reconstructed frames so that they can be used later on as
     * reference for P or B-frames encoding.
     */
    VAPictureH264   ReferenceFrames[16];
    /**
     * \brief Output encoded bitstream.
     *
     * \ref coded_buf has type #VAEncCodedBufferType. It should be
     * large enough to hold the compressed NAL slice and possibly SPS
     * and PPS NAL units.
     */
    VABufferID      coded_buf;

    /** \brief The picture parameter set referred to in the slice header. */
    unsigned char   pic_parameter_set_id;
    /** \brief The active sequence parameter set. Range: 0 to 31, inclusive. */
    unsigned char   seq_parameter_set_id;

    /**
     * \brief OR'd flags describing whether the picture is the last one or not.
     *
     * This fields holds 0 if the picture to be encoded is not the last
     * one in the stream or sequence. Otherwise, it is a combination of
     * \ref H264_LAST_PICTURE_EOSEQ or \ref H264_LAST_PICTURE_EOSTREAM.
     */
    unsigned char   last_picture;

    /** \brief The picture identifier.
     *   Range: 0 to \f$2^{log2\_max\_frame\_num\_minus4 + 4} - 1\f$, inclusive.
     */
    unsigned short  frame_num;

    /** \brief \c pic_init_qp_minus26 + 26. */
    unsigned char   pic_init_qp;
    /** \brief Maximum reference index for reference picture list 0.
     *   Range: 0 to 31, inclusive.
     */
    unsigned char   num_ref_idx_l0_active_minus1;
    /** \brief Maximum reference index for reference picture list 1.
     *  Range: 0 to 31, inclusive.
     */
    unsigned char   num_ref_idx_l1_active_minus1;

    /** \brief Range: -12 to 12, inclusive. */
    signed char     chroma_qp_index_offset;
    /** \brief Range: -12 to 12, inclusive. */
    signed char     second_chroma_qp_index_offset;

    union {
        struct {
            /** \brief Is picture an IDR picture? */
            unsigned int idr_pic_flag                           : 1;
            /** \brief Is picture a reference picture? */
            unsigned int reference_pic_flag                     : 2;
            /** \brief Selects CAVLC (0) or CABAC (1) entropy coding mode. */
            unsigned int entropy_coding_mode_flag               : 1;
            /** \brief Is weighted prediction applied to P slices? */
            unsigned int weighted_pred_flag                     : 1;
            /** \brief Range: 0 to 2, inclusive. */
            unsigned int weighted_bipred_idc                    : 2;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int constrained_intra_pred_flag            : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int transform_8x8_mode_flag                : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int deblocking_filter_control_present_flag : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int redundant_pic_cnt_present_flag         : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int pic_order_present_flag                 : 1;
            /** \brief Same as the H.264 bitstream syntax element. */
            unsigned int pic_scaling_matrix_present_flag        : 1;
        } bits;
        unsigned int value;
    } pic_fields;
} VAEncPictureParameterBufferH264;

/**
 * \brief Slice parameter for H.264 encoding in baseline, main & high profiles.
 *
 * This structure holds information for \c
 * slice_layer_without_partitioning_rbsp() as defined by the H.264
 * specification.
 *
 * If packed slice headers mode is used, i.e. if the encoding
 * pipeline was configured with the #VA_ENC_PACKED_HEADER_SLICE
 * flag, then the driver expects two more buffers to be provided to
 * the same \c vaRenderPicture() as this buffer:
 * - a #VAEncPackedHeaderParameterBuffer with type set to
 *   VAEncPackedHeaderType::VAEncPackedHeaderSlice ;
 * - a #VAEncPackedHeaderDataBuffer which holds the actual packed
 *   header data.
 *
 * If per-macroblock encoder configuration is needed, \c macroblock_info
 * references a buffer of type #VAEncMacroblockParameterBufferH264. This
 * buffer is not passed to vaRenderPicture(). i.e. it is not destroyed
 * by subsequent calls to vaRenderPicture() and then can be re-used
 * without re-allocating the whole buffer.
 */
typedef struct _VAEncSliceParameterBufferH264 {
    /** \brief Starting MB address for this slice. */
    unsigned int    macroblock_address;
    /** \brief Number of macroblocks in this slice. */
    unsigned int    num_macroblocks;
    /**
     * \brief Per-MB encoder configuration buffer, or \c VA_INVALID_ID.
     *
     * If per-MB encoder configuration is needed, then \ref macroblock_info
     * references a buffer of type #VAEncMacroblockParameterBufferH264
     * (\c VAEncMacroblockParameterBufferType). Otherwise, buffer id
     * is set to \c VA_INVALID_ID and per-MB configuration is derived
     * from this slice parameter.
     *
     * The \c macroblock_info buffer must hold \ref num_macroblocks
     * elements.
     */
    VABufferID      macroblock_info;
    /** \brief Slice type.
     *  Range: 0..2, 5..7, i.e. no switching slices.
     */
    unsigned char   slice_type;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   pic_parameter_set_id;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned short  idr_pic_id;

    /** @name If pic_order_cnt_type == 0 */
    /**@{*/
    /** \brief The picture order count modulo MaxPicOrderCntLsb. */
    unsigned short  pic_order_cnt_lsb;
    /** \brief Valid if \c pic_order_present_flag and this is a bottom field. */
    int             delta_pic_order_cnt_bottom;
    /**@}*/
    /** @name If pic_order_cnt_type == 1 && !delta_pic_order_always_zero_flag */
    /**@{*/
    /** \brief [0]: top, [1]: bottom. */
    int             delta_pic_order_cnt[2];
    /**@}*/

    /** @name If slice_type == B */
    /**@{*/
    unsigned char   direct_spatial_mv_pred_flag;
    /**@}*/

    /** @name If slice_type == P */
    /**@{*/
    /** \brief Specifies if
     * \ref _VAEncPictureParameterBufferH264::num_ref_idx_l0_active_minus1 or
     * \ref _VAEncPictureParameterBufferH264::num_ref_idx_l1_active_minus1 are
     * overriden by the values for this slice.
     */
    unsigned char   num_ref_idx_active_override_flag;
    /** \brief Maximum reference index for reference picture list 0.
     *  Range: 0 to 31, inclusive.
     */
    unsigned char   num_ref_idx_l0_active_minus1;
    /** \brief Maximum reference index for reference picture list 1.
     *  Range: 0 to 31, inclusive.
     */
    unsigned char   num_ref_idx_l1_active_minus1;
    /** \brief Reference picture list 0 (for P slices). */
    VAPictureH264   RefPicList0[32];
    /** \brief Reference picture list 1 (for B slices). */
    VAPictureH264   RefPicList1[32];
    /**@}*/

    /** @name pred_weight_table() */
    /**@{*/
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   luma_log2_weight_denom;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   chroma_log2_weight_denom;
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   luma_weight_l0_flag;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    luma_weight_l0[32];
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    luma_offset_l0[32];
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   chroma_weight_l0_flag;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    chroma_weight_l0[32][2];
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    chroma_offset_l0[32][2];
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   luma_weight_l1_flag;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    luma_weight_l1[32];
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    luma_offset_l1[32];
    /** \brief Same as the H.264 bitstream syntax element. */
    unsigned char   chroma_weight_l1_flag;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    chroma_weight_l1[32][2];
    /** \brief Same as the H.264 bitstream syntax element. */
    signed short    chroma_offset_l1[32][2];
    /**@}*/

    /** \brief Range: 0 to 2, inclusive. */
    unsigned char   cabac_init_idc;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed char     slice_qp_delta;
    /** @name If deblocking_filter_control_present_flag */
    /**@{*/
    /** \brief Range: 0 to 2, inclusive. */
    unsigned char   disable_deblocking_filter_idc;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed char     slice_alpha_c0_offset_div2;
    /** \brief Same as the H.264 bitstream syntax element. */
    signed char     slice_beta_offset_div2;
    /**@}*/
} VAEncSliceParameterBufferH264;

/**
 * @name Macroblock neighbour availability bits
 *
 * \anchor api_enc_h264_mb_pred_avail_bits
 * Definitions for macroblock neighbour availability bits used in
 * intra prediction mode (non MBAFF only).
 *
 * @{
 */
/** \brief References macroblock in the top-left corner. */
#define VA_MB_PRED_AVAIL_TOP_LEFT         (1 << 2)
/** \brief References macroblock above the current macroblock. */
#define VA_MB_PRED_AVAIL_TOP              (1 << 4)
/** \brief References macroblock in the top-right corner. */
#define VA_MB_PRED_AVAIL_TOP_RIGHT        (1 << 3)
/** \brief References macroblock on the left of the current macroblock. */
#define VA_MB_PRED_AVAIL_LEFT             (1 << 6)
/**@}*/

/**
 * \brief Macroblock parameter for H.264 encoding in baseline, main & high 
 * profiles.
 *
 * This structure holds per-macroblock information. The buffer must be
 * allocated with as many elements (macroblocks) as necessary to fit
 * the slice to be encoded. Besides, the per-macroblock records must
 * be written in a strict raster order and with no gap. i.e. every
 * macroblock, regardless of its type, shall have an entry.
 */
typedef struct _VAEncMacroblockParameterBufferH264 {
    /**
     * \brief Quantization parameter.
     *
     * Requested quantization parameter. Range: 0 to 51, inclusive.
     * If \ref qp is set to 0xff, then the actual value is derived
     * from the slice-level value: \c pic_init_qp + \c slice_qp_delta.
     */
    unsigned char   qp;

    union {
        /** @name Data for intra macroblock */
        /**@{*/
        struct {
            union {
                /**
                 * \brief Flag specified to override MB neighbour
                 * availability bits from VME stage.
                 *
                 * This flag specifies that macroblock neighbour
                 * availability bits from the VME stage are overriden
                 * by the \ref pred_avail_flags hereunder.
                 */
                unsigned int    pred_avail_override_flag        : 1;
                /**
                 * \brief Bitwise representation of which macroblocks
                 * are available for intra prediction.
                 *
                 * If the slice is intra-coded, this field represents
                 * the macroblocks available for intra prediction.
                 * See \ref api_enc_h264_mb_pred_avail_bits
                 * "macroblock neighbour availability" bit definitions.
                 */
                unsigned int    pred_avail_flags                : 8;
            } bits;
            unsigned int value;
        } intra_fields;
        /**@}*/

        /** @name Data for inter macroblock */
        /**@{*/
        struct {
            union {
            } bits;
            unsigned int value;
        } inter_fields;
        /**@}*/
    } info;
} VAEncMacroblockParameterBufferH264;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* VA_ENC_H264_H */
