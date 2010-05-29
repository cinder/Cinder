/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/Xml.h"
#include "xpath_processor.h"
#include "tinyxml.h"

namespace cinder {

XmlDocument::XmlDocument()
	: tiDoc( new TiXmlDocument() )
{}

XmlDocument::XmlDocument( const std::string &filePath )
	: tiDoc( new TiXmlDocument() )
{
	tiDoc->Parse( filePath.c_str() );
}

XmlDocument::XmlDocument( std::istream &is )
	: tiDoc( new TiXmlDocument() )
{
	is >> *tiDoc;
}

XmlDocument::XmlDocument( shared_ptr<cinder::IStream> is )
	: tiDoc( new TiXmlDocument() )
{
	cinder_istream aStdIstream( is );
	aStdIstream >> *tiDoc;
}

XmlDocument::XmlDocument( DataSourceRef dataSource )
	: tiDoc( new TiXmlDocument() )
{
	cinder_istream aStdIstream( dataSource->createStream() );
	aStdIstream >> *tiDoc;
}

std::istream& operator>>( std::istream &in, XmlDocument &base )
{
	//TiXmlDocument aDoc = base.tinyXmlDoc();
	in >> *(base.tiDoc);
	return in;
}

std::ostream& operator<<( std::ostream &out, const XmlDocument &base )
{
	out << *(base.tiDoc);
	return out;
}

XmlElement XmlDocument::rootNode() const 
{ 
	TiXmlElement * node = tiDoc->FirstChildElement();
	if ( node ) {
		return XmlElement( node );
	}
	return XmlElement( 0 );
}

std::vector<XmlElement> XmlDocument::xpath( const char * aXpathExpr ) const
{
	return rootNode().xpath( aXpathExpr );
}

//////////////////////////////////////////////////////////////////////////////////////
// XmlElement
std::string XmlElement::name() const
{
	if ( tiNode ) {
		return tiNode->ValueStr(); 
	}
	return std::string("");
}

bool XmlElement::hasChildren() const
{
	if ( tiNode ) {
		const TiXmlElement * child = tiNode->FirstChildElement();
		if( child ) {
			return true;
		}
	}
	return false;
}


std::vector<XmlElement> XmlElement::xpath( const char *aXpathExpr ) const
{
	std::vector<XmlElement> v;
	if( tiNode ) {
		TinyXPath::xpath_processor aProcessor( tiNode, aXpathExpr );
		//TinyXPath::expression_result aExpressionResult = aProcessor.er_compute_xpath(); 
		//TinyXPath::node_set * aResult = aExpressionResult.nsp_get_node_set(); 
		
		unsigned int count = aProcessor.u_compute_xpath_node_set();
		for( unsigned int i = 0; i < count; i++ ) {
			TiXmlNode * aNode = aProcessor.XNp_get_xpath_node( i );
			if( aNode->ToElement() ) {
				v.push_back( XmlElement( aNode->ToElement() ) );
			}
		}
	}
	return v;
}

XmlElement XmlElement::findChild( const char *aNodeName ) const
{
	if( tiNode ) {
		TiXmlElement * child = tiNode->FirstChildElement( aNodeName );
		if( child ) {
			return XmlElement( child );
		}
	}
	return XmlElement( 0 );
}

XmlElement XmlElement::parent() const
{
	if( tiNode ) {
		TiXmlNode * parent = tiNode->Parent();
		if ( parent && parent->ToElement() ) {
			return XmlElement( parent->ToElement() );
		}
	}
	return XmlElement( 0 );
}

std::vector<XmlElement> XmlElement::children() const
{
	std::vector<XmlElement> children;
	for(TiXmlElement * child = tiNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
		children.push_back( XmlElement( child ) );
	}
	
	return children;
}

XmlElement XmlElement::firstChild() const
{
	if( tiNode ) {
		TiXmlElement * child = tiNode->FirstChildElement();
		if ( child ) {
			return XmlElement( child );
		}
	}
	return XmlElement( 0 );
}

XmlElement XmlElement::lastChild() const
{
	if( tiNode ) {
		for( TiXmlNode * child = tiNode->LastChild(); child != NULL; child = child->PreviousSibling() ) {
			if ( child->ToElement() ) {
				return XmlElement( child->ToElement() );
			}
		}
	}
	return XmlElement( 0 );
}

XmlElement XmlElement::previousSibling() const
{
	if( tiNode ) {
		for( TiXmlNode * sib = tiNode->PreviousSibling(); sib != NULL; sib = sib->PreviousSibling() ) {
			if ( sib->ToElement() ) {
				return XmlElement( sib->ToElement() );
			}
		}
	}
	return XmlElement( 0 );
}

XmlElement XmlElement::nextSibling() const
{
	if( tiNode ) {
		TiXmlElement * sib = tiNode->NextSiblingElement();
		if( sib ) {
			return XmlElement( sib );
		}
	}
	return XmlElement( 0 );
}

std::string XmlElement::value() const
{
	if( tiNode ) {
		TiXmlPrinter printer;
		printer.SetStreamPrinting();
		for( const TiXmlNode * child = tiNode->FirstChild(); child != NULL; child = child->NextSibling() ) {
			child->Accept( &printer );
		}
		return printer.Str();
	}
	return std::string();
}

std::map<std::string, std::string> XmlElement::attributes() const {
	std::map<std::string, std::string> attrs;
	if( tiNode ) {
		for( TiXmlAttribute * attr = tiNode->FirstAttribute(); attr; attr = attr->Next() ) {
			attrs[attr->Name()] = attr->Value();
		}
	}
	return attrs;
}

void XmlElement::setAttribute( const char * attrName, const char * attrValue )
{
	if ( tiNode ) {
		tiNode->SetAttribute( attrName, attrValue );
	}
}

void XmlElement::addChild( const char * name, const char * value )
{
	//TODO
}

std::ostream& operator<<( std::ostream &out, const XmlElement &base )
{
/*	out << "<" << base.name() << ">" << std::endl;
	for( TiXmlElement *child = base.tiNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
		if( child )
			out << XmlElement( child ) << std::endl;
	}
	out << base.value();
	out << "</" << base.name() << ">";*/
	out << *(base.tiNode);

	return out;
}

} // namespace cinder