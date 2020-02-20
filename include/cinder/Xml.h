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
#include "cinder/Utilities.h"

#include <string>
#include <vector>
#include <list>

//! \cond
namespace rapidxml {
	template<class Ch> class xml_document;
	template<class Ch> class xml_node;
};
//! \endcond

namespace cinder {

class CI_API XmlTree {
  public:

	//! \cond
	typedef std::list<std::unique_ptr<XmlTree> > Container;
	//! \endcond

	//! A const iterator over the children of an XmlTree.
	class CI_API ConstIter {
	  public:
		//! \cond
		ConstIter( const Container *sequence );
		ConstIter( const Container *sequence, Container::const_iterator iter );
		ConstIter( const XmlTree &root, const std::string &filterPath, bool caseSensitive = false, char separator = '/' );
		//! \endcond

		//! Returns a reference to the XmlTree the iterator currently points to.
		const XmlTree&		operator*() const { return **mIterStack.back(); }
		//! Returns a pointer to the XmlTree the iterator currently points to.
		const XmlTree*		operator->() const { return &(**mIterStack.back()); }

		//! Increments the iterator to the next child. If using a non-empty filterPath increments to the next child which matches the filterPath.
		ConstIter& operator++() {
			increment();
			return *this;
		}
		
		//! Increments the iterator to the next child. If using a non-empty filterPath increments to the next child which matches the filterPath.
		const ConstIter operator++(int) {
			ConstIter prev( *this );
			++(*this);
			return prev; 
		}
		
		bool operator!=( const ConstIter &rhs ) { return ( mSequenceStack.back() != rhs.mSequenceStack.back() ) || ( mIterStack.back() != rhs.mIterStack.back() ); }
		bool operator==( const ConstIter &rhs ) { return ( mSequenceStack.back() == rhs.mSequenceStack.back() ) && ( mIterStack.back() == rhs.mIterStack.back() ); }
		
	  protected:
		//! \cond
		void	increment();
		void	setToEnd( const Container *seq );
		bool	isDone() const;
		
		std::vector<const Container*>				mSequenceStack;
		std::vector<Container::const_iterator>		mIterStack;
		std::vector<std::string>					mFilter;
		bool										mCaseSensitive;
		//! \endcond		
	};

	//! An iterator over the children of an XmlTree.	
	class CI_API Iter : public XmlTree::ConstIter {
	  public:
		//! \cond
		Iter( Container *sequence )
			: ConstIter( sequence )
		{}
		
		Iter( Container *sequence, Container::iterator iter )
			: ConstIter( sequence, iter )
		{}
	
		Iter( XmlTree &root, const std::string &filterPath, bool caseSensitive, char separator )
			: ConstIter( root, filterPath, caseSensitive, separator )
		{}
		//! \endcond

		
		//! Returns a reference to the XmlTree the iterator currently points to.
		XmlTree&		operator*() const { return const_cast<XmlTree&>(**mIterStack.back()); }
		//! Returns a pointer to the XmlTree the iterator currently points to.
		XmlTree*		operator->() const { return const_cast<XmlTree*>( &(**mIterStack.back()) ); }

		//! Increments the iterator to the next child. If using a non-empty filterPath increments to the next child which matches the filterPath.
		Iter& operator++() {
			increment();
			return *this;
		}
		
		//! Increments the iterator to the next child. If using a non-empty filterPath increments to the next child which matches the filterPath.
		const Iter operator++(int) {
			Iter prev( *this );
			++(*this);
			return prev; 
		}
		
		bool operator!=( const Iter &rhs ) { return ( mSequenceStack.back() != rhs.mSequenceStack.back() ) || ( mIterStack.back() != rhs.mIterStack.back() ); }
		bool operator==( const Iter &rhs ) { return ( mSequenceStack.back() == rhs.mSequenceStack.back() ) && ( mIterStack.back() == rhs.mIterStack.back() ); }

	};

	//! XML attribute.
	class CI_API Attr {
	  public:
		//! Constructs an XML attribute named \a name with the value \a value.
		Attr( XmlTree *xml, const std::string &name, const std::string &value )
			: mXml( xml ), mName( name ), mValue( value )
		{}

