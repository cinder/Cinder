#include "cinder/Cinder.h"
#include "cinder/Signals.h"
#include "cinder/app/Event.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace ci;
using namespace ci::signals;

std::string toString( float value, int precision )
{
	std::ostringstream out;
	out << std::fixed << std::setprecision( precision ) << value;
	return out.str();
}

inline string demangleTypeName( const char *mangledName )
{
#if defined( CINDER_COCOA )
	int status = 0;

	std::unique_ptr<char, void(*)(void *)> result { abi::__cxa_demangle( mangledName, NULL, NULL, &status ), std::free };

	return ( status == 0 ) ? result.get() : mangledName;
#else
	return mangledName;
#endif
}

struct BasicSignalTests {

	static string sAccum;

	struct Foo {
		char fooBool( float f, int i, string s )
		{
			sAccum += "Foo: " + toString( f + i + s.size(), 2 ) + "\n";
			return true;
		}
	};

	static char floatCallback( float f, int, string )
	{
		sAccum += "float: " + toString( f, 2 ) + "\n";
		return 0;
	}

	static void run()
	{
		sAccum = "";

		Connection conn0;
		assert( ! conn0.isConnected() );

		Signal<char (float, int, string)> sig1;
		auto conn1 = sig1.connect( floatCallback );
		auto conn2 = sig1.connect( [] ( float, int i, string ) { sAccum += "int: " + to_string( i ) + "\n"; return 0; } );
		auto conn3 = sig1.connect( [] ( float, int, const string &s ) { sAccum += "string: " + s + "\n"; return 0; } );

		assert( sig1.getNumSlots() == 3 );
		assert( conn1.isConnected() && conn2.isConnected() && conn3.isConnected() );

		sig1.emit( 0.3f, 4, "huhu" );

		bool success;
		success = conn1.disconnect();	assert( success == true  );	assert( sig1.getNumSlots() == 2 );
		success = conn1.disconnect();	assert( success == false ); assert( sig1.getNumSlots() == 2 );
		success = conn2.disconnect();	assert( success == true  );	assert( sig1.getNumSlots() == 1 );
		success = conn3.disconnect();	assert( success == true  );	assert( sig1.getNumSlots() == 0 );
		success = conn3.disconnect();	assert( success == false ); assert( sig1.getNumSlots() == 0 );
		success = conn2.disconnect();	assert( success == false );	assert( sig1.getNumSlots() == 0 );

		assert( ! conn1.isConnected() && ! conn2.isConnected() && ! conn3.isConnected() );

		Foo foo;
		sig1.connect( slot( foo, &Foo::fooBool ) );
		sig1.connect( slot( &foo, &Foo::fooBool ) );

		sig1.connect( bind( &Foo::fooBool, foo, placeholders::_1, placeholders::_2, placeholders::_3 ) );

		sig1.emit( 0.5f, 1, "12" );

		Signal<void (string, int)> sig2;
		sig2.connect( [] ( string msg, int ) { sAccum += "msg: " + msg; } );
		sig2.connect( [] ( string, int d )   { sAccum += " *" + to_string( d ) + "*\n"; } );
		sig2.emit( "in sig2", 17 );

		sAccum += "DONE";

		const char *expected =
			"float: 0.30\n"
			"int: 4\n"
			"string: huhu\n"
			"Foo: 3.50\n"
			"Foo: 3.50\n"
			"Foo: 3.50\n"
			"msg: in sig2 *17*\n"
			"DONE";

		cout << "sAccum: " << sAccum << endl;
		assert( sAccum == expected );
	}
};

string BasicSignalTests::sAccum;

struct TestMemberCallback {
	TestMemberCallback() : mAccum( 0 )	{}

	int mAccum;

	void handler()	{ mAccum++;	}

