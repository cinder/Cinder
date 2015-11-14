/*
 * Copyright (c) 2007 Intel Corporation. All Rights Reserved.
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
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Video Decode Acceleration -Backend API
 */

#ifndef _VA_BACKEND_H_
#define _VA_BACKEND_H_

#include <va/va.h>
#include <linux/videodev2.h>

typedef struct VADriverContext *VADriverContextP;
typedef struct VADisplayContext *VADisplayContextP;

/** \brief VA display types. */
enum {
    /** \brief Mask to major identifier for VA display type. */
    VA_DISPLAY_MAJOR_MASK = 0xf0,

    /** \brief VA/X11 API is used, through vaGetDisplay() entry-point. */
    VA_DISPLAY_X11      = 0x10,
    /** \brief VA/GLX API is used, through vaGetDisplayGLX() entry-point. */
    VA_DISPLAY_GLX      = (VA_DISPLAY_X11 | (1 << 0)),
    /** \brief VA/Android API is used, through vaGetDisplay() entry-point. */
    VA_DISPLAY_ANDROID  = 0x20,
    /** \brief VA/DRM API is used, through vaGetDisplayDRM() entry-point. */
    VA_DISPLAY_DRM      = 0x30,
    /** \brief VA/DRM API is used, with a render-node device path */
    VA_DISPLAY_DRM_RENDERNODES = (VA_DISPLAY_DRM | (1 << 0)),
    /** \brief VA/Wayland API is used, through vaGetDisplayWl() entry-point. */
    VA_DISPLAY_WAYLAND  = 0x40,
};

struct VADriverVTable
{
	VAStatus (*vaTerminate) ( VADriverContextP ctx );

	VAStatus (*vaQueryConfigProfiles) (
		VADriverContextP ctx,
		VAProfile *profile_list,	/* out */
		int *num_profiles			/* out */
	);

	VAStatus (*vaQueryConfigEntrypoints) (
		VADriverContextP ctx,
		VAProfile profile,
		VAEntrypoint  *entrypoint_list,	/* out */
		int *num_entrypoints			/* out */
	);

	VAStatus (*vaGetConfigAttributes) (
		VADriverContextP ctx,
		VAProfile profile,
		VAEntrypoint entrypoint,
		VAConfigAttrib *attrib_list,	/* in/out */
		int num_attribs
	);

	VAStatus (*vaCreateConfig) (
		VADriverContextP ctx,
		VAProfile profile, 
		VAEntrypoint entrypoint, 
		VAConfigAttrib *attrib_list,
		int num_attribs,
		VAConfigID *config_id		/* out */
	);

	VAStatus (*vaDestroyConfig) (
		VADriverContextP ctx,
		VAConfigID config_id
	);

	VAStatus (*vaQueryConfigAttributes) (
		VADriverContextP ctx,
		VAConfigID config_id, 
		VAProfile *profile,		/* out */
		VAEntrypoint *entrypoint, 	/* out */
		VAConfigAttrib *attrib_list,	/* out */
		int *num_attribs		/* out */
	);

	VAStatus (*vaCreateSurfaces) (
		VADriverContextP ctx,
		int width,
		int height,
		int format,
		int num_surfaces,
		VASurfaceID *surfaces		/* out */
	);

	VAStatus (*vaDestroySurfaces) (
		VADriverContextP ctx,
		VASurfaceID *surface_list,
		int num_surfaces
	);

	VAStatus (*vaCreateContext) (
		VADriverContextP ctx,
		VAConfigID config_id,
		int picture_width,
		int picture_height,
		int flag,
		VASurfaceID *render_targets,
		int num_render_targets,
		VAContextID *context		/* out */
	);

	VAStatus (*vaDestroyContext) (
		VADriverContextP ctx,
		VAContextID context
	);

	VAStatus (*vaCreateBuffer) (
		VADriverContextP ctx,
		VAContextID context,		/* in */
		VABufferType type,		/* in */
		unsigned int size,		/* in */
		unsigned int num_elements,	/* in */
		void *data,			/* in */
		VABufferID *buf_id		/* out */
	);

	VAStatus (*vaBufferSetNumElements) (
		VADriverContextP ctx,
		VABufferID buf_id,	/* in */
		unsigned int num_elements	/* in */
	);

	VAStatus (*vaMapBuffer) (
		VADriverContextP ctx,
		VABufferID buf_id,	/* in */
		void **pbuf         /* out */
	);

	VAStatus (*vaUnmapBuffer) (
		VADriverContextP ctx,
		VABufferID buf_id	/* in */
	);

	VAStatus (*vaDestroyBuffer) (
		VADriverContextP ctx,
		VABufferID buffer_id
	);

	VAStatus (*vaBeginPicture) (
		VADriverContextP ctx,
		VAContextID context,
		VASurfaceID render_target
	);

