#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Timer.h"
#include "cinder/Vector.h"
using namespace ci;

#include "TestMatrix22.h"
#include "TestMatrix33.h"
#include "TestMatrix44.h"

static const std::string kPre = "   ";

std::string MakeIndentedBlock( const std::stringstream& src )
{
	std::stringstream ss;
	std::string data = src.str();	
	if( ! data.empty() ) {
		ss << kPre;
		for( size_t i = 0; i < data.size(); ++i ) {
			char c = data[i];
			ss << c;
			if( '\n' == c ) {
				ss << kPre;
			}
		}
	}
	return ss.str();
}

void DoTest( const std::string& name, void (*testFunc)(std::ostream&) );

#define DO_TEST( _FUNC_ ) \
	DoTest( #_FUNC_, &_FUNC_ );

//template <typename T> void TestMatrix22( std::ostream& os );
//template <typename T> void TestMatrix33( std::ostream& os );
//template <typename T> void TestMatrix44( std::ostream& os );

int main( int argc, char **argv )
{
	std::cout << std::endl;

	/*
	mat4 a(
		 0,  1,  2,  3,
		 4,  5,  6,  7,
		 8,  9, 10, 11,
		12, 13, 14, 15
	);

	mat4 b(
		 1,  2,  1,  3,
		 4,  1,  5,  1,
		 1,  6,  1,  7,
		 8,  1,  9, -5
	);

	mat4 c = MatrixAlgo::SseMultiply( a, b );

	std::cout << a << "\n";
	std::cout << b << "\n";
	std::cout << c;
	*/

	DO_TEST( TestMatrix22<float> );
	DO_TEST( TestMatrix33<float> );
	DO_TEST( TestMatrix44<float> );
	
	std::cout << std::endl;

	return 0;
}

void DoTest( const std::string& name, void (*testFunc)( std::ostream& os ) )
{
	std::cout << "Testing " << name << "\n";
	std::stringstream ss;
	testFunc( ss );
	std::string output = MakeIndentedBlock( ss );
	if( ! output.empty() ) {
		std::cout << output << "\n";
	}
	std::cout << std::endl;
}
