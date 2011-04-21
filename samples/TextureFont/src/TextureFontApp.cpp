#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureFontApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	void setup();
	void mouseDrag( MouseEvent event );	
	void keyDown( KeyEvent event );
	void update();
	void draw();

	void render();
	
	Vec2f				mSize;
	Font				mFont;

	gl::TextureFontRef	mTextureFont;
};

void TextureFontApp::setup()
{
	mFont = Font( "ACaslonPro-Italic", 24 );
	//mFont = Font( "Arial", 24 );
	mSize = Vec2f( 100, 100 );
	render();
}

void TextureFontApp::mouseDrag( MouseEvent event )
{
	mSize = event.getPos();
	render();
}

void TextureFontApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case '=':
		case '+':
			mFont = Font( mFont.getName(), mFont.getSize() + 1 );
			render();
		break;
		case '-':
			mFont = Font( mFont.getName(), mFont.getSize() - 1 );
			render();
		break;
		case 'f':
			mFont = Font( Font::getNames()[Rand::randInt() % Font::getNames().size()], mFont.getSize() );
			console() << mFont.getName() << std::endl;
			render();
		break;
	}
}

void TextureFontApp::render()
{
	string txt = "Here is some text that is larger than can fit naturally inside of 100 pixels.\nAnd here is another line after a hard break.";
	TextBox tbox = TextBox().alignment( TextBox::RIGHT ).font( mFont ).size( Vec2i( mSize.x, TextBox::GROW ) ).text( txt ).premultiplied( true );
	tbox.setColor( Color( 1.0f, 0.65f, 0.35f ) );
	tbox.setBackgroundColor( ColorA( 0.5, 0, 0, 1 ) );
	Vec2i sz = tbox.measure();

	mTextureFont = gl::TextureFont::create( mFont );
//mTextureFont = gl::TextureFont::create( mFont, "to" );
}

void TextureFontApp::update()
{
}

void TextureFontApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( Color( 0,0, 0 ) );
	
	//std::string str( "123 Hyphae, Hello worlds, hellf, florida Hello" );
	std::string str( "To show that a faith or a philosophy is true from every standpoint would be too big an undertaking even for a much bigger book than this; it is necessary to follow one path of argument; and this is the path that I here propose to follow. I wish to set forth my faith as particularly answering this double spiritual need, the need for that mixture of the familiar and the unfamiliar which Christendom has rightly named romance. For the very word \"romance\" has in it the mystery and ancient meaning of Rome. Any one setting out to dispute anything ought always to begin by saying what he does not dispute. Beyond stating what he proposes to prove he should always state what he does not propose to prove. The thing I do not propose to prove, the thing I propose to take as common ground between myself and any average reader, is this desirability of an active and imaginative life, picturesque and full of a poetical curiosity, a life such as western man at any rate always seems to have desired. If a man says that extinction is better than existence or blank existence better than variety and adventure, then he is not one of the ordinary people to whom I am talking. If a man prefers nothing I can give him nothing. But nearly all people I have ever met in this western society in which I live would agree to the general proposition that we need this life of practical romance; the combination of something that is strange with something that is secure. We need so to view the world as to combine an idea of wonder and an idea of welcome. We need to be happy in this wonderland without once being merely comfortable. It is THIS achievement of my creed that I shall chiefly pursue in these pages. But I have a peculiar reason for mentioning the man in a yacht, who discovered England. For I am that man in a yacht. I discovered England. I do not see how this book can avoid being egotistical; and I do not quite see (to tell the truth) how it can avoid being dull. Dulness will, however, free me from the charge which I most lament; the charge of being flippant. Mere light sophistry is the thing that I happen to despise most of all things, and it is perhaps a wholesome fact that this is the thing of which I am generally accused. I know nothing so contemptible as a mere paradox; a mere ingenious defence of the indefensible. If it were true (as has been said) that Mr. Bernard Shaw lived upon paradox, then he ought to be a mere common millionaire; for a man of his mental activity could invent a sophistry every six minutes. It is as easy as lying; because it is lying. The truth is, of course, that Mr. Shaw is cruelly hampered by the fact that he cannot tell any lie unless he thinks it is the truth. I find myself under the same intolerable bondage. I never in my life said anything merely because I thought it funny; though of course, I have had ordinary human vainglory, and may have thought it funny because I had said it. It is one thing to describe an interview with a gorgon or a griffin, a creature who does not exist. It is another thing to discover that the rhinoceros does exist and then take pleasure in the fact that he looks as if he didn't. One searches for truth, but it may be that one pursues instinctively the more extraordinary truths. And I offer this book with the heartiest sentiments to all the jolly people who hate what I write, and regard it (very justly, for all I know), as a piece of poor clowning or a single tiresome joke. For if this book is a joke it is a joke against me. I am the man who with the utmost daring discovered what had been discovered before. If there is an element of farce in what follows, the farce is at my own expense; for this book explains how I fancied I was the first to set foot in Brighton and then found I was the last. It recounts my elephantine adventures in pursuit of the obvious. No one can think my case more ludicrous than I think it myself; no reader can accuse me here of trying to make a fool of him: I am the fool of this story, and no rebel shall hurl me from my throne. I freely confess all the idiotic ambitions of the end of the nineteenth century. I did, like all other solemn little boys, try to be in advance of the age. Like them I tried to be some ten minutes in advance of the truth. And I found that I was eighteen hundred years behind it. I did strain my voice with a painfully juvenile exaggeration in uttering my truths. And I was punished in the fittest and funniest way, for I have kept my truths: but I have discovered, not that they were not truths, but simply that they were not mine. When I fancied that I stood alone I was really in the ridiculous position of being backed up by all Christendom. It may be, Heaven forgive me, that I did try to be original; but I only succeeded in inventing all by myself an inferior copy of the existing traditions of civilized religion. The man from the yacht thought he was the first to find England; I thought I was the first to find Europe. I did try to found a heresy of my own; and when I had put the last touches to it, I discovered that it was orthodoxy." );
	Rectf boundsRect( 100, mTextureFont->getAscent(), getWindowWidth() - 100, getWindowHeight() - 100 );
//	gl::drawString( str, pos, ColorA( 1, 1, 0.5, 1 ), mFont );
	
	gl::color( Color( 1,1,1) );
	gl::color( ColorA( 1, 0.5f, 0.25f, 1.0f ) );
//	mTextureFont->drawString( str, Vec2f( 0, 100 ) );
	mTextureFont->drawStringWrapped( str, boundsRect );
	
	gl::color( Color( 1,1,1) );
	mTextureFont->drawString( toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );
}


CINDER_APP_NATIVE( TextureFontApp, RendererGl(0) )
