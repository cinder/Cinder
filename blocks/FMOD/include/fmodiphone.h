/* ============================================================================================ */
/* FMOD iPhone Specific header file. Copyright (c), Firelight Technologies Pty, Ltd. 2005-2011. */
/* ============================================================================================ */

#ifndef _FMODIPHONE_H
#define _FMODIPHONE_H

#include "fmod.h"


/*
[ENUM]
[
    [DESCRIPTION]
    This will set the default audio session category to use for your application, see
    the Apple documentation for detailed information about what each audio session
    category does.
 
    [REMARKS]
    When using the recording API, FMOD will automatically switch to the "PlayAndRecord"
    audio session category then switch back when recording is complete.
 
    [PLATFORMS]
    iPhone

    [SEE_ALSO]
]
*/
typedef enum
{
    FMOD_IPHONE_SESSIONCATEGORY_DEFAULT,            /* Default for the device OS version (MediaPlayback for iPhone OS 2.0 to 2.1, SoloAmbientSound for iPhone OS 2.2 onwards) */
    FMOD_IPHONE_SESSIONCATEGORY_AMBIENTSOUND,       /* kAudioSessionCategory_AmbientSound ~ obeys slient switch, silent when locked, mixes with other audio, not allowed in background */
    FMOD_IPHONE_SESSIONCATEGORY_SOLOAMBIENTSOUND,   /* kAudioSessionCategory_SoloAmbientSound (iPhone OS >= 2.2 required) ~ obeys slient switch, silent when locked, doesn't mix with other audio, not allowed in background */
    FMOD_IPHONE_SESSIONCATEGORY_MEDIAPLAYBACK,      /* kAudioSessionCategory_MediaPlayback ~ ignores slient switch, plays when locked, doesn't mix with other audio (unless forced), allowed in background */
    FMOD_IPHONE_SESSIONCATEGORY_PLAYANDRECORD,      /* kAudioSessionCategory_PlayAndRecord ~ ignores slient switch, plays when locked, doesn't mix with other audio (unless forced), allowed in background */
    
    FMOD_IPHONE_SESSIONCATEGORY_FORCEINT = 65536    /* Makes sure this enum is signed 32bit */
} FMOD_IPHONE_SESSIONCATEGORY;


/*
[ENUM]
[
    [DESCRIPTION]
    Control whether the sound will use a the dedicated hardware decoder or a software codec.
 
    [REMARKS]
    Currently in all iPhone OS devices there is only one hardware decoder codec. However fevices
    that are running OS 3.0 or newer have access to software codecs.
 
    [PLATFORMS]
    iPhone

    [SEE_ALSO]
]
*/
typedef enum
{
    FMOD_AUDIOQUEUE_CODECPOLICY_DEFAULT,            /* Try hardware first, if it's in use or prohibited by audio session, try software. */
    FMOD_AUDIOQUEUE_CODECPOLICY_SOFTWAREONLY,       /* kAudioQueueHardwareCodecPolicy_UseSoftwareOnly ~ try software, if not available fail. */
    FMOD_AUDIOQUEUE_CODECPOLICY_HARDWAREONLY,       /* kAudioQueueHardwareCodecPolicy_UseHardwareOnly ~ try hardware, if not available fail. */
    
    FMOD_AUDIOQUEUE_CODECPOLICY_FORCEINT = 65536    /* Makes sure this enum is signed 32bit */
} FMOD_AUDIOQUEUE_CODECPOLICY;


/*
[STRUCTURE] 
[
    [DESCRIPTION]
    Use this structure to set the information required for iPhone initialization.

    Pass this structure in as the "extradriverdata" parameter in System::init.

    [REMARKS]

    [PLATFORMS]
    iPhone

    [SEE_ALSO]
    System::init
]
*/
typedef struct FMOD_IPHONE_EXTRADRIVERDATA
{
    FMOD_IPHONE_SESSIONCATEGORY  sessionCategory;        /* (IN)  Default audio session category to use for output */
    bool                         forceSpeakerOutput;     /* (IN)  Force audio out the main speaker, don't allow output to be redirected to the receiver speaker when recording (iPhone OS >= 2.1 required) */
    bool                         forceMixWithOthers;     /* (IN)  Force mixing behavior allowing iPod audio to play with FMOD even if the audio session doesn't usually permit this */ 
} FMOD_IPHONE_EXTRADRIVERDATA;

#ifdef __cplusplus
extern "C"
{
#endif

/*
[
	[DESCRIPTION]
    Determine if external audio (i.e. iPod) is playing

	[PARAMETERS]
    'playing'    Whether any external audio is playing or not.

	[RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    iPhone

    [SEE_ALSO]
]
*/
FMOD_RESULT F_API FMOD_IPhone_OtherAudioIsPlaying(bool *playing);


/*
[
	[DESCRIPTION]
    Duck any external audio (i.e. iPod)

	[PARAMETERS]
    'duck'    Whether to duck external audio or return it to normal volume.

	[RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    iPhone

    [SEE_ALSO]
]
*/
FMOD_RESULT F_API FMOD_IPhone_DuckOtherAudio(bool duck);


/*
[
	[DESCRIPTION]
    Set the mixable state for (dis)allowing iPod audio to play with FMOD even if the audio session doesn't usually permit this

	[PARAMETERS]
    'mixable'    Whether to force mixing behavior.

	[RETURN_VALUE]

    [REMARKS]

    [PLATFORMS]
    iPhone

    [SEE_ALSO]
]
*/
FMOD_RESULT F_API FMOD_IPhone_MixWithOtherAudio(bool mix);


/*
[
	[DESCRIPTION]
    Restore playback when the application has been interrupted in a backgrounded state.

	[PARAMETERS]

	[RETURN_VALUE]

    [REMARKS]
    Safe to always call when entering the foreground to ensure playback is restored.
 
    [PLATFORMS]
    iPhone

    [SEE_ALSO]
]
*/
FMOD_RESULT F_API FMOD_IPhone_RestoreAudioSession();

#ifdef __cplusplus
}
#endif
    
#endif  /* _FMODIPHONE_H */
