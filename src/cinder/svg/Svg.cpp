/*
 Copyright (c) 2012, The Cinder Project
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

#include "cinder/svg/Svg.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Base64.h"
#include "cinder/Text.h"
#include "cinder/Log.h"
#include "cinder/Unicode.h"

using namespace std;

namespace cinder { namespace svg {


namespace {


bool isNumeric( char c )
{
	return ( c >= '0' && c <= '9' ) || c == '.' || c == '-' || c == 'e' || c == 'E' || c == '+';
}

float parseFloat( const char **sInOut )
{
	char temp[256];
	size_t i = 0;
	const char *s = *sInOut;
	while( *s && (isspace(*s) || *s == ',') )
		s++;
	if( ! s )
		throw PathParseExc();
	if( isNumeric( *s ) ) {
		while( *s == '-' || *s == '+' ) {
			if( i < sizeof(temp) )
				temp[i++] = *s;
			s++;
		}
		bool parsingExponent = false, startingExponent;
		bool seenDecimal = false;
		while( *s && ( parsingExponent || (*s != '-' && *s != '+')) && isNumeric(*s) ) {
			startingExponent = false;
			if( *s == '.' && seenDecimal )
				break;
			else if( *s == '.' )
				seenDecimal = true;
			if( i < sizeof(temp) )
				temp[i++] = *s;
			if( *s == 'e' || *s == 'E' ) {
				parsingExponent = true;
				startingExponent = true;
			}
			else
				parsingExponent = false;
			s++;
		}
		if( startingExponent ) { // if we got a false positive on an exponent, for example due to "ex" or "em" unit, back up one
			--i;
			--s;
		}
		temp[i] = 0;
		float result = (float)atof( temp );
		*sInOut = s;
		return result;
	}
	else
		throw FloatParseExc();
}

// parses float from comma-separated parenthetical list
vector<float> parseFloatList( const char **c )
{
	vector<float> result;
	while( **c && isspace( **c ) ) (*c)++;
	if( **c != '(' )
		return result; // failure
	(*c)++;
	do {
		result.push_back( parseFloat( c ) );
		while( **c && ( **c == ',' || isspace( **c ) ) ) (*c)++;
	} while( **c && **c != ')' );
	
	// get rid of trailing closing paren
	if( **c ) (*c)++;
	
	return result;
}

vector<Value> parseValueList( const char **c, bool requireParens = true )
{
	vector<Value> result;
	while( **c && isspace( **c ) ) (*c)++;
	if( requireParens ) {
		if( **c != '(' )
			return result; // failure
		(*c)++;
	}
	do {
		result.push_back( Value::parse( c ) );
		while( **c && ( **c == ',' || isspace( **c ) ) ) (*c)++;
	} while( **c && **c != ')' );
	
	// get rid of trailing closing paren
	if( requireParens && **c ) (*c)++;
	
	return result;
}

vector<Value> readValueList( const std::string &s, bool requireParens = true ) {
	const char *temp = s.c_str();
	return parseValueList( &temp, requireParens );
}

vector<Value> readValueList( const char *c, bool requireParens = true ) {
	const char *temp = c;
	return parseValueList( &temp, requireParens );
}

Value readValue( const std::string &s, float minV, float maxV )
{
	const char *temp = s.c_str();
	Value result = Value::parse( &temp );
	if( result.mValue < minV ) result = minV;
	if( result.mValue > maxV ) result = maxV;
	return result;
}

Value readValue( const std::string &s )
{
	const char *temp = s.c_str();
	Value result = Value::parse( &temp );
	return result;
}

// breaks comma-separated list into strings, optionally strips single or double quotes; removes all leading and trailing white space
vector<string> readStringList( const std::string &s, bool stripQuotes = false )
{
	vector<string> result = ci::split( s, "," );
	for( vector<string>::iterator resultIt = result.begin(); resultIt != result.end(); ++resultIt ) {
		auto trimmed = ci::trim( *resultIt );
		if( stripQuotes ) {
			trimmed.erase( std::remove( trimmed.begin(), trimmed.end(), '"' ), trimmed.end() );
			trimmed.erase( std::remove( trimmed.begin(), trimmed.end(), '\'' ), trimmed.end() );
		}
		*resultIt = trimmed;
	}
	
	return result;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////
// Renderer
void Renderer::setVisitor( const function<bool(const Node&, svg::Style *)> &visitor )
{
	mVisitor = shared_ptr<function<bool(const Node&, svg::Style *)> >( new function<bool(const Node&, svg::Style *)>( visitor ) );
}

////////////////////////////////////////////////////////////////////////////////////
// Statics
namespace {
	const Paint sPaintNone = svg::Paint();
	const Paint sPaintBlack = svg::Paint( Color::black() );
}

////////////////////////////////////////////////////////////////////////////////////
// Paint
Paint::Paint()
	: mType( NONE ), mSpecifiesTransform( false )
{
	mStops.push_back( std::make_pair( 0.0f, ColorA8u::black() ) );
}

Paint::Paint( uint8_t type )
	: mType( type ), mSpecifiesTransform( false )
{
	mStops.push_back( std::make_pair( 0.0f, ColorA8u::black() ) );
}

Paint::Paint( const ColorA8u &color )
	: mType( COLOR ), mSpecifiesTransform( false )
{
	mStops.push_back( std::make_pair( 0.0f, color ) );
}

Paint Paint::parse( const char *value, bool *specified, const Node *parentNode )
{
	*specified = false;
	while( *value && isspace( *value ) )
		value++;
	
	if( ! *value )
		return Paint();
	
	if( ! strncmp( value, "inherit", 7 ) ) {
		*specified = false;
		return Paint();
	}
	if( value[0] == '#' ) { // hex color
		uint32_t v = 0;
		if( strlen( value ) > 4 ) {
			for( int c = 0; c < 6; ++c ) {
				char ch = toupper( value[1+c] );
				uint32_t col = ch - ( ( ch > '9' ) ? ( 'A' - 10 ) : '0' );
				v += col << ( (5-c) * 4 );
			}
		}
		else { // 3-digit hex shorthand; double each digit
			for( int c = 0; c < 3; ++c ) {
				char ch = toupper( value[1+c] );
				uint32_t col = ch - ( ( ch > '9' ) ? ( 'A' - 10 ) : '0' );
				v += col << ( (5-(c*2+0)) * 4 );
				v += col << ( (5-(c*2+1)) * 4 );
			}
		}
		*specified = true;
		return Paint( ColorA8u( v >> 16, ( v >> 8 ) & 255, v & 255, 255 ) );
	}
	else if( ! strncmp( value, "none", 4 ) ) {
		*specified = true;
		return Paint();
	}
	else if( ! strncmp( value, "rgb", 3 ) ) {
		vector<Value> values = readValueList( value + 3 );
		if( values.size() == 3 ) {
			*specified = true;
			return Paint( ColorA8u( (uint8_t)values[0].asUser( 255 ), (uint8_t)values[1].asUser( 255 ), (uint8_t)values[2].asUser( 255 ), 255 ) );
		}
		*specified = false;
		return Paint();
	}
	else if( ! strncmp( value, "url", 3 ) ) {
		char id[1024];
		const char *hash = strchr( value, '#' );
		const char *closeParen = strchr( value, ')' );
		if( ( ! closeParen ) || ( ! hash ) || ( closeParen - hash >= 1024 ) )
			return Paint();
		strncpy( id, hash + 1, closeParen - hash - 1 );
		id[closeParen - hash - 1] = 0;
		*specified = true;
		return parentNode->findPaintInAncestors( id );
	}
	else { // try to find color amongst named colors
		Color8u result = svgNameToRgb( value, specified );
		if( specified )
			return Paint( result );
		else
			return Paint();
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Style
Style::Style()
{
	clear();
}

Style::Style( const XmlTree &xml, const Node *parent )
{
	clear();

	for( list<XmlTree::Attr>::const_iterator attIt = xml.getAttributes().begin(); attIt != xml.getAttributes().end(); ++attIt ) {
		if( attIt->getName() == "style" )
			parseStyleAttribute( attIt->getValue(), parent );
		else
			parseProperty( attIt->getName(), attIt->getValue(), parent );
	}
}

Style Style::makeGlobalDefaults()
{
	Style result;
	
	result.setFill( getFillDefault() );
	result.setStroke( getStrokeDefault() );
	result.setFillOpacity( getFillOpacityDefault() );
	result.setStrokeOpacity( getStrokeOpacityDefault() );
	
	result.setStrokeWidth( getStrokeWidthDefault() );
	result.setFillRule( getFillRuleDefault() );
	result.setLineCap( getLineCapDefault() );
	result.setLineJoin( getLineJoinDefault() );
	
	result.setFontFamilies( getFontFamiliesDefault() );
	result.setFontSize( getFontSizeDefault() );
	result.setFontWeight( getFontWeightDefault() );
	
	result.setVisible( true );
	result.setDisplayNone( false );
	
	return result;
}

void Style::clear()
{
	mSpecifiesFill = mSpecifiesStroke = false;
	mSpecifiesOpacity = mSpecifiesFillOpacity = mSpecifiesStrokeOpacity = false;
	mOpacity = 1.0f;
	mSpecifiesStrokeWidth = false;  	
	mSpecifiesFillRule = false;
	mSpecifiesLineCap = false;
	mSpecifiesLineJoin = false;
	mSpecifiesFontFamilies = mSpecifiesFontSize = mSpecifiesFontWeight = false;
	mSpecifiesVisible = false;
	mDisplayNone = false;
}

const Paint& Style::getFillDefault() 
{ 
	return sPaintBlack; 
}
const Paint& Style::getStrokeDefault() 
{ 
	return sPaintNone; 
}

const std::vector<std::string>&	Style::getFontFamiliesDefault()
{
	static shared_ptr<vector<string> > sDefault;
	if( ! sDefault ) {
		sDefault = shared_ptr<vector<string> >( new vector<string>() );
		sDefault->push_back( "Arial" );
	}
	
	return *sDefault;
}

void Style::parseStyleAttribute( const std::string &stylePropertyString, const Node *parent )
{
	// separate into pairs based on semicolons
	vector<string> valuePairs = split( stylePropertyString, ';' );
	for( vector<string>::const_iterator pairIt = valuePairs.begin(); pairIt != valuePairs.end(); ++pairIt ) {
		vector<string> valuePair = split( *pairIt, ':' );
		if( valuePair.size() != 2 )
			continue;
		parseProperty( valuePair[0], valuePair[1], parent );
	}
}

bool Style::parseProperty( const std::string &key, const std::string &value, const Node *parent )
{
	if( key == "fill" ) {
		mFill = Paint::parse( value.c_str(), &mSpecifiesFill, parent );
		return true;
	}
	else if( key == "stroke" ) {
		mStroke = Paint::parse( value.c_str(), &mSpecifiesStroke, parent );
		return true;
	}
	else if( key == "opacity" ) {
		mOpacity = readValue( value.c_str(), 0, 1 ).asUser();
		mSpecifiesOpacity = true;
		return true;
	}
	else if( key == "fill-opacity" ) {
		mFillOpacity = readValue( value.c_str(), 0, 1 ).asUser();
		mSpecifiesFillOpacity = true;
		return true;
	}
	else if( key == "stroke-opacity" ) {
		mStrokeOpacity = readValue( value.c_str(), 0, 1 ).asUser();
		mSpecifiesStrokeOpacity = true;
		return true;
	}
	else if( key == "stroke-width" ) {
		if( value != "inherit" ) {
			mSpecifiesStrokeWidth = true;
			mStrokeWidth = (float)atof( value.c_str() );
		}
		return true;
	}
	else if( key == "fill-rule" ) {
		if( value == "evenodd" ) {
			mSpecifiesFillRule = true;
			mFillRule = FILL_RULE_EVENODD;
		}
		else if( value == "nonzero" ) {
			mSpecifiesFillRule = true;
			mFillRule = FILL_RULE_NONZERO;
		}
		return true;
	}
	else if( key == "stroke-linecap" ) {
		if( value == "butt" ) {
			mSpecifiesLineCap = true;
			mLineCap = LINE_CAP_BUTT;
		}
		else if( value == "round" ) {
			mSpecifiesLineCap = true;
			mLineCap = LINE_CAP_ROUND;
		}
		else if( value == "square" ) {
			mSpecifiesLineCap = true;
			mLineCap = LINE_CAP_SQUARE;
		}
		return true;
	}
	else if( key == "stroke-linejoin" ) {
		if( value == "miter" ) {
			mSpecifiesLineJoin = true;
			mLineJoin = LINE_JOIN_MITER;
		}
		else if( value == "round" ) {
			mSpecifiesLineJoin = true;
			mLineJoin = LINE_JOIN_ROUND;
		}
		else if( value == "bevel" ) {
			mSpecifiesLineJoin = true;
			mLineJoin = LINE_JOIN_BEVEL;
		}
		return true;
	}
	else if( key == "font-family" ) {
		mSpecifiesFontFamilies = true;
		setFontFamilies( readStringList( value, true ) );
		return true;
	}
	else if( key == "font-size" ) {
		if( ! value.empty() && ( isdigit(value[0]) ) ) { // we don't parse something like font-size:medium
			mSpecifiesFontSize = true;
			setFontSize( readValue( value ) );
		}
		return true;
	}
	else if( key == "font-weight" ) {
		string weightString = ci::trim( value );
		if( isdigit( weightString[0] ) ) {
			int v = atoi( weightString.c_str() );
			if( v > 900 ) v = 900;
			if( v < 100 ) v = 100;
			mFontWeight = FontWeight( static_cast<int>(WEIGHT_100) + ( ( v / 100 ) - 1 ) );
			mSpecifiesFontWeight = true;
		}
		else if( ci::asciiCaseEqual( weightString, "normal" ) ) {
			mFontWeight = WEIGHT_NORMAL;
			mSpecifiesFontWeight = true;
		}
		else if( ci::asciiCaseEqual( weightString, "bold" ) ) {
			mFontWeight = WEIGHT_BOLD;
			mSpecifiesFontWeight = true;
		}
		return true;
	}
	else if( key == "display" ) {
		// we can't handle most of the possiblities yet; only 'none'
		if( value == "none" )
			mDisplayNone = true;
		else
			mDisplayNone = false;
		return true;
	}
	else if( key == "visibility" ) {
		mSpecifiesVisible = true;
		if( value == "hidden" || value == "collapse" )
			mVisible = false;
		else
			mVisible = true;
		return true;
	}
	else
		return false;
}

void Style::startRender( Renderer &renderer, bool isNodeDrawable ) const
{
	if( mSpecifiesFill )
		renderer.pushFill( mFill ); 
	if( mSpecifiesStroke )
		renderer.pushStroke( mStroke );
	if( mSpecifiesOpacity ) {
		// if this node draws, we'll force both fill opacity and stroke opacity to be 'opacity'
		if( isNodeDrawable ) {
			renderer.pushFillOpacity( mOpacity );
			renderer.pushStrokeOpacity( mOpacity );
		}
	}
	else {
		if( mSpecifiesFillOpacity )
			renderer.pushFillOpacity( mFillOpacity );
		if( mSpecifiesStrokeOpacity )
			renderer.pushStrokeOpacity( mStrokeOpacity );
	}
	if( mSpecifiesStrokeWidth )
		renderer.pushStrokeWidth( mStrokeWidth );
	if( mSpecifiesFillRule )
		renderer.pushFillRule( mFillRule );
	if( mSpecifiesLineCap )
		renderer.pushLineCap( mLineCap );
	if( mSpecifiesLineJoin )
		renderer.pushLineJoin( mLineJoin );
}

void Style::finishRender( Renderer &renderer, bool isNodeDrawable ) const
{
	if( mSpecifiesFill )
		renderer.popFill();
	if( mSpecifiesStroke )
		renderer.popStroke();
	if( ( mSpecifiesOpacity && isNodeDrawable ) || ( ( ! mSpecifiesOpacity ) && mSpecifiesFillOpacity ) )
		renderer.popFillOpacity();
	if( ( mSpecifiesOpacity && isNodeDrawable ) || ( ( ! mSpecifiesOpacity ) && mSpecifiesStrokeOpacity ) )
		renderer.popStrokeOpacity();
	if( mSpecifiesStrokeWidth )
		renderer.popStrokeWidth();
	if( mSpecifiesFillRule )
		renderer.popFillRule();		
	if( mSpecifiesLineCap )
		renderer.popLineCap();
	if( mSpecifiesLineJoin )
		renderer.popLineJoin();
}

////////////////////////////////////////////////////////////////////////////////////
// Value
float Value::asUser( float percentOf, float dpi, float fontSize, float fontXHeight ) const
{
	switch( mUnit ) {
		case USER:
		case PX:
			return mValue;
		case PERCENT:
			return mValue * percentOf / 100;
		case PT:
			return mValue * ( dpi / 72 );
		case MM:
			return mValue * dpi / 25.4f; // 25.4mm in an inch
		case CM:
			return mValue * dpi / 2.54f; // 2.54cm in an inch
		case INCH:
			return mValue * dpi;
		case EM:
			return mValue * fontSize;
		case EX:
			return mValue * fontXHeight;
		default:
			throw ValueExc();
	}
}

// Reads the suffix and converts it to user units based on dpi
Value Value::parse( const char **sInOut )
{
	float v = parseFloat( sInOut );
	if( strncmp( *sInOut, "px", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::PX );
	}
	else if( **sInOut == '%' ) {
		*sInOut += 1;
		return Value( v, Value::PERCENT );
	}
	else if( strncmp( *sInOut, "pt", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::PT );
	}	
	else if( strncmp( *sInOut, "pc", 2 ) == 0 ) { // picas
		*sInOut += 2;
		return Value( v, Value::PC );
	}	
	else if( strncmp( *sInOut, "mm", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::MM );
	}
	else if( strncmp( *sInOut, "cm", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::CM );
	}
	else if( strncmp( *sInOut, "in", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::INCH );
	}
	else if( strncmp( *sInOut, "em", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::EM );
	}
	else if( strncmp( *sInOut, "ex", 2 ) == 0 ) {
		*sInOut += 2;
		return Value( v, Value::EX );
	}
	else
		return Value( v, Value::USER );
}

Value Value::parse( const std::string &s )
{
	const char *temp = s.c_str();
	return Value::parse( &temp );
}

////////////////////////////////////////////////////////////////////////////////////
// Node
Node::Node( Node *parent, const XmlTree &xml )
	: mParent( parent ), mStyle( xml, this ), mBoundingBoxCached( false )
{
	mSpecifiesTransform = false;
	mId = xml["id"];
	if( xml.hasAttribute( "transform" ) ) {
		mSpecifiesTransform = true;
		mTransform = parseTransform( xml["transform"] );
	}
	else
		mTransform = mat3();
}

Doc* Node::getDoc() const
{
	const Node *parent = this;
	while( parent && parent->mParent )
		parent = parent->mParent;

	if( typeid(Doc) == typeid(*parent) )
		return const_cast<Doc*>( reinterpret_cast<const Doc*>( parent ) );
	else
		return 0;
}

string Node::getDomPath() const
{
	string result = mId;
	const Node *parent = this;
	while( parent && parent->mParent ) {
		parent = parent->mParent;
		result = parent->getId() + string("/") + result;
	}
	
	return result;
}

const Paint& Node::getFill() const
{
	if( mStyle.specifiesFill() ) return mStyle.getFill();
	else if( mParent ) return mParent->getFill();
	else return Style::getFillDefault();
}

const Paint& Node::getStroke() const
{
	if( mStyle.specifiesStroke() ) return mStyle.getStroke();
	else if( mParent ) return mParent->getStroke();
	else return Style::getStrokeDefault();
}

float Node::getStrokeWidth() const
{
	if( mStyle.specifiesStrokeWidth() ) return mStyle.getStrokeWidth();
	else if( mParent ) return mParent->getStrokeWidth();
	else return Style::getStrokeWidthDefault();
}

float Node::getOpacity() const
{
	if( mStyle.specifiesOpacity() ) return mStyle.getOpacity();
	else if( mParent ) return mParent->getOpacity();
	else return Style::getOpacityDefault();
}

float Node::getFillOpacity() const
{
	if( mStyle.specifiesFillOpacity() ) return mStyle.getFillOpacity();
	else if( mParent ) return mParent->getFillOpacity();
	else return Style::getFillOpacityDefault();
}

float Node::getStrokeOpacity() const
{
	if( mStyle.specifiesStrokeOpacity() ) return mStyle.getStrokeOpacity();
	else if( mParent ) return mParent->getStrokeOpacity();
	else return Style::getStrokeOpacityDefault();
}

FillRule Node::getFillRule() const
{
	if( mStyle.specifiesFillRule() ) return mStyle.getFillRule();
	else if( mParent ) return mParent->getFillRule();
	else return Style::getFillRuleDefault();
}

LineCap	Node::getLineCap() const
{
	if( mStyle.specifiesLineCap() ) return mStyle.getLineCap();
	else if( mParent ) return mParent->getLineCap();
	else return Style::getLineCapDefault();
}

LineJoin Node::getLineJoin() const
{
	if( mStyle.specifiesLineJoin() ) return mStyle.getLineJoin();
	else if( mParent ) return mParent->getLineJoin();
	else return Style::getLineJoinDefault();
}

const vector<string>& Node::getFontFamilies() const
{
	if( mStyle.specifiesFontFamilies() ) return mStyle.getFontFamilies();
	else if( mParent ) return mParent->getFontFamilies();
	else return Style::getFontFamiliesDefault();
}

Value Node::getFontSize() const
{
	if( mStyle.specifiesFontSize() ) return mStyle.getFontSize();
	else if( mParent ) return mParent->getFontSize();
	else return Style::getFontSizeDefault();
}

bool Node::isVisible() const
{
	if( mStyle.specifiesVisible() ) return mStyle.isVisible();
	else if( mParent ) return mParent->isVisible();
	else return true;
}

Paint Node::parsePaint( const char *value, bool *specified, const Node *parentNode )
{
	*specified = false;
	while( *value && isspace( *value ) )
		value++;
	
	if( ! *value )
		return Paint();
	
	if( ! strncmp( value, "inherit", 7 ) ) {
		*specified = false;
		return Paint();
	}
	if( value[0] == '#' ) { // hex color
		uint32_t v = 0;
		if( strlen( value ) > 4 ) {
			for( int c = 0; c < 6; ++c ) {
				char ch = toupper( value[1+c] );
				uint32_t col = ch - ( ( ch > '9' ) ? ( 'A' - 10 ) : '0' );
				v += col << ( (5-c) * 4 );
			}
		}
		else { // 3-digit hex shorthand; double each digit
			for( int c = 0; c < 3; ++c ) {
				char ch = toupper( value[1+c] );
				uint32_t col = ch - ( ( ch > '9' ) ? ( 'A' - 10 ) : '0' );
				v += col << ( (5-(c*2+0)) * 4 );
				v += col << ( (5-(c*2+1)) * 4 );
			}
		}
		*specified = true;
		return Paint( ColorA8u( v >> 16, ( v >> 8 ) & 255, v & 255, 255 ) );
	}
	else if( ! strncmp( value, "none", 4 ) ) {
		*specified = true;
		return Paint();
	}
	else if( ! strncmp( value, "rgb", 3 ) ) {
		vector<Value> values = readValueList( value + 3 );
		if( values.size() == 3 ) {
			*specified = true;
			return Paint( ColorA8u( (uint8_t)values[0].asUser( 255 ), (uint8_t)values[1].asUser( 255 ), (uint8_t)values[2].asUser( 255 ), 255 ) );
		}
		*specified = false;
		return Paint();
	}
	else if( ! strncmp( value, "url", 3 ) ) {
		char id[1024];
		const char *hash = strchr( value, '#' );
		const char *closeParen = strchr( value, ')' );
		if( ( ! closeParen ) || ( ! hash ) || ( closeParen - hash >= 1024 ) )
			return Paint();
		strncpy( id, hash + 1, closeParen - hash - 1 );
		id[closeParen - hash - 1] = 0;
		*specified = true;
		return parentNode->findPaintInAncestors( id );
	}
	else { // try to find color amongst named colors
		Color8u result = svgNameToRgb( value, specified );
		if( specified )
			return Paint( result );
		else
			return Paint();
	}
}

mat3 Node::parseTransform( const std::string &value )
{
	const char *c = value.c_str();
	mat3 curMat;
	mat3 nextMat;
	while( parseTransformComponent( &c, &nextMat ) ) {
		curMat = curMat * nextMat;
	}
	return curMat;
}

bool Node::parseTransformComponent( const char **c, mat3 *result )
{
	// skip leading whitespace
	while( **c && ( isspace( **c ) || ( **c == ',' ) ) )
		(*c)++;
	
	mat3 m;
	if( ! strncmp( *c, "scale", 5 ) ) {
		*c += 5; //strlen( "scale" );
		vector<float> v = parseFloatList( c );
		if( v.size() == 1 ) {
			m = glm::scale( mat3(), vec2( v[0] ) );
		}
		else if( v.size() == 2 ) {
			m = glm::scale( mat3(), vec2( v[0], v[1] ) );
		}
		else
			throw TransformParseExc();
	}
	else if( ! strncmp( *c, "translate", 9 ) ) {
		*c += 9; //strlen( "translate" );
		vector<float> v = parseFloatList( c );
		if( v.size() == 1 )
			m = glm::translate( mat3(), vec2( v[0], 0 ) );
		else if( v.size() == 2 ) {
			m = glm::translate( mat3(), vec2( v[0], v[1] ) );
		}
		else
			throw TransformParseExc();
	}
	else if( ! strncmp( *c, "rotate", 6 ) ) {
		*c += 6; //strlen( "rotate" );
		vector<float> v = parseFloatList( c );
		if( v.size() == 1 ) {
			float a = toRadians( v[0] );
			m = glm::rotate( mat3(), a );
			//m33[0] = math<float>::cos( a ); m33[1] = math<float>::sin( a );
			//m33[3] = -math<float>::sin( a ); m33[4] = math<float>::cos( a );
		}
		else if( v.size() == 3 ) { // rotate around point
			float a = toRadians( v[0] );
			vec2 origin( v[1], v[2] );
			m = glm::translate( mat3(), origin );
			m = glm::rotate( m, a );
			m = glm::translate( m, -origin );
		}
		else
			throw TransformParseExc();
	}
	else if( ! strncmp( *c, "matrix", 6 ) ) {
		*c += 6; //strlen( "matrix" );
		vector<float> v = parseFloatList( c );
		if( v.size() == 6 )
			m = mat3( v[0], v[1], 0, v[2], v[3], 0, v[4], v[5], 1 );
		else
			throw TransformParseExc();
	}
	else if( ! strncmp( *c, "skewX", 5 ) ) {
		*c += 5; //strlen( "skewX" );
		vector<float> v = parseFloatList( c );
		if( v.size() == 1 ) {
			float a = toRadians( v[0] );
			m = glm::shearY2D( mat3(), tan( a ) );
		}
		else
			throw TransformParseExc();
	}
	else if( ! strncmp( *c, "skewY", 5 ) ) {
		*c += 5; //strlen( "skewY" );
		vector<float> v = parseFloatList( c );
		if( v.size() == 1 ) {
			float a = toRadians( v[0] );
			//m33[1] = math<float>::tan( a );
			m = glm::shearX2D( mat3(), tan( a ) );
		}
		else
			throw TransformParseExc();
	}
	else
		return false;

	*result = m;
	return true;		
}

// Parse a 'style' attribute searching for the key 'key', and returning its correspoding value or the empty string if not found
std::string Node::findStyleValue( const std::string &styleString, const std::string &key )
{
	vector<string> valuePairs = split( styleString, ';' );
	for( vector<string>::const_iterator pairIt = valuePairs.begin(); pairIt != valuePairs.end(); ++pairIt ) {
		vector<string> valuePair = split( *pairIt, ':' );
		if( valuePair.size() != 2 )
			continue;
		if( valuePair[0] == key )
			return valuePair[1];
	}
	return string();
}

Style Node::calcInheritedStyle() const
{
	Style result;
	result.setFill( getFill() );
	result.setStroke( getStroke() );
	result.setOpacity( getOpacity() );
	result.setFillOpacity( getFillOpacity() );
	result.setStrokeOpacity( getStrokeOpacity() );
	result.setFillRule( getFillRule() );
	result.setLineCap( getLineCap() );
	result.setLineJoin( getLineJoin() );
	result.setStrokeWidth( getStrokeWidth() );
	result.setFontFamilies( getFontFamilies() );
	result.setFontSize( getFontSize() );
	return result;
}

void Node::render( Renderer &renderer ) const
{
	Style style = calcInheritedStyle();
	if( mParent )
		renderer.pushMatrix( mParent->getTransformAbsolute() );
	
	startRender( renderer, style );
	renderSelf( renderer );
	finishRender( renderer, style );
}

void Node::firstStartRender( Renderer &renderer ) const
{
	renderer.pushFill( getFill() );
}

void Node::startRender( Renderer &renderer, const Style &style ) const
{
	if( mSpecifiesTransform )
		renderer.pushMatrix( mTransform );
	renderer.pushStyle( style );		
	style.startRender( renderer, this->isDrawable() );
}

void Node::finishRender( Renderer &renderer, const Style &style ) const
{
	if( mSpecifiesTransform )
		renderer.popMatrix();
	renderer.popStyle();		
	style.finishRender( renderer, this->isDrawable() );
}

const Node* Node::findInAncestors( const std::string &elementId ) const
{
	if( mId == elementId )
		return this;
	else if( mParent )
		return mParent->findInAncestors( elementId );
	else
		return 0;
}

Paint Node::findPaintInAncestors( const std::string &paintName ) const
{
	const Node *node = findInAncestors( paintName );
	if( ! node )
		return Paint();
	
	if( typeid(LinearGradient) == typeid(*node) ) {
		const LinearGradient *linearGradient = static_cast<const LinearGradient*>( node );
		return linearGradient->asPaint();
	}
	else if( typeid(RadialGradient) == typeid(*node) ) {
		const RadialGradient *radialGradient = static_cast<const RadialGradient*>( node );
		return radialGradient->asPaint();
	}
	else
		return Paint();
}

mat3 Node::getTransformAbsolute() const
{
	mat3 result;
	if( mSpecifiesTransform )
		result = mTransform;
	else
		result = mat3();
	
	const Node *parent = mParent;
	while( parent ) {
		if( parent->specifiesTransform() )
			result = parent->getTransform() * result;
		parent = parent->getParent();
	}
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Gradient
Gradient::Gradient( Node *parent, const XmlTree &xml )
	: Node( parent, xml ), mUseObjectBoundingBox( true ), mSpecifiesTransform( false )
{
	parse( parent, xml );
}

void Gradient::parse( const Node *parent, const XmlTree &xml )
{
	if( xml.hasAttribute( "xlink:href" ) ) {
		string ref = xml.getAttributeValue<string>( "xlink:href" );
		if( ref.size() > 1 ) {
			if( ref[0] == '#' ) {
				string elementId = ref.substr( 1, string::npos );
				const Gradient *referencedGrad = dynamic_cast<const Gradient*>( findInAncestors( elementId ) );
				if( referencedGrad ) {
					copyAttributesFrom( *referencedGrad );
				}
			}
		}
	}
	for( XmlTree::ConstIter stopsIt = xml.begin( "stop" ); stopsIt != xml.end(); ++stopsIt ) {
		mStops.push_back( Stop( parent, *stopsIt ) );
	}
	if( xml.hasAttribute( "gradientUnits" ) )
		mUseObjectBoundingBox = xml.getAttributeValue<string>( "gradientUnits" ) != string("userSpaceOnUse");
	if( xml.hasAttribute( "gradientTransform" ) ) {
		mSpecifiesTransform = true;
		mTransform = parseTransform( xml.getAttributeValue<string>( "gradientTransform" ) );
	}
}

void Gradient::copyAttributesFrom( const Gradient &rhs )
{
	mStops = rhs.mStops;
	mCoords0 = rhs.mCoords0;
	mCoords1 = rhs.mCoords1;
	mUseObjectBoundingBox = rhs.mUseObjectBoundingBox;
	if( rhs.mSpecifiesTransform ) {
		mSpecifiesTransform = true;
		mTransform = rhs.mTransform;
	}
}

Gradient::Stop::Stop( const Node *parent, const XmlTree &xml )
	: mOffset( 0 ), mSpecifiesColor( false ), mSpecifiesOpacity( false )
{
	if( xml.hasAttribute( "offset" ) )
		mOffset = Value::parse( xml.getAttributeValue<string>( "offset" ) ).asUser();
	if( xml.hasAttribute( "stop-color" ) )
		mColor = Node::parsePaint( xml.getAttributeValue<string>( "stop-color" ).c_str(), &mSpecifiesColor, parent ).getColor();
	if( xml.hasAttribute( "stop-opacity" ) ) {
		mSpecifiesOpacity = true;
		mColor.a = (uint8_t)(Value::parse( xml.getAttributeValue<string>( "stop-opacity" ) ).asUser() * 255);
	}
	if( xml.hasAttribute( "style" ) ) {
		string stopColorString = Node::findStyleValue( xml.getAttributeValue<string>( "style" ), "stop-color" );
		if( ! stopColorString.empty() )
			mColor = Node::parsePaint( stopColorString.c_str(), &mSpecifiesColor, parent ).getColor();	
		string stopOpacityString = Node::findStyleValue( xml.getAttributeValue<string>( "style" ), "stop-opacity" );
		if( ! stopOpacityString.empty() ) {
			mColor.a = (uint8_t)(Value::parse( stopOpacityString ).asUser() * 255);
		}
	}
}

Paint Gradient::asPaint() const
{
	Paint result;
	if( ! mStops.empty() ) {
		result.mStops.clear();
		for( vector<Stop>::const_iterator stopIt = mStops.begin(); stopIt != mStops.end(); ++stopIt )
			result.mStops.push_back( make_pair( stopIt->mOffset, stopIt->mColor ) );
	}
	result.mCoords0 = mCoords0;
	result.mCoords1 = mCoords1;
	result.mUseObjectBoundingBox = mUseObjectBoundingBox;
	if( mSpecifiesTransform ) {
		result.mSpecifiesTransform = true;
		result.mTransform = mTransform;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// LinearGradient
LinearGradient::LinearGradient( Node *parent, const XmlTree &xml )
	: Gradient( parent, xml )
{
	parse( xml );
}

void LinearGradient::parse( const XmlTree &xml )
{
	mCoords0.x = xml.getAttributeValue( "x1", 0.0f );
	mCoords0.y = xml.getAttributeValue( "y1", 0.0f );
	mCoords1.x = xml.getAttributeValue( "x2", 1.0f );
	mCoords1.y = xml.getAttributeValue( "y2", 0.0f );
}

Paint LinearGradient::asPaint() const
{
	Paint result = Gradient::asPaint();
	result.mType = Paint::LINEAR_GRADIENT;
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// RadialGradient
RadialGradient::RadialGradient( Node *parent, const XmlTree &xml )
	: Gradient( parent, xml )
{
	parse( xml );
}

void RadialGradient::parse( const XmlTree &xml )
{
	mCoords0.x = xml.getAttributeValue( "cx", 0.5f );
	mCoords0.y = xml.getAttributeValue( "cy", 0.5f );
	mCoords1.x = xml.getAttributeValue( "fx", mCoords0.x );
	mCoords1.y = xml.getAttributeValue( "fy", mCoords0.y );
	mRadius = xml.getAttributeValue( "r", 0.5f );
}

Paint RadialGradient::asPaint() const
{
	Paint result = Gradient::asPaint();
	result.mType = Paint::RADIAL_GRADIENT;
	result.mRadius = mRadius;
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Circle
Circle::Circle( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	mCenter.x = xml.getAttributeValue( "cx", 0.0f );
	mCenter.y = xml.getAttributeValue( "cy", 0.0f );
	mRadius = xml.getAttributeValue( "r", 0.0f );	
}

void Circle::renderSelf( Renderer &renderer ) const
{
	renderer.drawCircle( *this );
}

Shape2d Circle::getShape() const
{
	Shape2d result;
	result.arc( mCenter, mRadius, 0, (float)M_PI * 2 );
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Ellipse
Ellipse::Ellipse( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	mCenter.x = xml.getAttributeValue( "cx", 0.0f );
	mCenter.y = xml.getAttributeValue( "cy", 0.0f );
	mRadiusX = xml.getAttributeValue( "rx", 0.0f );
	mRadiusY = xml.getAttributeValue( "ry", 0.0f );
}

void Ellipse::renderSelf( Renderer &renderer ) const
{
	renderer.drawEllipse( *this );
}

bool Ellipse::containsPoint( const vec2 &pt ) const
{
	float x = (pt.x - mCenter.x) * (pt.x - mCenter.x) / ( mRadiusX * mRadiusX );
	float y = (pt.y - mCenter.y) * (pt.y - mCenter.y) / ( mRadiusY * mRadiusY );
	return x + y < 1;
}

Shape2d	Ellipse::getShape() const
{
	Shape2d result;

	const float magic =	0.552284749830793398402f; // 4/3*(sqrt(2)-1)
	const vec2 offset( mRadiusX * magic, mRadiusY * magic );

	result.moveTo( vec2( mCenter.x - mRadiusX, mCenter.y ) );
	result.curveTo( vec2( mCenter.x - mRadiusX, mCenter.y - offset.y ), vec2( mCenter.x - offset.x, mCenter.y - mRadiusY ), vec2( mCenter.x, mCenter.y - mRadiusY ) );
	result.curveTo( vec2( mCenter.x + offset.x, mCenter.y - mRadiusY ), vec2( mCenter.x + mRadiusX, mCenter.y - offset.y ), vec2( mCenter.x + mRadiusX, mCenter.y ) );
	result.curveTo( vec2( mCenter.x + mRadiusX, mCenter.y + offset.y ), vec2( mCenter.x + offset.x, mCenter.y + mRadiusY ), vec2( mCenter.x, mCenter.y + mRadiusY ) );
	result.curveTo( vec2( mCenter.x - offset.x, mCenter.y + mRadiusY ), vec2( mCenter.x - mRadiusX, mCenter.y + offset.y ), vec2( mCenter.x - mRadiusX, mCenter.y ) );
	result.close();

	return result;
}

////////////////////////////////////////////////////////////////////////////////////
//
void ellipticalArc( Shape2d &path, float x1, float y1, float x2, float y2, float rx, float ry, float xAxisRotation, bool largeArcFlag, bool sweepFlag )
{
	// This is a translation of the  spec section "Elliptical Arc Implementation Notes"
	// http://www.w3.org/TR//implnote.html#ArcImplementationNotes
	float cosXAxisRotation = cosf( xAxisRotation );
	float sinXAxisRotation = sinf( xAxisRotation );
	const vec2 cPrime( cosXAxisRotation * (x2 - x1) * 0.5f + sinXAxisRotation * (y2 - y1) * 0.5f, -sinXAxisRotation * (x2 - x1) * 0.5f + cosXAxisRotation * (y2 - y1) * 0.5f );

	// http://www.w3.org/TR//implnote.html#ArcCorrectionOutOfRangeRadii
	float radiiScale = (cPrime.x * cPrime.x) / ( rx * rx ) + (cPrime.y * cPrime.y) / ( ry * ry );
	if( radiiScale > 1 ) {
		radiiScale = math<float>::sqrt( radiiScale );
		rx *= radiiScale;
		ry *= radiiScale;
	}

	vec2 invRadius( 1.0f / rx, 1.0f / ry );
	vec2 point1 = vec2( cosXAxisRotation * x1 + sinXAxisRotation * y1, -sinXAxisRotation * x1 + cosXAxisRotation * y1 ) * invRadius;
	vec2 point2 = vec2( cosXAxisRotation * x2 + sinXAxisRotation * y2, -sinXAxisRotation * x2 + cosXAxisRotation * y2 ) * invRadius;
	vec2 delta = point2 - point1;
	float d = delta.x * delta.x + delta.y * delta.y;
	if( d <= 0 )
		return;
	
	float theta1, thetaDelta;
	vec2 center;
		
	float s = math<float>::sqrt( std::max<float>( 1 / d - 0.25f, 0 ) );
	if( sweepFlag == largeArcFlag )
		s = -s;

	center = vec2( 0.5f * (point1.x + point2.x) - delta.y * s, 0.5f * (point1.y + point2.y) + delta.x * s );

	theta1 = math<float>::atan2( point1.y - center.y, point1.x - center.x );
	float theta2 = math<float>::atan2( point2.y - center.y, point2.x - center.x );

	thetaDelta = theta2 - theta1;
	if( thetaDelta < 0 && sweepFlag )
		thetaDelta += 2 * (float)M_PI;
	else if( thetaDelta > 0 && ( ! sweepFlag ) )
		thetaDelta -= 2 * (float)M_PI;

	// divide the full arc delta into pi/2 arcs and convert those to cubic beziers
	int segments = (int)(ceilf( fabsf(thetaDelta / ( (float)M_PI / 2 )) ) + 1);
	for( int i = 0; i < segments; ++i ) {
		float thetaStart = theta1 + i * thetaDelta / segments;
		float thetaEnd = theta1 + (i + 1) * thetaDelta / segments;
		float t = (4 / 3.0f) * tanf( 0.25f * (thetaEnd - thetaStart) );
		float sinThetaStart = math<float>::sin( thetaStart );
		float cosThetaStart = math<float>::cos( thetaStart );
		float sinThetaEnd = math<float>::sin( thetaEnd );
		float cosThetaEnd = math<float>::cos( thetaEnd );

		vec2 startPoint = vec2( cosThetaStart - t * sinThetaStart, sinThetaStart + t * cosThetaStart ) + center;
		startPoint = vec2( cosXAxisRotation * startPoint.x * rx - sinXAxisRotation * startPoint.y * ry, sinXAxisRotation * startPoint.x * rx + cosXAxisRotation * startPoint.y * ry );
		vec2 endPoint = vec2( cosThetaEnd, sinThetaEnd ) + center;
		vec2 transformedEndPoint = vec2( cosXAxisRotation * endPoint.x * rx - sinXAxisRotation * endPoint.y * ry, sinXAxisRotation * endPoint.x * rx + cosXAxisRotation * endPoint.y * ry );
		vec2 midPoint = endPoint + vec2( t * sinThetaEnd, -t * cosThetaEnd );
		midPoint = vec2( cosXAxisRotation * midPoint.x * rx - sinXAxisRotation * midPoint.y * ry, sinXAxisRotation * midPoint.x * rx + cosXAxisRotation * midPoint.y * ry );
		path.curveTo( startPoint, midPoint, transformedEndPoint );
    }
}

static const char* getNextPathItem(const char* s, char it[64] )
{
	int i = 0;
	it[0] = '\0';
	// Skip white spaces and commas
	while( *s && (isspace(*s) || *s == ',') )
		s++;
	if( ! *s )
		return s;
	if( isNumeric(*s) ) {
		while( *s == '-' || *s == '+' ) {
			if( i < 63 )
				it[i++] = *s;
			s++;
		}
		bool parsingExponent = false;
		while( *s && ( parsingExponent || (*s != '-' && *s != '+')) && isNumeric(*s) ) {
			if( i < 63 )
				it[i++] = *s;
			if( *s == 'e' || *s == 'E' )
				parsingExponent = true;
			else
				parsingExponent = false;
			s++;
		}
		it[i] = '\0';
	}
	else {
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}

	return s;
}

char readNextCommand( const char **sInOut )
{
	const char *s = *sInOut;
	while( *s && (isspace(*s) || *s == ',') )
		s++;
	*sInOut = s + 1;
	return *s;
}

bool readFlag( const char **sInOut )
{
	const char *s = *sInOut;
	while( *s && ( isspace(*s) || *s == ',' || *s == '-' || *s == '+' ) )
		s++;
	*sInOut = s + 1;		
	return *s != '0';
}

bool nextItemIsFloat( const char *s )
{
	while( *s && (isspace(*s) || *s == ',') )
		s++;
	return isNumeric( *s );
}

Shape2d parsePath( const std::string &p )
{
	const char* s = p.c_str();
	vec2 v0, v1, v2;
	vec2 lastPoint, lastPoint2;

	Shape2d result;
	bool done = false;
	bool firstCmd = true;
	char prevCmd = '\0';
	while( ! done ) {
		char cmd = readNextCommand( &s );
		switch( cmd ) {
			case 'm':
			case 'M':
				v0.x = parseFloat( &s ); v0.y = parseFloat( &s );
				if( ( ! firstCmd ) && ( cmd == 'm' ) )
					v0 += lastPoint;
				result.moveTo( v0 );
				lastPoint2 = lastPoint;
				lastPoint = v0;
				while( nextItemIsFloat( s ) ) {
					v0.x = parseFloat( &s ); v0.y = parseFloat( &s );
					if( cmd == 'm' )
						v0 += lastPoint;
					result.lineTo( v0 );
					lastPoint2 = lastPoint;
					lastPoint = v0;
				} 
			break;
			case 'l':
			case 'L':
				do {
					v0.x = parseFloat( &s ); v0.y = parseFloat( &s );
					if( cmd == 'l' )
						v0 += lastPoint;
					result.lineTo( v0 );
					lastPoint2 = lastPoint;
					lastPoint = v0;
				} while( nextItemIsFloat( s ) );
			break;
			case 'H':
			case 'h':
				do {
					float x = parseFloat( &s );
					v0 = vec2( ( cmd == 'h' ) ? (lastPoint.x + x) : x, lastPoint.y );
					result.lineTo( v0 );
					lastPoint2 = lastPoint;
					lastPoint = v0;
				} while( nextItemIsFloat( s ) );
			break;
			case 'V':
			case 'v':
				do {
					float y = parseFloat( &s );
					v0 = vec2( lastPoint.x, ( cmd == 'v' ) ? (lastPoint.y + y) : (y) );
					result.lineTo( v0 );
					lastPoint2 = lastPoint;
					lastPoint = v0;
				} while( nextItemIsFloat( s ) );
			break;
			case 'C':
			case 'c':
				do {
					v0.x = parseFloat( &s ); v0.y = parseFloat( &s );
					v1.x = parseFloat( &s ); v1.y = parseFloat( &s );
					v2.x = parseFloat( &s ); v2.y = parseFloat( &s );
					if( cmd == 'c' ) { // relative
						v0 += lastPoint; v1 += lastPoint; v2 += lastPoint;
					}
					result.curveTo( v0, v1, v2 );
					lastPoint2 = v1;
					lastPoint = v2;
				} while( nextItemIsFloat( s ) );
			break;
			case 'S':
			case 's':
				do {
					if( prevCmd == 's' || prevCmd == 'S' || prevCmd == 'c' || prevCmd == 'C' )
						v0 = lastPoint * 2.0f - lastPoint2;
					else
						v0 = lastPoint;
					prevCmd = cmd; // set this now in case we loop
					v1.x = parseFloat( &s ); v1.y = parseFloat( &s );
					v2.x = parseFloat( &s ); v2.y = parseFloat( &s );
					if( cmd == 's' ) { // relative
						v1 += lastPoint; v2 += lastPoint;
					}
					result.curveTo( v0, v1, v2 );
					lastPoint2 = v1;
					lastPoint = v2;
				} while( nextItemIsFloat( s ) );				
			break;
			case 'Q':
			case 'q':
				do {
					v0.x = parseFloat( &s ); v0.y = parseFloat( &s );
					v1.x = parseFloat( &s ); v1.y = parseFloat( &s );
					if( cmd == 'q' ) { // relative
						v0 += lastPoint; v1 += lastPoint;
					}
					result.quadTo( v0, v1 );
					lastPoint2 = v0;
					lastPoint = v1;
				} while( nextItemIsFloat( s ) );					
			break;
			case 'T':
			case 't':
				do {
					if( prevCmd == 't' || prevCmd == 'T' || prevCmd == 'q' || prevCmd == 'Q' )
						v0 = lastPoint * 2.0f - lastPoint2;
					else
						v0 = lastPoint;
					prevCmd = cmd; // set this now in case we loop						
					v1.x = parseFloat( &s ); v1.y = parseFloat( &s );
					if( cmd == 't' ) { // relative
						v1 += lastPoint;
					}
					result.quadTo( v0, v1 );
					lastPoint2 = v0;
					lastPoint = v1;
				} while( nextItemIsFloat( s ) );				
			break;
			case 'a':
			case 'A': {
				do {
					float ra = parseFloat( &s );
					float rb = parseFloat( &s );
					float xAxisRotation = parseFloat( &s ) * (float)M_PI / 180.0f;
					bool largeArc = readFlag( &s );
					bool sweepFlag = readFlag( &s );
					v0.x = parseFloat( &s ); v0.y = parseFloat( &s );
					if( cmd == 'a' ) { // relative
						v0 += lastPoint;
					}
					ellipticalArc( result, lastPoint.x, lastPoint.y, v0.x, v0.y, ra, rb, xAxisRotation, largeArc, sweepFlag );
					lastPoint2 = lastPoint;
					lastPoint = v0;
				} while( nextItemIsFloat( s ) );
			}
			break;
			case 'z':
			case 'Z':
				result.close();
				lastPoint2 = lastPoint;
				lastPoint = (result.empty() || result.getContours().back().empty() ) ? vec2() : result.getContours().back().getPoint(0);
			break;
			case '\0':
			default: // technically noise at the end of the string is acceptable according to the spec; see W3C_SVG_11/paths-data-18.svg
				done = true;
			break;
		}
		firstCmd = false;
		prevCmd = cmd;
	}
				
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Path
Path::Path( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	std::string p = xml.getAttributeValue<string>( "d", "" );
	if( ! p.empty() ) {
		mPath = parsePath( p );
	}
}

void Path::appendShape2d( Shape2d *appendTo ) const
{
	for( vector<Path2d>::const_iterator pathIt = mPath.getContours().begin(); pathIt != mPath.getContours().end(); ++pathIt ) {
		appendTo->appendContour( *pathIt );
	}
}

void Path::renderSelf( Renderer &renderer ) const
{
	renderer.drawPath( *this );
}

////////////////////////////////////////////////////////////////////////////////////
// Line
Line::Line( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	mPoint1.x = xml.getAttributeValue<float>( "x1", 0 );
	mPoint1.y = xml.getAttributeValue<float>( "y1", 0 );	
	mPoint2.x = xml.getAttributeValue<float>( "x2", 0 );
	mPoint2.y = xml.getAttributeValue<float>( "y2", 0 );	
}

void Line::renderSelf( Renderer &renderer ) const
{
	renderer.drawLine( *this );
}

Shape2d	Line::getShape() const
{
	Shape2d result;
	result.moveTo( mPoint1 );
	result.lineTo( mPoint2 );
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Rect
Rect::Rect( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	float width = 0, height = 0;
	
	if( xml.hasAttribute( "x" ) )
		mRect.x1 = Value::parse( xml["x"] ).asUser();
	else
		mRect.x1 = 0;
	if( xml.hasAttribute( "y" ) )
		mRect.y1 = Value::parse( xml["y"] ).asUser();
	else
		mRect.y1 = 0;
	if( xml.hasAttribute( "width" ) )
		width = Value::parse( xml["width"] ).asUser();
	if( xml.hasAttribute( "height" ) )
		height = Value::parse( xml["height"] ).asUser();	
	mRect.x2 = mRect.x1 + width;
	mRect.y2 = mRect.y1 + height;
	mBoundingBox = mRect;
}

void Rect::renderSelf( Renderer &renderer ) const
{
	renderer.drawRect( *this );
}

Shape2d	Rect::getShape() const
{
	Shape2d result;
	result.moveTo( mRect.x1, mRect.y1 );
	result.lineTo( mRect.x2, mRect.y1 );
	result.lineTo( mRect.x2, mRect.y2 );
	result.lineTo( mRect.x1, mRect.y2 );
	result.close();
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Polygon
vector<vec2> parsePointList( const std::string &p )
{
	vector<vec2> result;
	
	if( ! p.empty() ) {
		char item[64];
		const char *s = p.c_str();
		bool odd = false;
		float lastVal;
		while( *s ) {
			s = getNextPathItem( s, item );
			if( ! odd )
				lastVal = (float)atof( item );
			else
				result.push_back( vec2( lastVal, (float)atof( item ) ) );
			odd = ! odd;
		}
	}

	return result;
}

Polygon::Polygon( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	mPolyLine = PolyLine2f( parsePointList( xml.getAttributeValue<string>( "points", "" ) ) );
	mPolyLine.setClosed( true );
}

void Polygon::renderSelf( Renderer &renderer ) const
{
	renderer.drawPolygon( *this );
}

Shape2d	Polygon::getShape() const
{
	Shape2d result;

	if( mPolyLine.getPoints().size() <= 1 )
		return result;
	
	result.moveTo( mPolyLine.getPoints()[0] );
	for( vector<vec2>::const_iterator ptIt = mPolyLine.getPoints().begin() + 1; ptIt != mPolyLine.getPoints().end(); ++ptIt )
		result.lineTo( *ptIt );

	result.close();
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Polyline
Polyline::Polyline( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	mPolyLine = PolyLine2f( parsePointList( xml.getAttributeValue<string>( "points", "" ) ) );
	mPolyLine.setClosed( false );
}

void Polyline::renderSelf( Renderer &renderer ) const
{
	renderer.drawPolyline( *this );
}

Shape2d	Polyline::getShape() const
{
	Shape2d result;

	if( mPolyLine.getPoints().size() <= 1 )
		return result;
	
	result.moveTo( mPolyLine.getPoints()[0] );
	for( vector<vec2>::const_iterator ptIt = mPolyLine.getPoints().begin() + 1; ptIt != mPolyLine.getPoints().end(); ++ptIt )
		result.lineTo( *ptIt );
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
// Group
Group::Group( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	parse( xml );
}

Group::~Group()
{
	for( list<Node*>::iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt )
		delete *childIt;
}

void Group::parse( const XmlTree &xml )
{
	for( XmlTree::ConstIter treeIt = xml.begin(); treeIt != xml.end(); ++treeIt ) {
		if( treeIt->getTag() == "g" )
			mChildren.push_back( new Group( this, *treeIt ) );
		else if( treeIt->getTag() == "path" )
			mChildren.push_back( new Path( this, *treeIt ) );
		else if( treeIt->getTag() == "polygon" )
			mChildren.push_back( new Polygon( this, *treeIt ) );
		else if( treeIt->getTag() == "polyline" )
			mChildren.push_back( new Polyline( this, *treeIt ) );
		else if( treeIt->getTag() == "line" )
			mChildren.push_back( new Line( this, *treeIt ) );
		else if( treeIt->getTag() == "rect" )
			mChildren.push_back( new Rect( this, *treeIt ) );
		else if( treeIt->getTag() == "circle" )
			mChildren.push_back( new Circle( this, *treeIt ) );
		else if( treeIt->getTag() == "ellipse" )
			mChildren.push_back( new Ellipse( this, *treeIt ) );
		else if( treeIt->getTag() == "use" )
			mChildren.push_back( new Use( this, *treeIt ) );
		else if( treeIt->getTag() == "defs" )
			mDefs = shared_ptr<Group>( new Group( this, *treeIt ) );
		else if( treeIt->getTag() == "image" )
			mChildren.push_back( new Image( this, *treeIt ) );
		else if( treeIt->getTag() == "linearGradient" )
			mChildren.push_back( new LinearGradient( this, *treeIt ) );
		else if( treeIt->getTag() == "radialGradient" )
			mChildren.push_back( new RadialGradient( this, *treeIt ) );
		else if( treeIt->getTag() == "text" )
			mChildren.push_back( new Text( this, *treeIt ) );
	}
}

const Node* Group::findNodeByIdContains( const std::string &idPartial, bool recurse ) const
{
	for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
		if( (*childIt)->getId().find( idPartial ) != string::npos ) {
			return *childIt;
		}
	}

	if( mDefs ) {
		const Node *result = mDefs->findNodeByIdContains( idPartial, recurse );
		if( result )
			return result;
	}

	if( recurse ) {
		for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
			auto childItPtr = *childIt;
			if( typeid(*childItPtr) == typeid(Group) ) {
				Group* group = static_cast<Group*>(*childIt);
				const Node* result = group->findNodeByIdContains( idPartial );
				if( result )
					return result;
			}
		}
	}
	return NULL;
}

const Node* Group::findNode( const std::string &id, bool recurse ) const
{
	// see if any immediate children are named 'id'
	for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
		if( (*childIt)->getId() == id ) {
			return *childIt;
		}
	}
	
	// see if any members of our defs are named 'id'
	if( mDefs ) {
		const Node *result = mDefs->findNode( id, true );
		if( result )
			return result;
	}

	// see if any groups contain children named 'id'
	if( recurse ) {
		for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
			auto childItPtr = *childIt;
			if( typeid(*childItPtr) == typeid(Group) ) {
				Group* group = static_cast<Group*>(*childIt);
				const Node* result = group->findNode( id );
				if( result )
					return result;
			}
		}
	}
	
	return NULL;
}

Node* Group::nodeUnderPoint( const vec2 &absolutePoint, const mat3 &parentInverseMatrix ) const
{
	mat3 invTransform = parentInverseMatrix;
	if( mSpecifiesTransform )
		invTransform = inverse( mTransform ) * invTransform;
	vec2 localPt = vec2( invTransform * vec3( absolutePoint, 1 ) );
	
	for( list<Node*>::const_reverse_iterator nodeIt = mChildren.rbegin(); nodeIt != mChildren.rend(); ++nodeIt ) {
		auto nodeItPtr = *nodeIt;
		if( typeid(*nodeItPtr) == typeid(svg::Group) ) {
			Node *node = static_cast<svg::Group*>( *nodeIt )->nodeUnderPoint( absolutePoint, invTransform );
			if( node )
				return node;
		}
		else {
			if( (*nodeIt)->specifiesTransform() ) {
				mat3 childInvTransform = (*nodeIt)->getTransformInverse() * invTransform;
				if( (*nodeIt)->containsPoint( vec2( childInvTransform * vec3( absolutePoint, 1 ) ) ) )
					return *nodeIt;	
			}
			else if( (*nodeIt)->containsPoint( localPt ) )
				return *nodeIt;
		}
	}
	
	return NULL;
}

const Node* Group::findInAncestors( const std::string &elementId ) const
{
	const Node *result;

	if( getId() == elementId )
		return this;
	else if( ( result = findNode( elementId, false ) ) != 0 )
		return result;
	else if( getParent() )
		return getParent()->findInAncestors( elementId );
	else
		return 0;
}

const Node&	Group::getChild( const std::string &id ) const
{
	const Node *result = findNode( id, false );
	if( ! result )
		throw ExcChildNotFound( id );
	else return *result;
}

Shape2d	Group::getMergedShape2d() const
{
	Shape2d result;
	appendMergedShape2d( &result );
	return result;
}

void Group::appendMergedShape2d( Shape2d *appendTo ) const
{
	for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
		auto childItPtr = *childIt;
		if( typeid(*childItPtr) == typeid(Group) )
			reinterpret_cast<Group*>( *childIt )->appendMergedShape2d( appendTo );
		else
			appendTo->append( (*childIt)->getShape() );
	}
}

const Node&	Group::getChild( size_t index ) const
{
	auto childIt = mChildren.begin();
	while( index ) {
		--index;
		if( childIt == mChildren.end() )
			break;
	}

	if( childIt == mChildren.end() )
		throw ExcChildNotFound( "index " + to_string( index ) );

	return **childIt;
}

void Group::renderSelf( Renderer &renderer ) const
{
	renderer.pushGroup( *this, mStyle.getOpacity() );
	for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
		Style style = (*childIt)->getStyle();
		if( ! renderer.visit( **childIt, &style ) )
			continue;
		if( (*childIt)->getStyle().isDisplayNone() ) // display: none we don't even descend groups
			continue;
		auto childItPtr = *childIt;
		if( (! childItPtr->isVisible()) && ( typeid(svg::Group) != typeid(*childItPtr) ) ) // if this isn't visible and isn't a group, just move along
			continue;
		(*childIt)->startRender( renderer, style );
		(*childIt)->renderSelf( renderer );
		(*childIt)->finishRender( renderer, style );
	}
	renderer.popGroup();
}

Rectf Group::calcBoundingBox() const
{
	bool empty = true;
	Rectf result( 0, 0, 0, 0 );
	for( list<Node*>::const_iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
		Rectf childBounds = (*childIt)->getBoundingBoxAbsolute();
		// only use child area if it exists (text nodes return [0,0,0,0])
		if( ( childBounds.getWidth() > 0 ) || ( childBounds.getHeight() > 0 ) ) {
			if( empty ) {
				result = childBounds;
				empty = false;
			}
			else {
				result.include( childBounds );
			}
		}
	}
	return result;
}

void Group::iterate( const std::function<void(Node*)> &fn )
{
	for( list<Node*>::iterator childIt = mChildren.begin(); childIt != mChildren.end(); ++childIt ) {
		fn( *childIt );
		if( typeid(**childIt) == typeid(svg::Group) )
			static_cast<svg::Group*>(*childIt)->iterate( fn );
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Use
Use::Use( Node *parent, const XmlTree &xml )
	: Node( parent, xml ), mReferenced( 0 )
{
	parse( xml );
}

void Use::parse( const XmlTree &xml )
{
	if( xml.hasAttribute( "xlink:href" ) ) {
		string ref = xml.getAttributeValue<string>( "xlink:href" );
		if( ref.size() > 1 ) {
			if( ref[0] == '#' ) {
				string elementId = ref.substr( 1, string::npos );
				mReferenced = findInAncestors( elementId );
			}
		}
	}
}

void Use::renderSelf( Renderer &renderer ) const
{
	if( mReferenced ) {
		Style style = mReferenced->getStyle();
		if( ! renderer.visit( *mReferenced, &style ) )
			return;
		mReferenced->startRender( renderer, style );
		mReferenced->renderSelf( renderer );
		mReferenced->finishRender( renderer, style );
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Image
Image::Image( Node *parent, const XmlTree &xml )
	: Node( parent, xml )
{
	mRect.x1 = xml.getAttributeValue<float>( "x", 0 );
	mRect.y1 = xml.getAttributeValue<float>( "y", 0 );
	float width = xml.getAttributeValue<float>( "width", 0 );
	float height = xml.getAttributeValue<float>( "height", 0 );	
	mRect.x2 = mRect.x1 + width;
	mRect.y2 = mRect.y1 + height;

	if( xml.hasAttribute( "xlink:href" ) ) {
		std::string s = xml.getAttributeValue<string>( "xlink:href" );
		if( s.find( "data:" ) == 0 )
			mImage = parseDataImage( s );
		else
			mFilePath = s;
	}

	if( ! mFilePath.empty() )
		mImage = getDoc()->loadImage( mFilePath );
}

std::shared_ptr<Surface8u> Image::parseDataImage( const string &data )
{
	size_t dataOffset = data.find( "data:" ) + 5;
	size_t semi = data.find( ";" );
	size_t comma = data.find( "," ); 
	if( semi == string::npos || comma == string::npos )
		return std::shared_ptr<Surface8u>();
	string mime = data.substr( dataOffset, semi - dataOffset ), extension;
	if( mime == "image/png" ) extension = "png";
	else if( mime == "image/jpeg" ) extension = "jpeg";	
	size_t len = data.size() - comma - 1;
	auto buf = make_shared<Buffer>( fromBase64( &data[comma + 1], len ) );
	try {
		shared_ptr<Surface8u> result( new Surface8u( ci::loadImage( DataSourceBuffer::create( buf ), ImageSource::Options(), extension ) ) );
		return result;
	}
	catch( std::exception &exc ) {
		CI_LOG_W( "failed to parse data image, what: " << exc.what() );
	}
	return std::shared_ptr<Surface8u>();
}

void Image::renderSelf( Renderer &renderer ) const
{
	if( mImage )
		renderer.drawImage( *this );
}

////////////////////////////////////////////////////////////////////////////////////
// Text
Text::Text( Node *parent, const XmlTree &xml )
	: Node( parent, xml ), mAttributes( xml )
{
	for( XmlTree::ConstIter treeIt = xml.begin(); treeIt != xml.end(); ++treeIt ) {
		if( treeIt->getTag() == "" ) { // data!
			mSpans.push_back( TextSpanRef( new TextSpan( this, treeIt->getValue() ) ) );
		}
		else if( treeIt->getTag() == "tspan" ) { // tspan!
			mSpans.push_back( TextSpanRef( new TextSpan( this, *treeIt ) ) );
		}
	}
}

#if 0 
Shape2d Text::getShape() const
{
	Shape2d result;	
	for( vector<TextSpanRef>::const_iterator spanIt = mSpans.begin(); spanIt != mSpans.end(); ++spanIt )
		result.append( (*spanIt)->getShape() );
	
	return result;
}
#endif

vec2 Text::getTextPen() const
{
	if( ( mAttributes.mX.size() != 1 ) || ( mAttributes.mY.size() != 1 ) ) {
		return vec2();
	}
	else
		return vec2( mAttributes.mX[0].asUser(), mAttributes.mY[0].asUser() );
}

float Text::getRotation() const
{
	if( mAttributes.mRotate.size() != 1 ) {
		return 0;
	}
	else
		return mAttributes.mRotate[0].asUser();
}

Value Text::getLetterSpacing() const
{
	if( mAttributes.mLetterSpacing.size() != 1 ) {
		return Value( 0 );
	}
	else
		return mAttributes.mLetterSpacing[0];
}

void Text::renderSelf( Renderer &renderer ) const
{
	renderer.pushTextPen( vec2() ); // this may be overridden by the attributes, but that's ok
	mAttributes.startRender( renderer );
	for( vector<TextSpanRef>::const_iterator spanIt = mSpans.begin(); spanIt != mSpans.end(); ++spanIt ) {
		(*spanIt)->renderSelf( renderer );
	}
	mAttributes.finishRender( renderer );
	renderer.popTextPen();
}

////////////////////////////////////////////////////////////////////////////////////
// TextSpan
TextSpan::TextSpan( Node *parent, const XmlTree &xml )
	: Node( parent, xml ), mAttributes( xml ), mIgnoreAttributes( false )
{
	for( XmlTree::ConstIter treeIt = xml.begin(); treeIt != xml.end(); ++treeIt ) {
		if( treeIt->getTag() == "" ) { // data!
			mSpans.push_back( TextSpanRef( new TextSpan( this, treeIt->getValue() ) ) );
		}
		else if( treeIt->getTag() == "tspan" ) { // tspan!
			mSpans.push_back( TextSpanRef( new TextSpan( this, *treeIt ) ) );
		}
	}
}

TextSpan::TextSpan( Node *parent, const std::string &str )
	: Node( parent ), mIgnoreAttributes( true )
{
	// replace all multi-char whitespace with single space
	/*size_t c = 0;
	while( str[c] ) {
		if( isspace(str[c]) ) mString += ' ';
		while( str[c] && isspace(str[c]) )
			nextCharUtf8( str.c_str(), &c );
		while( str[c] && ( ! isspace(str[c]) ) ) // this is not really correct - does not work with UTF32 code points that are >255
			mString += nextCharUtf8( str.c_str(), &c );
	}*/
	// Technically multi-char whitespace should be reduced to single chars; needs to be revisited with unicode-aware version of this method
	mString = str;
}