		//! Returns an empty string for a non-existent attribute
		operator const std::string&() const { return mValue; }
		//! Assigns the Attr a new value, and creates it if it doesn't exist. The equivalent of calling <tt>setAttribute( this->getName(), toString( newValue ) )</tt>.
		template<typename T>
		Attr&	operator=( const T& val ) { mValue = toString( val ); mXml->setAttribute( mName, mValue ); return *this; }
		
		bool	operator==( const char *rhs ) const { return mValue == rhs; }
		bool	operator==( const std::string &rhs ) const { return mValue == rhs; }
		bool	operator!=( const char *rhs ) const { return mValue != rhs; }
		bool	operator!=( const std::string &rhs ) const { return mValue != rhs; }
		
		//! Returns the value of the attribute cast to T using ci::fromString().
		template<typename T>
		T		as() const { return fromString<T>( mValue ); }
		
		//! Returns true if the Attr value is empty
		bool	empty() const { return mValue.empty(); }
		
		//! Returns the name of the attribute as a string.
		const std::string&		getName() const { return mName; }
		//! Returns the value of the attribute as a string.
		std::string				getValue() const { return mValue; }
		/** \brief Returns the value of the attribute parsed as a T. Requires T to support the istream>> operator. 
		<br><tt>float size = myAttr.getValue<float>( "size" );</tt> **/		
		template<typename T>
		T						getValue() const { return fromString<T>( mValue ); }
		
		/** Sets the value of the attribute to \a value. **/
		void					setValue( const std::string &value ) { mValue = value; }
		/** Sets the value of the attribute to \a value, which is cast to a string first. Requires T to support the ostream<< operator. **/
		template<typename T>
		void					setValue( const T &value ) { mValue = toString<T>( value ); }

	  private:
	  	XmlTree			*mXml;
		std::string		mName, mValue;
	};

	//! Options for XML parsing. Passed to the XmlTree constructor.
	class CI_API ParseOptions {
	  public:
		//! Default options. Disables parsing comments, enables collapsing CDATA, ignores data children.
		ParseOptions() : mParseComments( false ), mCollapseCData( true ), mIgnoreDataChildren( true ) {}
		
		//! Sets whether XML comments are parsed or not.
		ParseOptions& parseComments( bool parse = true ) { mParseComments = parse; return *this; }
		//! Sets whether CDATA blocks are collapsed automatically or not.
		ParseOptions& collapseCData( bool collapse = true ) { mCollapseCData = collapse; return *this; }
		//! Sets whether data nodes are created as children, in addition to being available as the value of the parent. Default true.
		ParseOptions& ignoreDataChildren( bool ignore = true ) { setIgnoreDataChildren( ignore ); return *this; }
		
		//! Returns whether XML comments are parsed or not.
		bool	getParseComments() const { return mParseComments; }
		//! Sets whether XML comments are parsed or not.
		void	setParseComments( bool parseComments = true ) { mParseComments = parseComments; }
		//! Returns whether CDATA blocks are collapsed automatically or not.
		bool	getCollapseCData() const { return mCollapseCData; }
		//! Sets whether CDATA blocks are collapsed automatically or not.
		void	setCollapseCData( bool collapseCData = true ) { mCollapseCData = collapseCData; }
		//! Returns whether data nodes are created as children, in addition to being available as the value of the parent.
		bool	getIgnoreDataChildren() const { return mIgnoreDataChildren; }
		//! Sets whether data nodes are created as children, in addition to being available as the value of the parent.
		void	setIgnoreDataChildren( bool ignore = true ) { mIgnoreDataChildren = ignore; }
		
	  private:
		bool	mParseComments, mCollapseCData, mIgnoreDataChildren;
	};

	//! Enum listing all types of XML nodes understood by the parser.
	typedef enum { NODE_UNKNOWN, NODE_DOCUMENT, NODE_ELEMENT, NODE_CDATA, NODE_COMMENT, NODE_DATA } NodeType;

	//! Default constructor, creating an empty node.
	XmlTree() : mParent( 0 ), mNodeType( NODE_ELEMENT ) {}

	//! Copy constuctor
	XmlTree( const XmlTree &rhs );
	XmlTree& operator=( const XmlTree &rhs );
	
