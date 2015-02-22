#include "cinder/Cinder.h"
#include "cinder/Signals.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace cinder::signals;

static uint64_t timestampBenchmark()
{
	auto now = clock();
	return 1.0e9 / CLOCKS_PER_SEC * now;
}

static uint64_t sTestCounter = 0;

struct TestCounter {
	static uint64_t get()						{ return sTestCounter; }
	static void     set( uint64_t v )			{ sTestCounter = v; }
	static void     add2( void*, uint64_t v )	{ sTestCounter +=v; }
};

void        (*testCounterAdd2)( void*, uint64_t ) = TestCounter::add2; // external symbol to prevent easy inlining

static void printSize()
{
    Signal<void ()> sig;
    cout << "sizeof( signal ): " << sizeof( sig ) << " bytes" << endl;
}

// profile time for emission with 1 slot
static void benchSignalEmission1()
{
	Signal<void ( void*, uint64_t )> sigIncrement;
	sigIncrement.connect( testCounterAdd2 );

	const uint64_t startCounter = TestCounter::get();
	const uint64_t benchStart = timestampBenchmark();

	uint64_t i;
	for( i = 0; i < 1000000; i++ )
		sigIncrement.emit( nullptr, 1 );

	const uint64_t benchDone = timestampBenchmark();
	const uint64_t endCounter = TestCounter::get();
	assert( endCounter - startCounter == i );

	cout << "OK" << endl;
	cout << "\tper emission: " << double( benchDone - benchStart ) / double( i ) << "ns" << endl;
}

// profile time for emission with 5 slots
static void benchSignalEmission5()
{
	Signal<void ( void*, uint64_t )> sigIncrement;
	sigIncrement.connect( testCounterAdd2 );
	sigIncrement.connect( testCounterAdd2 );
	sigIncrement.connect( testCounterAdd2 );
	sigIncrement.connect( testCounterAdd2 );
	sigIncrement.connect( testCounterAdd2 );

	const uint64_t startCounter = TestCounter::get();
	const uint64_t benchStart = timestampBenchmark();

	uint64_t i;
	for( i = 0; i < 1000000; i++ )
		sigIncrement.emit( nullptr, 1 );

	const uint64_t benchDone = timestampBenchmark();
	const uint64_t endCounter = TestCounter::get();

	assert( endCounter - startCounter == ( i * 5 ) );

	cout << "OK" << endl;
	cout << "\tper emission: " << double( benchDone - benchStart ) / double( i ) << "ns"
	     << ", per slot: " << double( benchDone - benchStart ) / double( i * 5 ) << "ns"
		 << endl;
}

// profile time for emission with 4 slots, 2 in group 0 and 2 in group 1
static void benchSignalEmissionGroups()
{
	Signal<void ( void*, uint64_t )> sigIncrement;
	sigIncrement.connect( testCounterAdd2 );
	sigIncrement.connect( testCounterAdd2 );
	sigIncrement.connect( 1, testCounterAdd2 );
	sigIncrement.connect( 1, testCounterAdd2 );

	const uint64_t startCounter = TestCounter::get();
	const uint64_t benchStart = timestampBenchmark();

	uint64_t i;
	for( i = 0; i < 1000000; i++ )
		sigIncrement.emit( nullptr, 1 );

	const uint64_t benchDone = timestampBenchmark();
	const uint64_t endCounter = TestCounter::get();

	assert( endCounter - startCounter == ( i * 4 ) );

	cout << "OK" << endl;
	cout << "\tper emission: " << double( benchDone - benchStart ) / double( i ) << "ns"
	<< ", per slot: " << double( benchDone - benchStart ) / double( i * 4 ) << "ns"
	<< endl;
}

// the time of a plain callback
static void benchPlainCallbackLoop()
{
	void (*counterIncrement) (void*, uint64_t) = testCounterAdd2;

	const uint64_t startCounter = TestCounter::get();
	const uint64_t benchStart = timestampBenchmark();

	uint64_t i;
	for( i = 0; i < 1000000; i++ )
		counterIncrement( nullptr, 1 );

	const uint64_t benchDone = timestampBenchmark();
	const uint64_t endCounter = TestCounter::get();
	assert( endCounter - startCounter == i );

	cout << "OK" << endl;
	cout << "\tper round: " << double( benchDone - benchStart ) / double( i ) << "ns" << endl;
}

// the time of a std::function callback
static void benchStdFunctionCallbackLoop()
{
	void *someData = nullptr;
	std::function<void( uint64_t )> counterIncrement = [someData]( uint64_t incr ) { TestCounter::add2( someData, 1 ); };

	const uint64_t startCounter = TestCounter::get();
	const uint64_t benchStart = timestampBenchmark();

	uint64_t i;
	for( i = 0; i < 1000000; i++ )
		counterIncrement( 1 );

	const uint64_t benchDone = timestampBenchmark();
	const uint64_t endCounter = TestCounter::get();
	assert( endCounter - startCounter == i );

	cout << "OK" << endl;
	cout << "\tper round: " << double( benchDone - benchStart ) / double( i ) << "ns" << endl;
}

int main()
{
	printSize();

	cout << "Benchmark: emmission (1 slot): ";
	benchSignalEmission1();
	cout << "Benchmark: emmission (5 slots): ";
	benchSignalEmission5();
	cout << "Benchmark: emmission with groups (2 groups, 4 slots): ";
	benchSignalEmissionGroups();
	cout << "Benchmark: plain callback loop: ";
	benchPlainCallbackLoop();
	cout << "Benchmark: std::function callback loop: ";
	benchStdFunctionCallbackLoop();

	return 0;
}