void TextSpan::renderSelf( Renderer &renderer ) const
{
	Style style = mStyle;
	if( ! renderer.visit( *this, &style ) )
		return;
	startRender( renderer, style );
	if( ! mIgnoreAttributes ) // TextSpans that are actually the contents of Text's attributes should be ignored
		mAttributes.startRender( renderer );
	if( ! mString.empty() ) {
		renderer.drawTextSpan( *this );
	}
	for( vector<TextSpanRef>::const_iterator spanIt = mSpans.begin(); spanIt != mSpans.end(); ++spanIt ) {
		(*spanIt)->renderSelf( renderer );
	}
	if( ! mIgnoreAttributes )
		mAttributes.finishRender( renderer );
	finishRender( renderer, style );		
}

std::vector<std::pair<uint16_t,vec2> > TextSpan::getGlyphMeasures() const
{
	if( ! mGlyphMeasures ) {		
		TextBox tbox = TextBox().font( *getFont() ).text( mString );
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
		auto tmpGlyphs = tbox.measureGlyphs();
		mGlyphMeasures = shared_ptr<std::vector<std::pair<uint16_t,vec2> > >( 
			new std::vector<std::pair<uint16_t,vec2> >( tmpGlyphs.size() ) );
		for( size_t i = 0; i < tmpGlyphs.size(); ++i ) {
			const auto& src = tmpGlyphs[i];
			auto& dst = (*mGlyphMeasures)[i];
			dst.first = (uint16_t)src.first;
			dst.second = src.second;
		}
#else	
		mGlyphMeasures = shared_ptr<std::vector<std::pair<uint16_t,vec2> > >( 
			new std::vector<std::pair<uint16_t,vec2> >( tbox.measureGlyphs() ) );
#endif		
	}
	
	return *mGlyphMeasures;
}

