#ifndef GENERATED_LOAD_TEST_HPP
#define GENERATED_LOAD_TEST_HPP
namespace glload
{
	/**
	\brief Stores data to detect if an extension loaded correctly.
	
	You can test this class in conditions (it uses the safe-bool idiom, so it
	should be fine). If it tests false, then the extension failed to load at
	all. It will only be false if the extension is not in the extensions list
	at all.
	
	If it tests true, then the extension loaded at least partially. You may
	call NumMissing() to query the number of functions that couldn't be loaded.
	If this number is 0, then the extension loaded perfectly.
	
	Please do not reset this class or use constructors/copy assignment to do
	so.
	**/
	class LoadTest
	{
	private:
		//Safe bool idiom. Joy!
		typedef void (LoadTest::*bool_type)() const;
		void big_long_name_that_really_doesnt_matter() const {}
		
	public:
		///Converts to false if the extension wasn't advertised. True if it is, even if a partial load.
		operator bool_type() const
		{
			return m_isLoaded ? &LoadTest::big_long_name_that_really_doesnt_matter : 0;
		}
		
		///Returns the number of functions that failed to load.
		int GetNumMissing() const {return m_numMissing;}
		
		LoadTest() : m_isLoaded(false), m_numMissing(0) {}
		LoadTest(bool isLoaded, int numMissing) : m_isLoaded(isLoaded), m_numMissing(isLoaded ? numMissing : 0) {}
	private:
		bool m_isLoaded;
		int m_numMissing;
	};
}
#endif /*GENERATED_LOAD_TEST_HPP*/
