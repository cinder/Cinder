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
   \file xpath_processor.h
   \author Yves Berquin
*/
/*
 @history:
 
 Modified on 18 December 2006 by Aman Aggarwal
 ::Added support for translate()  
*/
#ifndef __XPATH_PROCESSOR_H
#define __XPATH_PROCESSOR_H

#include "action_store.h"
#include "xpath_expression.h"
#include "xpath_stream.h"
#include "xpath_stack.h"

namespace TinyXPath
{

/// Exceptions
class execution_error {public : execution_error (int i_) {i=i_;} int i;};
class error_not_yet : public execution_error {public : error_not_yet () : execution_error (-2){}};

/// XPath execution class
class xpath_processor : public xpath_stream
{
public :
   /// Constructor
   xpath_processor (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);
   virtual ~ xpath_processor () {}
   expression_result er_compute_xpath ();
   TIXML_STRING S_compute_xpath ();
   int i_compute_xpath ();
   bool o_compute_xpath ();
   double d_compute_xpath ();
   unsigned u_compute_xpath_node_set ();
   void v_get_xpath_base (unsigned u_order, const TiXmlBase * & XBp_res, bool & o_attribute);
   TiXmlNode * XNp_get_xpath_node (unsigned u_order);
   TiXmlAttribute * XAp_get_xpath_attribute (unsigned u_order);
   enum {e_no_error, e_error_syntax, e_error_overflow, e_error_execution, e_error_stack} e_error;

protected :
   virtual void v_action (xpath_construct , unsigned , unsigned , const char * );
   virtual int i_get_action_counter ();

   xpath_stack xs_stack;
   action_store as_action_store;
   void v_execute_stack ();
   void v_pop_one_action (xpath_construct & xc_action, unsigned & u_sub, unsigned & u_ref, TIXML_STRING & S_literal);
   void v_execute_one (xpath_construct xc_rule, bool o_skip_only);

   void v_execute_absolute_path (unsigned u_action_position, bool o_with_rel, bool o_everywhere);
   void v_execute_step (int & i_relative_action, bool o_skip_only);
   bool o_check_predicate (const TiXmlElement * XEp_child, bool o_by_name);

   void v_execute_function (TIXML_STRING & S_name, unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_ceiling (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_concat (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_contains (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_count (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_false (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_floor (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_last (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_name (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_normalize_space (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_not (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_position (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_starts_with (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_string_length (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_substring (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_sum (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_text (unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_translate(unsigned u_nb_arg, expression_result ** erpp_arg);
   void v_function_true (unsigned u_nb_arg, expression_result ** erpp_arg);

   void v_function_equal (expression_result ** erpp_arg);
   void v_function_not_equal (expression_result ** erpp_arg);
   void v_function_equal_node_and_other (expression_result * erp_node, expression_result * erp_non);
   void v_function_equal_2_node (expression_result * erp_node_set_, expression_result * erp_node_set_2);
   void v_function_union (node_set & ns_1, node_set & ns_2);
   void v_function_minus (expression_result ** erpp_arg);
   void v_function_plus (expression_result ** erpp_arg);
   void v_function_or (expression_result ** erpp_arg);
   void v_function_and (expression_result ** erpp_arg);
   void v_function_relational (expression_result ** erpp_arg, unsigned u_sub);
   void v_function_opposite ();
   void v_function_mult (expression_result ** erpp_arg, unsigned u_sub);

   void v_push_int (int i_val, const char * cp_comment = "")   {xs_stack . v_push_int (i_val, cp_comment);}
   void v_push_string (TIXML_STRING S_val)   {xs_stack . v_push_string (S_val);}
   void v_push_bool (bool o_val)   {xs_stack . v_push_bool (o_val);}
   void v_push_double (double d_val)   {xs_stack . v_push_double (d_val);}
   void v_push_node_set (node_set * nsp_val)   {xs_stack . v_push_node_set (nsp_val);}

   bool o_pop_bool () {return xs_stack . o_pop_bool ();}
   int i_pop_int () {return xs_stack . i_pop_int ();}
   TIXML_STRING S_pop_string () {return xs_stack . S_pop_string ();}
   node_set ns_pop_node_set () {return xs_stack . ns_pop_node_set ();}

   void v_set_context (const TiXmlElement * XEp_in, bool o_by_name);
   const TiXmlElement * XEp_get_context () {return XEp_context;}
   /// Current context
   const TiXmlElement * XEp_context;
   /// The result of the XPath evaluation, for further node retrieving by v_get_xpath_base
   expression_result er_result;
   bool o_is_context_by_name;

   /// Base node
   const TiXmlNode * XNp_base;
   /// Node above the caller:    
   const TiXmlNode * XNp_base_parent;
   void v_build_root ();
} ;

}

#endif
