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

#pragma once

#include <map>
#include <string>
#include <vector>
#include "cinder/Cinder.h"
#include "cinder/Stream.h"
#include "cinder/DataSource.h"

class TiXmlDocument;
class TiXmlElement;

namespace cinder {

class XmlElement;

class XmlDocument {
  public:
	XmlDocument();
	XmlDocument( const std::string &filePath );
	XmlDocument( std::istream& is );
	XmlDocument( shared_ptr<cinder::IStream> is );
	XmlDocument( DataSourceRef dataSource );
	
	//XmlDocument( const char * str ) : tiDoc( str ) {}
	//XmlDocument( const std::string& str ) : tiDoc( str ) {}
	
	friend std::istream& operator>>( std::istream& in, XmlDocument &base );
	friend std::ostream& operator<<( std::ostream& out, const XmlDocument &base );
	
	XmlElement rootNode() const;
	
	std::vector<XmlElement> xpath( const char *aXpathExpr ) const;
	
  private:
	shared_ptr<TiXmlDocument>	tiDoc;
};

class XmlElement {
  public:
	XmlElement() : tiNode( 0 ) {}
	XmlElement( TiXmlElement * aTiNode ) : tiNode( aTiNode ) {}
	
	std::string name() const;
	
	bool operator==( const XmlElement &rhs ) const
	{
		return ( tiNode == rhs.tiNode );
	}
	
	bool operator!=( const XmlElement &rhs ) const
	{ 
		return ! ( *this == rhs );
	}
	
	bool hasChildren() const;
	
	std::vector<XmlElement> xpath( const char *aXpathExpr ) const;
	XmlElement findChild( const char *aNodeName ) const;
	
	XmlElement parent() const;
	std::vector<XmlElement> children() const;
	
	XmlElement firstChild() const;
	XmlElement lastChild() const;
	
	XmlElement nextSibling() const;
	XmlElement previousSibling() const;
	
	std::string value() const;
	
	std::map<std::string,std::string> attributes() const;
	
	void setAttribute( const char *attrName, const char *attrValue );
	void addChild( const char *name, const char *value = "" );
	
	friend std::ostream& operator<<( std::ostream &out, const XmlElement &base );
  private:
	TiXmlElement *tiNode;
};

} // namespace cinder
