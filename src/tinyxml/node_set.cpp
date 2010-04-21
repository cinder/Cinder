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

#include "node_set.h"

namespace TinyXPath
{

/// Copy constructor
node_set::node_set (const node_set & ns2)
{  
   * this = ns2;
}

/// Assignation operator. Allows one to write expressions like ns_1 = ns_2;
node_set & node_set::operator = (const node_set & ns2)
{
   u_nb_node = ns2 . u_nb_node;
   if (u_nb_node)
   {
      vpp_node_set = new const void * [u_nb_node];
      memcpy (vpp_node_set, ns2 . vpp_node_set, u_nb_node * sizeof (void *));
      op_attrib = new bool [u_nb_node];
      memcpy (op_attrib, ns2 . op_attrib, u_nb_node * sizeof (bool));
   }
   else
   {
      vpp_node_set = NULL;
      op_attrib = NULL;
   }
   return * this;
}

/// Copy all element children of a node to the node_set
void node_set::v_copy_node_children (
   const TiXmlNode * XNp_root)   ///< The father of the nodes to be copied
{
   v_copy_node_children (XNp_root, NULL);
}

/// Copy all element children of a node to the node_set, if their name matches a given name
void node_set::v_copy_node_children (
   const TiXmlNode * XNp_root,   ///< The father of the nodes to be copied
   const char * cp_lookup)       ///< Lookup name (or NULL)
{
   const TiXmlNode * XNp_child;

   XNp_child = XNp_root -> FirstChildElement ();
   while (XNp_child)
   {
      if ((! cp_lookup) || ! strcmp (XNp_child -> Value (), cp_lookup))
         v_add_node_in_set (XNp_child);
      XNp_child = XNp_child -> NextSiblingElement ();
   }
}

/// Copy all nodes in the tree to the node_set
void node_set::v_copy_selected_node_recursive (
   const TiXmlNode * XNp_root)      ///< The node to be copied
{
   v_copy_selected_node_recursive (XNp_root, NULL);
}

/// Copy all nodes in the tree to the node_set, knowing that we are copying the root
void node_set::v_copy_selected_node_recursive_root_only (
   const TiXmlNode * XNp_root, const TiXmlNode * XNp_base)      
{
   v_add_node_in_set (XNp_root);
   v_copy_selected_node_recursive (XNp_base, NULL);
}

/// Copy all nodes in the tree to the node_set
void node_set::v_copy_selected_node_recursive (
   const TiXmlNode * XNp_root,      ///< The node to be copied
   const char * cp_lookup)          ///< Lookup name (or NULL)
{
   const TiXmlAttribute * XAp_attrib;
   const TiXmlNode * XNp_child;

   if ((! cp_lookup) || ! strcmp (XNp_root -> Value (), cp_lookup))
      v_add_node_in_set (XNp_root);
   if (XNp_root -> Type () == TiXmlNode::ELEMENT)
   {
      XAp_attrib = XNp_root -> ToElement () -> FirstAttribute ();      
      while (XAp_attrib)
      {  
         v_add_attrib_in_set (XAp_attrib);
         XAp_attrib = XAp_attrib -> Next ();
      }
   }
   XNp_child = XNp_root -> FirstChild ();
   while (XNp_child)
   {
      v_copy_selected_node_recursive (XNp_child, cp_lookup);
      XNp_child = XNp_child -> NextSiblingElement ();
   }
}

/// Copy all nodes in the tree to the node_set, excluding attributes
void node_set::v_copy_selected_node_recursive_no_attrib (
   const TiXmlNode * XNp_root,   ///< Node whole children are to be copied
   const char * cp_lookup)       ///< Lookup name or NULL
{
   const TiXmlElement * XEp_child;

   XEp_child = XNp_root -> FirstChildElement ();
   while (XEp_child)
   {
      if ((! cp_lookup) || ! strcmp (XEp_child -> Value (), cp_lookup))
         v_add_node_in_set (XEp_child);
      v_copy_selected_node_recursive_no_attrib (XEp_child, cp_lookup);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

/// Return the string value aka concatenation of all text items
TIXML_STRING node_set::S_get_string_value () const
{
   TIXML_STRING S_res;
   const TiXmlNode * XNp_node;
   unsigned u_node;

   S_res = "";
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      if (! op_attrib [u_node])
      {
         XNp_node = (const TiXmlNode *) vpp_node_set [u_node];
         if (XNp_node -> Type () == TiXmlNode::TEXT)
            S_res += XNp_node -> Value ();
      }
   }
   return S_res;
}

/// Checks if a node exist in the node set
bool node_set::o_exist_in_set (
   const TiXmlBase * XBp_member)    ///< Check if a base exist in the node set
{
   unsigned u_node;

   for (u_node = 0; u_node < u_nb_node; u_node++)
      if (vpp_node_set [u_node] == XBp_member)
         return true;
   return false;
}

/// Adds a new node in the node set
void node_set::v_add_base_in_set (
   const TiXmlBase * XBp_member,    ///< Base to add (node or attribute)
   bool o_attrib)                   ///< true if the base is an attribute, false if it's a node
{
   const void ** vpp_new_list;
   bool * op_new_list;

   if (o_exist_in_set (XBp_member))
      return;

/*      
   printf ("add ");
   if (o_attrib)
      printf ("attrib %s='%s'", ((TiXmlAttribute *) XBp_member)->Name(),((TiXmlAttribute *) XBp_member)->Value());
   else
   {
      TiXmlNode * XNp_node;
      XNp_node = (TiXmlNode *) XBp_member;
      switch (XNp_node->Type())
      {
         case TiXmlNode::TEXT :
            printf ("text (%s)", XNp_node->ToText()->Value ());
            break;
         case TiXmlNode::DOCUMENT :
            printf ("document");
            break;
         case TiXmlNode::ELEMENT :
            printf ("element <%s>", XNp_node->ToElement()->Value ());
            break;
         case TiXmlNode::COMMENT :
            printf ("comment <%s>", XNp_node->ToComment()->Value ());
            break;
      }
   }
   printf ("\n");
*/
      
   vpp_new_list = new const void * [u_nb_node + 1];
   op_new_list = new bool [u_nb_node + 1];
   if (u_nb_node)
   {
      memcpy (vpp_new_list, vpp_node_set, u_nb_node * sizeof (void *));
      delete [] vpp_node_set;
      memcpy (op_new_list, op_attrib, u_nb_node * sizeof (bool));
      delete [] op_attrib;
   }
   vpp_new_list [u_nb_node] = (const void *) XBp_member;
   vpp_node_set = vpp_new_list;
   op_new_list [u_nb_node] = o_attrib;
   op_attrib = op_new_list;
   u_nb_node++;
}

/// Populate the node set with all following nodes.
/// \n Exerpt : \n
/// the following axis contains all nodes in the same document as the context 
/// node that are after the context node in document order, excluding any 
/// descendants and excluding attribute nodes and namespace nodes
void node_set::v_add_all_foll_node (
   const TiXmlNode * XNp_node,   ///< base node
   const TIXML_STRING & S_name)  ///< lookup name (or "*")
{
   const TiXmlNode * XNp_ptr;
   const char * cp_lookup;

   if (S_name == "*")
      cp_lookup = NULL;
   else
      cp_lookup = S_name . c_str ();
   XNp_ptr = XNp_node -> NextSiblingElement ();
   while (XNp_ptr)
   {
      v_add_node_in_set_if_name_or_star (XNp_ptr, S_name);
      v_copy_node_children (XNp_ptr, cp_lookup);
      XNp_ptr = XNp_ptr -> NextSiblingElement ();
   }
   XNp_ptr = XNp_node -> Parent ();
   if (XNp_ptr && XNp_ptr -> Type () == TiXmlNode::ELEMENT)
      v_add_all_foll_node (XNp_ptr, S_name);
}

/// Populate the node set with all preceding nodes.
/// \n Exerpt : \n
/// the preceding axis contains all nodes in the same document as the context 
/// node that are before the context node in document order, excluding any 
/// ancestors and excluding attribute nodes and namespace nodes
void node_set::v_add_all_prec_node (
   const TiXmlNode * XNp_node,      ///< base node
   const TIXML_STRING & S_name)     ///< lookup name (or "*")
{
   const TiXmlNode * XNp_ptr;
   const char * cp_lookup;

   if (S_name == "*")
      cp_lookup = NULL;
   else
      cp_lookup = S_name . c_str ();
   XNp_ptr = XNp_node -> PreviousSibling ();
   while (XNp_ptr)
   {
      if (XNp_ptr -> Type () == TiXmlNode::ELEMENT)
      {
         v_add_node_in_set_if_name_or_star (XNp_ptr, S_name);
         v_copy_node_children (XNp_ptr, cp_lookup);
      }
      XNp_ptr = XNp_ptr -> PreviousSibling ();
   }
}

/// Internal utility class for the node set sorting
class ptr_2_and_flag 
{
public :
   const void * vp_node;
   const TiXmlNode * XNp_root;
   bool o_flag;
} ;

enum {e_not_found, e_lower, e_higher, e_same};

/// Find which node is first in tree, in document order (recursive calls)
static int i_compare_node_in_tree (const TiXmlNode * XNp_root, const TiXmlBase * XBp_1, const TiXmlBase * XBp_2)
{
   const TiXmlNode * XNp_child;
   const TiXmlAttribute * XAp_attrib;
   int i_res;
   
   if (! XNp_root || ! XBp_1 || ! XBp_2)
      return e_not_found;
   if (XNp_root == XBp_1)
      if (XNp_root == XBp_2)
         return e_same;
      else
         return e_lower;
   else
      if (XNp_root == XBp_2)
         return e_higher;
   if (XNp_root -> ToElement ())
   {
      // We have an element in the tree, let's see if one of the argument is an attribute of this element
      XAp_attrib = XNp_root -> ToElement () -> FirstAttribute ();   
      while (XAp_attrib)
      {
         if (XAp_attrib == XBp_1)
            if (XAp_attrib == XBp_2)
               return e_same;
            else
               return e_lower;
         else
            if (XAp_attrib == XBp_2)
               return e_higher;
         XAp_attrib = XAp_attrib -> Next ();
      }
   }
   XNp_child = XNp_root -> FirstChild ();
   while (XNp_child)
   {
      i_res = i_compare_node_in_tree (XNp_child, XBp_1, XBp_2);
      if (i_res != e_not_found)
         return i_res;
      XNp_child = XNp_child -> NextSibling ();
   }
   return e_not_found;
}

/// Internal utility function for node set sorting
static int i_compare_ptr_2_and_flag (
   const void * vp_1,   ///< Ptr to first element to compare
   const void * vp_2)   ///< Ptr to second element to compare
{
   const ptr_2_and_flag * p2afp_1, * p2afp_2;
   int i_res;

   p2afp_1 = (const ptr_2_and_flag *) vp_1;
   p2afp_2 = (const ptr_2_and_flag *) vp_2;
   i_res = i_compare_node_in_tree (p2afp_1 -> XNp_root, (const TiXmlBase *) p2afp_1 -> vp_node, (const TiXmlBase *) p2afp_1 -> vp_node);
   switch (i_res)
   {
      case e_lower :
         return -1;
      case e_higher : 
         return 1;
   }
   return 0;
}


/// Sort the node set according to the document order.
/// \n We do sort these nodes on the fly, comparing parents and orders for each
void node_set::v_document_sort (const TiXmlNode * XNp_root)
{
   ptr_2_and_flag * p2afp_list;
   unsigned u_node;

   if (u_nb_node < 2)
      return;

   p2afp_list = new ptr_2_and_flag [u_nb_node];
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      p2afp_list [u_node] . vp_node = vpp_node_set [u_node];
      p2afp_list [u_node] . o_flag = op_attrib [u_node];
      p2afp_list [u_node] . XNp_root = XNp_root;
   }
   qsort (p2afp_list, u_nb_node, sizeof (ptr_2_and_flag), i_compare_ptr_2_and_flag);
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      vpp_node_set [u_node] = p2afp_list [u_node] . vp_node;
      op_attrib [u_node] = p2afp_list [u_node] . o_flag;
   }
   delete [] p2afp_list;
}

/// Debug function to print the content of a node set to stdout
void node_set::v_dump ()
{
   unsigned u_node;
   const TiXmlAttribute * XAp_att;
   const TiXmlNode * XNp_node;

   printf ("-- start node set (%d items) --\n", u_nb_node);
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      if (op_attrib [u_node])
      {
         XAp_att = XAp_get_attribute_in_set (u_node);
         printf ("   [%d] : Attribute : %s=%s\n", u_node, XAp_att -> Name (), XAp_att -> Value ());
      }
      else
      {
         XNp_node = XNp_get_node_in_set (u_node);
         printf ("   [%d] : Node : %s\n", u_node, XNp_node -> Value ());
      }
   }
   printf ("-- end node set --\n");
}

}