	static void run()
	{
		TestMemberCallback self;

		Signal<void ()> sig;
		auto slot = std::bind( &TestMemberCallback::handler, &self );
		sig.connect( slot );

		sig.emit();
		assert( self.mAccum == 1 );

		self.mAccum = 0;

		sig.connect( slot );
		sig.connect( slot );

		sig.emit();
		assert( self.mAccum == 3 );
	}
};

// Test disconnect from within a signal callback
struct TestSignalReentry {
	static void run()
	{
		Signal<void ()>	sig;

		int i = 0;

		auto conn1 = sig.connect( [&]{ i++; } );

		sig.connect( [&] { conn1.disconnect(); i++; } );

		assert( sig.getNumSlots() == 2 );

		sig.emit();

		assert( i == 2 );
		assert( sig.getNumSlots() == 1 );

		sig.emit();

		assert( i == 3 ); // only one signal, so i only incremented by 1
		assert( sig.getNumSlots() == 1 );

		sig.connect( [&]{ i++; } );
		sig.emit();

		assert( i == 5 );
		assert( sig.getNumSlots() == 2 );
	}
};

struct TestSignalPriorityGroups {
	static void run()
	{
		// default group
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( [&] { accum += "b"; } );
			sig.connect( [&] { accum += "c"; } );

			sig.emit();

			assert( accum == "abc" );
		}

		// negative priorities
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( -1, [&] { accum += "b"; } );
			sig.connect( -2, [&] { accum += "c"; } );

			sig.emit();

			assert( accum == "abc" );
		}

		// positive priorities
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( 1, [&] { accum += "b"; } );
			sig.connect( 2, [&] { accum += "c"; } );
			
			sig.emit();
			
			assert( accum == "cba" );
		}

		// mixed priorities
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( 1, [&] { accum += "b"; } );
			sig.connect( 1, [&] { accum += "c"; } );
			sig.connect( 2, [&] { accum += "d"; } );
			sig.connect( -100, [&] { accum += "e"; } );
			sig.connect( 100, [&] { accum += "f"; } );
			sig.connect( [&] { accum += "g"; } );

			sig.emit();

			assert( accum == "fdbcage" );
		}
	}
};

struct TestSignalPriorityGroupDisconnections {
	static void run()
	{
		// default group
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( [&] { accum += "a"; } );

			sig.emit();
			assert( accum == "a" );

			bool success = conn1.disconnect(); assert( success ); assert( sig.getNumSlots() == 0 );

			sig.emit();
			assert( accum == "a" );
		}

		// default group, two connections
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( [&] { accum += "a"; } );
			auto conn2 = sig.connect( [&] { accum += "b"; } );

			sig.emit();
			assert( accum == "ab" );

			bool success = conn1.disconnect();	assert( success ); assert( sig.getNumSlots() == 1 );

			sig.emit();
			assert( accum == "abb" );

			success = conn2.disconnect();		assert( success ); assert( sig.getNumSlots() == 0 );
			sig.emit();
			assert( accum == "abb" );
		}

		// default group and another group, three connections
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( [&] { accum += "a"; } );
			auto conn2 = sig.connect( 2, [&] { accum += "b"; } );
			auto conn3 = sig.connect( 2, [&] { accum += "c"; } );

			sig.emit();
			assert( accum == "bca" );

			bool success = conn2.disconnect();	assert( success ); assert( sig.getNumSlots() == 2 );
			accum.clear();
			sig.emit();
			assert( accum == "ca" );

			success = conn3.disconnect();		assert( success ); assert( sig.getNumSlots() == 1 );
			accum.clear();
			sig.emit();
			assert( accum == "a" );

			success = conn1.disconnect();		assert( success ); assert( sig.getNumSlots() == 0 );
			accum.clear();
			sig.emit();
			assert( accum == "" );
		}

		// add to group, remove all of group, then add another
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( 2, [&] { accum += "a"; } );
			auto conn2 = sig.connect( 2, [&] { accum += "b"; } );
			auto conn3 = sig.connect( 2, [&] { accum += "c"; } );

			bool success = conn1.disconnect();	assert( success ); assert( sig.getNumSlots() == 2 );
			success = conn2.disconnect();	assert( success ); assert( sig.getNumSlots() == 1 );
			success = conn3.disconnect();	assert( success ); assert( sig.getNumSlots() == 0 );

			auto conn4 = sig.connect( 2, [&] { accum += "d"; } );

			sig.emit();
			assert( accum == "d" );
		}

	}
};