#if 0
// This is not implemented
Shape2d TextSpan::getShape() const
{
	if( ! mShape ) {
		mShape = shared_ptr<Shape2d>( new Shape2d() );
		
		if( ! mString.empty() ) {
			shared_ptr<Font> font = getFont();
			if( ! font )
				return Shape2d();
			TextBox tbox = TextBox().font( *font ).text( mString );
			vector<pair<uint16_t,vec2> > glyphs = getGlyphMeasures();
			vec2 textPen = getTextPen();
			float rotation = getRotation();
			bool shouldRotate = fabs( rotation ) > 0.0001f;
			MatrixAffine2f rotationMatrix = MatrixAffine2f::makeRotate( toRadians( rotation ) );
			for( size_t g = 0; g < glyphs.size(); ++g ) {
				MatrixAffine2f m = MatrixAffine2f::makeTranslate( textPen + vec2( glyphs[g].second.x, 0 ) );
				if( shouldRotate )
					m *= rotationMatrix;
				mShape->append( font->getGlyphShape( glyphs[g].first ).getTransform( m ) );
			}
		}

		for( vector<TextSpanRef>::const_iterator spanIt = mSpans.begin(); spanIt != mSpans.end(); ++spanIt )
			mShape->append( (*spanIt)->getShape() );
	}
		
	return *mShape;
}
#endif

