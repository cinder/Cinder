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
   \file xpath_static.h
   \author Yves Berquin
*/

#ifndef __XPATH_STATIC_H
#define __XPATH_STATIC_H

#include "xpath_processor.h"

namespace TinyXPath
{
   // no check static functions
   extern int i_xpath_int (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);
   extern double d_xpath_double (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);
   extern bool o_xpath_bool (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);
   extern TIXML_STRING S_xpath_string (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);
   extern TiXmlNode * XNp_xpath_node (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);
   extern TiXmlAttribute * XAp_xpath_attribute (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr);

   // check static functions
   extern bool o_xpath_int (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr, int & i_res);
   extern bool o_xpath_double (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr, double & d_res);
   extern bool o_xpath_bool (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr, bool & o_res);
   extern bool o_xpath_string (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr, TIXML_STRING & S_res);
   extern bool o_xpath_node (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr, const TiXmlNode * & XNp_node);
   extern bool o_xpath_attribute (const TiXmlNode * XNp_source_tree, const char * cp_xpath_expr, const TiXmlAttribute * & XAp_attrib);
}

#endif
