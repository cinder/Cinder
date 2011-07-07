/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for MIDI Integration for Cinder framework developed by The Barbarian Group, 2010
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */


#pragma once

#include "cinder/Cinder.h"

#ifdef CINDER_LINUX
	#define __LINUX_ALSASEQ__
#elif defined(CINDER_MSW)
	#define __WINDOWS_MM__
#elif defined(CINDER_MAC)
	#define __MACOSX_CORE__
#endif


// channel info
#define    MIDI_NOTE_OFF             128
#define    MIDI_NOTE_ON              144
#define    MIDI_POLY_PRESSURE        160
#define    MIDI_CONTROL_CHANGE       176
#define    MIDI_PROGRAM_CHANGE       192
#define    MIDI_CHANNEL_PRESSURE     208
#define    MIDI_PITCH_BEND           224

// system exclusive
#define    MIDI_NOTE_ON              144

// system common
#define    MIDI_TIME_CODE            241
#define    MIDI_SONG_POS_POINTER     242
#define    MIDI_SONG_SELECT          243
#define    MIDI_TUNE_REQUEST         244
#define    MIDI_EOX                  247

// system realtime
#define    MIDI_TIME_CLOCK           248
#define    MIDI_START                250
#define    MIDI_CONTINUE             251
#define    MIDI_STOP                 252
#define    MIDI_ACTIVE_SENSING       254
#define    MIDI_SYSTEM_RESET         255