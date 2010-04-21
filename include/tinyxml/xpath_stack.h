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
   \file xpath_stack.h
   \author Yves Berquin
*/

#ifndef __XPATH_STACK_H
#define __XPATH_STACK_H

#include "tinyxpath_conf.h"
#include "xpath_expression.h"
#include "xpath_stream.h"

namespace TinyXPath
{

class result_and_next;

/// The XPath execution stack
/// \n All elements are result_and_next elements
class xpath_stack 
{
public : 
   xpath_stack ();
   void v_set_root (const TiXmlNode * XNp_in_root)
   {
      XNp_root = XNp_in_root;
   }
   ~ xpath_stack ();
   void v_push (expression_result er_res);
   void v_push_int (int i_elem, const char * cp_comment = NULL);
   void v_push_bool (bool o_in);
   void v_push_string (TIXML_STRING S_in);
   void v_push_double (double d_elem);
   void v_push_node_set (node_set * nsp_ptr);

   expression_result * erp_top ();
   int i_top_int ();

   void v_pop (unsigned u_nb = 1);
   int i_pop_int ();
   TIXML_STRING S_pop_string ();
   bool o_pop_bool ();
   node_set ns_pop_node_set ();
   /// Return the stack's size
   unsigned u_get_size () {return u_size;}
   expression_result * erp_previous (unsigned u_nb);
   #ifdef TINYXPATH_DEBUG
      void v_dump ();
   #endif
protected :
   /// First element in the stack
   result_and_next * rnp_first;
   /// Stack size
   unsigned u_size;
   void v_pop_one ();
   const TiXmlNode * XNp_root;
} ;

}

#endif
