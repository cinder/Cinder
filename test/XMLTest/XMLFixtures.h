#include "flint/XML.h"

using namespace fli;

struct F 
{
	F() : xdoc("<rss version=\"2.0\">"
	"<channel>"
    "<title>The Barbarian Group Homepage</title>"
    "<link>http://www.barbariangroup.com/posts</link>"
    "<description>Items of interest from our homepage on things like recent blog posts, case studies, projects, and random ramblings. Yeah, you know you like it!</description>"
    "<language>en-us</language>"
	"<item>"
    "  <title>New Work in the Portfolio!</title>"
    "  <description>&lt;div class=\"t_block\"&gt;Hello all!&lt;/div&gt;&lt;br /&gt;&lt;div class=\"t_block\"&gt;We&amp;#8217;ve been sorta lax about updating our portfolio.</description>"
    "  <author>Rick Webb</author>"
    "  <pubDate>Fri, 07 Nov 2008 12:53:07 -0500</pubDate>"
    "  <link>http://feeds.feedburner.com/~r/barbariangrouphomepage/~3/445738722/1227-new_work_in_the_portfolio</link>"
    "  <guid isPermaLink=\"false\">http://www.barbariangroup.com/posts/1227-new_work_in_the_portfolio</guid>"
    "</item>"
    "<item>"
    "  <title>Oodles of Shirts!</title>"
    "  <description>&lt;div class=\"t_block\"&gt;There&amp;#8217;s a nice &lt;a href=\"http://www.adweek.com/aw/content_display/news/politics/e3i7f30abddb493befb52e761b2f01991e5\" target=\"_blank\"&gt;writeup today in Adweek today&lt;/a&gt; about the upsurge in sales &lt;span class=\"caps\"&gt;CNN&lt;/span&gt; is getting on their T-Shirts site today and yesterday, thanks to Obama&amp;#8217;s victory.  Awesome!&lt;/div&gt;&lt;br /&gt;&lt;img src=\"http://feeds.feedburner.com/~r/barbariangrouphomepage/~4/444689586\" height=\"1\" width=\"1\"/&gt;</description>"
    "  <author>Rick Webb</author>"
    "  <pubDate>Thu, 06 Nov 2008 14:22:46 -0500</pubDate>"
    "  <link>http://feeds.feedburner.com/~r/barbariangrouphomepage/~3/444689586/1225-oodles_of_shirts</link>"
    "  <guid isPermaLink=\"false\">http://www.barbariangroup.com/posts/1225-oodles_of_shirts</guid>"
    "</item>"
    "<item>"
    "  <title>Audio-driven landscape.</title>"
    "  <description>&lt;div class=\"t_block\"&gt;Not a new idea here, but it was still fun to make. While in &lt;span class=\"caps\"&gt;NYC&lt;/span&gt; last week, I spoke at the Boards Summit. They asked if Andrew Bell and I would give a talk on Processing. I made the following demo because I wanted an easy way to showcase what Processing can handle in real-time [...]&lt;/div&gt;&lt;br /&gt;&lt;img src=\"http://feeds.feedburner.com/~r/barbariangrouphomepage/~4/438491255\" height=\"1\" width=\"1\"/&gt;</description>"
    "  <author>Robert Hodgin</author>"
    "  <pubDate>Wed, 29 Oct 2008 04:02:10 -0400</pubDate>"
    "  <link>http://feeds.feedburner.com/~r/barbariangrouphomepage/~3/438491255/1183-audio_driven_landscape</link>"
    "  <guid isPermaLink=\"false\">http://www.barbariangroup.com/posts/1183-audio_driven_landscape</guid>"
    "</item>"
	"</channel>"
	"</rss>") {}
	~F() {}
	
	XMLDocument xdoc;
};