// TextSpan::Atributes
TextSpan::Attributes::Attributes( const XmlTree &xml )
{
	if( xml.hasAttribute( "x" ) )
		mX = readValueList( xml["x"], false );
	if( xml.hasAttribute( "y" ) )
		mY = readValueList( xml["y"], false );
	if( xml.hasAttribute( "rotate" ) )
		mRotate = readValueList( xml["rotate"], false );
	if( xml.hasAttribute( "letter-spacing" ) )
		mLetterSpacing = readValueList( xml["letter-spacing"], false );
}

const std::shared_ptr<Font>	TextSpan::getFont() const
{
	if( ! mFont ) {
		const vector<string> &fontFamilies = getFontFamilies();
		float fontSize = getFontSize().asUser();		
		for( vector<string>::const_iterator familyIt = fontFamilies.begin(); familyIt != fontFamilies.end(); ++familyIt ) {
			try {
				mFont = shared_ptr<Font>( new Font( *familyIt, fontSize ) );
				break;
			}
			catch( ci::Exception &exc ) {
				CI_LOG_W( "failed to load font with name: " << *familyIt << ", size: " << fontSize << ". what: " << exc.what() << "\t - loading default font." );
				mFont = shared_ptr<Font>( new Font( Font::getDefault() ) );
			}
		}
	}
	
	return mFont;
}

