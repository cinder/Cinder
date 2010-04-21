#include <boost/test/unit_test.hpp>

#include <iostream>
#include <vector>
#include <cstring>
#include "flint/Flint.h"
#include "flint/XML.h"

#include "XMLFixtures.h"

using namespace boost::unit_test;
using namespace fli;

BOOST_AUTO_TEST_SUITE(XMLDocument_Tests)

BOOST_AUTO_TEST_CASE(string_contructor_test)
{
	
	XMLDocument doc( "<element>value</element>" );
	BOOST_CHECK_EQUAL( doc.rootNode().name(), "element" );
}

BOOST_AUTO_TEST_CASE(std_istream_constructor_test)
{
	std::stringstream iostr;
	iostr << "<element>value</element>";
	XMLDocument doc( iostr );
	BOOST_CHECK_EQUAL( doc.rootNode().name(), "element" );
}

BOOST_AUTO_TEST_CASE(fli_IStream_contructor_test)
{
	char * xmlStr = "<element>value</element>";
	fli::IStreamMem *result = new fli::IStreamMem( xmlStr, strlen(xmlStr) );
	shared_ptr<fli::IStreamMem> istr( result );
	
	XMLDocument doc( istr );
	BOOST_CHECK_EQUAL( doc.rootNode().name(), "element" );
}

BOOST_AUTO_TEST_CASE(friend_operator_in_test)
{
	std::stringstream iostr;
	iostr << "<element>value</element>";

	std::stringstream anotherIostr;
	anotherIostr << "<anotherelement>value</anotherelement>";

	XMLDocument doc;
	iostr >> doc;
	BOOST_CHECK_EQUAL( doc.rootNode().name(), "element" );
	
	//revist -- this maybe just adds nodes on
	anotherIostr >> doc;
	BOOST_CHECK_EQUAL( doc.rootNode().nextSibling().name(), "anotherelement" );
}

BOOST_AUTO_TEST_CASE(friend_operator_out_test)
{
	std::stringstream iostr;
	XMLDocument doc( "<element>value</element>" );
	iostr << doc;
	
	BOOST_CHECK_EQUAL( iostr.str(), "<element>value</element>" );
}

BOOST_FIXTURE_TEST_CASE(xpath_test, F)
{
	std::vector<XMLElement> elems( xdoc.xpath( "/rss/channel/item" ) );
	BOOST_CHECK_EQUAL( elems.size(), 3 );
	
	BOOST_CHECK_EQUAL( elems[0].name(), "item" );
	BOOST_CHECK_EQUAL( elems[0].findChild( "title" ).value(), "New Work in the Portfolio!" );
	
	XMLElement bottomNode = elems[0].findChild( "title" );
	std::vector<XMLElement> emptyVec( bottomNode.xpath( "/made/up" ) );
	BOOST_CHECK_EQUAL( emptyVec.size(), 0 );
}

BOOST_AUTO_TEST_SUITE_END()
