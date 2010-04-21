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

#ifndef __NODE_SET_H
#define __NODE_SET_H

#include "tinyxml.h"
#include "tinyxpath_conf.h"

namespace TinyXPath
{

/// Node set class. A node set is an unordered collection of node
class node_set
{
public :
   /// constructor : creates an empty set
   node_set () 
   {
      u_nb_node = 0; 
      vpp_node_set = NULL;
      op_attrib = NULL;
   }
   /// copy constructor
   node_set (const node_set & ns2);
   /// destructor
   ~ node_set ()
   {
      if (u_nb_node && vpp_node_set) 
         delete [] vpp_node_set;
      if (u_nb_node && op_attrib)
         delete [] op_attrib;
      u_nb_node = 0; 
      vpp_node_set = NULL;
      op_attrib = NULL;
   }

   node_set & operator = (const node_set & ns2);
   void v_add_base_in_set (const TiXmlBase * XBp_member, bool o_attrib);

   /// Adds an attribute in the node set
   void v_add_attrib_in_set (const TiXmlAttribute * XAp_attrib)
   {
      v_add_base_in_set (XAp_attrib, true);
   }

   /// Adds a node in the node set
   void v_add_node_in_set (const TiXmlNode * XNp_node)
   {
      v_add_base_in_set (XNp_node, false);
   }

   bool o_exist_in_set (const TiXmlBase * XBp_member);
   void v_add_all_foll_node (const TiXmlNode * XNp_node, const TIXML_STRING & S_name);
   void v_add_all_prec_node (const TiXmlNode * XNp_node, const TIXML_STRING & S_name);
   
   /// Add a new node, if the name is "*" or if the name is the same as the node
   void v_add_node_in_set_if_name_or_star (const TiXmlNode * XNp_node, const TIXML_STRING & S_name)
   {
      bool o_keep;
      if (S_name == "*")
         o_keep = true;
      else
         o_keep = ! strcmp (XNp_node -> Value (), S_name . c_str ());
      if (o_keep)
         v_add_base_in_set (XNp_node, false);
   }

   /// Add a new attrib, if the name is "*" or if the name is the same as the node
   void v_add_attrib_in_set_if_name_or_star (const TiXmlAttribute * XAp_attrib, const TIXML_STRING & S_name)
   {
      bool o_keep;
      if (S_name == "*")
         o_keep = true;
      else
         o_keep = ! strcmp (XAp_attrib -> Name (), S_name . c_str ());
      if (o_keep)
         v_add_base_in_set (XAp_attrib, true);
   }

   /// Get nb of nodes in the node set
   unsigned u_get_nb_node_in_set () const 
   {
      return u_nb_node;
   }

   /// Get a node or an attribute
   const TiXmlBase * XBp_get_base_in_set (unsigned u_which) 
   {
      assert (u_which < u_nb_node);
      return (const TiXmlBase *) vpp_node_set [u_which];
   }

   /// Get a node
   const TiXmlNode * XNp_get_node_in_set (unsigned u_which) 
   {
      assert (u_which < u_nb_node);
      assert (! o_is_attrib (u_which));
      return (const TiXmlNode *) vpp_node_set [u_which];
   }

   /// Get an attribute
   const TiXmlAttribute * XAp_get_attribute_in_set (unsigned u_which) 
   {
      assert (u_which < u_nb_node);
      assert (o_is_attrib (u_which));
      return (const TiXmlAttribute *) vpp_node_set [u_which];
   }

   /// Check if a node is an attribute or another node. This is needed because TinyXML has a weird exception for
   /// attributes not being children of TiXmlNode
   bool o_is_attrib (unsigned u_which)
   {
      assert (u_which < u_nb_node);
      return op_attrib [u_which];
   }

   /// Get a node value. The value is the name for an element, and the attribute value for an attribute
   TIXML_STRING S_get_value (unsigned u_which)
   {
      TIXML_STRING S_res;

      if (o_is_attrib (u_which))
         S_res = XAp_get_attribute_in_set (u_which) -> Value ();
      else
         S_res = XNp_get_node_in_set (u_which) -> Value ();
      return S_res;
   }

   /// Get the integer value of a node
   int i_get_value (unsigned u_which)
   {
      return atoi (S_get_value (u_which) . c_str ());
   }

   /// Get the real value of a node
   double d_get_value (unsigned u_which)
   {
      return atof (S_get_value (u_which) . c_str ());
   }

   void v_copy_node_children (const TiXmlNode * XNp_root);
   void v_copy_node_children (const TiXmlNode * XNp_root, const char * cp_lookup);
   void v_copy_selected_node_recursive (const TiXmlNode * XNp_root);
   void v_copy_selected_node_recursive (const TiXmlNode * XNp_root, const char * cp_lookup);
   void v_copy_selected_node_recursive_no_attrib (const TiXmlNode * XNp_root, const char * cp_lookup);
   void v_copy_selected_node_recursive_root_only (const TiXmlNode * XNp_root, const TiXmlNode * XNp_base);
   TIXML_STRING S_get_string_value () const;
   void v_dump ();
   void v_document_sort (const TiXmlNode * XNp_root);
protected :
   /// Nb of nodes in the set
   unsigned u_nb_node;
   /// List of node pointers to the 
	const void ** vpp_node_set;
   /// Attributes flag list
   bool * op_attrib;
} ;

}

#endif