	/** \brief Parses XML contained in \a dataSource using the options \a parseOptions. Commonly used with the results of loadUrl(), loadFile() or loadResource().
		<br><tt>XmlTree myDoc( loadUrl( "http://rss.cnn.com/rss/cnn_topstories.rss" ) );</tt> **/
	explicit XmlTree( DataSourceRef dataSource, ParseOptions parseOptions = ParseOptions() ) {
		loadFromDataSource( dataSource, this, parseOptions );
	}

	//! Parses the XML contained in the string \a xmlString using the options \a parseOptions.
	explicit XmlTree( const std::string &xmlString, ParseOptions parseOptions = ParseOptions() );

	//! Constructs an XML node with the tag \a tag, the value \a value. Optionally sets the pointer to the node's parent and sets the node type.
	explicit XmlTree( const std::string &tag, const std::string &value, XmlTree *parent = 0, NodeType type = NODE_ELEMENT )
		: mTag( tag ), mValue( value ), mParent( parent ), mNodeType( type )
	{}

	//! Returns an XML document node
	static XmlTree		createDoc() { return XmlTree( "", "", 0, NODE_DOCUMENT ); }

	//! Returns the type of this node as a NodeType.
	NodeType					getNodeType() const { return mNodeType; }
	//! Sets the type of this node to NodeType \a type.
	void						setNodeType( NodeType type ) { mNodeType = type; }
	//! Returns whether this node is a document node, meaning it is a root node.
	bool						isDocument() const { return mNodeType == NODE_DOCUMENT; }
	//! Returns whether this node is an element node.
	bool						isElement() const { return mNodeType == NODE_ELEMENT; }
	//! Returns whether this node represents CDATA. Only possible when a document's ParseOptions disabled collapsing CDATA.
	bool						isCData() const { return mNodeType == NODE_CDATA; }
	//! Returns whether this node represents a comment. Only possible when a document's ParseOptions enabled parsing commments.
	bool						isComment() const { return mNodeType == NODE_COMMENT; }

	//! Returns the tag or name of the node as a string.
	const std::string&			getTag() const { return mTag; }
	//! Sets the tag or name of the node to the string \a tag.
	void						setTag( const std::string &tag ) { mTag = tag; }
	
	//! Returns the value of the node as a string.
	std::string					getValue() const { return mValue; }
	//! Returns the value of the node parsed as a T. Requires T to support the istream>> operator.
	template<typename T>
	T							getValue() const { return fromString<T>( mValue ); }
	//! Returns the value of the node parsed as a T. If the value is empty or fails to parse \a defaultValue is returned. Requires T to support the istream>> operator.
	template<typename T>
	T							getValue( const T &defaultValue ) const { try { return fromString<T>( mValue ); } catch( ... ) { return defaultValue; } }

	//! Sets the value of the node to the string \a value.
	void						setValue( const std::string &value ) { mValue = value; }
	//! Sets the value of the node to \a value which is converted to a string first. Requires T to support the ostream<< operator.
	template<typename T>
	void						setValue( const T &value ) { mValue = toString( value ); }

	//! Returns whether this node has a parent node.
	bool						hasParent() const { return mParent != NULL; }
	//! Returns a reference to the node which is the parent of this node.
	XmlTree&					getParent() { return *mParent; }
	//! Returns a reference to the node which is the parent of this node.
	const XmlTree&				getParent() const { return *mParent; }
	
	//! Returns the first child that matches \a relativePath or end() if none matches
	Iter						find( const std::string &relativePath, bool caseSensitive = false, char separator = '/' ) { return Iter( *this, relativePath, caseSensitive, separator ); }
	//! Returns the first child that matches \a relativePath or end() if none matches
	ConstIter					find( const std::string &relativePath, bool caseSensitive = false, char separator = '/' ) const { return ConstIter( *this, relativePath, caseSensitive, separator ); }
	//! Returns whether at least one child matches \a relativePath
	bool						hasChild( const std::string &relativePath, bool caseSensitive = false, char separator = '/' ) const;