struct TestScopedConnection {

	static void run()
	{
		Signal<void ()> sig;
		int accum = 0;
		auto slot = [&] { accum++; };

		sig.connect( slot );
		ScopedConnection conn1 = sig.connect( slot );

		{
			ScopedConnection conn2 = sig.connect( slot );
			assert( sig.getNumSlots() == 3 );
		}

		assert( sig.getNumSlots() == 2 );

		sig.emit();
		assert( accum == 2 );
	}
};

struct TestCollectorVector {

	static int handler1()	{ return 1; }
	static int handler42()  { return 42; }
	static int handler777()	{ return 777; }

	static void	run()
	{
		Signal<int (), CollectorVector<int>> sig;

		sig.connect( handler777 );
		sig.connect( handler42 );
		sig.connect( handler1 );
		sig.connect( handler42 );
		sig.connect( handler777 );

		vector<int> results = sig.emit();

		const vector<int> reference = { 777, 42, 1, 42, 777 };
		assert( results == reference );
	}
};

struct TestCollectorUntil0 {
	bool check1, check2;

	TestCollectorUntil0()
		: check1( false ) , check2( false )
	{}

	bool handlerTrue()		{ check1 = true; return true; }
	bool handlerFalse()		{ check2 = true; return false; }
	bool handlerAbort()		{ abort(); }

	static void	run()
	{
		TestCollectorUntil0 self;
		Signal<bool (), CollectorUntil0<bool>> sig;

		sig.connect( slot( self, &TestCollectorUntil0::handlerTrue ) );
		sig.connect( slot( self, &TestCollectorUntil0::handlerFalse ) );
		sig.connect( slot( self, &TestCollectorUntil0::handlerAbort ) );

		assert( ! self.check1 && ! self.check2 );

		const bool result = sig.emit();

		assert( ! result && self.check1 && self.check2 );
	}
};

struct TestCollectorWhile0 {
	bool check1, check2;

	TestCollectorWhile0()
		: check1( false ), check2( false )
	{}

	bool handler0()		{ check1 = true; return false; }
	bool handler1()		{ check2 = true; return true; }
	bool handlerAbort()	{ abort(); }

	static void	run()
	{
		TestCollectorWhile0 self;
		Signal<bool (), CollectorWhile0<bool>> sig;

		sig.connect( slot( self, &TestCollectorWhile0::handler0 ) );
		sig.connect( slot( self, &TestCollectorWhile0::handler1 ) );
		sig.connect( slot( self, &TestCollectorWhile0::handlerAbort ) );

		assert( ! self.check1 && ! self.check2 );

		const bool result = sig.emit();

		assert( result == true && self.check1 && self.check2 );
	}
};

struct TestCollectorBooleanAnd {
	bool check1, check2, check3;

	TestCollectorBooleanAnd()
		: check1( false ), check2( false ), check3( false )
	{}

	bool handlerTrue()		{ check1 = true; return true; }
	bool handlerFalse()		{ check2 = true; return false; }
	bool handlerTrue2()		{ check3 = true; return true; }

	static void	run()
	{
		TestCollectorBooleanAnd self;
		Signal<bool (), CollectorBooleanAnd> sig;

		sig.connect( slot( self, &TestCollectorBooleanAnd::handlerTrue ) );
		sig.connect( slot( self, &TestCollectorBooleanAnd::handlerFalse ) );
		sig.connect( slot( self, &TestCollectorBooleanAnd::handlerTrue2 ) );

		assert( ! self.check1 && ! self.check2 && ! self.check3 );

		const bool result = sig.emit();

		assert( ! result && self.check1 && self.check2 && self.check3 );
	}
};

