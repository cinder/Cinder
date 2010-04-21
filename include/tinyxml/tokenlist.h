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
   \file tokenlist.h
   \author Yves Berquin
   XPath Syntax analyzer for TinyXPath project : token list handling
*/

#ifndef __TOKENLIST_H
#define __TOKENLIST_H

#include "lex_util.h"
#include "lex_token.h"

namespace TinyXPath
{

/**
   A token list class. This is the output of the lexical analysis  \n
   Note that the empty list consist of a single null element in order to speed
   up later insertions
*/
class token_list 
{
protected :
   /// Pointer to first element
   lex_token * ltp_first;
   /// Pointer to last element
   lex_token * ltp_last;
   /// Pointer to current element. This is for external usage : we keep track of where it
   /// is, but it's not needed to manage the list structure
   lex_token * ltp_current;
public :
   /// constructor
   token_list () 
   {
      ltp_first = new lex_token (lex_null, NULL, 0);
      ltp_last = ltp_first;
      ltp_first -> v_set_prev (ltp_first);
      ltp_first -> v_set_next (ltp_first);
      ltp_current = NULL;
   }
   /// destructor
   virtual ~ token_list ()
   {
      ltp_current = ltp_first -> ltp_get_next ();
      while (ltp_current -> o_is_valid ())
         v_delete_current ();
      delete ltp_first;
   }
   /// Adds a lexical token
   void v_add_token (lexico l_in_enum, const _byte_ * bp_in_value, unsigned u_in_size)
   {
      lex_token * ltp_new;

      ltp_new = new lex_token (l_in_enum, bp_in_value, u_in_size);
      ltp_last    -> v_set_next (ltp_new);
      ltp_new     -> v_set_next (ltp_first);
      ltp_first   -> v_set_prev (ltp_new);
      ltp_new     -> v_set_prev (ltp_last);
      ltp_last = ltp_new;
   }

   /// Set current to first real element
   void v_set_current_top ()
   {
      ltp_current = ltp_first -> ltp_get_next (1);
   }

   /// Set current 
   void v_set_current (lex_token * ltp_cur)
   {
      ltp_current = ltp_cur;
   }

   /// Return the current token
   lex_token * ltp_freeze ()
   {
      return ltp_current;
   }

   /// Get next X linear token
   lex_token * ltp_get (int i_offset) 
   {
      if (! ltp_current)
         return NULL;
      return ltp_current -> ltp_get_next (i_offset);
   }

   /// Increments the linear counter
   void v_inc_current (int i_rel)
   {
      if (! ltp_current)
         return;
      ltp_current = ltp_current -> ltp_get_next (i_rel);
   }

   /// Replaces the current element 
   void v_replace_current (lexico lex_in, const char * cp_rep)
   {
      if (! ltp_current)
         return;
      ltp_current -> v_set (lex_in, cp_rep);
   }

   /// Deletes the current element
   void v_delete_current ()
   {
      lex_token * ltp_temp;

      assert (ltp_current);
      ltp_temp = ltp_current;
      ltp_temp -> ltp_get_prev () -> v_set_next (ltp_temp -> ltp_get_next ());
      ltp_temp -> ltp_get_next () -> v_set_prev (ltp_temp -> ltp_get_prev ());
      ltp_current = ltp_temp -> ltp_get_next ();
      delete ltp_temp;
   }

   /// Deletes the next element
   void v_delete_next ()
   {
      lex_token * ltp_temp;

      assert (ltp_current);
      ltp_temp = ltp_current -> ltp_get_next ();
      ltp_current -> v_set_next (ltp_temp -> ltp_get_next ());
      ltp_temp -> ltp_get_next () -> v_set_prev (ltp_current);
      delete ltp_temp;
   }
   void v_tokenize_expression ();
} ;

}

#endif
