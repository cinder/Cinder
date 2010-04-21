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
   \file xml_util.cpp
   \author Yves Berquin
*/

#include "xml_util.h"

namespace TinyXPath
{

/// Cardinality in the terms of XPath counts from 1 for the first element
int i_xml_cardinality (
   const TiXmlElement * XEp_elem,      ///< Base element. Must not be null
   bool o_by_name)                     ///< true if we ask for the cardinality for our name only
{
	const TiXmlNode * XNp_parent;
	const TiXmlElement * XEp_child;
   TIXML_STRING S_name; 
	int i_res;

   assert (XEp_elem);
	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
   if (o_by_name)
   {
      S_name = XEp_elem -> Value ();
	   XEp_child = XNp_parent -> FirstChildElement (S_name . c_str ());
	   i_res = 1;
	   while (XEp_child)
	   {
	      if (XEp_child == XEp_elem)
			   return i_res;
		   else
		   {
            i_res++;
			   XEp_child = XEp_child -> NextSiblingElement (S_name . c_str ());
		   }
	   }
   }
   else
   {
	   XEp_child = XNp_parent -> FirstChildElement ();
	   i_res = 1;
	   while (XEp_child)
	   {
	      if (XEp_child == XEp_elem)
			   return i_res;
		   else
		   {
            i_res++;
			   XEp_child = XEp_child -> NextSiblingElement ();
		   }
	   }
   }
	assert (false);
	return -1;
}

/// Family size : Nb of sibling elements (including ourselves)
int i_xml_family_size (
   const TiXmlElement * XEp_elem)   ///< Base element. Must not be null 
{
	const TiXmlElement * XEp_child;
	const TiXmlNode * XNp_parent;
   int i_res;

   assert (XEp_elem);
	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
	XEp_child = XNp_parent -> FirstChildElement ();
   i_res = 0;
	while (XEp_child)
	{
      i_res++;
		XEp_child = XEp_child -> NextSiblingElement ();
	}
	return i_res;
}

}
