#pragma once

#include <string>
#include <vector>

class OtherClass {
public:

    void  functionA( const std::string& s );
    void  functionB( int x, int y, char* s );
    void  functionC( const std::vector<std::string>& v );
    void  functionD( const void* ptr );
};