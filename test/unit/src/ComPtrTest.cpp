#include "catch.hpp"
#include "cinder/msw/CinderMsw.h"

using namespace cinder;
using namespace cinder::msw;

// Mock COM interface for testing
class MockCom : public IUnknown {
  public:
	MockCom()
		: mRefCount( 0 )
		, mAddRefCount( 0 )
		, mReleaseCount( 0 )
	{
	}

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID riid, void** ppv ) override
	{
		if( riid == IID_IUnknown ) {
			*ppv = static_cast<IUnknown*>( this );
			AddRef();
			return S_OK;
		}
		*ppv = nullptr; // COM spec requires setting to nullptr on failure
		return E_NOINTERFACE;
	}

	STDMETHODIMP_( ULONG ) AddRef() override
	{
		mAddRefCount++;
		return ++mRefCount;
	}

	STDMETHODIMP_( ULONG ) Release() override
	{
		mReleaseCount++;
		ULONG count = --mRefCount;
		if( count == 0 ) {
			delete this;
		}
		return count;
	}

	// Test helpers
	ULONG getRefCount() const { return mRefCount; }
	ULONG getAddRefCount() const { return mAddRefCount; }
	ULONG getReleaseCount() const { return mReleaseCount; }

  private:
	ULONG mRefCount;
	ULONG mAddRefCount;
	ULONG mReleaseCount;
};