	VAStatus (*vaRenderPicture) (
		VADriverContextP ctx,
		VAContextID context,
		VABufferID *buffers,
		int num_buffers
	);

	VAStatus (*vaEndPicture) (
		VADriverContextP ctx,
		VAContextID context
	);

	VAStatus (*vaSyncSurface) (
		VADriverContextP ctx,
		VASurfaceID render_target
	);

	VAStatus (*vaQuerySurfaceStatus) (
		VADriverContextP ctx,
		VASurfaceID render_target,
		VASurfaceStatus *status	/* out */
	);

	VAStatus (*vaQuerySurfaceError) (
		VADriverContextP ctx,
		VASurfaceID render_target,
                VAStatus error_status,
                void **error_info /*out*/
	);

	VAStatus (*vaPutSurface) (
    		VADriverContextP ctx,
		VASurfaceID surface,
		void* draw, /* Drawable of window system */
		short srcx,
		short srcy,
		unsigned short srcw,
		unsigned short srch,
		short destx,
		short desty,
		unsigned short destw,
		unsigned short desth,
		VARectangle *cliprects, /* client supplied clip list */
		unsigned int number_cliprects, /* number of clip rects in the clip list */
		unsigned int flags /* de-interlacing flags */
	);

	VAStatus (*vaQueryImageFormats) (
		VADriverContextP ctx,
		VAImageFormat *format_list,        /* out */
		int *num_formats           /* out */
	);

	VAStatus (*vaCreateImage) (
		VADriverContextP ctx,
		VAImageFormat *format,
		int width,
		int height,
		VAImage *image     /* out */
	);

	VAStatus (*vaDeriveImage) (
		VADriverContextP ctx,
		VASurfaceID surface,
		VAImage *image     /* out */
	);

	VAStatus (*vaDestroyImage) (
		VADriverContextP ctx,
		VAImageID image
	);
	
	VAStatus (*vaSetImagePalette) (
	        VADriverContextP ctx,
	        VAImageID image,
	        /*
                 * pointer to an array holding the palette data.  The size of the array is
                 * num_palette_entries * entry_bytes in size.  The order of the components
                 * in the palette is described by the component_order in VAImage struct
                 */
                unsigned char *palette
	);
	
	VAStatus (*vaGetImage) (
		VADriverContextP ctx,
		VASurfaceID surface,
		int x,     /* coordinates of the upper left source pixel */
		int y,
		unsigned int width, /* width and height of the region */
		unsigned int height,
		VAImageID image
	);

	VAStatus (*vaPutImage) (
		VADriverContextP ctx,
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

	VAStatus (*vaQuerySubpictureFormats) (
		VADriverContextP ctx,
		VAImageFormat *format_list,        /* out */
		unsigned int *flags,       /* out */
		unsigned int *num_formats  /* out */
	);

	VAStatus (*vaCreateSubpicture) (
		VADriverContextP ctx,
		VAImageID image,
		VASubpictureID *subpicture   /* out */
	);

	VAStatus (*vaDestroySubpicture) (
		VADriverContextP ctx,
		VASubpictureID subpicture
	);

        VAStatus (*vaSetSubpictureImage) (
                VADriverContextP ctx,
                VASubpictureID subpicture,
                VAImageID image
        );

	VAStatus (*vaSetSubpictureChromakey) (
		VADriverContextP ctx,
		VASubpictureID subpicture,
		unsigned int chromakey_min,
		unsigned int chromakey_max,
		unsigned int chromakey_mask
	);

	VAStatus (*vaSetSubpictureGlobalAlpha) (
		VADriverContextP ctx,
		VASubpictureID subpicture,
		float global_alpha 
	);

	VAStatus (*vaAssociateSubpicture) (
		VADriverContextP ctx,
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
		 * whether to enable chroma-keying or global-alpha
		 * see VA_SUBPICTURE_XXX values
		 */
		unsigned int flags
	);

	VAStatus (*vaDeassociateSubpicture) (
		VADriverContextP ctx,
		VASubpictureID subpicture,
		VASurfaceID *target_surfaces,
		int num_surfaces
	);

	VAStatus (*vaQueryDisplayAttributes) (
		VADriverContextP ctx,
		VADisplayAttribute *attr_list,	/* out */
		int *num_attributes		/* out */
        );

	VAStatus (*vaGetDisplayAttributes) (
		VADriverContextP ctx,
		VADisplayAttribute *attr_list,	/* in/out */
		int num_attributes
        );
        
        VAStatus (*vaSetDisplayAttributes) (
		VADriverContextP ctx,
                VADisplayAttribute *attr_list,
                int num_attributes
        );

        /* used by va trace */        
        VAStatus (*vaBufferInfo) (
                   VADriverContextP ctx,      /* in */
                   VABufferID buf_id,         /* in */
                   VABufferType *type,        /* out */
                   unsigned int *size,        /* out */
                   unsigned int *num_elements /* out */
        );

        /* lock/unlock surface for external access */    
        VAStatus (*vaLockSurface) (
		VADriverContextP ctx,
                VASurfaceID surface,
                unsigned int *fourcc, /* out  for follow argument */
                unsigned int *luma_stride,
                unsigned int *chroma_u_stride,
                unsigned int *chroma_v_stride,
                unsigned int *luma_offset,
                unsigned int *chroma_u_offset,
                unsigned int *chroma_v_offset,
                unsigned int *buffer_name, /* if it is not NULL, assign the low lever
                                            * surface buffer name
                                            */
                void **buffer /* if it is not NULL, map the surface buffer for
                                * CPU access
                                */
        );
    
        VAStatus (*vaUnlockSurface) (
		VADriverContextP ctx,
                VASurfaceID surface
        );

        /* DEPRECATED */
        VAStatus
        (*vaGetSurfaceAttributes)(
            VADriverContextP    dpy,
            VAConfigID          config,
            VASurfaceAttrib    *attrib_list,
            unsigned int        num_attribs
        );

        VAStatus
        (*vaCreateSurfaces2)(
            VADriverContextP    ctx,
            unsigned int        format,
            unsigned int        width,
            unsigned int        height,
            VASurfaceID        *surfaces,
            unsigned int        num_surfaces,
            VASurfaceAttrib    *attrib_list,
            unsigned int        num_attribs
        );

        VAStatus
        (*vaQuerySurfaceAttributes)(
            VADriverContextP    dpy,
            VAConfigID          config,
            VASurfaceAttrib    *attrib_list,
            unsigned int       *num_attribs
        );

        VAStatus
        (*vaAcquireBufferHandle)(
            VADriverContextP    ctx,
            VABufferID          buf_id,         /* in */
            VABufferInfo *      buf_info        /* in/out */
        );

        VAStatus
        (*vaReleaseBufferHandle)(
            VADriverContextP    ctx,
            VABufferID          buf_id          /* in */
        );
};

struct VADriverContext
{
    void *pDriverData;

