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

/*
 @history:
 
 Modified on  16 December 2006 by  Aman Aggarwal
 ::Added support for Expressions like ( Expr or Expr or Expr) 
 Modified on 18 December 2006 by Aman Aggarwal
 ::Added support for translate()  
*/
#include <math.h>
#include "xpath_processor.h"
#include "xml_util.h"

using namespace TinyXPath;

#ifdef TINYXPATH_DEBUG
   // only define DUMP_ACTION if TINYXPATH_DEBUG is defined
   #define DUMP_ACTION
#endif

/// xpath_processor constructor
xpath_processor::xpath_processor (
   const TiXmlNode * XNp_source_tree,  ///< Source XML tree
   const char * cp_xpath_expr)         ///< XPath expression
      : xpath_stream (cp_xpath_expr)
{
   if (XNp_source_tree && cp_xpath_expr)
      XNp_base = XNp_source_tree;
   else
      XNp_base = NULL;
   er_result . v_set_root (XNp_base);
   xs_stack . v_set_root (XNp_base);
   XEp_context = NULL;
   o_is_context_by_name = false;
   XNp_base_parent = NULL;
}

/// Compute an XPath expression, and return the number of nodes in the resulting node set.
/// \n Returns 0 if the result is not a node set
unsigned xpath_processor::u_compute_xpath_node_set ()
{
   er_compute_xpath ();
   if (er_result . e_type != e_node_set)
      return 0;
   return er_result . nsp_get_node_set () -> u_get_nb_node_in_set ();
}

/// Get one of the XML nodes from the resulting node set. Can only be used after a call to u_compute_xpath_node_set
void xpath_processor::v_get_xpath_base (
   unsigned u_order,             ///< Order of the node. Must be between 0 and the number of nodes - 1
   const TiXmlBase * & XBp_res,  ///< Output node
   bool & o_attrib)              ///< True if the output node is an attribute, false if it's a TiXmlNode
{
   XBp_res = NULL;
   o_attrib = false;
   if (er_result . e_type != e_node_set)
      return;
   if (u_order >= er_result . nsp_get_node_set () -> u_get_nb_node_in_set ())
      return;
   XBp_res = er_result . nsp_get_node_set () -> XBp_get_base_in_set (u_order);
   o_attrib = er_result . nsp_get_node_set () -> o_is_attrib (u_order);
}

/// Retrieves an XPath node from the node set. This assumes you know it's not an attribute
TiXmlNode * xpath_processor::XNp_get_xpath_node (
   unsigned u_order)    ///< Order of the node. Must be between 0 and the number of nodes - 1
{
   bool o_attrib;
   const TiXmlBase * XBp_res;

   o_attrib = false;
   XBp_res = NULL;
   v_get_xpath_base (u_order, XBp_res, o_attrib);
   if (o_attrib)
      return NULL;
   return (TiXmlNode *) XBp_res;
}

/// Retrieves an XPath attribute from the node set. This assumes you know it's an attribute
TiXmlAttribute * xpath_processor::XAp_get_xpath_attribute (
   unsigned u_order)    ///< Order of the node. Must be between 0 and the number of nodes - 1
{
   bool o_attrib;
   const TiXmlBase * XBp_res;

   o_attrib = false;
   XBp_res = NULL;
   v_get_xpath_base (u_order, XBp_res, o_attrib);
   if (! o_attrib)
      return NULL;
   return (TiXmlAttribute *) XBp_res;
}

void xpath_processor::v_build_root ()
{
   if (XNp_base)
   {
      XNp_base_parent = XNp_base -> Parent ();
      if (! XNp_base_parent)
         // no correct initialization of the xpath_processor object
         throw execution_error (1);
      // set the main node as the context one, if it's an element
      if (XNp_base -> ToElement ())
         XEp_context = XNp_base -> ToElement ();
   }
   else
      XNp_base_parent = NULL;
}

/// Compute an XPath expression 
expression_result xpath_processor::er_compute_xpath ()
{
   try
   {
      XNp_base_parent = XNp_base -> Parent ();
      if (! XNp_base_parent)
         // no correct initialization of the xpath_processor object
         throw execution_error (1);
      // set the main node as the context one, if it's an element
      if (XNp_base -> ToElement ())
         XEp_context = XNp_base -> ToElement ();

      // Decode XPath expression
      v_evaluate ();

      // Compute result
      v_execute_stack ();

      /// The executions stack need to contain 1 and only 1 element, otherwize it's not valid
      if (xs_stack . u_get_size () == 1)
      {
         er_result = * xs_stack . erp_top ();
         xs_stack . v_pop ();
         e_error = e_no_error;
      }
      else
      {
         expression_result er_null (NULL);
         er_result = er_null;
         e_error = e_error_stack;
      }
   }
   catch (syntax_error)
   {
      expression_result er_null (NULL);
      er_result = er_null;
      e_error = e_error_syntax;
   }
   catch (syntax_overflow)
   {
      expression_result er_null (NULL);
      er_result = er_null;
      e_error = e_error_overflow;
   }
   catch (execution_error)
   {
      expression_result er_null (NULL);
      er_result = er_null;
      e_error = e_error_execution;
   }

   return er_result;
}

/// Compute an XPath expression and return the result as a string
TIXML_STRING xpath_processor::S_compute_xpath ()
{
   expression_result er_res (XNp_base);
   TIXML_STRING S_res;

   er_res = er_compute_xpath ();
   S_res = er_res . S_get_string ();   
   return S_res;
}

/// Compute an XPath expression and return the result as an integer
int xpath_processor::i_compute_xpath ()
{
   expression_result er_res (XNp_base);
   int i_res;

   er_res = er_compute_xpath ();
   i_res = er_res . i_get_int ();   
   return i_res;
}

bool xpath_processor::o_compute_xpath ()
{
   expression_result er_res (XNp_base);
   bool o_res;

   er_res = er_compute_xpath ();
   o_res = er_res . o_get_bool ();   
   return o_res;
}

double xpath_processor::d_compute_xpath ()
{
   expression_result er_res (XNp_base);
   double d_res;

   er_res = er_compute_xpath ();
   d_res = er_res . d_get_double ();   
   return d_res;
}

/// Callback from the XPath decoder : a rule has to be applied
void xpath_processor::v_action (
	xpath_construct xc_rule,      ///< XPath Rule 
	unsigned u_sub,               ///< Rule sub number
	unsigned u_variable,          ///< Parameter, depends on the rule
	const char * cp_literal)      ///< Input literal, depends on the rule
{
   as_action_store . v_add (xc_rule, u_sub, u_variable, cp_literal);
   #ifdef TINYXPATH_DEBUG
      printf ("Action %2d : %s (%d,%d,%s)\n", as_action_store . i_get_size () - 1, cp_disp_construct (xc_rule), u_sub, u_variable, cp_literal);
   #endif
}

/// Internal use. Retrieves the current action counter
int xpath_processor::i_get_action_counter ()
{
   // callback for current stack position
   return as_action_store . i_get_size ();
}

/// Internal use. Executes the XPath expression. The executions starts at the end of the as_action_store list
void xpath_processor::v_execute_stack () 
{
   as_action_store . v_set_position (as_action_store . i_get_size () - 1);
   v_execute_one (xpath_expr, false);
}

