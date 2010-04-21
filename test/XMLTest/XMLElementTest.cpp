#include <boost/test/unit_test.hpp>

#include <iostream>
#include <vector>
#include "flint/XML.h"

#include "XMLFixtures.h"

using namespace boost::unit_test;
using namespace fli;

BOOST_AUTO_TEST_SUITE(XMLElement_Tests)

BOOST_AUTO_TEST_CASE(tinyxml_constructor_test) 
{
	//TODO
}

BOOST_AUTO_TEST_CASE(name_test)
{
	XMLDocument doc( "<element>value</element>" );
	BOOST_CHECK_EQUAL( doc.rootNode().name(), "element" );
}

BOOST_AUTO_TEST_CASE(operator_equal_test)
{
	XMLDocument doc( "<element><child>value</child></element>" );
	BOOST_CHECK( doc.rootNode().firstChild() == doc.rootNode().firstChild() );
	BOOST_CHECK( ! ( doc.rootNode() == doc.rootNode().firstChild() ) );
}

BOOST_AUTO_TEST_CASE(operator_not_equal_test)
{
	XMLDocument doc( "<element><child>value</child></element>" );
	BOOST_CHECK( doc.rootNode() != doc.rootNode().firstChild() );
	BOOST_CHECK( ! ( doc.rootNode().firstChild() != doc.rootNode().firstChild() ) );
}

BOOST_AUTO_TEST_CASE(has_children_test)
{
	XMLDocument doc( "<element>value</element>" );
	BOOST_CHECK( ! doc.rootNode().hasChildren() );
	
	XMLDocument docWithChild( "<element><child>value</child></element>" );
	BOOST_CHECK( docWithChild.rootNode().hasChildren() );
}


BOOST_FIXTURE_TEST_CASE(xpath_test, F)
{
	std::vector<XMLElement> elems( xdoc.rootNode().xpath( "/rss/channel/item" ) );
	BOOST_CHECK_EQUAL( elems.size(), 3 );
	
	BOOST_CHECK_EQUAL( elems[0].name(), "item" );
	BOOST_CHECK_EQUAL( elems[0].findChild( "title" ).value(), "New Work in the Portfolio!" );
	
	XMLElement bottomNode = elems[0].findChild( "title" );
	std::vector<XMLElement> emptyVec( bottomNode.xpath( "/made/up" ) );
	BOOST_CHECK_EQUAL( emptyVec.size(), 0 );
}

BOOST_AUTO_TEST_CASE(find_child_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	
	XMLElement node = doc.rootNode().findChild( "second" );
	BOOST_CHECK_EQUAL( node.name(), "second" );
}

BOOST_AUTO_TEST_CASE(parent_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	
	XMLElement child = doc.rootNode().firstChild();
	BOOST_CHECK_EQUAL( doc.rootNode().name(), child.parent().name() );
	//BOOST_CHECK_EQUAL( doc.rootNode(), child.parent() );
}

BOOST_AUTO_TEST_CASE(children_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	std::vector<XMLElement> children = doc.rootNode().children();
	
	BOOST_CHECK_EQUAL( children.size(), 3 );
	BOOST_CHECK_EQUAL( children[0].name(), "first" );
	BOOST_CHECK_EQUAL( children[1].name(), "second" );
	BOOST_CHECK_EQUAL( children[2].name(), "third" );
	
	std::vector<XMLElement> emptyChildren = doc.rootNode().firstChild().children();
	BOOST_CHECK_EQUAL( emptyChildren.size(), 0 );
}

BOOST_AUTO_TEST_CASE(first_child_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	BOOST_CHECK_EQUAL( doc.rootNode().firstChild().name(), "first" );
	BOOST_CHECK_EQUAL( doc.rootNode().firstChild().value(), "value1" );
}

BOOST_AUTO_TEST_CASE(last_child_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	BOOST_CHECK_EQUAL( doc.rootNode().lastChild().name(), "third" );
	BOOST_CHECK_EQUAL( doc.rootNode().lastChild().value(), "value3" );
}

