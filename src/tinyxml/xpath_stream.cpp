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

/**
   \file xpath_stream.cpp
   \author Yves Berquin
   Specialized byte stream for the TinyXPath project
*/

#include "xpath_syntax.h"
#include "xpath_stream.h"

namespace TinyXPath
{

/// Redefinition of a token_syntax_decoder, with an xpath_stream pointer
/// \n We could have made this a double dependency on token_syntax_decoder and xpath_stream, 
/// but this is not good pratice IMHO
class token_redef : public token_syntax_decoder 
{
protected :
   /// pointer to an xpath_stream instance
	xpath_stream * xsp_stream;
public :
   /// constructor
	token_redef (
      xpath_stream * xsp_in)  ///< Pointer to an xpath_stream instance
	{
		xsp_stream = xsp_in;
	}
   /// dispatcher of the xpath_stream::v_action 
   virtual void v_action (xpath_construct xc_rule, unsigned u_sub, unsigned u_variable = 0, const char * cp_literal = "")
	{
		xsp_stream -> v_action (xc_rule, u_sub, u_variable, cp_literal);
	}
   /// dispatcher of the xpath_stream::i_get_action_counter
   virtual int i_get_action_counter () {return xsp_stream -> i_get_action_counter ();}
} ;

/// xpath_stream constructor
xpath_stream::xpath_stream (const char * cp_in) : byte_stream (cp_in) 
{
   tlp_list = new token_redef (this);
}

}
