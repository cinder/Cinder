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
   \file xpath_stack.cpp
   \author Yves Berquin
*/

#include "xpath_stack.h"
#include "xpath_processor.h"

namespace TinyXPath
{

/// xpath_stack element. Derives from expression_result, and also contains a pointer to the next element on the stack
class result_and_next : public expression_result 
{
public :
   /// Constructor
   result_and_next (expression_result er_in, result_and_next * rnp_in_next)  :
      expression_result (er_in)
   {
      rnp_next = rnp_in_next;
   }
   /// Return next element
   result_and_next * rnp_get_next () 
   {
      return rnp_next;
   }
   #ifdef TINYXPATH_DEBUG
      void v_dump ();
   #endif
protected :
   /// Next element in the stack
   result_and_next * rnp_next;
} ;

#ifdef TINYXPATH_DEBUG
   /// Dump a stack element to stdout
   void result_and_next::v_dump ()
   {
      expression_result::v_dump ();
   }
#endif

/// Constructor
xpath_stack::xpath_stack () 
{
   rnp_first = NULL;
   u_size = 0;
}

/// Destructor
xpath_stack::~ xpath_stack () 
{
   while (rnp_first)
      v_pop_one ();
}

/// Pops the top integer from the stack
int xpath_stack::i_pop_int ()
{
   int i_res;
   
   i_res = erp_top () -> i_get_int ();
   v_pop ();
   return i_res;   
}

/// Pops the top string from the stack
TIXML_STRING xpath_stack::S_pop_string ()
{
   TIXML_STRING S_res;
   
   S_res = erp_top () -> S_get_string ();
   v_pop ();
   return S_res;   
}

/// Pops the top bool from the stack
bool xpath_stack::o_pop_bool ()
{
   bool o_res;

   o_res = erp_top () -> o_get_bool ();
   v_pop ();
   return o_res;
}

/// Pops the top node_set from the stack
node_set xpath_stack::ns_pop_node_set ()
{
   node_set ns_ret;

   ns_ret = * (erp_top () -> nsp_get_node_set ());
   v_pop ();
   return ns_ret;
}

/// Query the top integer
int xpath_stack::i_top_int ()
{
   int i_res;
   
   i_res = erp_top () -> i_get_int ();
   return i_res;   
}

/// Push a new element on the stack
void xpath_stack::v_push (expression_result er_res)
{
   result_and_next * rnp_new;

   rnp_new = new result_and_next (er_res, rnp_first);
   rnp_first = rnp_new;
   u_size++;
}

/// Push an integer on the stack
void xpath_stack::v_push_int (int i_elem, const char * cp_comment)
{
   expression_result er_res (XNp_root);
   er_res . v_set_int (i_elem);
   if (cp_comment)
      er_res . v_set_comment (cp_comment);
   v_push (er_res);
}

/// Push a double on the stack
void xpath_stack::v_push_double (double d_elem)
{
   expression_result er_res (XNp_root);
   er_res . v_set_double (d_elem);
   v_push (er_res);
}

/// Push a node_set on the stack
void xpath_stack::v_push_node_set (node_set * nsp_ptr)
{
   expression_result er_res (XNp_root);
   er_res . v_set_node_set (nsp_ptr);
   v_push (er_res);
}

/// Push a bool on the stack
void xpath_stack::v_push_bool (bool o_in)
{
   expression_result er_res (XNp_root);
   er_res . v_set_bool (o_in);
   v_push (er_res);
}

/// Push a string on the stack
void xpath_stack::v_push_string (TIXML_STRING S_in)
{
   expression_result er_res (XNp_root);
   er_res . v_set_string (S_in);
   v_push (er_res);
}

/// Retrieve top expression from the stack
expression_result * xpath_stack::erp_top ()
{
   if (! rnp_first)
      throw execution_error (34);
   return rnp_first;
}

/// Retrieve a previous expression from the stack
expression_result * xpath_stack::erp_previous (
   unsigned u_nb)   ///< Nb of items to skip.
{
   result_and_next * rnp_current;
   unsigned u_prev;

   assert (u_nb <= u_size);
   rnp_current = rnp_first;
   for (u_prev = 0; u_prev < u_nb; u_prev++)
   {
      assert (rnp_current);
      rnp_current = rnp_current -> rnp_get_next ();
   }
   return rnp_current;
}

/// Pop one element from the stack
void xpath_stack::v_pop_one ()
{
   result_and_next * rnp_old;

   if (! u_size)
      throw execution_error (35);
   if (! rnp_first)
      throw execution_error (36);
   rnp_old = rnp_first;
   rnp_first = rnp_first -> rnp_get_next ();
   delete rnp_old;
   u_size--;
}

/// Pop N element from the stack
void xpath_stack::v_pop (unsigned u_nb)
{
   unsigned u_elem;

   for (u_elem = 0; u_elem < u_nb; u_elem++)
      v_pop_one ();
}

#ifdef TINYXPATH_DEBUG
   /// Dumps the content of the stack to stdout
   void xpath_stack::v_dump ()
   {
      result_and_next * rnp_ptr;

      printf ("--- stack start (%d elem) ---\n", u_size);
      rnp_ptr = rnp_first;
      while (rnp_ptr)
      {
         rnp_ptr -> v_dump ();
         rnp_ptr = rnp_ptr -> rnp_get_next ();
      }
      printf ("--- stack end ---\n");
   }
#endif

}
