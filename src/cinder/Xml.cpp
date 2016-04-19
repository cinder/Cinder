/*
 Copyright (c) 2010, The Cinder Project
 All rights reserved.
 
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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
#include "cinder/Utilities.h"
#include <boost/algorithm/string.hpp>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace std;

namespace cinder {

void parseItem( const rapidxml::xml_node<> &node, XmlTree *parent, XmlTree *result, const XmlTree::ParseOptions &parseOptions );

namespace {
bool tagsMatch( const std::string &tag1, const std::string &tag2, bool caseSensitive )
{
	if( caseSensitive && ( tag1 == tag2 ) )
		return true;
	else if( ( ! caseSensitive ) && ( boost::iequals( tag1, tag2 ) ) )
		return true;
	else
		return false;
}
} // anonymous namespace

XmlTree::ConstIter::ConstIter( const Container *sequence )
{
	mSequenceStack.push_back( sequence );
	mIterStack.push_back( sequence->begin() );
}

XmlTree::ConstIter::ConstIter( const Container *sequence, Container::const_iterator iter )
{
	mSequenceStack.push_back( sequence );
	mIterStack.push_back( iter );
}

XmlTree::ConstIter::ConstIter( const XmlTree &root, const string &filterPath, bool caseSensitive, char separator )
	: mCaseSensitive( caseSensitive )
{
	mFilter = split( filterPath, separator );

	// we ignore a leading separator so that "/one/two" is equivalent to "one/two"
	if( ( ! filterPath.empty() ) && ( filterPath[0] == separator ) && ( ! mFilter.empty() ) )
		mFilter.erase( mFilter.begin() );

	if( mFilter.empty() ) { // empty filter means nothing matches
		setToEnd( &root.getChildren() );
		return;
	}	

	for( vector<string>::const_iterator filterComp = mFilter.begin(); filterComp != mFilter.end(); ++filterComp ) {
		if( mIterStack.empty() ) // first item
			mSequenceStack.push_back( &root.getChildren() );
		else
			mSequenceStack.push_back( &(*mIterStack.back())->getChildren() );
		
		Container::const_iterator child = findNextChildNamed( *mSequenceStack.back(), mSequenceStack.back()->begin(), *filterComp, mCaseSensitive );
		if( child != (mSequenceStack.back())->end() )
			mIterStack.push_back( child );
		else { // failed to find an item that matches this part of the filter; mark as finished and return
			setToEnd( &root.getChildren() );
			return;
		}
	}
}

// sets the iterator to be pointing to the end, meaning iteration is done
void XmlTree::ConstIter::setToEnd( const Container *seq )
{
	mSequenceStack.clear();
	mSequenceStack.push_back( seq );
	mIterStack.clear();
	mIterStack.push_back( seq->end() );
}

bool XmlTree::ConstIter::isDone() const
{
	return ( mSequenceStack.size() == 1 ) && ( mIterStack.back() == mSequenceStack[0]->end() );
}

void XmlTree::ConstIter::increment()
{
	++mIterStack.back();
	
	if( ! mFilter.empty() ) {
	
		bool found = false;
		do {
			Container::const_iterator next = findNextChildNamed( *mSequenceStack.back(), mIterStack.back(), mFilter[mSequenceStack.size()-1], mCaseSensitive );
			if( next == mSequenceStack.back()->end() ) { // we've finished this part of the sequence stack
				if( mSequenceStack.size() > 1 ) { // we might already be done, in which case incrementing would be bad
					mIterStack.pop_back();
					mSequenceStack.pop_back();
					++mIterStack.back(); // next in the new top, which was formerly top-1
				}
				else
					mIterStack[0] = mSequenceStack.back()->end(); // all done
			}
			else if( mSequenceStack.size() < mFilter.size() ) { // we're not on a leaf, so push this onto the stack
				mIterStack[mIterStack.size()-1] = next;
				mSequenceStack.push_back( &(*next)->getChildren() );
				mIterStack.push_back( (*next)->getChildren().begin() );
			}
			else {
				mIterStack[mIterStack.size()-1] = next;
				found = true;
			}
		} while( ( ! isDone() ) && ( ! found ) );
	}
}

XmlTree::Container::const_iterator XmlTree::findNextChildNamed( const Container &sequence, Container::const_iterator firstCandidate, const string &searchTag, bool caseSensitive )
{
	Container::const_iterator result = firstCandidate;
	while( result != sequence.end() ) {
		if( tagsMatch( (*result)->getTag(), searchTag, caseSensitive ) )
			break;
		else
			++result;
	}
	return result;
}

XmlTree::XmlTree( const XmlTree &rhs )
	: mNodeType( rhs.mNodeType ), mTag( rhs.mTag ), mValue( rhs.mValue ), mDocType( rhs.mDocType ),
	 mParent( 0 ), mAttributes( rhs.mAttributes )
{
	for( XmlTree::ConstIter childIt = rhs.begin(); childIt != rhs.end(); ++childIt ) {
		mChildren.push_back( unique_ptr<XmlTree>( new XmlTree( *childIt ) ) );
		mChildren.back()->mParent = this;
	}
}

XmlTree& XmlTree::operator=( const XmlTree &rhs )
{
	mNodeType = rhs.mNodeType;
	mTag = rhs.mTag;
	mValue = rhs.mValue;
	mDocType = rhs.mDocType;
	mParent = 0;
	mAttributes = rhs.mAttributes;

	mChildren.clear();

	for( XmlTree::ConstIter childIt = rhs.begin(); childIt != rhs.end(); ++childIt ) {
		mChildren.push_back( unique_ptr<XmlTree>( new XmlTree( *childIt ) ) );
		mChildren.back()->mParent = this;
	}
	
	return *this;
}

XmlTree::XmlTree( const std::string &xmlString, ParseOptions parseOptions )
{
	std::string strCopy( xmlString );
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
			case rapidxml::node_data: {
				if( ! options.getIgnoreDataChildren() )
					type = XmlTree::NODE_DATA;
				else
					continue;
			}
			break;
			default:
				continue;
		}
		
		result->getChildren().push_back( unique_ptr<XmlTree>( new XmlTree ) );
		parseItem( *item, result, result->getChildren().back().get(), options );
		result->getChildren().back()->setNodeType( type );
	}

	for( rapidxml::xml_attribute<> *attr = node.first_attribute(); attr; attr = attr->next_attribute() )
		result->getAttributes().push_back( XmlTree::Attr( result, attr->name(), attr->value() ) );
}

void XmlTree::loadFromDataSource( DataSourceRef dataSource, XmlTree *result, const XmlTree::ParseOptions &parseOptions )
{
	auto buf = dataSource->getBuffer();
	size_t dataSize = buf->getSize();
	unique_ptr<char[]> bufString( new char[dataSize+1] );
	memcpy( bufString.get(), buf->getData(), buf->getSize() );
	bufString.get()[dataSize] = 0;
	rapidxml::xml_document<> doc;    // character type defaults to char
	if( parseOptions.getParseComments() )
		doc.parse<rapidxml::parse_comment_nodes | rapidxml::parse_doctype_node>( bufString.get() );
	else
		doc.parse<rapidxml::parse_doctype_node>( bufString.get() );
	parseItem( doc, NULL, result, parseOptions );
	result->setNodeType( NODE_DOCUMENT ); // call this after parse - constructor replaces it
}

bool XmlTree::hasChild( const string &relativePath, bool caseSensitive, char separator ) const
{
	return getNodePtr( relativePath, caseSensitive, separator ) != NULL;
}

const XmlTree& XmlTree::getChild( const string &relativePath, bool caseSensitive, char separator ) const
{
	XmlTree* child = getNodePtr( relativePath, caseSensitive, separator );
	if( child )
		return *child;
	else
		throw ExcChildNotFound( *this, relativePath );
}

XmlTree& XmlTree::getChild( const string &relativePath, bool caseSensitive, char separator )
{
	XmlTree* child = getNodePtr( relativePath, caseSensitive, separator );
	if( child )
		return *child;
	else
		throw ExcChildNotFound( *this, relativePath );
}

const XmlTree::Attr& XmlTree::getAttribute( const string &attrName ) const
{
	for( list<Attr>::const_iterator attrIt = mAttributes.begin(); attrIt != mAttributes.end(); ++attrIt )
		if( attrIt->getName() == attrName )
			return *attrIt;
	throw ExcAttrNotFound( *this, attrName );
}

XmlTree& XmlTree::setAttribute( const std::string &attrName, const std::string &value )
{
	list<Attr>::iterator atIt;
	for( atIt = mAttributes.begin(); atIt != mAttributes.end(); ++atIt )
		if( atIt->getName() == attrName )
			break;
	
	if( atIt == mAttributes.end() )
		mAttributes.push_back( Attr( this, attrName, value ) );
	else
		atIt->setValue( value );
		
	return *this;
}

bool XmlTree::hasAttribute( const std::string &attrName ) const
{
	for( list<Attr>::const_iterator atIt = mAttributes.begin(); atIt != mAttributes.end(); ++atIt )
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
	mChildren.push_back( unique_ptr<XmlTree>( new XmlTree( newChild ) ) );
	mChildren.back()->mParent = this;
}

XmlTree* XmlTree::getNodePtr( const string &relativePath, bool caseSensitive, char separator ) const
{
	XmlTree *curNode = const_cast<XmlTree*>( this );

	vector<string> pathComponents = split( relativePath, separator );
	for( vector<string>::const_iterator pathIt = pathComponents.begin(); pathIt != pathComponents.end(); ++pathIt ) {
		if( pathIt->empty() )
			continue;
		Container::const_iterator node = XmlTree::findNextChildNamed( curNode->getChildren(), curNode->getChildren().begin(), *pathIt, caseSensitive )	;
		if( node != curNode->getChildren().end() )
			curNode = const_cast<XmlTree*>( node->get() );
		else
			return 0;
	}

	return curNode;
}

void XmlTree::appendRapidXmlNode( rapidxml::xml_document<char> &doc, rapidxml::xml_node<char> *parent ) const
{
	rapidxml::node_type type;
	switch( getNodeType() ) {
		case XmlTree::NODE_ELEMENT: type = rapidxml::node_element; break;
		case XmlTree::NODE_COMMENT: type = rapidxml::node_comment; break;
		case XmlTree::NODE_CDATA: type = rapidxml::node_cdata; break;
		case XmlTree::NODE_DATA: type = rapidxml::node_data; break;
		
		default: throw ExcUnknownNodeType();
	}
	rapidxml::xml_node<char> *node = 0;
	if( type == rapidxml::node_data ) {
		node = doc.allocate_node( type, NULL, doc.allocate_string( getValue().c_str() ) );
	}
	else if( type == rapidxml::node_comment ) {
		node = doc.allocate_node( type, doc.allocate_string( getTag().c_str() ), doc.allocate_string( getValue().c_str() ) );
	}
	else {
		node = doc.allocate_node( type, doc.allocate_string( getTag().c_str() ), NULL );
		if( ! getValue().empty() )
			node->append_node( doc.allocate_node( rapidxml::node_data, NULL, doc.allocate_string( getValue().c_str() ) ) );
	}
	parent->append_node( node );

	for( list<Attr>::const_iterator attrIt = mAttributes.begin(); attrIt != mAttributes.end(); ++attrIt )
		node->append_attribute( doc.allocate_attribute( doc.allocate_string( attrIt->getName().c_str() ), doc.allocate_string( attrIt->getValue().c_str() ) ) );
		
	for( Container::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt )
		(*childIt)->appendRapidXmlNode( doc, node );
}

shared_ptr<rapidxml::xml_document<char> > XmlTree::createRapidXmlDoc( bool createDocument ) const
{
	shared_ptr<rapidxml::xml_document<char> > result( new rapidxml::xml_document<>() );	
	if( isDocument() || createDocument ) {
		rapidxml::xml_node<char> *declarationNode = result->allocate_node( rapidxml::node_declaration, "", "" );
		result->append_node( declarationNode );
		if( isDocument() && ( ! mDocType.empty() ) )
			result->append_node( result->allocate_node( rapidxml::node_doctype, "", result->allocate_string( mDocType.c_str() ) ) );

		if( isDocument() ) {
			for( Container::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt )
				(*childIt)->appendRapidXmlNode( *result, result.get() );
		}
		else {
			appendRapidXmlNode( *result, result.get() );
		}

		declarationNode->append_attribute( result->allocate_attribute( "version", "1.0" ) );
		declarationNode->append_attribute( result->allocate_attribute( "encoding", "utf-8" ) );
	}
	else
		appendRapidXmlNode( *result, result.get() );
	return result;
}

ostream& operator<<( ostream &out, const XmlTree &xml )
{
	shared_ptr<rapidxml::xml_document<char> > doc = xml.createRapidXmlDoc( false );
	return rapidxml::print( out, *doc, 0 );
}

void XmlTree::write( DataTargetRef target, bool createDocument )
{
	// this could do with a more efficient implementation
	shared_ptr<rapidxml::xml_document<char> > doc = createRapidXmlDoc( createDocument );
	OStreamRef os = target->getStream();
	std::ostringstream ss;
	ss << *doc;
	os->writeData( ss.str().c_str(), ss.str().length() );
}

XmlTree::ExcChildNotFound::ExcChildNotFound( const XmlTree &node, const string &childPath ) throw()
{
#if defined( CINDER_MSW )
	sprintf_s( mMessage, "Could not find child: %s for node: %s", childPath.c_str(), node.getPath().c_str() );
#else
	sprintf( mMessage, "Could not find child: %s for node: %s", childPath.c_str(), node.getPath().c_str() );
#endif
}

XmlTree::ExcAttrNotFound::ExcAttrNotFound( const XmlTree &node, const string &attrName ) throw()
{
#if defined( CINDER_MSW )
	sprintf_s( mMessage, "Could not find attribute: %s for node: %s", attrName.c_str(), node.getPath().c_str() );
#else
	sprintf( mMessage, "Could not find attribute: %s for node: %s", attrName.c_str(), node.getPath().c_str() );
#endif
}

} // namespace cinder