    /**
     * The core VA implementation hooks.
     *
     * This structure is allocated from libva with calloc().
     */
    struct VADriverVTable *vtable;

    /**
     * The VA/GLX implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * VA/GLX API. The driver implementation is responsible for the
     * allocation and deallocation of this structure.
     */
    struct VADriverVTableGLX *vtable_glx;

    /**
     * The VA/EGL implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * VA/EGL API. The driver implementation is responsible for the
     * allocation and deallocation of this structure.
     */
    struct VADriverVTableEGL *vtable_egl;

    /**
     * The third-party/private implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * private API. The driver implementation is responsible for the
     * allocation and deallocation of this structure.
     */
    void *vtable_tpi;

    void *native_dpy;
    int x11_screen;
    int version_major;
    int version_minor;
    int max_profiles;
    int max_entrypoints;
    int max_attributes;
    int max_image_formats;
    int max_subpic_formats;
    int max_display_attributes;
    const char *str_vendor;

    void *handle;			/* dlopen handle */

    /**
     * \brief DRM state.
     *
     * This field holds driver specific data for DRM-based
     * drivers. This structure is allocated from libva with
     * calloc(). Do not deallocate from within VA driver
     * implementations.
     *
     * All structures shall be derived from struct drm_state. So, for
     * instance, this field holds a dri_state structure for VA/X11
     * drivers that use the DRM protocol.
     */
    void *drm_state;

    void *glx;				/* opaque for GLX code */

    /** \brief VA display type. */
    unsigned long display_type;

    /**
     * The VA/Wayland implementation hooks.
     *
     * This structure is intended for drivers that implement the
     * VA/Wayland API. libVA allocates this structure with calloc()
     * and owns the resulting memory.
     */
    struct VADriverVTableWayland *vtable_wayland;

    /**
     * \brief The VA/VPP implementation hooks.
     *
     * This structure is allocated from libva with calloc().
     */
    struct VADriverVTableVPP *vtable_vpp;

    char *override_driver_name;

    unsigned long reserved[41];         /* reserve for future add-ins, decrease the subscript accordingly */
};

#define VA_DISPLAY_MAGIC 0x56414430 /* VAD0 */
struct VADisplayContext
{
    int vadpy_magic;
    
    VADisplayContextP pNext;
    VADriverContextP pDriverContext;

    int (*vaIsValid) (
	VADisplayContextP ctx
    );

    void (*vaDestroy) (
	VADisplayContextP ctx
    );

    VAStatus (*vaGetDriverName) (
	VADisplayContextP ctx,
	char **driver_name
    );

    void *opaque; /* opaque for display extensions (e.g. GLX) */
    void *vatrace; /* opaque for VA trace context */
    void *vafool; /* opaque for VA fool context */
};

typedef VAStatus (*VADriverInit) (
    VADriverContextP driver_context
);

#endif /* _VA_BACKEND_H_ */
