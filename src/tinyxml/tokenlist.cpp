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
   \file tokenlist.cpp
   \author Yves Berquin
   XPath Syntax analyzer for TinyXPath project : token list handling
*/

#include "tokenlist.h"

namespace TinyXPath
{

/// Decodes an XPath expression, further manipulating a token list
/// \n On input, we have a list of basic lexical tokens. We only merge here the 
/// multiple tokens : like '::' or '!='. We also delete whitespace tokens
void token_list::v_tokenize_expression ()
{
   v_set_current_top ();
   while (ltp_get (1))
   {
      switch (ltp_get (0) -> lex_get_value ())
      {
         case lex_colon :
            if (ltp_get (1) -> lex_get_value () == lex_colon)
            {
               v_replace_current (lex_2_colon, "::");
               v_delete_next ();
            }
            else
               v_inc_current (1);
            break;
         case lex_slash :
            if (ltp_get (1) -> lex_get_value () == lex_slash)
            {
               v_replace_current (lex_2_slash, "//");
               v_delete_next ();
            }
            else
               v_inc_current (1);
            break;
         case lex_exclam :
            if (ltp_get (1) -> lex_get_value () == lex_equal)
            {
               v_replace_current (lex_not_equal, "!=");
               v_delete_next ();
            }
            else
               v_inc_current (1);
            break;
         case lex_lt :
            if (ltp_get (1) -> lex_get_value () == lex_equal)
            {
               v_replace_current (lex_lt_equal, "<=");
               v_delete_next ();
            }
            else
               v_inc_current (1);
            break;
         case lex_gt :
            if (ltp_get (1) -> lex_get_value () == lex_equal)
            {
               v_replace_current (lex_gt_equal, ">=");
               v_delete_next ();
            }
            else
               v_inc_current (1);
            break;
         case lex_dot :
            if (ltp_get (1) -> lex_get_value () == lex_dot)
            {
               v_replace_current (lex_2_dot, "..");
               v_delete_next ();
            }
            else
               v_inc_current (1);
            break;
         case lex_space :
            v_delete_current ();
            break;
         default :
            v_inc_current (1);
            break;
      }     // switch
   }     // while
}

}
