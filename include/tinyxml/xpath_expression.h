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

#ifndef __EXPR_H
#define __EXPR_H

#include "tinyxpath_conf.h"
#include "tinyxml.h"
#include "node_set.h"

namespace TinyXPath
{

/// Expression types
typedef enum {e_bool, e_string, e_int, e_double, e_node_set, e_invalid} e_expression_type;

/// Class holding the result of an expression (e_expression_type)
class expression_result
{
protected :	
   /// String content
	TIXML_STRING S_content;
   #ifdef TINYXPATH_DEBUG
      /// Comment. This is for debuging only, for stack dump 
	   TIXML_STRING S_comment;
   #endif
   /// bool content
	bool o_content;
   /// integer content
	int i_content;
   /// double content
	double d_content;
   /// node set content
   node_set ns_set;
   /// Ptr to the root node
   const TiXmlNode * XNp_root;

public :
   /// expression type
	e_expression_type e_type;
   /// Dummy constructor
 	expression_result (const TiXmlNode * XNp_in_root) : XNp_root (XNp_in_root)
	{
		e_type = e_invalid;
      o_content = false;
      i_content = 0;
      d_content = 0.0;
	}

   expression_result ()
	{
	   XNp_root = NULL;
		e_type = e_invalid;
      o_content = false;
      i_content = 0;
      d_content = 0.0;
	}
	
	void v_set_root (const TiXmlNode * XNp_in) {XNp_root = XNp_in;}

   /// Copy constructor
	expression_result (const expression_result & er_2)
	{
      * this = er_2;
	}

   expression_result & operator = (const expression_result & er_2)
   {
      XNp_root = er_2 . XNp_root;
		e_type = er_2 . e_type;
      switch (e_type)
      {
         case e_bool :
            o_content = er_2 . o_content;
            break;
         case e_int :
            i_content = er_2 . i_content;
            break;
         case e_string :
            S_content = er_2 . S_content;
            break;
         case e_double :
            d_content = er_2 . d_content;
            break;
         case e_node_set :
            ns_set = er_2 . ns_set;
            break;
      }
      #ifdef TINYXPATH_DEBUG
         S_comment = er_2 . S_comment;
      #endif
      return * this;
   }
   /// Set expression_result to a bool
	void v_set_bool (bool o_in)
	{
		e_type = e_bool;
		o_content = o_in;
	}
   /// Set expression_result to an int
	void v_set_int (int i_in)
	{
		e_type = e_int;
		i_content = i_in;
	}
   /// Set expression_result to a string
	void v_set_string (const char * cp_in)
	{
		e_type = e_string;
		S_content = cp_in;
	}
   /// Set expression_result to a string
	void v_set_string (TIXML_STRING S_in)
	{
		e_type = e_string;
		S_content = S_in;
	}
   /// Set expression_result to a double
	void v_set_double (double d_in)
	{
		e_type = e_double;
		d_content = d_in;
	}
   /// Set the comment associated with a stack element. This is for debuging
	void v_set_comment (const char * cp_in)
	{
      #ifdef TINYXPATH_DEBUG
		   S_comment = cp_in;
      #endif
	}
	int i_get_int ();
	TIXML_STRING S_get_string ();
   /// Get the expression_result as a string
	const char * cp_get_string ()
	{
		assert (e_type == e_string);
		return S_content . c_str ();
	}	
	bool o_get_bool ();
	double d_get_double ();
   /// Set the expression_result as a node set
	void v_set_node_set (node_set * nsp_source)
	{
		e_type = e_node_set;
      ns_set = * nsp_source;
	}
   /// Set the expression_result as a node set
	void v_set_node_set (TiXmlNode * XNp_root)
	{
		e_type = e_node_set;
      ns_set . v_copy_node_children (XNp_root);
	}
   /// Set the expression_result as a node set
	void v_set_node_set (TiXmlNode * XNp_root, const char * cp_lookup)
	{
		e_type = e_node_set;
      ns_set . v_copy_node_children (XNp_root, cp_lookup);
	}
   /// Set the expression_result as a node set
	void v_set_node_set_recursive (TiXmlNode * XNp_root)
	{
		e_type = e_node_set;
      ns_set . v_copy_selected_node_recursive (XNp_root);
	}
   /// Set the expression_result as a node set
	void v_set_node_set_recursive (TiXmlNode * XNp_root, const char * cp_lookup)
	{
		e_type = e_node_set;
      ns_set . v_copy_selected_node_recursive (XNp_root, cp_lookup);
	}
   /// Set the expression_result as an empty node set
	void v_set_node_set ()
   {
      e_type = e_node_set;
   }
   /// Get the expression_result as a node set
   node_set * nsp_get_node_set ()
   {
      return & ns_set;
   }
   #ifdef TINYXPATH_DEBUG
      void v_dump ();
   #endif
} ;

}

#endif