struct TestCollectorBitwiseAnd {
	bool check1, check2;

	TestCollectorBitwiseAnd()
	: check1( false ), check2( false )
	{}

	uint8_t handler1()	{ check1 = true; return ( 1 << 0 ); }
	uint8_t handler2()	{ check2 = true; return ( 1 << 0 ) | ( 1 << 2 ); }

	static void	run()
	{
		TestCollectorBitwiseAnd self;
		Signal<uint8_t (), CollectorBitwiseAnd<uint8_t> > sig;

		sig.connect( slot( self, &TestCollectorBitwiseAnd::handler1 ) );
		sig.connect( slot( self, &TestCollectorBitwiseAnd::handler2 ) );

		assert( ! self.check1 && ! self.check2 );

		const uint8_t result = sig.emit();

		assert( self.check1 && self.check2 );
		assert( result & ( 1 << 0 ) );
		assert( ! ( result & ( 1 << 1 ) ) );
		assert( ! ( result & ( 1 << 2 ) ) );
	}
};

struct TestCollectorAppEvent {

	struct TestEvent : public app::Event {
	};

	static void	run()
	{
		testStandard();
		testWithPriorties();
	}

	static void testStandard()
	{
		Signal<void( TestEvent & ), app::CollectorEvent<TestEvent>> sig;

		bool check1 = false;
		bool check2 = false;
		bool check3 = false;

		sig.connect( [&]( TestEvent &event ) { check1 = true; } );
		sig.connect( [&]( TestEvent &event ) { check2 = true; event.setHandled(); } );
		sig.connect( [&]( TestEvent &event ) { check3 = true; } );

		TestEvent event;
		app::CollectorEvent<TestEvent> collector( &event );
		sig.emit( collector, event );

		assert( check1 );
		assert( check2 );
		assert( ! check3 );
	}

	static void testWithPriorties()
	{
		Signal<void( TestEvent & ), app::CollectorEvent<TestEvent>> sig;

		bool check1 = false;
		bool check2 = false;

		sig.connect( [&]( TestEvent &event ) { check1 = true; event.setHandled(); } );
		sig.connect( -1, [&]( TestEvent &event ) { check2 = true; } );

		TestEvent event;
		app::CollectorEvent<TestEvent> collector( &event );
		sig.emit( collector, event );

		assert( check1 );
		assert( ! check2 );
	}

};

struct TestConnectionToggling {
	static void run()
	{
		Signal<void (int)> signal;
		int sum = 0;

		auto connection = signal.connect( [&]( int amount ){ sum += amount; } );
		signal.emit( 5 );
		assert( sum == 5 );

		connection.disable();
		signal.emit( 5 );
		assert( ! connection.isEnabled() );
		assert( sum == 5 );

		connection.enable();
		signal.emit( 5 );
		assert( connection.isEnabled() );
		assert( sum == 10 );
	}
};

template <typename TestT>
void runTest()
{
	cout << demangleTypeName( typeid( TestT ).name() ) << ": ";
	TestT::run();
	cout << "OK" << endl;
}

int main()
{
	runTest<BasicSignalTests>();
	runTest<TestMemberCallback>();
	runTest<TestSignalReentry>();
	runTest<TestSignalPriorityGroups>();
	runTest<TestSignalPriorityGroupDisconnections>();
	runTest<TestScopedConnection>();
	runTest<TestCollectorVector>();
	runTest<TestCollectorUntil0>();
	runTest<TestCollectorWhile0>();
	runTest<TestCollectorBooleanAnd>();
	runTest<TestCollectorBitwiseAnd>();
	runTest<TestCollectorAppEvent>();
	runTest<TestConnectionToggling>();
	return 0;
}