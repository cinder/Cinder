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

#include "xpath_expression.h"
#include "lex_util.h"

namespace TinyXPath
{

/// Get the expression_result as an int
int expression_result::i_get_int ()
{
   switch (e_type)
   {
      case e_int :
         return i_content;
      case e_bool :
         return o_content ? 1 : 0;
      case e_double :
         return (int) (d_content);
      default :
         return atoi (S_get_string () . c_str ());
   }
}	

/// Get the expression_result as a double
double expression_result::d_get_double ()
{
	switch (e_type)
	{
		case e_double :
			return d_content;
		case e_int :
			return (double) i_content;
      case e_bool :
         return o_content ? 1.0 : 0.0;
      default :
         return atof (S_get_string () . c_str ());
	}
}

/// Get the expression_result as a string
TIXML_STRING expression_result::S_get_string ()
{
   TIXML_STRING S_res;
   node_set * nsp_ptr;

   S_res = "";
   switch (e_type)
   {
      case e_string :
         S_res = S_content;
         break;
      case e_int :
         v_assign_int_to_string (S_res, i_get_int ());
         break;
      case e_double :
         v_assign_double_to_string (S_res, d_get_double ());
         break;
      case e_node_set :
         // See XPath 1.0 spec, 4.2 :
         // An argument is converted to type string as if by calling the string function
         // ...
         // A node-set is converted to a string by returning the string-value of the node 
         // in the node-set that is first in document order. If the node-set is empty, an empty string is returned.
         nsp_ptr = nsp_get_node_set ();
         if (nsp_ptr -> u_get_nb_node_in_set ())
         {
            nsp_ptr -> v_document_sort (XNp_root);
            if (nsp_ptr -> o_is_attrib (0))
               S_res = nsp_ptr -> XAp_get_attribute_in_set (0) -> Value ();
            else
               S_res = nsp_ptr -> XNp_get_node_in_set (0) -> Value ();
         }
         break;
      case e_bool :  
         if (o_get_bool ())
            S_res = "true";
         else
            S_res = "false";
         break;
   }
   return S_res;
}	

/**
Get the expression_result as a bool\n
The boolean function converts its argument to a boolean as follows:
- a number is true if and only if it is neither positive or negative zero nor NaN
- a node-set is true if and only if it is non-empty
- a string is true if and only if its length is non-zero
- an object of a type other than the four basic types is converted to a boolean in a way that is dependent on that type
*/
bool expression_result::o_get_bool ()
{
   node_set * nsp_ptr;
   switch (e_type)
   {
      case e_int :
         return i_content != 0;
      case e_double :
         return (d_get_double () == 0.0);
      case e_string :
         return S_content . length () > 0;
         break;
      case e_node_set :
         // See XPath 1.0 spec, 3.2 :
         // An argument is converted to type string as if by calling the string function
         // ...
         // A node-set is converted to a string by returning the string-value of the node 
         // in the node-set that is first in document order. If the node-set is empty, an empty string is returned.
         nsp_ptr = nsp_get_node_set ();
         return nsp_ptr -> u_get_nb_node_in_set () != 0;
      case e_bool :  
         return o_content;
   }
	return false;
}	

#ifdef TINYXPATH_DEBUG

   /// Debug function to print an expression_result to stdout
   void expression_result::v_dump ()
   {
      switch (e_type)
      {
         case e_bool :
            printf ("   bool : %s\n", o_content ? "true" : "false");
            break;
         case e_string :
            printf ("   string : %s\n", S_content . c_str ());
            break;
         case e_int : 
            printf ("   int : %d", i_content);
            if (S_comment . length ())
               printf (" (%s)", S_comment . c_str ());
            printf ("\n");
            break;
         case e_double : 
            printf ("   double : %f\n", d_content);
            break;
         case e_node_set :
            printf ("  node set\n");
            ns_set . v_dump ();
            break;
         case e_invalid :
            printf ("  (invalid)\n");
            break;
      }
   }
#endif

}
