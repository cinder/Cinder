#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"
#include "cinder/Timer.h"

#include <algorithm>

using namespace ci;
using namespace ci::app;
using namespace std;

class RandTestApp : public AppNative {
  public:
	void setup();
};

template<typename T>
void printDistribution( const vector<T> &values, int numBins )
{
	auto range = std::minmax_element( values.begin(), values.end() );
	long double offset = *range.first;
	long double divisor = 1.0 / ( *range.second - *range.first );

	console() << "Range: " << *range.first << "-" << *range.second << std::endl;

	vector<int> bins( numBins, 0 );	
	for( auto valIt = values.cbegin(); valIt != values.cend(); ++valIt ) {
		int whichBin = std::min<int>( ((*valIt - offset) * divisor)*numBins, numBins - 1);
		assert( whichBin >= 0 && whichBin < numBins );
		++bins[whichBin];
	}

	// normalize the bin size
	int maxBinValue = *(std::max_element( bins.begin(), bins.end() )) + 1;
	for( int b = 0; b < numBins; ++b ) {
		int numAsterisks = 50 * bins[b] / maxBinValue;
		for( int a = 0; a < numAsterisks; ++a )
			console() << '*';
		console() << bins[b];
		console() << std::endl;
	}
}

void RandTestApp::setup()
{
	const int NUM_TESTS = 10000;

	// print randInt() distribution
	{
/*	vector<int> nums;
	for( int i = 0; i < NUM_TESTS; ++i ) {
		int v = randInt();
		nums.push_back( v );
	}
	console() << "randInt() distribution" << std::endl;
	printDistribution( nums, 20 );*/
	}

	// print randUint() distribution
	{
	vector<uint32_t> nums;
	for( int i = 0; i < NUM_TESTS; ++i ) {
		uint32_t v = randInt();
		nums.push_back( v );
	}
	console() << "randUint() distribution" << std::endl;
	printDistribution( nums, 20 );
	}

	// test randInt(a) returns [0,a)
	for( int i = 0; i < NUM_TESTS; ++i ) {
		int v = randInt(1234);
		assert( v >= 0 && v < 1234 );
	}
	console() << "randInt(a) returns [0,a)" << std::endl;

	// print distribution
	{
	vector<int> nums;
	for( int i = 0; i < NUM_TESTS; ++i ) {
		int v = randInt(345,6789);
		nums.push_back( v );
		assert( v >= 345 && v < 6789 );
	}
	console() << "randInt(a,b) returns [a,b)" << std::endl;
	printDistribution( nums, 20 );
	}

	// test randFloat() returns [0,1)
	for( int i = 0; i < NUM_TESTS; ++i ) {
		float v = randFloat();
		assert( v >= 0 && v < 1.0f );
	}
	console() << "randFloat() returns [0,1)" << std::endl;

	// test ranged randFloat(a) returns [0,a)
	for( int i = 0; i < NUM_TESTS; ++i ) {
		float v = randFloat( 10.0f );
		assert( v >= 0.0f && v < 10.0f );
	}
	console() << "ranged randFloat(a) returns [0,a)" << std::endl;

	// test ranged randFloat(a,b) returns [a,b)
	for( int i = 0; i < NUM_TESTS; ++i ) {
		float v = randFloat( 2.0f, 10.0f );
		assert( v >= 2.0f && v < 10.0f );
	}
	console() << "ranged randFloat(a,b) returns [a,b)" << std::endl;

	// test distribution on randFloat()
	{
	vector<float> nums;
	for( int i = 0; i < 10000000; ++i ) {
		nums.push_back( randFloat() );
	}
	console() << "randFloat() distribution" << std::endl;
	printDistribution( nums, 20 );
	}

	// test distribution on randGaussian()
	{
	vector<float> nums;
	for( int i = 0; i < 100000; ++i ) {
		nums.push_back( randGaussian() );
	}
	console() << "randGaussian() distribution" << std::endl;
	printDistribution( nums, 20 );	
	}
	
	// test timing for 100000000 floats
	{
		Timer t;
		t.start();
		for( int i = 0; i < 1000000000; ++i ) {
			float v = randFloat();
		}
		t.stop();
		console() << "Time for 100,000,000 randFloats: " << t.getSeconds() << std::endl;
	}
}

CINDER_APP_NATIVE( RandTestApp, RendererGl )