	//! Returns the first child that matches \a relativePath. Throws ExcChildNotFound if none matches.
	XmlTree&					getChild( const std::string &relativePath, bool caseSensitive = false, char separator = '/' );
	//! Returns the first child that matches \a relativePath. Throws ExcChildNotFound if none matches.
	const XmlTree&				getChild( const std::string &relativePath, bool caseSensitive = false, char separator = '/' ) const;
	//! Returns a reference to the node's list of children nodes.
	Container&			getChildren() { return mChildren; }
	//! Returns a reference to the node's list of children nodes.
	const Container&	getChildren() const { return mChildren; }

	//! Returns a reference to the node's list of attributes.	
	std::list<Attr>&			getAttributes() { return mAttributes; }
	//! Returns a reference to the node's list of attributes.	
	const std::list<Attr>&		getAttributes() const { return mAttributes; }

	//! Returns a reference to the node attribute named \a attrName. Throws AttrNotFoundExc if no attribute exists with that name.
	const Attr&					getAttribute( const std::string &attrName ) const;

	//! Returns an Attr accessor. If the attribute does not exists its Attr's value will be an empty string.
	const Attr					operator[]( const std::string &attrName ) const {  if( hasAttribute( attrName ) ) return getAttribute(attrName); else return Attr( const_cast<XmlTree*>( this ), attrName, "" ); }
	//! Returns an Attr accessor. If the attribute does not exists its Attr's value will be an empty string. Assigning the Attr is the equivalent of calling setAttribute( attrName ).
	Attr						operator[]( const std::string &attrName ) {  if( hasAttribute( attrName ) ) return getAttribute(attrName); else return Attr( this, attrName, "" ); }

	//! Returns the first child that matches \a childName. Throws ExcChildNotFound if none matches.
	const XmlTree&				operator/( const std::string &childName ) const { return getChild( childName ); }
	//! Returns the first child that matches \a childName. Throws ExcChildNotFound if none matches.
	XmlTree&					operator/( const std::string &childName ) { return getChild( childName ); }

	/** \brief Returns the value of the attribute \a attrName parsed as a T. Throws AttrNotFoundExc if no attribute exists with that name. Requires T to support the istream>> operator. 
		<br><tt>float size = myNode.getAttributeValue<float>( "size" );</tt> **/
	template<typename T>
	T							getAttributeValue( const std::string &attrName ) const { return getAttribute( attrName ).getValue<T>(); }
	/** \brief Returns the value of the attribute \a attrName parsed as a T. Returns \a defaultValue if no attribute exists with that name or the attribute fails to cast to T. Requires T to support the istream>> operator.
		<br><tt>float size = myNode.getAttributeValue<float>( "size", 1.0f );</tt> **/
	template<typename T>
	T							getAttributeValue( const std::string &attrName, const T &defaultValue ) const {
			if( hasAttribute( attrName ) ) {
				try {
					return getAttribute( attrName ).getValue<T>();
				}
				catch(...) {
					return defaultValue;
				}
			}
			else return defaultValue;
	}

	/** Sets the value of the attribute \a attrName to \a value. If the attribute does not exist it is appended. **/
	XmlTree&					setAttribute( const std::string &attrName, const std::string &value );
	/** Sets the value of the attribute \a attrName to \a value, which is cast to a string first. Requires T to support the ostream<< operator. If the attribute does not exist it is appended. **/
	template<typename T>
	XmlTree&					setAttribute( const std::string &attrName, const T &value ) { return setAttribute( attrName, toString( value ) ); }
	/** Returns whether the node has an attribute named \a attrName. **/
	bool						hasAttribute( const std::string &attrName ) const;
	/** Returns a path to this node, separated by the character \a separator. **/	
	std::string					getPath( char separator = '/' ) const;
	
	/** Returns an Iter to the first child node of this node. **/	
	Iter						begin() { return Iter( &mChildren ); }
	/** Returns an Iter to the children node of this node which match the path \a filterPath. **/	
	Iter						begin( const std::string &filterPath, bool caseSensitive = false, char separator = '/' ) { return Iter( *this, filterPath, caseSensitive, separator ); }	
	/** Returns an Iter to the first child node of this node. **/	
	ConstIter					begin() const { return ConstIter( &mChildren ); }
	/** Returns an Iter to the children node of this node which match the path \a filterPath. **/	
	ConstIter					begin( const std::string &filterPath, bool caseSensitive = false, char separator = '/' ) const { return ConstIter( *this, filterPath, caseSensitive, separator ); }	
	/** Returns an Iter which marks the end of the children of this node. **/	
	Iter						end() { return Iter( &mChildren, mChildren.end() ); }
	/** Returns an Iter which marks the end of the children of this node. **/	
	ConstIter					end() const { return ConstIter( &mChildren, mChildren.end() ); }
	/** Appends a copy of the node \a newChild to the children of this node. **/	
	void						push_back( const XmlTree &newChild );

