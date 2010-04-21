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
   \file tinyutil.cpp
   \author Yves Berquin
   Miscellaneous utilities for the TinyXPath project
*/

#include <string.h>
#include <stdio.h>

#include "lex_util.h"

namespace TinyXPath
{

/// Mapping of all the byte values into elementary lexical items
static lexico lex_char_map [256] =
 {
   /*                      0            1            2            3            4            5            6            7   */
   /*                       8            9            a            b            c            d            e            f  */
   /* 00 .. 07 */    lex_null,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* 08 .. 0f */    lex_none,   lex_space,   lex_space,    lex_none,    lex_none,   lex_space,    lex_none,    lex_none, 
   /* 10 .. 17 */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* 18 .. 1f */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* 20 .. 27 */   lex_space,  lex_exclam, lex_2_quote,    lex_none,  lex_dollar,    lex_none,    lex_none, lex_1_quote, 
   /* 28 .. 2f */  lex_oparen,  lex_cparen,    lex_star,    lex_plus,   lex_comma,   lex_minus,     lex_dot,   lex_slash, 
   /* 30 .. 37 */   lex_digit,   lex_digit,   lex_digit,   lex_digit,   lex_digit,   lex_digit,   lex_digit,   lex_digit, 
   /* 38 .. 3f */   lex_digit,   lex_digit,   lex_colon,  lex_scolon,      lex_lt,   lex_equal,      lex_gt,    lex_none, 
   /* 40 .. 47 */      lex_at,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* 48 .. 4f */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* 50 .. 57 */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* 58 .. 5f */   lex_bchar,   lex_bchar,   lex_bchar,  lex_obrack,    lex_none,  lex_cbrack,    lex_none,   lex_under, 
   /* 60 .. 67 */    lex_none,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* 68 .. 6f */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* 70 .. 77 */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* 78 .. 7f */   lex_bchar,   lex_bchar,   lex_bchar,    lex_none,  lex_orchar,    lex_none,    lex_none,    lex_none, 
   /* 80 .. 87 */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* 88 .. 8f */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* 90 .. 97 */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* 98 .. 9f */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* a0 .. a7 */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* a8 .. af */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* b0 .. b7 */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,  lex_extend, 
   /* b8 .. bf */    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none,    lex_none, 
   /* c0 .. c7 */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* c8 .. cf */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* d0 .. d7 */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,    lex_none, 
   /* d8 .. df */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* e0 .. e7 */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* e8 .. ef */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar, 
   /* f0 .. f7 */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,    lex_none, 
   /* f8 .. ff */   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar,   lex_bchar
 };

/// Dump a lexical element
const char * cp_disp_class_lex (lexico lex_in)
{
   switch (lex_in)
   {
      case lex_none :                     return "<small>none</small>";
      case lex_null :                     return "null";
      case lex_digit :                    return "digit";
      case lex_bchar :                    return "base char";
      case lex_space :                    return "space";
      case lex_extend :                   return "extender";
      case lex_slash :                    return "slash";
      case lex_at :                       return "at";
      case lex_dot :                      return "dot";
      case lex_minus :                    return "minus";
      case lex_under :                    return "under";
      case lex_colon :                    return "colon";
      case lex_scolon :                   return "semi colon";
      case lex_2_quote :                  return "double quote";
      case lex_1_quote :                  return "simple quote";
      case lex_dollar :                   return "dollar";
      case lex_oparen :                   return "opening parenthesis";
      case lex_cparen :                   return "closing parenthesis";
      case lex_star :                     return "star";
      case lex_plus :                     return "plus";
      case lex_comma :                    return "comma";
      case lex_lt :                       return "less than";
      case lex_equal :                    return "equal";
      case lex_gt :                       return "greater than";
      case lex_obrack :                   return "opening bracket";
      case lex_cbrack :                   return "closing bracket";
      case lex_orchar :                   return "or character (|)";
      case lex_exclam :                   return "exclamation (!)";
      case lex_2_colon :                  return "::";
      case lex_2_slash :                  return "//";
      case lex_2_dot :                    return "..";
      case lex_not_equal :                return "!=";
      case lex_lt_equal :                 return "<=";
      case lex_gt_equal :                 return ">=";

      case lex_ncname :                   return "NCName";
      case lex_number :                   return "Number";
      case lex_literal :                  return "literal";
      case lex_or :                       return "or"; 
      case lex_and :                      return "and"; 
      case lex_mod :                      return "mod"; 
      case lex_div :                      return "div";
      case lex_ancestor :                 return "ancestor"; 
      case lex_ancestor_or_self :         return "ancestor-or-self"; 
      case lex_attribute :                return "attribute"; 
      case lex_child :                    return "child"; 
      case lex_descendant :               return "descendant"; 
      case lex_descendant_or_self :       return "descendant-or-self"; 
      case lex_following :                return "following"; 
      case lex_following_sibling :        return "following-sibling"; 
      case lex_namespace :                return "namespace"; 
      case lex_parent :                   return "parent"; 
      case lex_preceding :                return "preceding"; 
      case lex_preceding_sibling :        return "preceding-sibling"; 
      case lex_self :                     return "self"; 
      case lex_processing_instruction :   return "processing-instruction";
      case lex_comment :                  return "comment";
      case lex_node :                     return "node";
      case lex_text :                     return "text";

   }
   return "???";
}

/// Get the lexical class of an XPath expression byte
lexico lex_get_class (_byte_ b_in)
{
   return lex_char_map [b_in];
}

/// Check if a lexical element can be an axis name
bool o_is_axis_name (lexico lex_test)
{
   if (lex_test >= lex_start_axis_name && lex_test <= lex_end_axis_name)
      return true;
   return false;
}

/// Check if an ID maps an existing keyword
/// \n Returns the new lexical element or lex_ncname if not found
lexico lex_test_id (const _byte_ * bp_str, unsigned u_size, lexico lex_next)
{
   char * cp_equi;
   unsigned u_lex;

   cp_equi = new char [u_size + 1];
   memcpy (cp_equi, bp_str, u_size);
   cp_equi [u_size] = 0;
   for (u_lex = lex_start_keyword; u_lex <= lex_end_keyword; u_lex++)
      if (! strcmp (cp_equi, cp_disp_class_lex (lexico (u_lex))))
         // DO not recognize the text keyword unless it is followed by an open parenthesis
         if ((u_lex != lex_text) || (lex_next == lex_oparen))
         {
            delete [] cp_equi;
            return lexico (u_lex);
         }
   delete [] cp_equi;
   return lex_ncname;
}

/// Removes all leading and trailing white spaces
TIXML_STRING S_remove_lead_trail (const char * cp_in)
{	
	TIXML_STRING S_ret;
	const char * cp_start, * cp_end;
	char * cp_new, * cp_out;

   // result can't be longer. we assign same size
	cp_new = new char [strlen (cp_in) + 1];
   cp_out = cp_new;
	cp_start = cp_in;
	while (* cp_start == ' ' || * cp_start == '\t')
		cp_start++;
	cp_end = cp_in + strlen (cp_in) - 1;
	while (cp_end >= cp_in && (* cp_end == ' ' || * cp_end == '\t'))
		cp_end--;
   while (cp_start <= cp_end)
   {
      if (* cp_start == ' ' || * cp_start == '\t')
      {
         * cp_out ++ = ' ';
         cp_start++;
         while (cp_start <= cp_end && (* cp_start == ' ' || * cp_start == '\t'))
            cp_start++;
      }
      else
         * cp_out++ = * cp_start++;
   }
   * cp_out = 0;
	S_ret = cp_new;
	delete [] cp_new;
	return S_ret;
}

/// Assign an integer to a string
void v_assign_int_to_string (TIXML_STRING & S_string, int i_val)
{
   char ca_int [80];

   sprintf (ca_int, "%d", i_val);
   S_string = ca_int;
}

/// Assign a double to a string, cleaning any trailing zeroes and the decimal point if there's no
/// decimal part
void v_assign_double_to_string (TIXML_STRING & S_string, double d_val)
{
   char ca_int [80];

   sprintf (ca_int, "%f", d_val);
   while (ca_int [strlen (ca_int) - 1] == '0')
      ca_int [strlen (ca_int) - 1] = 0;
   if (ca_int [strlen (ca_int) - 1] == '.')
      ca_int [strlen (ca_int) - 1] = 0;
   S_string = ca_int;
}

#ifdef TINYXPATH_DEBUG
   /// Return the name of an xpath_construct enumerated
   const char * cp_disp_construct (xpath_construct xc)
   {
      switch (xc)
      {
         case xpath_unknown : return "xpath_unknown"; 
         case xpath_location_path : return "xpath_location_path";
         case xpath_absolute_location_path : return "xpath_absolute_location_path";
         case xpath_relative_location_path : return "xpath_relative_location_path";
         case xpath_step : return "xpath_step";
         case xpath_axis_specifier : return "xpath_axis_specifier";
         case xpath_axis_name : return "xpath_axis_name";
         case xpath_node_test : return "xpath_node_test";
         case xpath_predicate : return "xpath_predicate";
         case xpath_predicate_expr : return "xpath_predicate_expr";
         case xpath_abbreviated_absolute_location_path : return "xpath_abbreviated_absolute_location_path";
         case xpath_abbrieviated_step : return "xpath_abbrieviated_step";
         case xpath_abbreviated_axis_specifier : return "xpath_abbreviated_axis_specifier";
         case xpath_expr : return "xpath_expr";
         case xpath_primary_expr : return "xpath_primary_expr";
         case xpath_function_call : return "xpath_function_call";
         case xpath_argument : return "xpath_argument";
         case xpath_union_expr : return "xpath_union_expr";
         case xpath_path_expr : return "xpath_path_expr";
         case xpath_filter_expr : return "xpath_filter_expr";
         case xpath_or_expr : return "xpath_or_expr";
         case xpath_or_expr_more : return "xpath_or_expr_more";
         case xpath_and_expr : return "xpath_and_expr";
         case xpath_equality_expr : return "xpath_equality_expr";
         case xpath_relational_expr : return "xpath_relational_expr";
         case xpath_additive_expr : return "xpath_additive_expr";
         case xpath_multiplicative_expr : return "xpath_multiplicative_expr";
         case xpath_unary_expr : return "xpath_unary_expr";
         case xpath_multiply_operator : return "xpath_multiply_operator";
         case xpath_variable_reference : return "xpath_variable_reference";
         case xpath_name_test : return "xpath_name_test";
         case xpath_xml_q_name : return "xpath_xml_q_name";
         case xpath_xml_prefix : return "xpath_xml_prefix";
         case xpath_xml_local_part : return "xpath_xml_local_part";
      }
      return "";
   }

   /// Generates an ascii table summarizing all possible bytes and their XPath properties
   void v_generate_ascii_htm ()
   {
      int c;
      FILE * Fp_out;
      Fp_out = fopen ("ascii.htm", "wt");
      fprintf (Fp_out, "<html><head><title>ASCII</title></head><body>\n");
      fprintf (Fp_out, "<table border=1><tr><th>dec</th><th>hex</th><th>char</th><th>class</th></tr>\n");
      for (c = 0; c < 256; c++)
         fprintf (Fp_out, "<tr><td>%3d</td><td>0x%02x</td><td>&#x%02x;</td><td>%s</td></tr>\n", c, c, c, cp_disp_class ((_byte_) c));
      fprintf (Fp_out, "</table>\n");
      fclose (Fp_out);
   }

   /// Display the lexical class of an XPath expression byte
   const char * cp_disp_class (_byte_ b_in)
   {
      return cp_disp_class_lex (lex_get_class (b_in));
   }


#endif

}