vec2 TextSpan::getTextPen() const
{
	if( mIgnoreAttributes || ( mAttributes.mX.size() != 1 ) || ( mAttributes.mY.size() != 1 ) ) {
		if( ! mParent ) return vec2();
		else if( typeid(*mParent) == typeid(TextSpan) ) return reinterpret_cast<const TextSpan*>( mParent )->getTextPen();
		else if( typeid(*mParent) == typeid(Text) ) return reinterpret_cast<const Text*>( mParent )->getTextPen();
		else return vec2();
	}
	else
		return vec2( mAttributes.mX[0].asUser(), mAttributes.mY[0].asUser() );
}

void TextSpan::setTextPen( const vec2 &textPen )
{
	if( mIgnoreAttributes ) {
		if( ! mParent ) return;
		else if( typeid(*mParent) == typeid(TextSpan) ) return reinterpret_cast<TextSpan*>( mParent )->setTextPen( textPen );
		else if( typeid(*mParent) == typeid(Text) ) return reinterpret_cast<Text*>( mParent )->setTextPen( textPen );
	}
	else
		mAttributes.setTextPen( textPen );
}

float TextSpan::getRotation() const
{
	if( mIgnoreAttributes || ( mAttributes.mRotate.size() != 1 ) ) {
		if( ! mParent ) return 0;
		else if( typeid(*mParent) == typeid(TextSpan) ) return reinterpret_cast<const TextSpan*>( mParent )->getRotation();
		else if( typeid(*mParent) == typeid(Text) ) return reinterpret_cast<const Text*>( mParent )->getRotation();
		else return 0;
	}
	else
		return mAttributes.mRotate[0].asUser();
}

