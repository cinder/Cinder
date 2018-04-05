/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include <windows.h>
#undef min
#undef max
#include <ostream>
#include <sstream>
#include <string>

#include "cinder/msw/CinderMsw.h"

// This is a wrapper to make ::OutputDebugStringA behave like std::ostream
// Derived from code written by Sven Axelsson
// Documented: http://www.codeproject.com/KB/debug/debugout.aspx

namespace cinder { namespace msw {

template <class CharT, class TraitsT = std::char_traits<CharT> >
class basic_debugbuf : 
    public std::basic_stringbuf<CharT, TraitsT>
{
public:

    virtual ~basic_debugbuf() {
        sync();
    }

protected:
    int sync() {
        output_debug_string(str().c_str());
        str(std::basic_string<CharT>());    // Clear the string buffer

        return 0;
    }

    void output_debug_string(const CharT *text) {}
};

template<>
void basic_debugbuf<char>::output_debug_string(const char *text)
{
	std::wstring textW = toWideString( text );
	::OutputDebugStringW( textW.c_str() );
}

template<>
void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text)
{
    ::OutputDebugStringW(text);
}

template<class CharT, class TraitsT = std::char_traits<CharT> >
class basic_dostream : 
    public std::basic_ostream<CharT, TraitsT>
{
public:

    basic_dostream() : std::basic_ostream<CharT, TraitsT>
                (new basic_debugbuf<CharT, TraitsT>()) {}
    ~basic_dostream() 
    {
        delete rdbuf(); 
    }
};

typedef basic_dostream<char>    dostream;
typedef basic_dostream<wchar_t> wdostream;

} } // namespace cinder::msw
