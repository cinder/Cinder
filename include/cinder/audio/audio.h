/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

// general
#include "cinder/audio/Buffer.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/Device.h"
#include "cinder/audio/Exception.h"
#include "cinder/audio/Param.h"
#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/Voice.h"
#include "cinder/audio/WaveTable.h"

// audio::Nodes
#include "cinder/audio/Node.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/NodeMath.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/InputNode.h"
#include "cinder/audio/OutputNode.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"

// audio::dsp
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/dsp/Biquad.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/dsp/Fft.h"
#include "cinder/audio/dsp/RingBuffer.h"
