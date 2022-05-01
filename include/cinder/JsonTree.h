/*
 Copyright (c) 2018, The Cinder Project
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

#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include "cinder/Exception.h"
#include "cinder/Utilities.h"

#include <string>
#include <list>

namespace Json {
	class Value;
}

namespace cinder {

class CI_API JsonTree {
  public:
	
	//! \cond
	typedef std::list<JsonTree> Container;

	typedef Container::const_iterator ConstIter;
	typedef Container::iterator Iter;
	//! \endcond

	//! Enum listing all types of JSON nodes understood by the parser.
	enum NodeType	{ NODE_UNKNOWN, NODE_NULL, NODE_ARRAY, NODE_OBJECT, NODE_VALUE };

	//! Options for JSON parsing. Passed to the JsonTree constructor.
	class CI_API ParseOptions {
	  public:
		//! Default options. Enables parsing errors.
		ParseOptions();
		//! Sets if JSON parse errors are ignored. Default \c false.
		ParseOptions& ignoreErrors( bool ignore = true );
		//! Returns whether JSON parse errors are ignored.
		bool	getIgnoreErrors() const;
		//! Sets if comments are allowed. Default \c true.
		ParseOptions& allowComments( bool allow = true );
		//! Returns whether comments are allowed or not.
		bool	getAllowComments() const;

	  private:
		bool	mIgnoreErrors, mAllowComments;
	};
	
	//! Options for JSON writing. Passed to the \c write method.
	class CI_API WriteOptions {
	public:
		//! Default options. Indents. Does not create root document.
		WriteOptions();
		//! Sets whether JSON is wrapped in a 'document root' object. Default \c false.
		WriteOptions& createDocument( bool createDocument = true );
		//! Sets whether JSON string is indented. Default \c true.
		WriteOptions& indented( bool indent = true );
		//! whether JSON is wrapped in a 'document root' object.
		bool	getCreateDocument() const;
		//! Returns whether JSON string is indented.
		bool	getIndented() const;
		
	private:
		//! \cond
		bool	mCreateDocument;
		bool	mIndented;
		//! \endcond
		
	};
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//! Creates a null JsonTree.
	explicit JsonTree();
	//! Parses the JSON contained in the string \a xmlString
	JsonTree( const JsonTree &jsonTree );
	/** \brief Parses JSON contained in \a dataSource. Commonly used with the results of loadUrl(), loadFile() or loadResource().
		<br><tt>JsonTree myDoc( loadUrl( "http://search.twitter.com/search.json?q=libcinder&rpp=10&result_type=recent" ) );</tt> **/
	explicit JsonTree( DataSourceRef dataSource, ParseOptions parseOptions = ParseOptions() );
	//! Parses the JSON contained in the string \a jsonString .
	explicit JsonTree( const std::string &jsonString, ParseOptions parseOptions = ParseOptions() );
	//! Creates a JsonTree with key \a key and boolean \a value .
	explicit JsonTree( const std::string &key, bool value );
	//! Creates a JsonTree with key \a key and double \a value .
	explicit JsonTree( const std::string &key, double value );
	//! Creates a JsonTree with key \a key and double \a value cast from float.
	explicit JsonTree( const std::string &key, float value );
	//! Creates a JsonTree with key \a key and int \a value .
	explicit JsonTree( const std::string &key, int value );
	//! Creates a JsonTree with key \a key and string \a value .
	explicit JsonTree( const std::string &key, const std::string &value );
	//! Creates a JsonTree with key \a key and string \a value cast from const char*.
	explicit JsonTree( const std::string &key, const char *value );
	//! Creates a JsonTree with key \a key and uint32_t \a value .
	explicit JsonTree( const std::string &key, uint32_t value );
	//! Creates a JsonTree with key \a key and int64_t \a value .
	explicit JsonTree( const std::string &key, int64_t value );
	//! Creates a JsonTree with key \a key and uint64_t \a value .
	explicit JsonTree( const std::string &key, uint64_t value );
	
	/**! Creates a JsonTree with key \a key and an empty array. **/
	static JsonTree					makeArray( const std::string &key = "" );
	/**! Creates a JsonTree with key \a key as an empty object. **/
	static JsonTree					makeObject( const std::string &key = "" );
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//! Returns the JsonTree as a string with standard formatting.
	std::string						serialize() const;

	//! Returns an Iter to the first child of this node.
	Iter							begin();
	//! Returns a ConstIter to the first child of this node.
	ConstIter						begin() const;
	//! Returns an Iter which marks the end of the children of this node.
	Iter							end();
	//! Returns a ConstIter which marks the end of the children of this node.
	ConstIter						end() const;

	//! Assigns the JsonTree a new value, and creates it if it doesn't exist. 
	JsonTree&						operator=( const JsonTree &jsonTree );
	
	/**! Returns the child at \a relativePath. Throws ExcChildNotFound if none matches. 
		<br><tt>JsonTree node = myNode[ "path.to.child" ];</tt> **/
	JsonTree&						operator[]( const std::string &relativePath );
	/**! Returns the child at \a relativePath. Throws ExcChildNotFound if none matches. 
		<br><tt>JsonTree node = myNode[ "path.to.child" ];</tt> **/
	const JsonTree&					operator[]( const std::string &relativePath ) const;
	//! Returns the child at \a index. Throws ExcChildNotFound if none matches.
	JsonTree&						operator[]( size_t index );
	//! Returns the child at \a index. Throws ExcChildNotFound if none matches.
	const JsonTree&					operator[]( size_t index ) const;
	//! Streams the JsonTree \a json to std::ostream \a out with standard formatting.
	friend CI_API std::ostream&		operator<<( std::ostream &out, const JsonTree &json );
	
	/**! Returns the child at \a relativePath. Throws ExcChildNotFound if none matches. 
		<br><tt>JsonTree node = myNode.getChild( "path.to.child" );</tt> **/
	JsonTree&						getChild( const std::string &relativePath, bool caseSensitive = false, char separator = '.' );
	/**! Returns the child at \a relativePath. Throws ExcChildNotFound if none matches. 
		<br><tt>JsonTree node = myNode.getChild( "path.to.child" );</tt> **/
	const JsonTree&					getChild( const std::string &relativePath, bool caseSensitive = false, char separator = '.' ) const;
	//! Returns the child at \a index. Throws ExcChildNotFound if none matches.
	JsonTree&						getChild( size_t index );
	//! Returns the child at \a index. Throws ExcChildNotFound if none matches.
	const JsonTree&					getChild( size_t index ) const;
	//! Returns a reference to the node's list of children nodes.
	const Container&				getChildren() const;
	//! Returns the number of child nodes.
	size_t							getNumChildren() const	{ return getChildren().size(); }

	/**! Returns whether the child at \a relativePath exists. 
		<br><tt>bool nodeExists = myNode.hasChild( "path.to.child" );</tt> **/
	bool							hasChild( const std::string &relativePath, bool caseSensitive = false, char separator = '.' ) const;
	//! Returns whether this node has a parent node.
	bool							hasChildren() const;

	//! Returns a reference to the node which is the parent of this node.
	JsonTree&						getParent();
	//! Returns a reference to the node which is the parent of this node.
	const JsonTree&					getParent() const;
	//! Returns whether this node has a parent node.
	bool							hasParent() const;

	//! Removes all child nodes
	void							clear();
	/**! Appends a copy of the node \a newChild to the children of this node.  
		If \a this is a value node, it will change to an object or an array. 
		If \a newChild has a key, \a this becomes an object node. 
		If not, \a this becomes an array node. Returns reference to itself. **/
	JsonTree&						addChild( const JsonTree &newChild );
	/**! Appends a copy of the node \a newChild to the children of this node.
		If \a this is a value node, it will change to an object or an array.
		If \a newChild has a key, \a this becomes an object node.
		If not, \a this becomes an array node. **/
	void							pushBack( const JsonTree &newChild );
	//! Removes the child at \a index. Throws ExcChildNotFound if none matches.
	void							removeChild( size_t index );
	//! Removes the child at \a pos. Throws ExcChildNotFound if none matches.
	Iter							removeChild( Iter pos );
	//! Replaces the child at \a index with JsonTree \a newChild. Throws ExcChildNotFound if none matches.
	void							replaceChild( size_t index, const JsonTree &newChild );
	//! Replaces the child at \a pos with JsonTree \a newChild. Throws ExcChildNotFound if none matches.
	void							replaceChild( Iter pos, const JsonTree &newChild );

	/**! Writes this JsonTree to \a path with standard formatting. 
	 If \a writeOptions creates a document then an implicit parent object node is created when necessary and \a this is treated as the root element.
	 If \a writeOptions indents then the JSON string will be indented.**/
	void							write( const fs::path &path, WriteOptions writeOptions = WriteOptions() );
	/**! Writes this JsonTree to \a target. 
		If \a writeOptions creates a document then an implicit parent object node is created when necessary and \a this is treated as the root element.
	    If \a writeOptions indents then the JSON string will be indented.**/
	void							write( DataTargetRef target, WriteOptions writeOptions = WriteOptions() );

	//! Returns the node's key as a string. Returns index if node does not have a key.
	const std::string&				getKey() const;
	
	/**! Returns a path to this node, separated by the character \a separator. **/
	std::string						getPath( char separator = '.' ) const;
	
	/**! \brief Returns the value of the node cast to T using ci::fromString().
		<br><tt>float value = myNode.getValue<float>();</tt> **/
	template <typename T> 
	inline T						getValue() const
	{
		try {
			return fromString<T>( mValue );
		} catch( ... ) {
			throw ExcNonConvertible( *this );
		}
		return (T)0; // Unreachable. Prevents warning.
	}
	//! Returns the value of the child at \a relativePath. Default type \a T is std::string. Convenience shortcut for: \code getChild( relativePath ).getValue<T>() \endcode
	template <typename T>
	inline T						getValueForKey( const std::string &relativePath, bool caseSensitive = false, char separator = '.' ) const 	{ return getChild( relativePath, caseSensitive, separator ).getValue<T>(); }
	//! Returns the value of the child at \a relativePath. Default type \a T is std::string. Convenience shortcut for: \code getChild( index ).getValue<T>() \endcode
	template <typename T>
	inline T						getValueAtIndex( size_t index ) const	{ return getChild( index ).getValue<T>(); }

	//! Returns the value of the node as a string.
	const std::string&				getValue() const { return mValue; }
	//! Returns the value of the child at \a relativePath. Default type \a T is std::string. Convenience shortcut for: \code getChild( relativePath ).getValue<T>() \endcode
	const inline std::string&		getValueForKey( const std::string &relativePath, bool caseSensitive = false, char separator = '.' ) const 	{ return getChild( relativePath, caseSensitive, separator ).getValue(); }
	//! Returns the value of the child at \a relativePath. Default type \a T is std::string. Convenience shortcut for: \code getChild( index ).getValue<T>() \endcode
	const inline std::string&		getValueAtIndex( size_t index ) const	{ return getChild( index ).getValue(); }

	//! Returns the type of the node.
	NodeType						getNodeType() const	{ return mNodeType; }

