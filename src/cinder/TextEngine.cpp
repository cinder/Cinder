#include "cinder/TextEngine.h"

#if defined( CINDER_COCOA )
  #include "cinder/TextEngineCoreText.h"
#elif defined( CINDER_MSW )
  #include "cinder/TextEngineGdiPlus.h"
#endif
#include "cinder/TextEngineFreeType.h"

#include "boost/format.hpp"

using std::map;
using std::string;

namespace cinder {

TextEngineRef TextEngine::sInstance = TextEngineRef();

TextEngine::~TextEngine()
{
}

TextEngine::EngineType TextEngine::getEngineType()
{
    return mEngineType;
}

void TextEngine::setup(EngineType engine)
{
    //  Default font engine already initialized
    if ( sInstance )
        return;

    //  Select plaform default text engine
    if ( engine == DEFAULT ) {
#if defined( CINDER_COCOA )
        engine = CORETEXT;
#elif defined( CINDER_MSW )
        engine = GDIPLUS;
#else
        engine = FREETYPE;
#endif
    }

    sInstance = TextEngine::create(engine);
}

void TextEngine::teardown()
{
    if ( sInstance )
        sInstance.reset();
}

TextEngine::TextEngine(EngineType engineType) : mEngineType(engineType)
{
}

TextEngineRef TextEngine::instance()
{
    if ( !sInstance )
        setup();

    return sInstance;
}

TextEngineRef TextEngine::create(EngineType engine)
{
    //  XXX Text engines should be singletons

    switch (engine) {
#if defined( CINDER_COCOA )
    case CORETEXT:
        return TextEngineRef(new TextEngineCoreText());
        break;
#endif
#if defined( CINDER_MSW )
    case GDIPLUS:
        return TextEngineRef(new TextEngineGdiPlus());
        break;
#endif
    case FREETYPE:
        return TextEngineRef(new TextEngineFreeType());
        break;
    default:
        throw;
        break;
    }
}

namespace {

string fontKey( const string& fontName, float size )
{
    return (boost::format("@%s/%.1f") % fontName % size).str();
}

string fontKey( DataSourceRef dataSource, float size )
{
    std::string path;

    path = dataSource->getFilePath().string();
    if ( !path.empty() )
        return (boost::format( "f:%s/%.1f" ) % path % size).str();

    path = dataSource->getUrl().str();
    if ( !path.empty() )
        return (boost::format( "p:%s/%.1f" ) % path % size).str();

    path = dataSource->getFilePathHint();
    if ( !path.empty() )
        return (boost::format( "h:%s/%.1f" ) % path % size).str();

    return string();
}

}

FontCacheRef FontCache::create(const TextEngineRef engine)
{
    return FontCacheRef(new FontCache(engine ? engine : TextEngine::instance()));
}

FontCache::FontCache(const TextEngineRef textEngine) : mTextEngine(textEngine)
{
}

FontRef FontCache::getFont( const std::string& name, float size )
{
    string key = fontKey( name, size );
    map<string, FontRef>::iterator it = mFontCache.find(key);
    if ( !name.empty() && it != mFontCache.end() )
        return it->second;

    FontRef font = mTextEngine->createFont( name, size );
    if (font)
        mFontCache[key] = font;

    return font;
}

FontRef FontCache::getFont( const DataSourceRef dataSource, float size )
{
    string key = fontKey( dataSource, size );
    if ( !key.empty() ) {
        map<string, FontRef>::iterator it = mFontCache.find(key);
        if ( it != mFontCache.end() )
            return it->second;

        FontRef font = mTextEngine->createFont( dataSource, size );
        if (font)
            mFontCache[key] = font;

        return font;
    }

    return FontRef();
}

void FontCache::releaseFont( const FontRef font )
{
    for ( map<string, FontRef>::iterator it = mFontCache.begin(); it != mFontCache.end(); ) {
        if ( it->second == font ) {
            mFontCache.erase( it++ );
        }
        else {
            ++it;
        }
    }
}


}