/// Retrieves one quadruplet from the action placeholder
void xpath_processor::v_pop_one_action (
   xpath_construct & xc_action,  ///< Next rule on placeholder
   unsigned & u_sub,             ///< Sub rule  
   unsigned & u_ref,             ///< Rule optional parameter
   TIXML_STRING & S_literal)     ///< Rule optional string
{
   int i_1, i_2, i_3;

   as_action_store . v_get (as_action_store . i_get_position (), i_1, i_2, i_3, S_literal);
   xc_action = (xpath_construct) i_1;
   u_sub = i_2;
   u_ref = i_3;
   as_action_store . v_dec_position ();
}

/// Executes one XPath rule
void xpath_processor::v_execute_one (
   xpath_construct xc_rule,      ///< Rule number
   bool o_skip_only)             ///< True if we only need to skip rules and not act on the data stack
{
	xpath_construct xc_action;
	unsigned u_sub;      
	unsigned u_variable;
	TIXML_STRING S_literal;
   TIXML_STRING S_temp;
	TIXML_STRING S_name;
   expression_result ** erpp_arg;
   unsigned u_arg;
   bool o_error;

   v_pop_one_action (xc_action, u_sub, u_variable, S_literal);
   // verify it's the rule we were waiting for
   if (xc_action != xc_rule)
      throw execution_error (2);
   switch (xc_action)
   {
      case xpath_expr :
         // [14] Expr ::= OrExpr 
         v_execute_one (xpath_or_expr, o_skip_only);
         break;

      case xpath_or_expr : 
         switch (u_sub)
         {
            case xpath_or_expr_simple :
               v_execute_one (xpath_and_expr, o_skip_only);
               break;
            case xpath_or_expr_or :
               o_error = false;
               erpp_arg = NULL;
               try
               {
                  v_execute_one (xpath_and_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_and_expr, o_skip_only); 
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     v_function_or (erpp_arg);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (3);
               break;
	         case xpath_or_expr_more :
	         {
		         // These  case is  involved for expressions like a or b or c
		         try
		         {
			         o_error = false;
			         erpp_arg = NULL;
			         v_execute_one (xpath_and_expr, o_skip_only);
			         if (! o_skip_only)
			         {
				         erpp_arg = new expression_result * [2];
				         memset (erpp_arg, 0, 2 * sizeof (expression_result *));
				         erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
				         xs_stack . v_pop ();
			         }
			         v_execute_one (xpath_or_expr, o_skip_only);
			         if (! o_skip_only)
			         {
				         erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
				         xs_stack . v_pop ();
				         v_function_or (erpp_arg);
			         }
		         }
		         catch (execution_error)
		         {
			         o_error = true;
		         }
		         if (erpp_arg)
		         {
			         for (u_arg = 0; u_arg < 2; u_arg++)
			         {
				         if (erpp_arg [u_arg])
					         delete erpp_arg [u_arg];
			         }
			         delete [] erpp_arg;
		         }
		         if (o_error)
			         throw execution_error (7);
	         }
	         break;	   
         }
         break;

      case xpath_and_expr :
         switch (u_sub)
         {
            case xpath_and_expr_simple :
               v_execute_one (xpath_equality_expr, o_skip_only);
               break;
            case xpath_and_expr_and :
               o_error = false;
               erpp_arg = NULL;
               try
               {
                  v_execute_one (xpath_equality_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_equality_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     v_function_and (erpp_arg);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (4);
               break;
         }
         break;

      case xpath_equality_expr :
         switch (u_sub)
         {
            case xpath_equality_expr_simple :
               v_execute_one (xpath_relational_expr, o_skip_only);
               break;
            case xpath_equality_expr_equal : 
            case xpath_equality_expr_not_equal :
               o_error = false;
               erpp_arg = NULL;
               try
               {
                  v_execute_one (xpath_relational_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_relational_expr, o_skip_only); // this is buggy. should be xpath_equality_expr
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     if (u_sub == xpath_equality_expr_equal)
                        v_function_equal (erpp_arg);
                     else
                        v_function_not_equal (erpp_arg);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (5);
               break;
         }
         break;

      case xpath_relational_expr :
         switch (u_sub)
         {
            case xpath_relational_expr_simple :
               v_execute_one (xpath_additive_expr, o_skip_only);
               break;
            case xpath_relational_expr_lt :
            case xpath_relational_expr_gt :
            case xpath_relational_expr_lte :
            case xpath_relational_expr_gte :
               o_error = false;
               erpp_arg = NULL;
               try
               {
                  v_execute_one (xpath_additive_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_additive_expr, o_skip_only); // this is buggy. should be xpath_equality_expr
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     v_function_relational (erpp_arg, u_sub);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (6);
               break;
            default :
               assert (false);
         }
         break;

      case xpath_additive_expr :
         switch (u_sub)
         {
            case xpath_additive_expr_simple :
               v_execute_one (xpath_multiplicative_expr, o_skip_only);
               break;
            case xpath_additive_expr_plus :  
            case xpath_additive_expr_minus :
               try
               {
                  o_error = false;
                  erpp_arg = NULL;
                  v_execute_one (xpath_multiplicative_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_multiplicative_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     if (u_sub == xpath_additive_expr_plus)
                        v_function_plus (erpp_arg);
                     else
                        v_function_minus (erpp_arg);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (7);
               break;
            case xpath_additive_expr_more_plus :  
            case xpath_additive_expr_more_minus :  
               // These 2 cases are involved for expressions like a+b+c 
               // The second argument is an additive expression, not a multiplicative as it is the case
               // when single a+b expressions are encountered
               try
               {
                  o_error = false;
                  erpp_arg = NULL;
                  v_execute_one (xpath_multiplicative_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_additive_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     if (u_sub == xpath_additive_expr_more_plus)
                        v_function_plus (erpp_arg);
                     else
                        v_function_minus (erpp_arg);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (7);
               break;
         }
         break;

      case xpath_multiplicative_expr :
         switch (u_sub)
         {
            case xpath_multiplicative_expr_simple :
               v_execute_one (xpath_unary_expr, o_skip_only);
               break;
            case xpath_multiplicative_expr_star :
            case xpath_multiplicative_expr_div :
            case xpath_multiplicative_expr_mod :
               try
               {
                  o_error = false;
                  erpp_arg = NULL;
                  v_execute_one (xpath_unary_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg = new expression_result * [2];
                     memset (erpp_arg, 0, 2 * sizeof (expression_result *));
                     erpp_arg [1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
                  v_execute_one (xpath_unary_expr, o_skip_only);
                  if (! o_skip_only)
                  {
                     erpp_arg [0] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                     v_function_mult (erpp_arg, u_sub);
                  }
               }
               catch (execution_error)
               {
                  o_error = true;
               }
               if (erpp_arg)
               {
                  for (u_arg = 0; u_arg < 2; u_arg++)
                  {
                     if (erpp_arg [u_arg])
                        delete erpp_arg [u_arg];
                  }
                  delete [] erpp_arg;
               }
               if (o_error)
                  throw execution_error (8);
               break;
         }
         break;
      case xpath_unary_expr :
         switch (u_sub)
         {
            case xpath_unary_expr_simple :   
               // [27]   UnaryExpr			::=   UnionExpr 
               v_execute_one (xpath_union_expr, o_skip_only);
               break;
            case xpath_unary_expr_minus :
               // [27]   UnaryExpr			::=   '-' UnaryExpr
               v_execute_one (xpath_unary_expr, o_skip_only);
               v_function_opposite ();
               break;
         }
         break;

      case xpath_union_expr :
         switch (u_sub)
         {
            case xpath_union_expr_simple :
               v_execute_one (xpath_path_expr, o_skip_only);
               break;
            case xpath_union_expr_union :
               v_execute_one (xpath_union_expr, o_skip_only);
               if (xs_stack . erp_top () -> e_type != e_node_set)
                  throw execution_error (9);
               // here after is a block, so that the node_set are locals to it
               {
                  node_set ns_1, ns_2;
                  ns_1 = ns_pop_node_set ();
                  v_execute_one (xpath_path_expr, o_skip_only);
                  ns_2 = ns_pop_node_set ();
                  v_function_union (ns_1, ns_2);
               }
               break;
         }
         break;

      case xpath_path_expr :
         switch (u_sub)
         {
            case xpath_path_expr_location_path :
               v_execute_one (xpath_location_path, o_skip_only);
               break;
            case xpath_path_expr_filter :
               v_execute_one (xpath_filter_expr, o_skip_only);
               break;
            case xpath_path_expr_slash :
               v_execute_one (xpath_filter_expr, o_skip_only);
               v_execute_one (xpath_relative_location_path, o_skip_only);
               break;
            case xpath_path_expr_2_slash :
               v_execute_one (xpath_filter_expr, o_skip_only);
               v_execute_one (xpath_relative_location_path, o_skip_only);
               break;
         }
         break;

      case xpath_filter_expr :
         switch (u_sub)
         {
            case xpath_filter_expr_primary :
               v_execute_one (xpath_primary_expr, o_skip_only);
               break;
            case xpath_filter_expr_predicate :
               v_execute_one (xpath_filter_expr, o_skip_only);
               v_execute_one (xpath_predicate, o_skip_only);
               break;
         }
         break;

      case xpath_primary_expr :
         switch (u_sub)
         {
            case xpath_primary_expr_variable :
               v_execute_one (xpath_variable_reference, o_skip_only);
               break;
            case xpath_primary_expr_paren_expr :
               v_execute_one (xpath_expr, o_skip_only);
               break;
            case xpath_primary_expr_literal :
               if (! o_skip_only)
                  v_push_string (S_literal);
               break;
            case xpath_primary_expr_number :
               if (! o_skip_only)
               {
                  if (strchr (S_literal . c_str (), '.'))
                     v_push_double (atof (S_literal . c_str ()));
                  else
                     v_push_int (atoi (S_literal . c_str ()), "primary number");
               }
               break;
            case xpath_primary_expr_function_call :
               v_execute_one (xpath_function_call, o_skip_only);
               break;
         }
         break;

      case xpath_function_call :
         erpp_arg = NULL;
         o_error = false;
         try
         {
            if (u_sub)
            {
               // execute arguments
               if (! o_skip_only)
               {
                  erpp_arg = new expression_result * [u_variable];
                  memset (erpp_arg, 0, u_variable * sizeof (expression_result *));
               }
               for (u_arg = 0; u_arg < u_variable; u_arg++)
               {
                  /// Compute each argument, and store them in a temporary list
                  v_execute_one (xpath_argument, o_skip_only);
                  if (! o_skip_only)
                  {
                     if (! xs_stack . u_get_size ())
                        throw execution_error (10);
                     erpp_arg [u_variable - u_arg - 1] = new expression_result (* xs_stack . erp_top ());
                     xs_stack . v_pop ();
                  }
               }
            }
            v_execute_one (xpath_xml_q_name, o_skip_only);
            if (! o_skip_only)
            {
               S_name = S_pop_string ();
               v_execute_function (S_name, u_variable, erpp_arg);
            }
         }
         catch (execution_error)
         {
            o_error = true;
         }
         if (erpp_arg)
         {
            for (u_arg = 0; u_arg < u_variable; u_arg++)
            {
               if (erpp_arg [u_arg])
                  delete erpp_arg [u_arg];
            }
            delete [] erpp_arg;
         }
         if (o_error)
            throw execution_error (11);
         break;

      case xpath_xml_q_name :
         switch (u_sub)
         {
            case xpath_xml_q_name_colon :
               v_execute_one (xpath_xml_local_part, o_skip_only);
               v_execute_one (xpath_xml_prefix, o_skip_only);
               break;
            case xpath_xml_q_name_simple :
               v_execute_one (xpath_xml_local_part, o_skip_only);
               break;
         }
         break;

      case xpath_xml_local_part :
         if (! o_skip_only)
            v_push_string (S_literal);
         break;

      case xpath_xml_prefix :
         if (! o_skip_only)
         {
            // we replace the current stack content (local_part) by the fully
            // qualified XML name (prefix:local_part)
            S_name = S_pop_string ();
            S_literal += ":";
            S_literal += S_name;
            v_push_string (S_literal);
         }
         break;

      case xpath_argument :
         v_execute_one (xpath_expr, o_skip_only);
         break;

      case xpath_location_path :
         switch (u_sub)
         {
            case xpath_location_path_rel :
               v_execute_one (xpath_relative_location_path, o_skip_only);
               break;
            case xpath_location_path_abs :
               v_execute_one (xpath_absolute_location_path, o_skip_only);
               break;
         }
         break;
      
      case xpath_relative_location_path :
         switch (u_sub)
         {
            case xpath_relative_location_path_rel_step :
               // RelativeLocationPath	::=   RelativeLocationPath '/' Step 
               v_execute_one (xpath_relative_location_path, o_skip_only);
               // v_execute_step (i_relative_action);
               break;
            case xpath_relative_location_path_rel_double_slash_step :
               // RelativeLocationPath	::= RelativeLocationPath '//' Step 
               break;
            case xpath_relative_location_path_step :
               // RelativeLocationPath	::=   Step
               int i_dummy;
               i_dummy = -2;
               v_execute_step (i_dummy, o_skip_only);
               break;
         }
         break;

      case xpath_absolute_location_path :
         switch (u_sub)
         {
            case xpath_absolute_location_path_slash :
               // AbsoluteLocationPath	::=   '/' 
               v_execute_absolute_path (u_variable, false, false);
               break;
            case xpath_absolute_location_path_slash_rel :
               // AbsoluteLocationPath	::=   '/' RelativeLocationPath
               v_execute_absolute_path (u_variable, true, false);
               break;
            case xpath_absolute_location_path_abbrev :
               // AbsoluteLocationPath	::= AbbreviatedAbsoluteLocationPath
               v_execute_absolute_path (u_variable, true, true);
               break;
         }
         break;

      case xpath_axis_specifier :
         switch (u_sub)
         {
            case xpath_axis_specifier_at :
               // AxisSpecifier			::= '@'
               if (! o_skip_only)
                  // will be used in the v_execute_step
                  v_push_int (1, "axis specifier is at");
               break;
            case xpath_axis_specifier_axis_name :
               // AxisSpecifier			::= AxisName '::'
               v_execute_one (xpath_axis_name, o_skip_only);
               break;
            case xpath_axis_specifier_empty :
               if (! o_skip_only)
                  // will be used in the v_execute_step
                  v_push_int (0, "axis specifier is empty");
               break;
         }
         break;

      case xpath_axis_name :
         if (! o_skip_only)
            // will be used in the v_execute_step
            v_push_int (u_variable, "axis is a name");
         break;

      case xpath_node_test :
         // to do : processing instructions ???
         switch (u_sub)
         {
            case xpath_node_test_reserved_keyword :
               // will be used in the v_execute_step
               if (! o_skip_only)
               {
                  i_pop_int ();
                  v_push_int (u_variable, "axis is a keyword");
                  S_temp = "*";
                  v_push_string (S_temp);
               }
               break;
            case xpath_node_test_pi :
               break;
            case xpath_node_test_pi_lit :
               break;
            case xpath_node_test_name_test :
               v_execute_one (xpath_name_test, o_skip_only);
               break;
         }
         break;

      case xpath_predicate :
         // [8]   Predicate				::=   '[' PredicateExpr ']' 
         v_execute_one (xpath_predicate_expr, o_skip_only);
         break;

      case xpath_predicate_expr :
         // [9]   PredicateExpr			::=   Expr
         v_execute_one (xpath_expr, o_skip_only);
         break;

      case xpath_name_test :
         switch (u_sub)
         {
            case xpath_name_test_star :
               if (! o_skip_only)
               {
                  S_temp = "*";
                  v_push_string (S_temp);
               }
               break;
            case xpath_name_test_ncname :
               break;
            case xpath_name_test_qname :
               v_execute_one (xpath_xml_q_name, o_skip_only);
               break;
         }
         break;

      default :
         throw execution_error (12);
   }
}

/// Execute a full set of absolute/relative/relative/.. computation
void xpath_processor::v_execute_absolute_path (
   unsigned u_action_position,      ///< Position of the placeholder after the rule
   bool o_with_rel,                 ///< true if there is some relative path
   bool o_everywhere)               ///< true if it's a '//' path
{
   unsigned u_end_action;
   int i_relative_action;
   bool o_do_last;

   u_end_action = u_action_position;

   if (o_with_rel)
   {
      int i_1, i_2, i_3;
      int i_bak_position, i_current, i_first, i_relative;
      TIXML_STRING S_lit;

      // compute position of the first (absolute) step
      i_current = as_action_store . i_get_position ();
      if (o_everywhere)
         i_relative = i_current - 2;
      else
         i_relative = i_current - 1;
      as_action_store . v_get (i_relative, i_1, i_2, i_3, S_lit);
      if (i_1 == xpath_relative_location_path)
      {
         o_do_last = true;
         i_first = i_3 - 1;
      }
      else
      {
         o_do_last = false;
         i_first = i_relative;
      }
      // i_first = i_3 - 1;
      i_bak_position = as_action_store . i_get_position ();
      as_action_store . v_set_position (i_first);
      if (o_everywhere)
         i_relative_action = -1;
      else
         i_relative_action = 0;
      v_execute_step (i_relative_action, false);

      bool o_end = false;
      do
      {
         i_relative--;
         as_action_store . v_get (i_relative, i_1, i_2, i_3, S_lit);
         if (i_1 != xpath_relative_location_path)
            o_end = true;
         else
         {
            as_action_store . v_set_position (i_3 - 1);
            v_execute_step (i_relative_action, false);
         }
      } while (! o_end);

      if (o_do_last)
      {
         // apply last one

         as_action_store . v_set_position (i_relative);
         v_execute_step (i_relative_action, false);
      }
      // resume the execution after the whole path construction
      as_action_store . v_set_position ((int) u_end_action - 1);
   }
}

/// One step execution
void xpath_processor ::v_execute_step (
   int & i_relative_action,         ///< Path position : -1 if first in a '//' path, 0 if first in a '/' path, > 0 if followers
   bool o_skip_only)
{
   bool o_by_name;
   int i_axis_type, i_end_store, i_node_store, i_pred_store;
   unsigned u_nb_node, u_node, u_pred, u_sub, u_variable;
	xpath_construct xc_action;
   TIXML_STRING S_literal, S_name;
   const TiXmlElement * XEp_child, * XEp_elem;
   const TiXmlNode * XNp_father;
   const TiXmlAttribute * XAp_attrib;
   const TiXmlNode * XNp_next, * XNp_parent;
   node_set ns_source, ns_target;

   if (! o_skip_only)
   {
      /// Initialize the source node set if it's the first step of a path
      switch (i_relative_action)
      {
         case -2 :
            // relative to context
            ns_source . v_add_node_in_set (XEp_context);
            i_relative_action = 1;
            break;
         case -1 :
            // everywhere
            ns_source . v_copy_selected_node_recursive_root_only (XNp_base_parent, XNp_base);
            i_relative_action = 1;
            break;
         case 0 :
            // first absolute
            ns_source . v_add_node_in_set (XNp_base_parent);
            i_relative_action = 1;
            break;
         default :
            // second and following steps
            ns_source = * (xs_stack . erp_top () -> nsp_get_node_set ());
            xs_stack . v_pop ();
            break;
      }
   }
            
   // Pop our step action from the action placeholder
   v_pop_one_action (xc_action, u_sub, u_variable, S_literal);

   // Skip the predicates
   i_pred_store = as_action_store . i_get_position ();
   for (u_pred = 0; u_pred < u_variable; u_pred++)
      v_execute_one (xpath_predicate, true);
   i_node_store = as_action_store . i_get_position ();

   // Skip the node test
   v_execute_one (xpath_node_test, true);

   // Run the axis
   v_execute_one (xpath_axis_specifier, o_skip_only);
   i_end_store = as_action_store . i_get_position ();

   // Run the node test
   as_action_store . v_set_position (i_node_store);
   v_execute_one (xpath_node_test, o_skip_only);
   as_action_store . v_set_position (i_pred_store);

   if (! o_skip_only)
   {
      S_name = S_pop_string ();
      o_by_name = ! (S_name == "*");

      // Retrieve the archive flag stored by the xpath_axis_specifier rule execution
      // o_attrib_flag = o_pop_bool ();
      i_axis_type = i_pop_int ();

      u_nb_node = ns_source . u_get_nb_node_in_set ();
      for (u_node = 0; u_node < u_nb_node; u_node++)
      {
         if (! ns_source . o_is_attrib (u_node))
         {
            XNp_father = ns_source . XNp_get_node_in_set (u_node);
            if (XNp_father)
            {
               switch (i_axis_type)
               {
                  case 0 :
                  case lex_child :
                     // none
                     XEp_child = XNp_father -> FirstChildElement ();
                     while (XEp_child)
                     {
                        ns_target . v_add_node_in_set_if_name_or_star (XEp_child, S_name);
                        XEp_child = XEp_child -> NextSiblingElement ();
                     }
                     break;
                  case 1 :
                  case lex_attribute :
                     // @
                     if (XNp_father -> ToElement ())
                        XAp_attrib = XNp_father -> ToElement () -> FirstAttribute ();
                     else
                        XAp_attrib = NULL;
                     while (XAp_attrib)
                     {
                        ns_target . v_add_attrib_in_set_if_name_or_star (XAp_attrib, S_name);
                        XAp_attrib = XAp_attrib -> Next ();
                     }
                     break;
                  case lex_parent :
                     XNp_parent = XNp_father -> Parent ();
                     if (XNp_parent)
                        ns_target . v_add_node_in_set_if_name_or_star (XNp_parent, S_name);
                     break;
                  case lex_ancestor :
                     XNp_parent = XNp_father -> Parent ();
                     // we have to exclude our own dummy parent
                     while (XNp_parent && XNp_parent != XNp_base_parent)
                     {
                        ns_target . v_add_node_in_set_if_name_or_star (XNp_parent, S_name);
                        XNp_parent = XNp_parent -> Parent ();
                     }
                     break;
                  case lex_ancestor_or_self :
                     if (XNp_father -> ToElement () && XNp_father != XNp_base_parent)
                        ns_target . v_add_node_in_set_if_name_or_star (XNp_father, S_name);
                     XNp_parent = XNp_father -> Parent ();
                     while (XNp_parent && XNp_parent != XNp_base_parent)
                     {
                        ns_target . v_add_node_in_set_if_name_or_star (XNp_parent, S_name);
                        XNp_parent = XNp_parent -> Parent ();
                     }
                     break;
                  case lex_following_sibling :
                     XNp_next = XNp_father -> NextSiblingElement ();
                     while (XNp_next)
                     {
                        ns_target . v_add_node_in_set_if_name_or_star (XNp_next, S_name);
                        XNp_next = XNp_next -> NextSiblingElement ();
                     }
                     break;
                  case lex_preceding_sibling :
                     XNp_next = XNp_father -> PreviousSibling ();
                     while (XNp_next)
                     {
                        if (XNp_next -> Type () == TiXmlNode::ELEMENT)
                           ns_target . v_add_node_in_set_if_name_or_star (XNp_next, S_name);
                        XNp_next = XNp_next -> PreviousSibling ();
                     }
                     break;
                  case lex_descendant :
                     if (XNp_father -> ToElement ())
                     {
                        if (S_name == "*")
                           ns_target . v_copy_selected_node_recursive_no_attrib (XNp_father, NULL);
                        else
                           ns_target . v_copy_selected_node_recursive_no_attrib (XNp_father, S_name . c_str ());
                     }
                     break;
                  case lex_descendant_or_self :
                     if (XNp_father -> ToElement ())
                     {
                        if (XNp_father != XNp_base_parent)
                           ns_target . v_add_node_in_set_if_name_or_star (XNp_father, S_name);
                        if (S_name == "*")
                           ns_target . v_copy_selected_node_recursive_no_attrib (XNp_father, NULL);
                        else
                           ns_target . v_copy_selected_node_recursive_no_attrib (XNp_father, S_name . c_str ());
                     }
                     break;
                  case lex_self :
                     if (XNp_father -> ToElement ())
                     {
                        if (XNp_father != XNp_base_parent && XNp_father -> ToElement ())
                           ns_target . v_add_node_in_set_if_name_or_star (XNp_father, S_name);
                     }
                     break;
                  case lex_following :
                     ns_target . v_add_all_foll_node (XNp_father, S_name);
                     break;
                  case lex_preceding :
                     ns_target . v_add_all_prec_node (XNp_father, S_name);
                     break;
                  case lex_comment :
                     XNp_next = XNp_father -> FirstChild ();               
                     while (XNp_next)
                     {
                        if (XNp_next -> Type () == TiXmlNode::COMMENT)
                           ns_target . v_add_node_in_set (XNp_next);
                        XNp_next = XNp_next -> NextSibling ();
                     }
                     break;
                  case lex_text :
                     XNp_next = XNp_father -> FirstChild ();               
                     while (XNp_next)
                     {
                        if (XNp_next -> Type () == TiXmlNode::TEXT)
                           ns_target . v_add_node_in_set (XNp_next);
                        XNp_next = XNp_next -> NextSibling ();
                     }
                     break;
                  case lex_node :
                     XNp_next = XNp_father -> FirstChild ();               
                     while (XNp_next)
                     {
                        ns_target . v_add_node_in_set (XNp_next);
                        XNp_next = XNp_next -> NextSibling ();
                     }
                     break;
                  default :
                     // an axis name followed by '::'
                     throw error_not_yet ();
                     break;
               }
            }
         }
      }

      if (u_variable)
      {
         // we have predicates to apply
         node_set ns_after_predicate;
         for (u_node = 0; u_node < ns_target .u_get_nb_node_in_set (); u_node++)
         {
            if (! ns_target . o_is_attrib (u_node))
            {
               XEp_elem = ns_target . XNp_get_node_in_set (u_node) -> ToElement ();
               if (XEp_elem)
               {
                  as_action_store . v_set_position (i_pred_store);
                  if (o_check_predicate (XEp_elem, o_by_name))
                     ns_after_predicate . v_add_node_in_set (XEp_elem);
               }
            }
         }
         v_push_node_set (& ns_after_predicate);
      }
      else
         v_push_node_set (& ns_target);
   }
   as_action_store . v_set_position (i_end_store);
}

/// Spec extract :
/// \n A PredicateExpr is evaluated by evaluating the Expr and converting the result to a 
/// boolean. If the result is a number, the result will be converted to true if the number 
/// is equal to the context position and will be converted to false otherwise; if the result 
/// is not a number, then the result will be converted as if by a call to the boolean function. 
/// Thus a location path para[3] is equivalent to para[position()=3].
bool xpath_processor::o_check_predicate (const TiXmlElement * XEp_child, bool o_by_name)
{
   expression_result * erp_top;
   bool o_keep;

   v_set_context (XEp_child, o_by_name);
   v_execute_one (xpath_predicate, false);
   v_set_context (NULL, false);
   erp_top = xs_stack . erp_top ();
   switch (erp_top -> e_type)
   {
      case e_double :
      case e_int :
         o_keep = (erp_top -> i_get_int () == i_xml_cardinality (XEp_child, o_by_name));
         break;
      default :
         o_keep = erp_top -> o_get_bool ();
         break;
   }
   xs_stack . v_pop ();
   return o_keep;
}

/**
Execute an XPath function. The arguments are in normal order in erpp_arg\n
Calls one of the following :
- v_function_ceiling
- v_function_concat
- v_function_contains
- v_function_count
- v_function_false
- v_function_floor
- v_function_last
- v_function_name
- v_function_normalize_space
- v_function_not
- v_function_position
- v_function_starts_with
- v_function_string_length
- v_function_substring
- v_function_sum
- v_function_true
*/
void xpath_processor::v_execute_function (
   TIXML_STRING & S_name,           ///< Function name
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   if (S_name == "ceiling")
      v_function_ceiling (u_nb_arg, erpp_arg);
   else

   if (S_name == "concat")
      v_function_concat (u_nb_arg, erpp_arg);
   else

   if (S_name == "contains")
      v_function_contains (u_nb_arg, erpp_arg);
   else

   if (S_name == "count")
      v_function_count (u_nb_arg, erpp_arg);
   else

   if (S_name == "false")
      v_function_false (u_nb_arg, erpp_arg);
   else

   if (S_name == "floor")
      v_function_floor (u_nb_arg, erpp_arg);
   else

   if (S_name == "last")
      v_function_last (u_nb_arg, erpp_arg);
   else

   if (S_name == "name")
      v_function_name (u_nb_arg, erpp_arg);
   else

	if (S_name == "normalize-space")
      v_function_normalize_space (u_nb_arg, erpp_arg);
   else

   if (S_name == "not")
      v_function_not (u_nb_arg, erpp_arg);
   else

   if (S_name == "position")
      v_function_position (u_nb_arg, erpp_arg);
   else

   if (S_name == "starts-with")
      v_function_starts_with (u_nb_arg, erpp_arg);
   else

   if (S_name == "string-length")
      v_function_string_length (u_nb_arg, erpp_arg);
   else

   if (S_name == "substring")
      v_function_substring (u_nb_arg, erpp_arg);
   else

   if (S_name == "sum")
      v_function_sum (u_nb_arg, erpp_arg);
   else

   if (S_name == "text")
      v_function_text (u_nb_arg, erpp_arg);
   else

   if (S_name == "translate")
      v_function_translate (u_nb_arg, erpp_arg);
   else

   if (S_name == "true")
      v_function_true (u_nb_arg, erpp_arg);
   else

      throw execution_error (13);
}

/// XPath \b ceiling function
void xpath_processor::v_function_ceiling (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   int i_val;

   if (u_nb_arg != 1)
      throw execution_error (14);
   switch (erpp_arg [0] -> e_type)
   {
      case e_int :
      case e_bool :
         i_val = erpp_arg [0] -> i_get_int ();
         break;
      case e_double :
			i_val = (int) ceil (erpp_arg [0] -> d_get_double ());
         break;
      default :
         i_val = 0;
         break;
   }
   v_push_int (i_val, "ceiling");
}

/// XPath \b concat function
void xpath_processor::v_function_concat (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_res;   
   unsigned u_arg;

   if (! u_nb_arg)
      throw execution_error (15);
   S_res = erpp_arg [0] -> S_get_string ();
   for (u_arg = 1; u_arg < u_nb_arg; u_arg++)
      S_res += erpp_arg [u_arg] -> S_get_string () . c_str ();
   v_push_string (S_res);
}

/// XPath \b contains function
void xpath_processor::v_function_contains (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_arg_1, S_arg_2;   

   if (u_nb_arg != 2)
      throw execution_error (16);
   S_arg_1 = erpp_arg [0] -> S_get_string ();
   S_arg_2 = erpp_arg [1] -> S_get_string ();
   v_push_bool (strstr (S_arg_1 . c_str (), S_arg_2 . c_str ()) ? true : false);
}

/// XPath \b count function
void xpath_processor::v_function_count (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   int i_res;

   if (! u_nb_arg)
      throw execution_error (17);
   if (erpp_arg [0] -> e_type != e_node_set)
      i_res = 0;
   else
      i_res = erpp_arg [0] -> nsp_get_node_set () -> u_get_nb_node_in_set ();
      
   v_push_int (i_res, "count result");
}

/// XPath \b false function
void xpath_processor::v_function_false (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   if (u_nb_arg)
      throw execution_error (18);
   v_push_bool (false);
}

/// XPath \b floor function
void xpath_processor::v_function_floor (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   int i_val;

   if (u_nb_arg != 1)
      throw execution_error (19);
   switch (erpp_arg [0] -> e_type)
   {
      case e_int :
      case e_bool :
         i_val = erpp_arg [0] -> i_get_int ();
         break;
      case e_double :
			i_val = (int) floor (erpp_arg [0] -> d_get_double ());
         break;
      default :
         i_val = 0;
         break;
   }
   v_push_int (i_val, "floor");
}

/// XPath \b last function
void xpath_processor::v_function_last (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   const TiXmlElement * XEp_context;

   if (u_nb_arg)
      throw execution_error (20);
   XEp_context = XEp_get_context ();
   if (! XEp_context)
      throw execution_error (21);
   v_push_int (i_xml_family_size (XEp_context), "last()");
}

/// XPath \b name function
/// \n XPath spec: If the argument it omitted, it defaults to a node-set with the context node as its only member.
void xpath_processor::v_function_name (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_res;   
   node_set * nsp_set;

   switch (u_nb_arg)
   {
      case 0 :
         // name of the context node
         XEp_context = XEp_get_context ();
         S_res = XEp_context -> Value ();
         break;
      case 1 :
         // name of the argument
         S_res = "";
         if (erpp_arg [0] -> e_type == e_node_set)
         {
            nsp_set = erpp_arg [0] -> nsp_get_node_set ();
            if (nsp_set -> u_get_nb_node_in_set ())
               if (nsp_set -> o_is_attrib (0))
                  S_res = nsp_set -> XAp_get_attribute_in_set (0) -> Name ();
               else
                  S_res = nsp_set -> XNp_get_node_in_set (0) -> Value ();
         }
         break;
      default :
         throw execution_error (22);
   }
   v_push_string (S_res);
}

/// XPath \b normalize-space function
void xpath_processor::v_function_normalize_space (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_arg, S_res;   

   if (u_nb_arg != 1)
      throw execution_error (23);
   S_arg = erpp_arg [0] -> S_get_string ();
   S_res = S_remove_lead_trail (S_arg . c_str ());
   v_push_string (S_res);
}

/// XPath \b not function
void xpath_processor::v_function_not (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   if (u_nb_arg != 1)
      throw execution_error (24);
   v_push_bool (! erpp_arg [0] -> o_get_bool ());
}

/// XPath \b position function
void xpath_processor::v_function_position (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   const TiXmlElement * XEp_context;

   if (u_nb_arg)
      throw execution_error (25);
   XEp_context = XEp_get_context ();
   if (! XEp_context)
      throw execution_error (26);
   v_push_int (i_xml_cardinality (XEp_context, o_is_context_by_name), "position()");
}

/// XPath \b starts-with function
void xpath_processor::v_function_starts_with (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_arg_1, S_arg_2;   

   if (u_nb_arg != 2)
      throw execution_error (27);
   S_arg_1 = erpp_arg [0] -> S_get_string ();
   S_arg_2 = erpp_arg [1] -> S_get_string ();
   v_push_bool (! strncmp (S_arg_1 . c_str (), S_arg_2 . c_str (), S_arg_2 . length ()));
}

/// XPath \b sttring-length function
void xpath_processor::v_function_string_length (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_arg;   

   if (u_nb_arg != 1)
      throw execution_error (28);
   S_arg = erpp_arg [0] -> S_get_string ();
   v_push_int (S_arg . length (), "string-length");
}

/**
XPath \b substring function\n
Standard excerpt:\n
The substring function returns the substring of the first argument starting at the position specified in the second argument 
with length specified in the third argument. For example, substring("12345",2,3) returns "234". If the third argument is not 
specified, it returns the substring starting at the position specified in the second argument and continuing to the end of 
the string. For example, substring("12345",2) returns "2345".
*/
void xpath_processor::v_function_substring (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_base, S_ret;   
   int i_length, i_start;
   const char * cp_base;
   char * cp_work;

   if (u_nb_arg != 2 && u_nb_arg != 3)
      throw execution_error (29);
   S_base = erpp_arg [0] -> S_get_string ();
   i_start = erpp_arg [1] -> i_get_int ();
   if (u_nb_arg == 3)
      i_length = erpp_arg [2] -> i_get_int ();
   else
      i_length = 0;

   if (i_start < 1)
      S_ret = "";
   else
   {
      i_start--;
      if (i_start >= (int) S_base . length ())
         S_ret = "";
      else
      {
         cp_base = S_base . c_str () + i_start;
         if (u_nb_arg == 2 || (int) strlen (cp_base) <= i_length)
            S_ret = cp_base;
         else
         {
            cp_work = new char [i_length + 1];
            strncpy (cp_work, cp_base, i_length);
            cp_work [i_length] = 0;
            S_ret = cp_work;
            delete [] cp_work;
         }
      }
   }
   v_push_string (S_ret);
}

/// XPath \b sum function\n
/// Standard exerpt :\n
/// The sum function returns the sum, for each node in the argument node-set, 
/// of the result of converting the string-values of the node to a number.
void xpath_processor::v_function_sum (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   int i_sum;
   double d_sum;
   unsigned u_node;
   node_set * nsp_set;
   bool o_out_double;

   if (u_nb_arg != 1)
      throw execution_error (30);
   i_sum = 0;
   d_sum = 0.0;
   o_out_double = false;
   if (erpp_arg [0] -> e_type != e_node_set)
      throw execution_error (31);
   nsp_set = erpp_arg [0] -> nsp_get_node_set ();
   assert (nsp_set);
   for (u_node = 0; u_node < nsp_set -> u_get_nb_node_in_set (); u_node++)
   {
      i_sum += nsp_set -> i_get_value (u_node);
      d_sum += nsp_set -> d_get_value (u_node);
      if (strchr (nsp_set -> S_get_value (u_node) . c_str (), '.'))
         o_out_double = true;
   }
   if (o_out_double)
      v_push_double (d_sum);
   else
      v_push_int (i_sum, "sum()");
}

/// XPath \b text function
void xpath_processor::v_function_text (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   const TiXmlElement * XEp_context;
   const TiXmlNode * XNp_child;
   TIXML_STRING S_res;

   if (u_nb_arg)
      throw execution_error (38);
   XEp_context = XEp_get_context ();
   if (! XEp_context)
      throw execution_error (39);
   XNp_child = XEp_context -> FirstChild ();
   while (XNp_child)
   {
      if (XNp_child -> Type () == TiXmlNode::TEXT)
         S_res += XNp_child -> Value ();
      XNp_child = XNp_child -> NextSibling ();
   }
   v_push_string (S_res);
}

/// XPath \b translate function\n
/// Standard exerpt :\n
///The translate function returns the first argument string with occurrences of 
///characters in the second argument string replaced by the character at the 
///corresponding position in the third argument string. 
void xpath_processor::v_function_translate (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   TIXML_STRING S_translated;
   char* cp_translated = NULL;
   //pre-conditions
   if (u_nb_arg != 3)
      throw execution_error (40);
   
   TIXML_STRING S_translate_me  = erpp_arg [0] -> S_get_string ();
   TIXML_STRING S_translation_table_lhs = erpp_arg [1] -> S_get_string ();
   TIXML_STRING S_translation_table_rhs = erpp_arg [2] -> S_get_string ();
   
   
   // Strings S_translation_table_lhs and S_translation_table_rhs represent
   // the translation  table's left hand side and right hand side respectively
   // e.g.  for   "abc"  "XYZ" ... we have the table
   //		"a   "X	
   //		 b    Y	
   //		 c    Z 
   //		 "    "
   //        lhs   rhs   
   
   cp_translated = new char[ S_translate_me.length() + 1];
   
   unsigned int u_write_at = 0 ;
	
   for(unsigned int u_read_at = 0; u_read_at < S_translate_me.length() ; u_read_at++)
   {
      //search in the translation scheme table
      unsigned int u_translation_rule_index = 0;
      for(; u_translation_rule_index < S_translation_table_lhs.size(); u_translation_rule_index++)
      {
         // this also ensures that if we have multiple translation rules for a single character then only the first one is selected
         if(S_translate_me[u_read_at] == S_translation_table_lhs [u_translation_rule_index])
         {
            //translation rule found for current character
            break;   
         }
      }
      if( u_translation_rule_index  <  S_translation_table_lhs.size())
      {
         //the current character has a translation rule
         if(u_translation_rule_index < S_translation_table_rhs.size())
         {
            cp_translated[u_write_at] = S_translation_table_rhs[u_translation_rule_index];
            u_write_at++;
         }
         else
         {
            // else empty translation scheme
            // so current charater skipped
         }   
      }
      else
      {
         //no translation scheme for  current charater
         //thus copy it as it is in cp_translated
         cp_translated[u_write_at] = S_translate_me[u_read_at];
         u_write_at++;
      }
   }
   cp_translated [u_write_at] = 0; 
   
   S_translated = cp_translated;
   
   delete [] cp_translated;
   
   v_push_string(S_translated);
}

/// XPath \b true function
void xpath_processor::v_function_true (
   unsigned u_nb_arg,               ///< Nb of arguments
   expression_result ** erpp_arg)   ///< Argument list
{
   if (u_nb_arg)
      throw execution_error (32);
   v_push_bool (true);
}

/**
This function is for internal use : evaluation of an equality expression \n
Standard excerpt :\n

If both objects to be compared are node-sets, then the comparison will be true if and only if there is a node in the first 
node-set and a node in the second node-set such that the result of performing the comparison on the string-values of the 
two nodes is true. \n

If one object to be compared is a node-set and the other is a number, then the comparison will be true 
if and only if there is a node in the node-set such that the result of performing the comparison on the number to be compared 
and on the result of converting the string-value of that node to a number using the number function is true. \n

If one object to be compared is a node-set and the other is a string, then the comparison will be true if and only if there is a node in 
the node-set such that the result of performing the comparison on the string-value of the node and the other string is true. \n

If one object to be compared is a node-set and the other is a boolean, then the comparison will be true if and only if the 
result of performing the comparison on the boolean and on the result of converting the node-set to a boolean using the boolean 
function is true.\n

When neither object to be compared is a node-set and the operator is = or !=, then the objects are compared by converting them 
to a common type as follows and then comparing them. 
-  If at least one object to be compared is a boolean, then each object to be compared is converted to a boolean as if by 
   applying the boolean function. 
-  Otherwise, if at least one object to be compared is a number, then each object to be compared is converted to a number as 
   if by applying the number function. 
-  Otherwise, both objects to be compared are converted to strings as if by applying the string function. The = comparison will 
   be true if and only if the objects are equal; the != comparison will be true if and only if the objects are not equal. 
-  Numbers are compared for equality according to IEEE 754. 
-  Two booleans are equal if either both are true or both are false. 
-  Two strings are equal if and only if they consist of the same sequence of UCS characters. 
*/
void xpath_processor::v_function_equal (expression_result ** erpp_arg)
{
   bool o_res;

   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);
   if (erpp_arg [0] -> e_type == e_node_set)
      if (erpp_arg [1] -> e_type == e_node_set)   
         v_function_equal_2_node (erpp_arg [0], erpp_arg [1]);
      else
         v_function_equal_node_and_other (erpp_arg [0], erpp_arg [1]);
   else
      if (erpp_arg [1] -> e_type == e_node_set)   
         v_function_equal_node_and_other (erpp_arg [1], erpp_arg [0]);
      else
      {
         // none are node sets : alternate decision table on bools, numbers and strings
         if (erpp_arg [0] -> e_type == e_bool || erpp_arg [1] -> e_type == e_bool)
            o_res = erpp_arg [0] -> o_get_bool () == erpp_arg [1] -> o_get_bool ();
         else
            if (erpp_arg [0] -> e_type == e_int || erpp_arg [1] -> e_type == e_int ||
                  erpp_arg [0] -> e_type == e_double || erpp_arg [1] -> e_type == e_double)
               o_res = erpp_arg [0] -> d_get_double () == erpp_arg [1] -> d_get_double ();
            else
               o_res = erpp_arg [0] -> S_get_string () == erpp_arg [1] -> S_get_string ();
         v_push_bool (o_res);
      }
}

/// This function is for internal use : evaluation of a non-equality expression
void xpath_processor::v_function_not_equal (expression_result ** erpp_arg)
{
   v_function_equal (erpp_arg);
   v_push_bool (! o_pop_bool ());
}

/**
Utility function that evaluates the equality between a node set and a non-node set\n

Standard excerpt :\n
If one object to be compared is a node-set and the other is a number, then the comparison will be true 
if and only if there is a node in the node-set such that the result of performing the comparison on the number to be compared 
and on the result of converting the string-value of that node to a number using the number function is true. \n

If one object to be compared is a node-set and the other is a string, then the comparison will be true if and only if there is a node in 
the node-set such that the result of performing the comparison on the string-value of the node and the other string is true. \n

If one object to be compared is a node-set and the other is a boolean, then the comparison will be true if and only if the 
result of performing the comparison on the boolean and on the result of converting the node-set to a boolean using the boolean 
function is true.\n
*/
void xpath_processor::v_function_equal_node_and_other (expression_result * erp_node_set, expression_result * erp_non)
{
   bool o_res;
   node_set * nsp_ptr;
   unsigned u_node;

   o_res = false;
   nsp_ptr = erp_node_set -> nsp_get_node_set ();
   switch (erp_non -> e_type)
   {
      case e_bool :
         o_res = erp_non -> o_get_bool () ==  erp_node_set -> o_get_bool ();
         break;
      case e_int :
      case e_double :
         for (u_node = 0; u_node < nsp_ptr -> u_get_nb_node_in_set (); u_node++)
            if (nsp_ptr -> i_get_value (u_node) == erp_non -> i_get_int ())
               o_res = true;
         break;
      case e_string :
         for (u_node = 0; u_node < nsp_ptr -> u_get_nb_node_in_set (); u_node++)
            if (nsp_ptr -> S_get_value (u_node) == erp_non -> S_get_string ())
               o_res = true;
         break;
      default :
         // this case should have been excluded before our call
         assert (false);
         break;
   }
   v_push_bool (o_res);
}

/// \todo : Implement this function. We don't compare 2 node sets yet
void xpath_processor::v_function_equal_2_node (expression_result * erp_node_set_, expression_result * erp_node_set_2)
{
   throw error_not_yet ();
}

/// Union function
void xpath_processor::v_function_union (node_set & ns_1, node_set & ns_2)
{
   node_set ns_target;
   unsigned u_node;
   const TiXmlBase * XBp_base;

   ns_target = ns_1;
   for (u_node = 0; u_node < ns_2 . u_get_nb_node_in_set (); u_node++)
   {
      XBp_base = ns_2 . XBp_get_base_in_set (u_node);
      ns_target . v_add_base_in_set (XBp_base, ns_2 . o_is_attrib (u_node));
   }
   v_push_node_set (& ns_target);
}

/// XPath <b> + </b> function
void xpath_processor::v_function_plus (expression_result ** erpp_arg)
{
   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);
   if (erpp_arg [0] -> e_type == e_double || erpp_arg [1] -> e_type == e_double)
      v_push_double (erpp_arg [0] -> d_get_double () + erpp_arg [1] -> d_get_double ());
   else
      v_push_int (erpp_arg [0] -> i_get_int () + erpp_arg [1] -> i_get_int (), "+");
}

/// XPath <b> - </b> function
void xpath_processor::v_function_minus (expression_result ** erpp_arg)
{
   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);
   if (erpp_arg [0] -> e_type == e_double || erpp_arg [1] -> e_type == e_double)
      v_push_double (erpp_arg [0] -> d_get_double () - erpp_arg [1] -> d_get_double ());
   else
      v_push_int (erpp_arg [0] -> i_get_int () - erpp_arg [1] -> i_get_int (), "-");
}

/// XPath \b or function
void xpath_processor::v_function_or (expression_result ** erpp_arg)
{
   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);
   v_push_bool (erpp_arg [0] -> o_get_bool () || erpp_arg [1] -> o_get_bool ());
}

/// XPath \b and function
void xpath_processor::v_function_and (expression_result ** erpp_arg)
{
   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);
   v_push_bool (erpp_arg [0] -> o_get_bool () && erpp_arg [1] -> o_get_bool ());
}

/// XPath relational comparison function 
void xpath_processor::v_function_relational (expression_result ** erpp_arg, unsigned u_sub)
{
   bool o_res;

   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);

   if (erpp_arg [0] -> e_type == e_double || erpp_arg [1] -> e_type == e_double)
   {  
      double d_arg_1, d_arg_2;

      d_arg_1 = erpp_arg [0] -> d_get_double ();
      d_arg_2 = erpp_arg [1] -> d_get_double ();
      switch (u_sub)
      {
         case xpath_relational_expr_lt :
            o_res = d_arg_1 < d_arg_2;
            break;
         case xpath_relational_expr_gt :
            o_res = d_arg_1 > d_arg_2;
            break;
         case xpath_relational_expr_lte :
            o_res = d_arg_1 <= d_arg_2;
            break;
         case xpath_relational_expr_gte :
            o_res = d_arg_1 >= d_arg_2;
            break;
         default :
            assert (false);
      }
   }
   else
   {
      int i_arg_1, i_arg_2;

      i_arg_1 = erpp_arg [0] -> i_get_int ();
      i_arg_2 = erpp_arg [1] -> i_get_int ();
      switch (u_sub)
      {
         case xpath_relational_expr_lt :
            o_res = i_arg_1 < i_arg_2;
            break;
         case xpath_relational_expr_gt :
            o_res = i_arg_1 > i_arg_2;
            break;
         case xpath_relational_expr_lte :
            o_res = i_arg_1 <= i_arg_2;
            break;
         case xpath_relational_expr_gte :
            o_res = i_arg_1 >= i_arg_2;
            break;
         default :
            assert (false);
      }
   }
   v_push_bool (o_res);
}

/// XPath <b> * </b> function (arithmetic)
void xpath_processor::v_function_mult (expression_result ** erpp_arg, unsigned u_sub)
{
   assert (erpp_arg);
   assert (erpp_arg [0]);
   assert (erpp_arg [1]);

   if (erpp_arg [0] -> e_type == e_double || erpp_arg [1] -> e_type == e_double || u_sub == xpath_multiplicative_expr_div)
   {  
      double d_arg_1, d_arg_2, d_res;

      d_arg_1 = erpp_arg [0] -> d_get_double ();
      d_arg_2 = erpp_arg [1] -> d_get_double ();
      switch (u_sub)
      {
         case xpath_multiplicative_expr_star :
            d_res = d_arg_1 * d_arg_2;
            break;
         case xpath_multiplicative_expr_div :
            if (fabs (d_arg_2) < 1.0e-6)
               throw execution_error (33);
            d_res = d_arg_1 / d_arg_2;
            break;
         case xpath_multiplicative_expr_mod :
            d_res = (int) d_arg_1 % (int) d_arg_2;
            break;
         default :
            assert (false);
      }
      v_push_double (d_res);
   }
   else
   {
      int i_arg_1, i_arg_2, i_res;

      i_arg_1 = erpp_arg [0] -> i_get_int ();
      i_arg_2 = erpp_arg [1] -> i_get_int ();
      switch (u_sub)
      {
         case xpath_multiplicative_expr_star :
            i_res = i_arg_1 * i_arg_2;
            break;
         case xpath_multiplicative_expr_mod :
            i_res = i_arg_1 % i_arg_2;
            break;
         default :
            assert (false);
      }
      v_push_int (i_res, "*");
   }
}


/// This function, because it only operates on one argument retrieves it himself from the stack
/// \n It computes the mathematical opposite
void xpath_processor::v_function_opposite ()
{
   expression_result er_arg (XNp_base);

   er_arg = * xs_stack . erp_top ();
   xs_stack . v_pop ();
   switch (er_arg . e_type)
   {
      case e_double :
         v_push_double (-1.0 * er_arg . d_get_double ());
         break;
      default :
         v_push_int (-1 * er_arg . i_get_int (), "unary -");
         break;
   }
}

/// Set the current context node for predicate evaluations
void xpath_processor::v_set_context (
   const TiXmlElement * XEp_in,     ///< Context node
   bool o_by_name)            ///< true if the current node search is by name, false if it's a *
{
   XEp_context = XEp_in; 
   o_is_context_by_name = o_by_name;
}
