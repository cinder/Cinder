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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Stream.h"
#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include "cinder/Exception.h"

#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

namespace rapidxml {
	template<class Ch> class xml_document;
	template<class Ch> class xml_node;
};

namespace cinder {

class XmlTree {
  public:
	class Iter {
	  public:
		Iter( std::vector<XmlTree> *sequence )
			: mSequence( sequence ), mEndSequence( sequence ), mIter( sequence->begin() ), mFilter( "" )
		{}
		
		Iter( std::vector<XmlTree> *sequence, std::vector<XmlTree>::iterator iter )
			: mSequence( sequence ), mEndSequence( sequence ), mIter( iter ), mFilter( "" )
		{}
		
		Iter( XmlTree &root, const std::string &filterPath, const char seperator = '/' );
		
		const XmlTree&	operator*() const { return *mIter; }
		XmlTree&		operator*() { return *mIter; }
		const XmlTree*	operator->() const { return &(*mIter); }
		XmlTree*		operator->() { return &(*mIter); }
		
		Iter& operator++() {
			if( ! mFilter.empty() ) {
				do {
					++mIter;
				} while( ( mIter != mSequence->end() ) && ( mIter->getTag() != mFilter ) );
				if( mIter == mSequence->end() ) { // true end; went off our sequence, so mark it as done
					mIter = mEndSequence->end();
					mSequence = mEndSequence;
				}
			}
			else
				++mIter;
			return *this;
		}
		
		const Iter operator++(int) {
			Iter prev( *this );
			++(*this);
			return prev; 
		}
		
		bool operator!=( const Iter &rhs ) { return ( mSequence != rhs.mSequence ) || ( mIter != rhs.mIter ); }
		bool operator==( const Iter &rhs ) { return ( mSequence == rhs.mSequence ) && ( mIter == rhs.mIter ); }
		
	  private:
		// mEndSequence* because begin( filterPath ) might send us to a child sequence
		std::vector<XmlTree>			*mSequence, *mEndSequence;
		std::vector<XmlTree>::iterator	mIter;
		std::string						mFilter;
	};

	class Attr {
	  public:
		Attr( const std::string &name, const std::string &value )
			: mName( name ), mValue( value )
		{}
		
		const std::string&		getName() const { return mName; }
		std::string				getValue() const { return mValue; }
		template<typename T>
		T						getValue() const { return boost::lexical_cast<T>( mValue ); }
		
		void					setValue( const std::string &val ) { mValue = val; }
		template<typename T>
		void					setValue( const T &val ) { mValue = boost::lexical_cast<std::string>( val ); }

	  private:
		std::string		mName, mValue;
	};
	
	class ParseOptions {
	  public:
		ParseOptions() : mParseComments( false ), mCollapseCData( true ) {}
		
		ParseOptions& parseComments( bool parse = true ) { mParseComments = parse; return *this; }
		ParseOptions& collapseCData( bool collapse = true ) { mCollapseCData = collapse; return *this; }
		
		bool	getParseComments() const { return mParseComments; }
		void	setParseComments( bool parseComments = true ) { mParseComments = parseComments; }
		bool	getCollapseCData() const { return mCollapseCData; }
		void	setCollapseCData( bool collapseCData = true ) { mCollapseCData = collapseCData; }
		
	  private:
		bool	mParseComments, mCollapseCData;
	};

	typedef enum NodeType { NODE_UNKNOWN, NODE_DOCUMENT, NODE_ELEMENT, NODE_CDATA, NODE_COMMENT };

	XmlTree() : mParent( 0 ), mNodeType( NODE_UNKNOWN ) {}
  
	explicit XmlTree( DataSourceRef dataSource, ParseOptions parseOptions = ParseOptions() ) {
		loadFromDataSource( dataSource, this, parseOptions );
	}

	explicit XmlTree( const std::string &xmlString, ParseOptions parseOptions = ParseOptions() );

	explicit XmlTree( const std::string &tag, const std::string &value, XmlTree *parent = 0, NodeType type = NODE_ELEMENT )
		: mTag( tag ), mValue( value ), mParent( parent )
	{}

	NodeType					getNodeType() const { return mNodeType; }
	void						setNodeType( NodeType type ) { mNodeType = type; }
	bool						isDocument() const { return mNodeType == NODE_DOCUMENT; }
	bool						isElement() const { return mNodeType == NODE_ELEMENT; }
	bool						isCData() const { return mNodeType == NODE_CDATA; }
	bool						isComment() const { return mNodeType == NODE_COMMENT; }

