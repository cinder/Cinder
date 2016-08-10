#include "OtherClass.h"

extern void myFunction();

void OtherClass::functionA( const std::string& s )
{
    int x;
    int y;
    char *ps;
    functionB( x, y, ps );
}

void OtherClass::functionB( int x, int y, char* s )
{
    std::vector<std::string> v;
    functionC( v );
}

void OtherClass::functionC( const std::vector<std::string>& v )
{
    const char *ptr;
    functionD( (const void*)ptr );
}

void OtherClass::functionD( const void* ptr )
{
    myFunction();
}

