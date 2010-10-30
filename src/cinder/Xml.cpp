/*
 Copyright (c) 2010, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace std;

namespace cinder {

void parseItem( const rapidxml::xml_node<> &node, XmlTree *parent, XmlTree *result, const XmlTree::ParseOptions &parseOptions );


XmlTree::Iter::Iter( XmlTree &root, const string &filterPath, const char seperator )
{
	mEndSequence = &root.getChildren();
	
	vector<XmlTree>::iterator childIt;
	if( ! root.getChildIterator( filterPath, seperator, &mFilter, &childIt ) ) { // false means couldn't find it - mark ourselves as done
		mIter = mEndSequence->end();
		mSequence = mEndSequence;
	}
	else {
		mIter = childIt;
		mSequence = &(mIter->getParent().getChildren());
	}
}

XmlTree::XmlTree( const std::string &xmlString, ParseOptions parseOptions )
{
	std::vector<char> strCopy( xmlString.begin(), xmlString.end() );
	rapidxml::xml_document<> doc;    // character type defaults to char
	if( parseOptions.getParseComments() )
		doc.parse<rapidxml::parse_comment_nodes | rapidxml::parse_doctype_node>( &strCopy[0] );
	else
		doc.parse<rapidxml::parse_doctype_node>( &strCopy[0] );
	parseItem( doc, NULL, this, parseOptions );
	setNodeType( NODE_DOCUMENT ); // call this after parse - constructor replaces it	
}

void parseItem( const rapidxml::xml_node<> &node, XmlTree *parent, XmlTree *result, const XmlTree::ParseOptions &options )
{
	*result = XmlTree( node.name(), node.value(), parent );
	for( const rapidxml::xml_node<> *item = node.first_node(); item; item = item->next_sibling() ) {
		XmlTree::NodeType type;
		switch( item->type() ) {
			case rapidxml::node_element:
				type = XmlTree::NODE_ELEMENT;
			break;
			case rapidxml::node_cdata: {
				if( options.getCollapseCData() ) {
					result->setValue( result->getValue() + item->value() );
					continue;
				}
				else {
					type = XmlTree::NODE_CDATA;
				}
			}
			break;						
			case rapidxml::node_comment:
				type = XmlTree::NODE_COMMENT;
			break;
			case rapidxml::node_doctype: {
				result->setDocType( item->value() );
				continue;
			}
			default:
				continue;
		}
		
		result->getChildren().push_back( XmlTree() );
		parseItem( *item, result, &result->getChildren().back(), options );
		result->getChildren().back().setNodeType( type );
	}

	for( rapidxml::xml_attribute<> *attr = node.first_attribute(); attr; attr = attr->next_attribute() )
		result->getAttributes().push_back( XmlTree::Attr( attr->name(), attr->value() ) );
}

void XmlTree::loadFromDataSource( DataSourceRef dataSource, XmlTree *result, const XmlTree::ParseOptions &parseOptions )
{
	Buffer buf = dataSource->getBuffer();
	size_t dataSize = buf.getDataSize();
	shared_ptr<char> bufString( new char[dataSize+1], checked_array_deleter<char>() );
	memcpy( bufString.get(), buf.getData(), buf.getDataSize() );
	bufString.get()[dataSize] = 0;
	rapidxml::xml_document<> doc;    // character type defaults to char
	if( parseOptions.getParseComments() )
		doc.parse<rapidxml::parse_comment_nodes | rapidxml::parse_doctype_node>( bufString.get() );
	else
		doc.parse<rapidxml::parse_doctype_node>( bufString.get() );
	parseItem( doc, NULL, result, parseOptions );
	result->setNodeType( NODE_DOCUMENT ); // call this after parse - constructor replaces it
}

bool XmlTree::hasChild( const string &relativePath, char separator ) const
{
	return getNodePtr( relativePath, separator ) != NULL;
}

const XmlTree& XmlTree::getChild( const string &relativePath, char separator ) const
{
	XmlTree* child = getNodePtr( relativePath, separator );
	if( child )
		return *child;
	else
		throw ChildNotFoundExc( *this, relativePath );
}

XmlTree& XmlTree::getChild( const string &relativePath, char separator )
{
	XmlTree* child = getNodePtr( relativePath, separator );
	if( child )
		return *child;
	else
		throw ChildNotFoundExc( *this, relativePath );
}

const XmlTree::Attr& XmlTree::getAttribute( const string &attrName ) const
{
	for( vector<Attr>::const_iterator attrIt = mAttributes.begin(); attrIt != mAttributes.end(); ++attrIt )
		if( attrIt->getName() == attrName )
			return *attrIt;
	throw AttrNotFoundExc( *this, attrName );
}

template<typename T>
T XmlTree::getAttributeValue( const string &attrName, const T &defaultValue ) const
{
	try {
		return getAttribute( attrName ).getValue<T>();
	}
	catch( ... ) {
		return defaultValue;
	}
}

void XmlTree::setAttribute( const std::string &attrName, const std::string &value )
{
	vector<Attr>::iterator atIt;
	for( atIt = mAttributes.begin(); atIt != mAttributes.end(); ++atIt )
		if( atIt->getName() == attrName )
			break;
	
	if( atIt == mAttributes.end() )
		mAttributes.push_back( Attr( attrName, value ) );
	else
		atIt->setValue( value );
}

bool XmlTree::hasAttribute( const std::string &attrName ) const
{
	for( vector<Attr>::const_iterator atIt = mAttributes.begin(); atIt != mAttributes.end(); ++atIt )
		if( atIt->getName() == attrName )
			return true;
	
	return false;
}

string	XmlTree::getPath( char separator ) const
{
	string result;
	
	const XmlTree *node = this;
	while( node ) {
		string nodeName = node->getTag();
		if( node != this )
			nodeName += separator;
		result = nodeName + result;
		node = node->mParent;
	}
		
	return result;
}

void XmlTree::push_back( const XmlTree &newChild )
{
	mChildren.push_back( newChild );
	mChildren.back().mParent = this;
}

XmlTree* XmlTree::getNodePtr( const string &relativePath, char separator ) const
{
	XmlTree *curNode = const_cast<XmlTree*>( this );
	size_t offset = 0;
	do {
		const string curElement = relativePath.substr( offset, relativePath.find( separator, offset ) - offset );
		vector<XmlTree> &children = curNode->getChildren();
		vector<XmlTree>::iterator childIt;
		for( childIt = children.begin(); childIt != children.end(); ++childIt ) {
			if( childIt->getTag() == curElement )
				break;
		}
		if( childIt == children.end() ) // couldn't find this one - return NULL
			return 0;
		curNode = &(*childIt );
		offset = relativePath.find( separator, offset );
		if( offset != string::npos )
			++offset;
	} while ( ( offset != string::npos ) && ( offset != relativePath.length() ) );
	
	return curNode;
}

// walks 'relativePath' and returns an iterator pointing to the first descendant matching
// 'resultFilter' contains the tail of the relativePath
// Returns mChildren.end() on failure
bool XmlTree::getChildIterator( const string &relativePath, char separator, string *resultFilter, vector<XmlTree>::iterator *resultIt )
{
	vector<XmlTree>::iterator curNode = mChildren.end();
	size_t offset = 0;
	
	if( mChildren.empty() )
		return false; // empty - failure
	
	vector<XmlTree> *curChildren( &mChildren );
	do {
		*resultFilter = relativePath.substr( offset, relativePath.find( separator, offset ) - offset );
		vector<XmlTree>::iterator childIt;
		for( childIt = curChildren->begin(); childIt != curChildren->end(); ++childIt )
			if( childIt->getTag() == *resultFilter )
				break;
		if( childIt == curChildren->end() )
			return false; // failure
		curNode = childIt;
		offset = relativePath.find( separator, offset );
		if( offset != string::npos )
			++offset;
		curChildren = &(curNode->getChildren());
	} while( ( offset != string::npos ) && ( offset != relativePath.length() ) );
	
	*resultIt = curNode;
	return true;
}

void XmlTree::appendRapidXmlNode( rapidxml::xml_document<char> &doc, rapidxml::xml_node<char> *parent ) const
{
	rapidxml::node_type type;
	switch( getNodeType() ) {
		case XmlTree::NODE_ELEMENT: type = rapidxml::node_element; break;
		case XmlTree::NODE_COMMENT: type = rapidxml::node_comment; break;
		case XmlTree::NODE_CDATA: type = rapidxml::node_cdata; break;
		
		default: throw UnknownNodeTypeExc();
	}
	rapidxml::xml_node<char> *node = doc.allocate_node( type, doc.allocate_string( getTag().c_str() ), doc.allocate_string( getValue().c_str() ) );
	parent->append_node( node );

	for( vector<Attr>::const_iterator attrIt = mAttributes.begin(); attrIt != mAttributes.end(); ++attrIt )
		node->append_attribute( doc.allocate_attribute( doc.allocate_string( attrIt->getName().c_str() ), doc.allocate_string( attrIt->getValue().c_str() ) ) );
		
	for( vector<XmlTree>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt )
		childIt->appendRapidXmlNode( doc, node );
}

shared_ptr<rapidxml::xml_document<char> > XmlTree::createRapidXmlDoc() const
{
	shared_ptr<rapidxml::xml_document<char> > result( new rapidxml::xml_document<>() );	
	if( isDocument() ) {
		rapidxml::xml_node<char> *declarationNode = result->allocate_node( rapidxml::node_declaration, "", "" );
		result->append_node( declarationNode );
		if( ! mDocType.empty() )
			result->append_node( result->allocate_node( rapidxml::node_doctype, "", result->allocate_string( mDocType.c_str() ) ) );

		for( vector<XmlTree>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt )
			childIt->appendRapidXmlNode( *result, result.get() );

		declarationNode->append_attribute( result->allocate_attribute( "version", "1.0" ) );
		declarationNode->append_attribute( result->allocate_attribute( "encoding", "utf-8" ) );
	}
	else
		appendRapidXmlNode( *result, result.get() );
	return result;
}

ostream& operator<<( ostream &out, const XmlTree &xml )
{
	shared_ptr<rapidxml::xml_document<char> > doc = xml.createRapidXmlDoc();
	return rapidxml::print( out, *doc, 0 );
}

void XmlTree::write( DataTargetRef target )
{
	// this could do with a more efficient implementation
	shared_ptr<rapidxml::xml_document<char> > doc = createRapidXmlDoc();
	OStreamRef os = target->getStream();
	std::ostringstream ss;
	ss << *doc;
	os->writeData( ss.str().c_str(), ss.str().length() );
}

XmlTree::ChildNotFoundExc::ChildNotFoundExc( const XmlTree &node, const string &childPath ) throw()
{
	sprintf( mMessage, "Could not find child: %s for node: %s", childPath.c_str(), node.getPath().c_str() );
}

XmlTree::AttrNotFoundExc::AttrNotFoundExc( const XmlTree &node, const string &attrName ) throw()
{
	sprintf( mMessage, "Could not find attribute: %s for node: %s", attrName.c_str(), node.getPath().c_str() );
}

} // namespace cinder