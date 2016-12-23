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

#include <ostream>
#include <sstream>
#include <string>

// This is a wrapper to make ::OutputDebugStringA behave like std::ostream
// Derived from code written by Sven Axelsson
// Documented: http://www.codeproject.com/KB/debug/debugout.aspx

namespace cinder { namespace android {

void LogCatOutput( const char *text );

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
        log_cat(this->str().c_str());
        this->str(std::basic_string<CharT>());    // Clear the string buffer

        return 0;
    }

    void log_cat(const CharT *text) {}
};

template<>
inline void basic_debugbuf<char>::log_cat(const char *text)
{
    LogCatOutput(text);
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
        delete this->rdbuf(); 
    }
};

typedef basic_dostream<char>    dostream;

} } // namespace cinder::android