	const std::string&			getTag() const { return mTag; }
	void						setTag( const std::string &tag ) { mTag = tag; }
	
	std::string					getValue() const { return mValue; }
	template<typename T>
	T							getValue() const { return boost::lexical_cast<T>( mValue ); }
	template<typename T>
	T							getValue( const T &defaultValue ) const { try { return boost::lexical_cast<T>( mValue ); } catch( ... ) { return defaultValue; } }

	void						setValue( const std::string &value ) { mValue = value; }
	template<typename T>
	T							setValue( const T &value ) { mValue = boost::lexical_cast<std::string>( value ); }

	bool						hasParent() const { return mParent != NULL; }
	XmlTree&					getParent() { return *mParent; }
	const XmlTree&				getParent() const { return *mParent; }
	
	bool						hasChild( const std::string &relativePath, char separator = '/' ) const;
	
	XmlTree&					getChild( const std::string &relativePath, char separator = '/' );
	const XmlTree&				getChild( const std::string &relativePath, char separator = '/' ) const;
	std::vector<XmlTree>&		getChildren() { return mChildren; }
	const std::vector<XmlTree>&	getChildren() const { return mChildren; }
	
	std::vector<Attr>&			getAttributes() { return mAttributes; }
	const std::vector<Attr>&	getAttributes() const { return mAttributes; }
	
	const Attr&					getAttribute( const std::string &attrName ) const;

	template<typename T>
	T							getAttributeValue( const std::string &attrName ) const { return getAttribute( attrName ).getValue<T>(); }
	template<typename T>
	T							getAttributeValue( const std::string &attrName, const T &defaultValue ) const;
	void						setAttribute( const std::string &attrName, const std::string &value );
	template<typename T>
	void						setAttribute( const std::string &attrName, const T &value ) { setAttribute( attrName, boost::lexical_cast<std::string>( value ) ); }
	
	bool						hasAttribute( const std::string &attrName ) const;
	
	std::string					getPath( char separator = '/' ) const;
	
	Iter						begin() { return Iter( &mChildren ); }
	Iter						begin( const std::string &filterPath, char separator = '/' ) { return Iter( *this, filterPath, separator ); }	
	Iter						end() { return Iter( &mChildren, mChildren.end() ); }
	void						push_back( const XmlTree &newChild );

	std::string					getDocType() const { return mDocType; }
	void						setDocType( const std::string &docType ) { mDocType = docType; }

	friend std::ostream& operator<<( std::ostream &out, const XmlTree &xml );
	void						write( DataTargetRef target );
	
	class Exception : public cinder::Exception {
	};
	
	class ChildNotFoundExc : public XmlTree::Exception {
	  public:
		ChildNotFoundExc( const XmlTree &node, const std::string &childPath ) throw();
	  
		virtual const char* what() const throw() { return mMessage; }
	  
	  private:
		char mMessage[2048];
	};

	class AttrNotFoundExc : public XmlTree::Exception {
	  public:
		AttrNotFoundExc( const XmlTree &node, const std::string &attrName ) throw();
			  
		virtual const char* what() const throw() { return mMessage; }
	  
	  private:
		char mMessage[2048];
	};

	class UnknownNodeTypeExc : public cinder::Exception {
	};

	std::shared_ptr<rapidxml::xml_document<char> >	createRapidXmlDoc() const;	

  private:
	XmlTree*	getNodePtr( const std::string &relativePath, char separator ) const;
	bool		getChildIterator( const std::string &relativePath, char separator, std::string *resultFilter, std::vector<XmlTree>::iterator *resultIt );
	void		appendRapidXmlNode( rapidxml::xml_document<char> &doc, rapidxml::xml_node<char> *parent ) const;

	NodeType					mNodeType;
  	std::string					mTag;
	std::string					mValue;
	std::string					mDocType; // only used on NodeType::NODE_DOCUMENT
	XmlTree						*mParent;
	std::vector<XmlTree>		mChildren;
	std::vector<Attr>			mAttributes;
	
	static void		loadFromDataSource( DataSourceRef dataSource, XmlTree *result, const ParseOptions &parseOptions );
};

std::ostream& operator<<( std::ostream &out, const XmlTree &xml );


} // namespace cinder