Value TextSpan::getLetterSpacing() const
{
	if( mIgnoreAttributes || ( mAttributes.mLetterSpacing.size() != 1 ) ) {
		if( ! mParent ) return 0;
		else if( typeid(*mParent) == typeid(TextSpan) ) return reinterpret_cast<const TextSpan*>( mParent )->getLetterSpacing();
		else if( typeid(*mParent) == typeid(Text) ) return reinterpret_cast<const Text*>( mParent )->getLetterSpacing();
		else return 0;
	}
	else
		return mAttributes.mLetterSpacing[0];
}

void TextSpan::Attributes::startRender( Renderer &renderer ) const
{
	if( mX.size() == 1 && mY.size() == 1 )
		renderer.pushTextPen( vec2( mX[0].asUser(), mY[0].asUser() ) );
	if( mRotate.size() == 1 )
		renderer.pushTextRotation( mRotate[0].asUser() );
	else
		renderer.pushTextRotation( 0 );
}

void TextSpan::Attributes::finishRender( Renderer &renderer ) const
{
	if( mX.size() == 1 && mY.size() == 1 )
		renderer.popTextPen();
	renderer.popTextRotation();
}

void TextSpan::Attributes::setTextPen( const vec2 &textPen )
{
	if( mX.empty() )
		mX.push_back( Value( textPen.x ) );
	else
		mX[0] = Value( textPen.x );
	if( mY.empty() )
		mY.push_back( Value( textPen.y ) );
	else
		mY[0] = Value( textPen.y );
}

