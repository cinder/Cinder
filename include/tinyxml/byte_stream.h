/*
www.sourceforge.net/projects/tinyxpath
Copyright (c) 2002-2004 Yves Berquin (yvesb@users.sourceforge.net)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#ifndef __TINYBYTESTREAM_H
#define __TINYBYTESTREAM_H

#include <string.h>
#include "lex_util.h"

namespace TinyXPath
{

/**
   A Byte stream class, very crude. The only purpose of this class is to walk in a byte stream,
   available at construction time.
*/
class byte_stream
{
   /// Length of the total string, + 1
   unsigned u_length;
   /// Total string
   _byte_ * bp_in;
   /// Current read position
   _byte_ * bp_current;
   /// First invalid position
   _byte_ * bp_end;
   /// true when there are still some byte to read
   bool o_valid;
public :
   /// constructor
   byte_stream (const char * cp_in)
   {
      u_length = strlen (cp_in) + 1;
      bp_in = new _byte_ [u_length] ;
      memcpy (bp_in, cp_in, u_length);
      bp_current = bp_in;
      bp_end = bp_in + u_length - 1;
      o_valid = (bp_current != bp_end);
   }
   /// destructor
   ~ byte_stream ()
   {
      if (bp_in)
         delete [] bp_in;
   }
   /// Returns the byte on top 
   _byte_ b_top ()
   {
      return * bp_current;
   }
   /// Consumes one byte
   _byte_ b_pop ()
   {
      if (! o_is_valid ())
         return 0;
      bp_current++;
      o_valid = (bp_current != bp_end);
      return * (bp_current - 1);
   }
   /// true if there are still some byte to consume
   bool o_is_valid ()
   {
      return o_valid;
   }
   /// number of bytes still to consume
   unsigned u_remain ()
   {
      return (unsigned) (bp_end - bp_current);
   }
   /// peek a byte a little further down the stream
   _byte_ b_forward (unsigned u_nb_char)
   {
      if (u_remain () > u_nb_char)
         return bp_current [u_nb_char];
      return 0;
   }
   /// get a byte backward pointer to the stream 
   const _byte_ * bp_get_backward (unsigned u_amount)
   {
      return bp_current - u_amount + 1;
   }
} ;

}

#endif
