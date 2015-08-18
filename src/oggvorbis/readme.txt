===============================================================================
Origin and arrangement of Ogg Vobis source code: http://www.xiph.org/downloads/

ogg (version: libogg-1.3.1)
- first be sure to run ./configure to generate the include/ogg/config_types.h file
- copied .c's in src folder to src/oggvorbis/ogg/
- copied .h's in include/ogg folder to include/oggvorbis/ogg/
- initialized  "ogg_int16_t temp = 0;" on os.h line 97 to silence warning.

vorbis (version: libvorbis-1.3.4)
- copied .[ch]'s from lib folder to src/oggvorbis/vorbis/
- copied .h's from include/vorbis folder to include/oggvorbis/vorbis/
- removed barkmel.c, and tone.c, psytune.c (utility programs)
- added '#pragma warning( disable: 4244 )' to many c files to silence conversion warnings
===============================================================================

Xiph.org license:

Copyright (c) 2002-2008 Xiph.org Foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