////////////////////////////////////////////////////////////////////////////////////
// Doc
Doc::Doc( const fs::path &filePath )
	: Group( 0 )
{
	loadDoc( loadFile( filePath ), filePath );
}

Doc::Doc( DataSourceRef dataSource, const fs::path &filePath )
	: Group( 0 )
{
	fs::path relativePath = filePath;
	if( filePath.empty() )
		relativePath = fs::path( dataSource->getFilePathHint() );
	loadDoc( dataSource, relativePath );
}

DocRef Doc::create( const fs::path &filePath )
{
	return DocRef( new svg::Doc( filePath ) );
}

DocRef Doc::create( DataSourceRef dataSource, const fs::path &filePath )
{
	return DocRef( new svg::Doc( dataSource, filePath ) );
}

DocRef Doc::createFromSvgz( DataSourceRef dataSource, const fs::path &filePath )
{
	fs::path relativePath = filePath;
	if( filePath.empty() )
		relativePath = dataSource->getFilePathHint();

	Buffer compressed( dataSource );
	BufferRef decompressed = make_shared<Buffer>( decompressBuffer( compressed, false, true ) );
	
	return DocRef( new svg::Doc( DataSourceBuffer::create( decompressed, relativePath ) ) );
}

void Doc::loadDoc( DataSourceRef source, fs::path filePath )
{
	if( ! filePath.empty() )
		mFilePath = filePath.parent_path();
	mXmlTree = shared_ptr<XmlTree>( new XmlTree( source, XmlTree::ParseOptions().ignoreDataChildren( false ) ) );

	const XmlTree &xml( mXmlTree->getChild( "svg" ) );

	if( xml.hasAttribute( "viewBox" ) ) {
		string vbox = xml.getAttributeValue<string>( "viewBox" );
		const char *vbCPtr = vbox.c_str();
		mViewBox.x1 = static_cast<int>( parseFloat( &vbCPtr ) );
		mViewBox.y1 = static_cast<int>( parseFloat( &vbCPtr ) );
		mViewBox.x2 = mViewBox.x1 + static_cast<int>( parseFloat( &vbCPtr ) );
		mViewBox.y2 = mViewBox.y1 + static_cast<int>( parseFloat( &vbCPtr ) );
	}
	else {
		mViewBox = Area( 0, 0, 0, 0 );
	}
	if( xml.hasAttribute( "width" ) ) {
		Value val = Value::parse( xml.getAttributeValue<string>( "width" ) );
		if( val.isPercent() )
			mWidth = static_cast<int>( val.asUser() * mViewBox.getWidth() / 100 );
		else
			mWidth = static_cast<int>( val.asUser( 100, getDpi() ) );
	}
	else
		mWidth = mViewBox.getWidth();
	if( xml.hasAttribute( "height" ) ) {
		Value val = Value::parse( xml.getAttributeValue<string>( "height" ) );
		if( val.isPercent() )
			mHeight = static_cast<int>( val.asUser() * mViewBox.getHeight() / 100 );
		else
			mHeight = static_cast<int>( val.asUser( 100, getDpi() ) );
	}
	else
		mHeight = mViewBox.getHeight();

	bool needsViewBoxMapping = mViewBox.getWidth() > 0 && mViewBox.getHeight() > 0 && mWidth > 0 && mHeight > 0;
	if( needsViewBoxMapping ) {
		mat3 m33;
		m33[0][0] = mViewBox.getWidth() / (float)mWidth; m33[1][1] = mViewBox.getHeight() / (float)mHeight;
		m33[2][0] = (float)-mViewBox.x1; m33[2][1] = (float)-mViewBox.y1;
		mTransform = m33;
		mSpecifiesTransform = true;
	}
	else
		mTransform = mat3();

	// we can't parse the group w/o having parsed the viewBox, dimensions, etc, so we have to do this manually:
	if( xml.hasChild( "switch" ) )		// when saved with "preserve Illustrator editing capabilities", svg data is inside a "switch"
		Group::parse( xml.getChild( "switch" ) );
	else
		Group::parse( xml );
}

shared_ptr<Surface8u> Doc::loadImage( fs::path relativePath )
{
	if( mImageCache.find( relativePath ) == mImageCache.end() ) {
		try {
#if defined( CINDER_UWP )
			fs::path fullPath = ( mFilePath / relativePath );
#else
			fs::path fullPath = ( mFilePath / relativePath ).make_preferred();
#endif

			if( fs::exists( fullPath ) )
				mImageCache[relativePath] = shared_ptr<Surface8u>( new Surface8u( ci::loadImage( fullPath ) ) );
		}
		catch(...) {
		}
	}

	if( mImageCache.find( relativePath ) != mImageCache.end() )
		return mImageCache[relativePath];
	else
		return shared_ptr<Surface8u>();
}

Node* Doc::nodeUnderPoint( const vec2 &pt )
{
	return Group::nodeUnderPoint( pt, mat3() );
}

void Doc::renderSelf( Renderer &renderer ) const
{
	Group::renderSelf( renderer );
}

ExcChildNotFound::ExcChildNotFound( const string &child )
{
	setDescription( "Could not find child: " + child );
}

} } // namespace cinder::svg