BOOST_AUTO_TEST_CASE(next_sibling_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	
	XMLElement firstNode = doc.rootNode().firstChild();
	BOOST_CHECK_EQUAL( firstNode.nextSibling().name(), "second" );
	
	XMLElement lastNode = firstNode.nextSibling().nextSibling();
	BOOST_CHECK_EQUAL( lastNode.name(), "third" );
	
	//TODO: figure out how to identify a NULL node
	//BOOST_CHECK_EQUAL( lastNode.nextSibling(),  )
}

BOOST_AUTO_TEST_CASE(previous_sibling_test)
{
	XMLDocument doc( "<root><first>value1</first><second>value2</second><third>value3</third></root>" );
	
	XMLElement lastNode = doc.rootNode().lastChild();
	BOOST_CHECK_EQUAL( lastNode.previousSibling().name(), "second" );
	
	XMLElement firstNode = lastNode.previousSibling().previousSibling();
	BOOST_CHECK_EQUAL( firstNode.name(), "first" );
}

BOOST_FIXTURE_TEST_CASE(value_test, F)
{
	XMLDocument doc( "<element>value</element>" );
	BOOST_CHECK_EQUAL( doc.rootNode().value(), "value" );
	
	XMLDocument doc2( "<root><element>value</element><element2>value2</element2></root>" );
	BOOST_CHECK_EQUAL( doc2.rootNode().value(), "<element>value</element><element2>value2</element2>" );
	
	std::vector<XMLElement> elems( xdoc.rootNode().xpath( "/rss/channel/item" ) );
	BOOST_CHECK_EQUAL( "<title>New Work in the Portfolio!</title>"
						"<description>&lt;div class=&quot;t_block&quot;&gt;Hello all!&lt;/div&gt;&lt;br /&gt;&lt;div class=&quot;t_block&quot;&gt;We&amp;#8217;ve been sorta lax about updating our portfolio.</description>"
						"<author>Rick Webb</author>"
						"<pubDate>Fri, 07 Nov 2008 12:53:07 -0500</pubDate>"
						"<link>http://feeds.feedburner.com/~r/barbariangrouphomepage/~3/445738722/1227-new_work_in_the_portfolio</link>"
						"<guid isPermaLink=\"false\">http://www.barbariangroup.com/posts/1227-new_work_in_the_portfolio</guid>",
						elems[0].value()
								);
}

BOOST_AUTO_TEST_CASE(attributes_test)
{
	XMLDocument doc( "<element attr1=\"value1\" attr2=\"value2\" attr3=\"value3\">value</element>" );
	std::map<std::string, std::string> attributes = doc.rootNode().attributes();
	
	BOOST_CHECK_EQUAL( attributes.size(), 3 );
	BOOST_CHECK_EQUAL( attributes["attr1"], "value1" );
	BOOST_CHECK_EQUAL( attributes["attr2"], "value2" );
	BOOST_CHECK_EQUAL( attributes["attr3"], "value3" );
}

BOOST_AUTO_TEST_CASE(set_attribute_test)
{
	XMLDocument doc( "<element attr1=\"value1\">value</element>" );
	doc.rootNode().setAttribute("newattr", "newvalue");
	
	std::map<std::string, std::string> attributes = doc.rootNode().attributes();
	BOOST_CHECK( attributes.find( "newattr" ) != attributes.end() );
	BOOST_CHECK_EQUAL( attributes["attr1"], "value1" );
	//BOOST_CHECK_EQUAL( attributes["newattr"], "value" );
	
	//overwrite attribute
	doc.rootNode().setAttribute( "attr1", "changedvalue" );
	std::map<std::string, std::string> changedAttributes = doc.rootNode().attributes();
	BOOST_CHECK_EQUAL( changedAttributes["attr1"], "changedvalue" );
}

BOOST_AUTO_TEST_CASE(add_child_test)
{
	//TODO
}

BOOST_AUTO_TEST_SUITE_END()