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
   \file xpath_stream.h
   \author Yves Berquin
   Specialized byte stream for the TinyXPath project
*/

#ifndef __TINYXPSTREAM_H
#define __TINYXPSTREAM_H

#include "lex_util.h"
#include "byte_stream.h"
#include "xpath_syntax.h"
#include "tinyxml.h"
#include "tinystr.h"

namespace TinyXPath
{

/**
   A specialized version of byte_stream for XPath 
*/
class xpath_stream : public byte_stream 
{
protected :
   /// List of tokens
   token_syntax_decoder * tlp_list;

public :
   /// constructor
   xpath_stream (const char * cp_in);
   /// destructor
   virtual ~ xpath_stream ()
   {
      delete tlp_list;
   }
   /// Decode the byte stream, and construct the lexical list
   void v_lexico_decode ()
   {
      enum {s_init, s_ncname, s_number, s_literal_1, s_literal_2, s_end} state;
      lexico lex_new, lex_next;
      unsigned u_size;
      bool o_dot_in_number;

      u_size = 0;            
      o_dot_in_number = false;
      state = s_init;
      while (state != s_end)
      {
         lex_next = lex_get_class (b_top ());
         switch (state)
         {
            case s_init :
               switch (lex_next)
               {
                  case lex_bchar :
                  case lex_under :
                     // [XML:4] NCName	::= (Letter | '_') (NCNameChar)* 

                     u_size = 1;
                     state = s_ncname;
                     b_pop ();
                     break;
                  case lex_null :
                     state = s_end;
                     break;
                  case lex_digit :
                     u_size = 1;
                     state = s_number;
                     o_dot_in_number = false;
                     b_pop ();
                     break;
                  case lex_dot :
                     if (lex_get_class (b_forward (1)) == lex_digit)
                     {
                        // [30]   Number				::=   Digits ('.' Digits?)? | '.' Digits 
                        // [31]   Digits				::=   [0-9]+ 
                        u_size = 1;
                        state = s_number;
                        o_dot_in_number = true;
                        b_pop ();
                     }
                     else
                     {
                        tlp_list -> v_add_token (lex_next, bp_get_backward (1), 1);                 
                        b_pop ();
                     }
                     break;

                  case lex_1_quote :
                     // [29]   Literal				::=   '"' [^"]* '"' | "'" [^']* "'" 
                     u_size = 0;
                     b_pop ();
                     state = s_literal_1;
                     break;

                  case lex_2_quote :
                     // [29]   Literal				::=   '"' [^"]* '"' | "'" [^']* "'" 
                     u_size = 0;
                     b_pop ();
                     state = s_literal_2;
                     break;

                  default :
                     tlp_list -> v_add_token (lex_next, bp_get_backward (1), 1);                 
                     b_pop ();
                     break;
               }
               break;
            case s_literal_1 :
               // [29]   Literal				::=   '"' [^"]* '"' | "'" [^']* "'" 
               switch (lex_next)
               {
                  case lex_1_quote :
                     tlp_list -> v_add_token (lex_literal, bp_get_backward (u_size + 1), u_size);                 
                     b_pop ();
                     state = s_init;
                     break;
                  default :
                     u_size++;
                     b_pop ();
                     break;
               }
               break;
            case s_literal_2 :
               // [29]   Literal				::=   '"' [^"]* '"' | "'" [^']* "'" 
               switch (lex_next)
               {
                  case lex_2_quote :
                     tlp_list -> v_add_token (lex_literal, bp_get_backward (u_size + 1), u_size);                 
                     b_pop ();
                     state = s_init;
                     break;
                  default :
                     u_size++;
                     b_pop ();
                     break;
               }
               break;
            case s_ncname :
               switch (lex_next)
               {
                  // [XML:5] NCNameChar ::= Letter | Digit | '.' | '-' | '_' | CombiningChar | Extender
                  case lex_bchar :
                  case lex_digit :
                  case lex_dot :
                  case lex_minus :
                  case lex_under :
                  case lex_extend :
                     u_size++;
                     b_pop ();
                     break;
                  default :
                     lex_new = lex_test_id (bp_get_backward (u_size + 1), u_size, lex_next);
                     tlp_list -> v_add_token (lex_new, bp_get_backward (u_size + 1), u_size);
                     state = s_init;
                     break;
               }
               break;
            case s_number :
               switch (lex_next)
               {
                  // [30]   Number				::=   Digits ('.' Digits?)? | '.' Digits 
                  // [31]   Digits				::=   [0-9]+ 
                  case lex_dot :
                     if (o_dot_in_number)
                     {
                        tlp_list -> v_add_token (lex_number, bp_get_backward (u_size + 1), u_size);
                        state = s_init;
                     }
                     else
                     {
                        o_dot_in_number = true;
                        u_size++;
                        b_pop ();
                     }
                     break;
                  case lex_digit :
                     u_size++;
                     b_pop ();
                     break;
                  default :
                     tlp_list -> v_add_token (lex_number, bp_get_backward (u_size + 1), u_size);
                     state = s_init;
                     break;
               }
               break;
         }
         if (lex_next == lex_null)
            state = s_end;
      }
   }

   /// Evaluate a XPath expression \n
   /// Decodes the lexical and syntax contents. 
   void v_evaluate ()
   {
      v_lexico_decode ();
      tlp_list -> v_syntax_decode ();
   }

   /// Callback used by token_syntax_decoder::v_syntax_decode to notify of an action to be made. Pure virtual
   virtual void v_action (xpath_construct , unsigned , unsigned , const char * ) = 0;

   /// Callback used by token_syntax_decoder::v_syntax_decode to know the action counter position. Pure virtual
   /// \n This can be any kind of nomenclature, provided that the redefinition is coherent
   virtual int i_get_action_counter () = 0;
} ;     // class xpath_stream

}

#endif
