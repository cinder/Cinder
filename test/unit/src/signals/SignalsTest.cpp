
#include "catch.hpp"
#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/Signals.h"
#include "cinder/app/Event.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace ci;
using namespace ci::signals;

namespace {

std::string toString( float value, int precision )
{
	std::ostringstream out;
	out << std::fixed << std::setprecision( precision ) << value;
	return out.str();
}

string sAccum = "";

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

} // namespace

TEST_CASE( "signals/Signals" )
{
	SECTION( "Basic signal tests" )
	{
		sAccum = "";

		Connection conn0;
		REQUIRE( ! conn0.isConnected() );

		Signal<char (float, int, string)> sig1;
		auto conn1 = sig1.connect( floatCallback );
		auto conn2 = sig1.connect( [] ( float, int i, string ) { sAccum += "int: " + to_string( i ) + "\n"; return 0; } );
		auto conn3 = sig1.connect( [] ( float, int, const string &s ) { sAccum += "string: " + s + "\n"; return 0; } );

		REQUIRE( sig1.getNumSlots() == 3 );
		REQUIRE( (conn1.isConnected() && conn2.isConnected() && conn3.isConnected()) );

		sig1.emit( 0.3f, 4, "huhu" );

		bool success;
		success = conn1.disconnect();	REQUIRE( success == true  );	REQUIRE( sig1.getNumSlots() == 2 );
		success = conn1.disconnect();	REQUIRE( success == false ); REQUIRE( sig1.getNumSlots() == 2 );
		success = conn2.disconnect();	REQUIRE( success == true  );	REQUIRE( sig1.getNumSlots() == 1 );
		success = conn3.disconnect();	REQUIRE( success == true  );	REQUIRE( sig1.getNumSlots() == 0 );
		success = conn3.disconnect();	REQUIRE( success == false ); REQUIRE( sig1.getNumSlots() == 0 );
		success = conn2.disconnect();	REQUIRE( success == false );	REQUIRE( sig1.getNumSlots() == 0 );

		REQUIRE( (! conn1.isConnected() && ! conn2.isConnected() && ! conn3.isConnected()) );

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

		REQUIRE( sAccum == expected );
	}

	SECTION( "Signals can call member functions of a class instance" )
	{
		class MemberClass {
		public:
			MemberClass(){ mAccum = 0; }
			void handler()	{ mAccum++;	}
			int mAccum;
		};

		MemberClass instance;
		Signal<void ()> sig;

		auto slot = std::bind( &MemberClass::handler, &instance );
		sig.connect( slot );

		sig.emit();
		REQUIRE( instance.mAccum == 1 );

		instance.mAccum = 0;

		sig.connect( slot );
		sig.connect( slot );

		sig.emit();
		REQUIRE( instance.mAccum == 3 );
	}

	SECTION( "Signals are reentrant" )
	{
		SECTION( "Connections can be disconnected within a signal callback." )
		{
			Signal<void ()>	sig;

			int i = 0;

			auto conn1 = sig.connect( [&]{ i++; } );

			sig.connect( [&] { conn1.disconnect(); i++; } );

			REQUIRE( sig.getNumSlots() == 2 );

			sig.emit();

			REQUIRE( i == 2 );
			REQUIRE( sig.getNumSlots() == 1 );

			sig.emit();

			REQUIRE( i == 3 ); // only one signal, so i only incremented by 1
			REQUIRE( sig.getNumSlots() == 1 );

			sig.connect( [&]{ i++; } );
			sig.emit();

			REQUIRE( i == 5 );
			REQUIRE( sig.getNumSlots() == 2 );
		}
	}

	SECTION( "Signal priority groups decide the order of function calls" )
	{
		SECTION( "Default Group" )
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( [&] { accum += "b"; } );
			sig.connect( [&] { accum += "c"; } );

			sig.emit();

			REQUIRE( accum == "abc" );
		}

		SECTION( "Negative priority connections are called later." )
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( -1, [&] { accum += "b"; } );
			sig.connect( -2, [&] { accum += "c"; } );

			sig.emit();

			REQUIRE( accum == "abc" );
		}

		SECTION( "Positive priority connections are called earlier." )
		{
			Signal<void ()>	sig;
			string accum;

			sig.connect( [&] { accum += "a"; } );
			sig.connect( 1, [&] { accum += "b"; } );
			sig.connect( 2, [&] { accum += "c"; } );

			sig.emit();

			REQUIRE( accum == "cba" );
		}

		SECTION( "Positive and negative priorities can be used on the same signal." )
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

			REQUIRE( accum == "fdbcage" );
		}
	}
	SECTION( "Priority group disconnection" )
	{
		SECTION( "Default Group" )
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( [&] { accum += "a"; } );

			sig.emit();
			REQUIRE( accum == "a" );

			bool success = conn1.disconnect(); REQUIRE( success ); REQUIRE( sig.getNumSlots() == 0 );

			sig.emit();
			REQUIRE( accum == "a" );
		}

		SECTION( "default group, two connections" )
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( [&] { accum += "a"; } );
			auto conn2 = sig.connect( [&] { accum += "b"; } );

			sig.emit();
			REQUIRE( accum == "ab" );

			bool success = conn1.disconnect();	REQUIRE( success ); REQUIRE( sig.getNumSlots() == 1 );

			sig.emit();
			REQUIRE( accum == "abb" );

			success = conn2.disconnect();		REQUIRE( success ); REQUIRE( sig.getNumSlots() == 0 );
			sig.emit();
			REQUIRE( accum == "abb" );
		}

		SECTION( "default group and another group, three connections" )
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( [&] { accum += "a"; } );
			auto conn2 = sig.connect( 2, [&] { accum += "b"; } );
			auto conn3 = sig.connect( 2, [&] { accum += "c"; } );

			sig.emit();
			REQUIRE( accum == "bca" );

			bool success = conn2.disconnect();	REQUIRE( success ); REQUIRE( sig.getNumSlots() == 2 );
			accum.clear();
			sig.emit();
			REQUIRE( accum == "ca" );

			success = conn3.disconnect();		REQUIRE( success ); REQUIRE( sig.getNumSlots() == 1 );
			accum.clear();
			sig.emit();
			REQUIRE( accum == "a" );

			success = conn1.disconnect();		REQUIRE( success ); REQUIRE( sig.getNumSlots() == 0 );
			accum.clear();
			sig.emit();
			REQUIRE( accum == "" );
		}

		SECTION( "add to group, remove all of group, then add another" )
		{
			Signal<void ()>	sig;
			string accum;

			auto conn1 = sig.connect( 2, [&] { accum += "a"; } );
			auto conn2 = sig.connect( 2, [&] { accum += "b"; } );
			auto conn3 = sig.connect( 2, [&] { accum += "c"; } );

			bool success = conn1.disconnect();	REQUIRE( success ); REQUIRE( sig.getNumSlots() == 2 );
			success = conn2.disconnect();	REQUIRE( success ); REQUIRE( sig.getNumSlots() == 1 );
			success = conn3.disconnect();	REQUIRE( success ); REQUIRE( sig.getNumSlots() == 0 );

			auto conn4 = sig.connect( 2, [&] { accum += "d"; } );

			sig.emit();
			REQUIRE( accum == "d" );
		}
	}

	SECTION( "Scoped connections" )
	{
		Signal<void ()> sig;
		int accum = 0;
		auto slot = [&] { accum++; };

		sig.connect( slot );
		ScopedConnection conn1 = sig.connect( slot );

		{
			ScopedConnection conn2 = sig.connect( slot );
			REQUIRE( sig.getNumSlots() == 3 );
		}

		REQUIRE( sig.getNumSlots() == 2 );

		sig.emit();
		REQUIRE( accum == 2 );
	}

	SECTION("Signal result collection")
	{
		SECTION("TestCollectorVector")
		{
			auto handler1 = [] { return 1; };
			auto handler42 = []  { return 42; };
			auto handler777 = []	{ return 777; };

			Signal<int (), CollectorVector<int>> sig;

			sig.connect( handler777 );
			sig.connect( handler42 );
			sig.connect( handler1 );
			sig.connect( handler42 );
			sig.connect( handler777 );

			vector<int> results = sig.emit();

			const vector<int> reference = { 777, 42, 1, 42, 777 };
			REQUIRE( results == reference );
		}
		SECTION( "TestCollectorUntil0" )
		{
			struct TestCollectorUntil0 {
				bool check1, check2;

				TestCollectorUntil0()
					: check1( false ) , check2( false )
				{}

				bool handlerTrue()		{ check1 = true; return true; }
				bool handlerFalse()		{ check2 = true; return false; }
				bool handlerAbort()		{ abort(); }
			};

			TestCollectorUntil0 self;
			Signal<bool (), CollectorUntil0<bool>> sig;

			sig.connect( slot( self, &TestCollectorUntil0::handlerTrue ) );
			sig.connect( slot( self, &TestCollectorUntil0::handlerFalse ) );
			sig.connect( slot( self, &TestCollectorUntil0::handlerAbort ) );

			REQUIRE( (! self.check1 && ! self.check2) );

			const bool result = sig.emit();

			REQUIRE( (! result && self.check1 && self.check2) );
		}

		SECTION( "TestCollectorWhile0" )
		{
			struct TestCollectorWhile0 {
				bool check1, check2;

				TestCollectorWhile0()
					: check1( false ), check2( false )
				{}

				bool handler0()		{ check1 = true; return false; }
				bool handler1()		{ check2 = true; return true; }
				bool handlerAbort()	{ abort(); }
			};

			TestCollectorWhile0 self;
			Signal<bool (), CollectorWhile0<bool>> sig;

			sig.connect( slot( self, &TestCollectorWhile0::handler0 ) );
			sig.connect( slot( self, &TestCollectorWhile0::handler1 ) );
			sig.connect( slot( self, &TestCollectorWhile0::handlerAbort ) );

			REQUIRE( (! self.check1 && ! self.check2) );

			const bool result = sig.emit();

			REQUIRE( (result == true && self.check1 && self.check2) );
		}

		SECTION( "TestCollectorBooleanAnd" )
		{
			struct TestCollectorBooleanAnd {
				bool check1, check2, check3;

				TestCollectorBooleanAnd()
					: check1( false ), check2( false ), check3( false )
				{}

				bool handlerTrue()		{ check1 = true; return true; }
				bool handlerFalse()		{ check2 = true; return false; }
				bool handlerTrue2()		{ check3 = true; return true; }
			};

			TestCollectorBooleanAnd self;
			Signal<bool (), CollectorBooleanAnd> sig;

			sig.connect( slot( self, &TestCollectorBooleanAnd::handlerTrue ) );
			sig.connect( slot( self, &TestCollectorBooleanAnd::handlerFalse ) );
			sig.connect( slot( self, &TestCollectorBooleanAnd::handlerTrue2 ) );

			REQUIRE( (! self.check1 && ! self.check2 && ! self.check3) );

			const bool result = sig.emit();

			REQUIRE( (! result && self.check1 && self.check2 && self.check3) );
		}

		SECTION( "TestCollectorBitwiseAnd" )
		{
			struct TestCollectorBitwiseAnd {
				bool check1, check2;

				TestCollectorBitwiseAnd()
				: check1( false ), check2( false )
				{}

				uint8_t handler1()	{ check1 = true; return ( 1 << 0 ); }
				uint8_t handler2()	{ check2 = true; return ( 1 << 0 ) | ( 1 << 2 ); }
			};

			TestCollectorBitwiseAnd self;
			Signal<uint8_t (), CollectorBitwiseAnd<uint8_t> > sig;

			sig.connect( slot( self, &TestCollectorBitwiseAnd::handler1 ) );
			sig.connect( slot( self, &TestCollectorBitwiseAnd::handler2 ) );

			REQUIRE( (! self.check1 && ! self.check2) );

			const uint8_t result = sig.emit();

			REQUIRE( ( self.check1 && self.check2 ) );
			REQUIRE( ( result & ( 1 << 0 ) ) );
			REQUIRE_FALSE( ( result & ( 1 << 1 ) ) );
			REQUIRE_FALSE( ( result & ( 1 << 2 ) ) );
		};

		SECTION( "TestCollectorAppEvent" )
		{

			struct TestEvent : public app::Event {
			};

			SECTION( "Test standard" )
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

				REQUIRE( check1 );
				REQUIRE( check2 );
				REQUIRE( ! check3 );
			}

			SECTION( "Test with priorities" )
			{
				Signal<void( TestEvent & ), app::CollectorEvent<TestEvent>> sig;

				bool check1 = false;
				bool check2 = false;

				sig.connect( [&]( TestEvent &event ) { check1 = true; event.setHandled(); } );
				sig.connect( -1, [&]( TestEvent &event ) { check2 = true; } );

				TestEvent event;
				app::CollectorEvent<TestEvent> collector( &event );
				sig.emit( collector, event );

				REQUIRE( check1 );
				REQUIRE( ! check2 );
			}
		}
	}

	SECTION( "Connection Toggling" )
	{
		SECTION( "Disabling a connection temporarily removes it from signal flow. It can be reenabled." )
		{
			Signal<void (int)> signal;
			int sum = 0;

			auto connection = signal.connect( [&]( int amount ){ sum += amount; } );
			signal.emit( 5 );
			REQUIRE( sum == 5 );

			connection.disable();
			signal.emit( 5 );
			REQUIRE( ! connection.isEnabled() );
			REQUIRE( sum == 5 );

			connection.enable();
			signal.emit( 5 );
			REQUIRE( connection.isEnabled() );
			REQUIRE( sum == 10 );
		}
	}
} // Signals