	/** Returns the DOCTYPE string for this node. Only meaningful on a document's root node. **/	
	std::string					getDocType() const { return mDocType; }
	/** Sets the DOCTYPE string for this node. Only meaningful on a document's root node. **/
	void						setDocType( const std::string &docType ) { mDocType = docType; }

	/** Streams the XmlTree \a xml to std::ostream \a out with standard formatting. **/
	friend CI_API std::ostream& operator<<( std::ostream &out, const XmlTree &xml );
	/** Writes this XmlTree to \a target with standard formatting. If \a createDocument is true then an implicit parent NODE_DOCUMENT is created when necessary and \a this is treated as the root element. **/
	void						write( DataTargetRef target, bool createDocument = true );

	//! Base class for XmlTree exceptions.
	class CI_API Exception : public cinder::Exception {
	};
	
	//! Exception expressing the absence of an expected child node.
	class CI_API ExcChildNotFound : public XmlTree::Exception {
	  public:
		ExcChildNotFound( const XmlTree &node, const std::string &childPath ) throw();
	  
		virtual const char* what() const throw() { return mMessage; }
	  
	  private:
		char mMessage[2048];
	};

	//! Exception expressing the absence of an expected attribute.
	class CI_API ExcAttrNotFound : public XmlTree::Exception {
	  public:
		ExcAttrNotFound( const XmlTree &node, const std::string &attrName ) throw();
			  
		virtual const char* what() const throw() { return mMessage; }
	  
	  private:
		char mMessage[2048];
	};

	//! Exception implying an XML node of an unknown type. Implies a low-level problem communicating with RapidXML.
	class CI_API ExcUnknownNodeType : public cinder::Exception {
	};

	//! Returns a shared_ptr to a RapidXML xml_document. If \a createDocument is true then an implicit parent NODE_DOCUMENT is created when necessary and \a this is treated as the root element.
	std::shared_ptr<rapidxml::xml_document<char> >	createRapidXmlDoc( bool createDocument = false ) const;	

  private:
	XmlTree*	getNodePtr( const std::string &relativePath, bool caseSensitive, char separator ) const;
	void		appendRapidXmlNode( rapidxml::xml_document<char> &doc, rapidxml::xml_node<char> *parent ) const;

	static Container::const_iterator	findNextChildNamed( const Container &sequence, Container::const_iterator firstCandidate, const std::string &searchTag, bool caseSensitive );

	NodeType					mNodeType;
  	std::string					mTag;
	std::string					mValue;
	std::string					mDocType; // only used on NodeType::NODE_DOCUMENT
	XmlTree						*mParent;
	Container					mChildren;
	std::list<Attr>				mAttributes;
	
	static void		loadFromDataSource( DataSourceRef dataSource, XmlTree *result, const ParseOptions &parseOptions );
};

CI_API std::ostream& operator<<( std::ostream &out, const XmlTree &xml );

} // namespace cinder

namespace std {

//! \cond
template<>
struct iterator_traits<cinder::XmlTree::Iter> {
	typedef cinder::XmlTree			value_type;
	typedef ptrdiff_t				difference_type;
	typedef forward_iterator_tag	iterator_category;
	typedef cinder::XmlTree*		pointer;
	typedef cinder::XmlTree&		reference;
};

template<>
struct iterator_traits<cinder::XmlTree::ConstIter> {
	typedef cinder::XmlTree			value_type;
	typedef ptrdiff_t				difference_type;
	typedef forward_iterator_tag	iterator_category;
	typedef const cinder::XmlTree*	pointer;
	typedef const cinder::XmlTree&	reference;
};
//! \endcond

} // namespace std