TEST_CASE( "ComPtr" )
{
	SECTION( "Default construction" )
	{
		ComPtr<MockCom> ptr;
		REQUIRE( ptr.get() == nullptr );
		REQUIRE( !ptr );
	}

	SECTION( "Construction from raw pointer - WRL semantics (adopts without AddRef)" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef(); // Simulates COM creation (refcount = 1)

		ComPtr<MockCom> ptr( raw );
		REQUIRE( ptr.get() == raw );
		REQUIRE( ptr );
		REQUIRE( raw->getRefCount() == 1 ); // Constructor DOES NOT call AddRef (WRL semantics)

		ptr.reset(); // Should call Release
		// raw is now deleted, cannot access it
	}

	SECTION( "Copy construction - shares ownership with AddRef" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef(); // refcount = 1

		ComPtr<MockCom> ptr1( raw ); // Adopts (no AddRef), refcount still 1
		ComPtr<MockCom> ptr2( ptr1 ); // Copy (calls AddRef), refcount = 2

		REQUIRE( ptr1.get() == raw );
		REQUIRE( ptr2.get() == raw );
		REQUIRE( raw->getRefCount() == 2 ); // ptr1 adopted, ptr2 AddRef'd

		ptr1.reset();
		REQUIRE( raw->getRefCount() == 1 );

		ptr2.reset();
		// raw is deleted
	}

	SECTION( "Move construction - transfers ownership without AddRef" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef(); // refcount = 1

		ComPtr<MockCom> ptr1( raw ); // Adopts (no AddRef), refcount still 1
		ULONG			addRefsBefore = raw->getAddRefCount();

		ComPtr<MockCom> ptr2( std::move( ptr1 ) );

		REQUIRE( ptr1.get() == nullptr ); // ptr1 should be empty
		REQUIRE( ptr2.get() == raw );
		REQUIRE( raw->getRefCount() == 1 );			   // Should still be 1 (no AddRef on move)
		REQUIRE( raw->getAddRefCount() == addRefsBefore ); // No additional AddRef

		ptr2.reset();
		// raw is deleted
	}

	SECTION( "Assignment from raw pointer - adopts without AddRef" )
	{
		MockCom* raw1 = new MockCom();
		MockCom* raw2 = new MockCom();
		raw1->AddRef(); // refcount = 1
		raw2->AddRef(); // refcount = 1

		ComPtr<MockCom> ptr( raw1 ); // Adopts, refcount = 1
		REQUIRE( raw1->getRefCount() == 1 );

		ptr = raw2; // Should release raw1 and adopt raw2 (no AddRef)
		REQUIRE( ptr.get() == raw2 );
		REQUIRE( raw2->getRefCount() == 1 ); // Adopted without AddRef

		ptr.reset();
		// raw2 is deleted
	}

	SECTION( "Copy assignment - shares ownership with AddRef" )
	{
		MockCom* raw1 = new MockCom();
		MockCom* raw2 = new MockCom();
		raw1->AddRef();
		raw2->AddRef();

		ComPtr<MockCom> ptr1( raw1 );
		ComPtr<MockCom> ptr2( raw2 );

		REQUIRE( raw1->getRefCount() == 1 );
		REQUIRE( raw2->getRefCount() == 1 );

		ptr1 = ptr2; // Should release raw1, AddRef raw2
		REQUIRE( ptr1.get() == raw2 );
		REQUIRE( ptr2.get() == raw2 );
		REQUIRE( raw2->getRefCount() == 2 ); // ptr1 and ptr2 both reference it

		ptr1.reset();
		REQUIRE( raw2->getRefCount() == 1 );

		ptr2.reset();
		// raw2 is deleted
	}

	SECTION( "Move assignment - transfers ownership without AddRef" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr1( raw );
		ComPtr<MockCom> ptr2;

		ULONG addRefsBefore = raw->getAddRefCount();
		ptr2 = std::move( ptr1 );

		REQUIRE( ptr1.get() == nullptr );
		REQUIRE( ptr2.get() == raw );
		REQUIRE( raw->getRefCount() == 1 );
		REQUIRE( raw->getAddRefCount() == addRefsBefore ); // No AddRef on move

		ptr2.reset();
		// raw is deleted
	}

	SECTION( "attach() adopts ownership without AddRef" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef(); // Simulates API returning pointer with refcount=1

		ComPtr<MockCom> ptr;
		ULONG			addRefsBefore = raw->getAddRefCount();

		ptr.attach( raw ); // Should take ownership WITHOUT calling AddRef

		REQUIRE( ptr.get() == raw );
		REQUIRE( raw->getRefCount() == 1 );			   // Should still be 1
		REQUIRE( raw->getAddRefCount() == addRefsBefore ); // No AddRef called

		ptr.reset(); // Calls Release, deletes object
	}

	SECTION( "attach() releases old pointer before adopting new one" )
	{
		MockCom* raw1 = new MockCom();
		MockCom* raw2 = new MockCom();
		raw1->AddRef();
		raw2->AddRef();

		ComPtr<MockCom> ptr;
		ptr.attach( raw1 );
		REQUIRE( raw1->getRefCount() == 1 );

		ptr.attach( raw2 ); // Should release raw1, adopt raw2
		REQUIRE( raw2->getRefCount() == 1 );

		ptr.reset(); // Releases raw2
	}

	SECTION( "detach() releases ownership without calling Release" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		MockCom* detached = ptr.detach();

		REQUIRE( ptr.get() == nullptr );
		REQUIRE( detached == raw );
		REQUIRE( raw->getRefCount() == 1 ); // Should still be 1 (detach doesn't call Release)

		detached->Release(); // Manual cleanup
	}

	SECTION( "releaseAndGetAddressOf() releases existing pointer" )
	{
		MockCom* raw1 = new MockCom();
		MockCom* raw2 = new MockCom();
		raw1->AddRef();
		raw2->AddRef();

		ComPtr<MockCom> ptr( raw1 );
		REQUIRE( raw1->getRefCount() == 1 );

		// Simulate COM API writing to output parameter
		MockCom** ppOut = ptr.releaseAndGetAddressOf();
		// raw1 should have been released and deleted

		*ppOut = raw2; // Simulates API call
		REQUIRE( ptr.get() == raw2 );
		REQUIRE( raw2->getRefCount() == 1 );

		ptr.reset();
	}

	SECTION( "getAddressOf() does NOT release existing pointer" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );
		REQUIRE( raw->getRefCount() == 1 );

		MockCom** ppOut = ptr.getAddressOf();
		REQUIRE( *ppOut == raw );		 // Should still point to raw
		REQUIRE( raw->getRefCount() == 1 ); // Should NOT have released

		ptr.reset();
	}

	SECTION( "copyTo() copies interface with AddRef" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );
		REQUIRE( raw->getRefCount() == 1 );

		MockCom* outPtr = nullptr;
		ptr.copyTo( &outPtr );

		REQUIRE( outPtr == raw );
		REQUIRE( raw->getRefCount() == 2 ); // copyTo should have called AddRef

		outPtr->Release(); // Clean up the copied reference
		ptr.reset();
	}

	SECTION( "Comparison operators" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr1;
		ComPtr<MockCom> ptr2;
		ptr1.attach( raw );

		REQUIRE( ptr1 != ptr2 );
		REQUIRE( ptr1 == raw );
		REQUIRE( ptr2 == nullptr );

		ptr2 = ptr1; // Copy assignment (AddRef)
		REQUIRE( ptr1 == ptr2 );

		ptr1.reset();
		ptr2.reset();
		REQUIRE( ptr1 == ptr2 );
	}

	SECTION( "swap()" )
	{
		MockCom* raw1 = new MockCom();
		MockCom* raw2 = new MockCom();
		raw1->AddRef();
		raw2->AddRef();

		ComPtr<MockCom> ptr1( raw1 );
		ComPtr<MockCom> ptr2( raw2 );

		ptr1.swap( ptr2 );

		REQUIRE( ptr1.get() == raw2 );
		REQUIRE( ptr2.get() == raw1 );
		REQUIRE( raw1->getRefCount() == 1 ); // No refcount changes
		REQUIRE( raw2->getRefCount() == 1 );

		ptr1.reset();
		ptr2.reset();
	}

	SECTION( "Destructor calls Release" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		{
			ComPtr<MockCom> ptr( raw );
			REQUIRE( raw->getRefCount() == 1 );
		} // ptr goes out of scope, should delete raw
	}

	SECTION( "Self-assignment safety (raw pointer)" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		ptr = raw; // Assign same raw pointer
		REQUIRE( ptr.get() == raw );
		REQUIRE( raw->getRefCount() == 1 ); // Should not have changed

		ptr.reset();
	}

	SECTION( "Self-assignment safety (ComPtr)" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		ptr = ptr; // Self-assignment
		REQUIRE( ptr.get() == raw );
		REQUIRE( raw->getRefCount() == 1 );

		ptr.reset();
	}

	SECTION( "Move self-assignment safety" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		ptr = std::move( ptr ); // Self-move-assignment
		REQUIRE( ptr.get() == raw );
		REQUIRE( raw->getRefCount() == 1 );

		ptr.reset();
	}

	SECTION( "reset() with no parameters clears to null" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );
		REQUIRE( ptr.get() == raw );
		REQUIRE( raw->getRefCount() == 1 );

		ptr.reset(); // Should release and set to null
		REQUIRE( ptr.get() == nullptr );
		// raw is deleted
	}

	SECTION( "as() QueryInterface helper" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		ComPtr<IUnknown> unkPtr;
		HRESULT			 hr = ptr.as( &unkPtr );

		REQUIRE( SUCCEEDED( hr ) );
		REQUIRE( unkPtr.get() != nullptr );
		REQUIRE( raw->getRefCount() == 2 ); // as() calls QueryInterface which calls AddRef

		unkPtr.reset();
		ptr.reset();
	}

	SECTION( "as() with nullptr returns E_POINTER and doesn't crash" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		ComPtr<IUnknown>* nullPtr = nullptr;
		HRESULT			  hr = ptr.as( nullPtr );

		REQUIRE( hr == E_POINTER );
		REQUIRE( ptr.get() == raw );		// ptr should be unchanged
		REQUIRE( raw->getRefCount() == 1 ); // refcount should be unchanged

		ptr.reset();
	}

	SECTION( "as() raw pointer overload clears output on failure" )
	{
		MockCom* raw = new MockCom();
		raw->AddRef();

		ComPtr<MockCom> ptr( raw );

		// Test that as() clears the output pointer even when QueryInterface fails
		// We'll use IDispatch which MockCom doesn't implement
		IDispatch* outPtr = reinterpret_cast<IDispatch*>( static_cast<uintptr_t>( 0xDEADBEEF ) ); // Set to non-null garbage
		HRESULT	   hr = ptr.as( &outPtr );

		REQUIRE( FAILED( hr ) );			// Should fail (MockCom doesn't implement IDispatch)
		REQUIRE( outPtr == nullptr );		// as() should clear output parameter upfront
		REQUIRE( ptr.get() == raw );		// ptr should be unchanged
		REQUIRE( raw->getRefCount() == 1 ); // refcount should be unchanged

		ptr.reset();
	}
}