private:

	//! \cond
	enum ValueType	{ VALUE_BOOL, VALUE_DOUBLE, VALUE_INT, VALUE_STRING, VALUE_UINT	};

	explicit JsonTree( const std::string &key, const Json::Value &value );

	Json::Value						createNativeDoc( WriteOptions writeOptions = WriteOptions() ) const;
	static Json::Value				deserializeNative( const std::string &jsonString, ParseOptions parseOptions );
	static std::string				serializeNative( const Json::Value &value );
   
	void							init( const std::string &key, const Json::Value &value, bool setType = false, 
		NodeType nodeType = NODE_VALUE, ValueType valueType = VALUE_STRING );
	
	JsonTree*						getNodePtr( const std::string &relativePath, bool caseSensitive, char separator ) const;
	static bool						isIndex( const std::string &key );

	std::string						replaceAll( const std::string& text, const std::string& search, const std::string& replace ) const;
	
	Container						mChildren;
	std::string						mKey;
	JsonTree						*mParent;
	NodeType						mNodeType;
	std::string						mValue;
	ValueType						mValueType;
	//! \endcond

  public:

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//! Base class for JsonTree exceptions.
	class CI_API Exception : public cinder::Exception 
	{
	};

	//! Exception expressing the absence of an expected child node.
	class CI_API ExcChildNotFound : public JsonTree::Exception {
	  public:
		ExcChildNotFound( const JsonTree &node, const std::string &key ) throw();
		virtual const char* what() const throw() 
		{ 
			return mMessage; 
		}

	  private:
		char mMessage[ 2048 ];
	};

	//! Exception expressing the inability to convert a node's value to a requested type.
	class CI_API ExcNonConvertible : public JsonTree::Exception {
	  public:
		ExcNonConvertible( const JsonTree &node ) throw();
		virtual const char* what() const throw() 
		{ 
			return mMessage; 
		}

	  private:
		char mMessage[ 2048 ];
	};

	//! Exception expressing the existence of errors when serializing or deserializing JSON.
	class CI_API ExcJsonParserError : public JsonTree::Exception {
	public:
		ExcJsonParserError( const std::string &errorMessage ) throw();
		virtual const char* what() const throw() 
		{ 
			return mMessage; 
		}
		
	private:
		char mMessage[ 2048 ];
	};

};

CI_API std::ostream& operator<<( std::ostream &out, const JsonTree &json );

} // namespace cinder
