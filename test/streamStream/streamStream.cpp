#include <iostream>
#include "flint/URL.h"
#include "flint/Stream.h"
#include "flint/Utilities.h"

int main( int argc, char * const argv[] )
{
	// Write apple.com to ~/apple.com.html
	fli::fli_istream is( fli::loadURL( "http://apple.com" ) );
	fli::fli_ostream os( fli::writeFile( fli::getHomeDirectory() + "apple.com.html" ) );

	std::string line;
	while( ! is.eof() ) {
		std::getline( is, line );
		os << line << std::endl; // write it to a file
		std::cout << line << std::endl; // and also write it to stdout
	}

	// create an iostream and exercise it
	fli::fli_iostream ios( fli::readWriteFile( fli::getHomeDirectory() + "someio.txt" ) );
	for( int i = 0; i < 10; ++i ) {
		ios << i << " ";
	}
	ios.seekg( 0, std::ios::beg );
	for( int i = 0; i < 10; ++i ) {
		int anInt;
		ios >> anInt;
		std::cout << anInt;
		assert( i == anInt );
	}

    return 0;
}
