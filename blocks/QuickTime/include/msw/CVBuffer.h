/*
 *  CVBuffer.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVBuffer.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion CVBufferRef types are abstract and only define ways to attach meta data to buffers (such as timestamps,
	        colorspace information, etc.).    CVBufferRefs do not imply any particular kind of data storage.  It could
		be compressed data, image data, etc.
		   
*/

#if !defined(__COREVIDEO_CVBUFFER_H__)
#define __COREVIDEO_CVBUFFER_H__ 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <QuartzCore/CVBase.h>
#include <QuartzCore/CVReturn.h>
#include <CoreFoundation/CFDictionary.h>
#include <stddef.h>
#include <stdint.h>
#else
#pragma warning (disable: 4068)		// ignore unknown pragmas
#include <CVBase.h>
#include <CVReturn.h>
#include <CFDictionary.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#pragma mark CVBufferRef attribute keys

/* The following two keys are useful with the CoreVideo pool and texture cache APIs so that you can specify
   an initial set of default buffer attachments to automatically be attached to the buffer when it is created. */
#if TARGET_OS_MAC
CV_EXPORT const CFStringRef kCVBufferPropagatedAttachmentsKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVBufferNonPropagatedAttachmentsKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
#else
#define kCVBufferPropagatedAttachmentsKey		CFSTR("PropagatedAttachments")
#define kCVBufferNonPropagatedAttachmentsKey		CFSTR("NonPropagatedAttachments")
#endif

#pragma mark CVBufferRef attachment keys

#if TARGET_OS_MAC
CV_EXPORT const CFStringRef kCVBufferMovieTimeKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	// Generally only available for frames emitted by QuickTime; CFDictionary containing these two keys:
CV_EXPORT const CFStringRef kCVBufferTimeValueKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVBufferTimeScaleKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
#else
#define kCVBufferMovieTimeKey					CFSTR("QTMovieTime")
#define kCVBufferTimeValueKey				    CFSTR("TimeValue")
#define kCVBufferTimeScaleKey				    CFSTR("TimeScale")
#endif


#pragma mark CVBufferRef

enum {
	kCVAttachmentMode_ShouldNotPropagate    = 0,
	kCVAttachmentMode_ShouldPropagate       = 1,
};
typedef uint32_t CVAttachmentMode;

/*!
    @typedef	CVBufferRef
    @abstract   Base type for all CoreVideo buffers

*/
typedef struct __CVBuffer *CVBufferRef;

/*!
    @function   CVBufferRetain
    @abstract   Retains a CVBuffer object
    @discussion Like CFRetain CVBufferRetain increments the retain count of a CVBuffer object. In contrast to the CF call it is NULL safe.
    @param      buffer A CVBuffer object that you want to retain.
    @result     A CVBuffer object that is the same as the passed in buffer.
*/
CV_EXPORT CVBufferRef CVBufferRetain(CVBufferRef buffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
/*!
    @function   CVBufferRelease
    @abstract   Release a CVBuffer object
    @discussion Like CFRetain CVBufferRetain decrements the retain count of a CVBuffer object. If that count consequently becomes zero the memory allocated to the object is deallocated and the object is destroyed. In contrast to the CF call it is NULL safe.
    @param      buffer A CVBuffer object that you want to release.
*/
CV_EXPORT void CVBufferRelease(CVBufferRef buffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#pragma mark CVBufferAttachment

/*!
    @function   CVBufferSetAttachment
    @abstract   Sets or adds a attachment of a CVBuffer object
    @discussion You can attach any CF object to a CVBuffer object to store additional information. CVBufferGetAttachment stores an attachement identified by a key. If the key doesn't exist, the attachment will be added. If the key does exist, the existing attachment will be replaced. In bouth cases the retain count of the attachment will be incremented. The value can be any CFType but nil has no defined behavior.
    @param      buffer  Target CVBuffer object.
    @param      key     Key in form of a CFString identifying the desired attachment.
    @param      value	Attachment in form af a CF object.
    @param      attachmentMode	Specifies which attachment mode is desired for this attachment.   A particular attachment key may only exist in
                                a single mode at a time.
*/
CV_EXPORT void	CVBufferSetAttachment(CVBufferRef buffer, CFStringRef key, CFTypeRef value, CVAttachmentMode attachmentMode) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;


/*!
    @function   CVBufferGetAttachment
    @abstract   Returns a specific attachment of a CVBuffer object
    @discussion You can attach any CF object to a CVBuffer object to store additional information. CVBufferGetAttachment retrieves an attachement identified by a key.
    @param      buffer  Target CVBuffer object.
    @param      key	Key in form of a CFString identifying the desired attachment.
    @param      attachmentMode.  Returns the mode of the attachment, if desired.  May be NULL.
    @result     If found the attachment object
*/
CV_EXPORT CFTypeRef CVBufferGetAttachment(CVBufferRef buffer,  CFStringRef key, CVAttachmentMode *attachmentMode) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVBufferRemoveAttachment
    @abstract   Removes a specific attachment of a CVBuffer object
    @discussion CVBufferRemoveAttachment removes an attachement identified by a key. If found the attachement is removed and the retain count decremented.
    @param      buffer  Target CVBuffer object.
    @param      key	Key in form of a CFString identifying the desired attachment.
*/
CV_EXPORT void	CVBufferRemoveAttachment(CVBufferRef buffer, CFStringRef key) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVBufferRemoveAllAttachments
    @abstract   Removes all attachments of a CVBuffer object
    @discussion While CVBufferRemoveAttachment removes a specific attachement identified by a key CVBufferRemoveAllAttachments removes all attachments of a buffer and decrements their retain counts.
    @param      buffer  Target CVBuffer object.
*/
CV_EXPORT void	CVBufferRemoveAllAttachments(CVBufferRef buffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVBufferGetAttachments
    @abstract   Returns all attachments of a CVBuffer object
    @discussion CVBufferGetAttachments is a convenience call that returns all attachments with their corresponding keys in a CFDictionary.
    @param      buffer  Target CVBuffer object.
    @result     A CFDictionary with all buffer attachments identified by there keys. If no attachment is present, the dictionary is empty.  Returns NULL
		for invalid attachment mode.
*/
CV_EXPORT CFDictionaryRef CVBufferGetAttachments(CVBufferRef buffer, CVAttachmentMode attachmentMode) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVBufferSetAttachments
    @abstract   Sets a set of attachments for a CVBuffer
    @discussion CVBufferSetAttachments is a convenience call that in turn calls CVBufferSetAttachment for each key and value in the given dictionary. All key value pairs must be in the root level of the dictionary.
    @param      buffer  Target CVBuffer object.
*/
CV_EXPORT void CVBufferSetAttachments(CVBufferRef buffer, CFDictionaryRef theAttachments, CVAttachmentMode attachmentMode) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVBufferPropagateAttachments
    @abstract   Copy all propagatable attachments from one buffer to another.
    @discussion CVBufferPropagateAttachments is a convenience call that copies all attachments with a mode of kCVAttachmentMode_ShouldPropagate from one
                buffer to another.
    @param      sourceBuffer  CVBuffer to copy attachments from.
    @param      destinationBuffer  CVBuffer to copy attachments to.
*/
CV_EXPORT void  CVBufferPropagateAttachments(CVBufferRef sourceBuffer, CVBufferRef destinationBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#if defined(__cplusplus)
}
#endif

#endif
