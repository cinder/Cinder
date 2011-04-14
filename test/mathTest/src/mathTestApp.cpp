#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cinder/Matrix.h"
#include "cinder/Timer.h"
#include "cinder/Vector.h"
using namespace ci;

static std::string pre = "   ";

template < typename T, int Dim > struct MatrixData {};

template < typename T > struct MatrixData< T, 2 > {

};

template < typename T > struct MatrixData< T, 3 > {

};

template < typename T > struct MatrixData< T, 4 > {
	static Matrix44<T> transposeControl() {
		return Matrix44<T>( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 );
	}

	static Matrix44<T> transposeTestData() {
		return Matrix44<T>( 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 );
	}

	static Matrix44<T> inverseControl() {
		return Matrix44<T>(
			(T)-0.119272, (T)-0.812728, (T)-0.024880, (T) 0.409916,
			(T) 0.353100, (T) 0.501818, (T) 0.013258, (T) 0.594132,
			(T)-1.185695, (T) 0.394550, (T) 0.019478, (T) 0.812060,
			(T) 0.000306, (T)-0.000121, (T) 0.000405, (T)-0.000446
		);
	}

	static Matrix44<T> inverseTestData() {
		return Matrix44<T>(
			(T) 0.161578, (T)0.760163, (T)-0.629320, (T)  15.290325,
			(T)-0.923644, (T)0.341056, (T) 0.174820, (T) -76.221232,
			(T) 0.347525, (T)0.553021, (T) 0.757228, (T)2432.976748,
			(T) 0.676350, (T)0.930949, (T) 0.209459, (T)   1.000000
		);
	}										 
};

std::string MakeIndentedBlock( const std::stringstream& src )
{
	std::stringstream ss;
	std::string data = src.str();	
	if( ! data.empty() ) {
		ss << pre;
		for( size_t i = 0; i < data.size(); ++i ) {
			char c = data[i];
			ss << c;
			if( '\n' == c ) {
				ss << pre;
			}
		}
	}
	return ss.str();
}

void DoTest( const std::string& name, bool (*testFunc)(std::ostream&) );

template < typename MatT >
bool TestMatrixTranslate( std::ostream& os );

template < typename MatT >
bool TestMatrixTranspose( std::ostream& os );

template < typename MatT >
bool TestMatrixInvert( std::ostream& os );

/*
void BenchMatrix44Invert( size_t count = 1000 )
{
	std::cout << "Benchmarking Matrix44 invert" << std::endl;

	std::vector<Matrix44f> array1;
	for( size_t i = 0; i < count; ++i ) {
		array1.push_back( Matrix44f::createRotation( Vec3f( (float)i, 0, 0 ) ) );
	}

	Timer timer;

	{
		timer.start();
		Matrix44f res;
		for( size_t i = 0; i < array1.size(); ++i ) {
			res *= array1[i].inverted();
		}
		timer.stop();
		// Print this out so the compiler doesn't optimize it away
		std::cout << "\n" << res;
		std::cout << "Matrix44f::inverted() : " << count << " inverts took " << timer.getSeconds() << " seconds" << std::endl;
	}

	{
		timer.start();
		Matrix44f res;
		for( size_t i = 0; i < array1.size(); ++i ) {
			res *= array1[i].inverted_hai();
		}
		timer.stop();
		// Print this out so the compiler doesn't optimize it away
		std::cout << "\n" << res;
		std::cout << "Matrix44f::inverted_hai() : " << count << " inverts took " << timer.getSeconds() << " seconds" << std::endl;
	}
}

void TestPaulInvert()
{
	Matrix44f mat(
		0.583440f,	  -0.17057f,		0.0f,		131.0f,
        -0.016667f,		0.35046f,      0.0f,		196.0f,
        0.0f,			 0.0f,      1.0f,         0.0f,
        -2.1701e-005f,  -0.0051346f,  0.0f,         1.0f );

	//std::cout << mat << std::endl;
	mat.transpose();
	std::cout << mat << std::endl;

	Matrix44f m0 = mat.inverted();
	Matrix44f m1 = mat.inverted_hai();

	std::cout << "m0:" << "\n";
	std::cout << m0;

	std::cout << "m1:" << "\n";
	std::cout << m1;

	std::cout << "Diff:" << "\n";
	std::cout << (m1 - m0);
}
*/

#define DO_TEST( _FUNC_ ) \
	DoTest( #_FUNC_, &_FUNC_ );

int main( int argc, char **argv )
{
	std::cout << std::endl;

	DO_TEST( TestMatrixTranslate<Matrix44f> );
	DO_TEST( TestMatrixTranspose<Matrix44f> );
	DO_TEST( TestMatrixInvert<Matrix44f> );
	
	//DoTest( "TestMatrix44Translate", &TestMatrix44Translate );
	//DoTest( "TestMatrix44Transpose", &TestMatrix44Transpose );
	//DoTest( "TestMatrix44Invert", &TestMatrix44Invert );

	////BenchMatrix44Invert( 10000000 );
	//TestPaulInvert();

	std::cout << std::endl;

	return 0;
}

void DoTest( const std::string& name, bool (*testFunc)( std::ostream& os ) )
{
	std::cout << "Testing " << name << "\n";
	std::stringstream ss;
	bool result = testFunc( ss );
	std::string output = MakeIndentedBlock( ss );
	if( ! output.empty() ) {
		std::cout << output << "\n";
	}
	std::cout << pre << "Result for " << name << ": " << (result ? "passed" : "failed" );
	std::cout << "\n" << std::endl;
}

template < typename MatT >
bool TestMatrixTranslate( std::ostream& os )
{
	typedef MatT::TYPE T;

	T x = (T)1.23;
	T y = (T)4.56;
	T z = (T)7.89;

	MatT control;
	control.setToIdentity();
	control.m03 = x;
	control.m13 = y;
	control.m23 = z;

	MatT src;
	src.setToIdentity();
	src.translate( Vec3<T>( x, y, z ) );

	bool result = (control == src);
	if( ! result ) {
		os << "Input Matrix:" << "\n";
		os << src;
		os << "Control Matrix: " << "\n";
		os << control;
		os << "Input translate values did not match control";
	}

	return (control == src);
}

template < typename MatT >
bool TestMatrixTranspose( std::ostream& os )
{
	typedef typename MatT::TYPE T; 	
	MatT control = MatrixData<T, MatT::DIM>::transposeControl();
	MatT src = MatrixData<T, MatT::DIM>::transposeTestData();

	// Transposed
	bool result = (control == src.transposed());
	if( ! result ) {
		os << "Input Matrix:" << "\n";
		os << src;
		os << "Input Matrix transposed():" << "\n";
		os << src;
		os << "Control Matrix: " << "\n";
		os << control;
		os << "Input tranposed() did not match control";
		return false;
	}

	// Transpose
	src.transpose();
	result = (control == src);
	if( ! result ) {
		os << "Input Matrix:" << "\n";
		os << src;
		os << "Control Matrix: " << "\n";
		os << control;
		os << "Input tranpose() (i.e. inplace) did not match control";
		return false;
		return false;
	}

	return true;
}

template < typename MatT >
bool TestMatrixInvert( std::ostream& os )
{
	typedef typename MatT::TYPE T; 	
	MatT control = MatrixData<T, MatT::DIM>::inverseControl();
	MatT src = MatrixData<T, MatT::DIM>::inverseTestData();
	MatT resMat;
	MatT identMat;
	identMat.setToIdentity();

	os << "Testing inverted()" << "\n";
	{
		bool result = (control == src.inverted());
		if( ! result ) {
			os << "Input Matrix:" << "\n";
			os << src;
			os << "Input Matrix inverted():" << "\n";
			os << src.inverted();
			os << "Control Matrix: " << "\n";
			os << control;
			os << "Input inverted() did not match control";
		}
		else {
			os << "inverted() passed" << "\n";
		}
	}
	
	int subSteps = 100;
	os << "\nTesting x-axis rotation inverse" << "\n";
	{
		bool result = true;
		int angle = 0;
		for( ; angle < 360*subSteps; ++angle ) {
			control = MatT::createRotation( Vec3<T>( (T)angle/(T)subSteps, 0, 0 ) );
			src = MatT::createRotation( Vec3<T>( (T)angle/(T)subSteps, 0, 0 ) );
			src.invert();
			resMat = control*src;
			if( resMat != identMat ) {
				result = false;
				break;
			}
		}
		if( ! result ) {
			os << "Angle: " << angle << "\n";
			os << "Input Matrix:" << "\n";
			os << src;
			os << "Control Matrix: " << "\n";
			os << control;
			os << "Result Matrix: (resMat = control*src)" << "\n";
			os << resMat;
			os << "Input result did not match control" << "\n";
		}
		else {
			os << "x-axis rotation inverse passed" << "\n";
		}
	}

	os << "\nTesting y-axis rotation inverse" << "\n";
	{
		bool result = true;
		int angle = 0;
		for( ; angle < 360*subSteps; ++angle ) {
			control = MatT::createRotation( Vec3<T>( 0, (T)angle/(T)subSteps, 0 ) );
			src = MatT::createRotation( Vec3<T>( 0, (T)angle/(T)subSteps, 0 ) );
			src.invert();
			resMat = control*src;
			if( resMat != identMat ) {
				result = false;
				break;
			}
		}
		if( ! result ) {
			os << "Angle: " << angle << "\n";
			os << "Input Matrix:" << "\n";
			os << src;
			os << "Control Matrix: " << "\n";
			os << control;
			os << "Input result did not match control" << "\n";
		}
		else {
			os << "y-axis rotation inverse passed" << "\n";
		}
	}

	os << "\nTesting z-axis rotation inverse" << "\n";
	{
		bool result = true;
		int angle = 0;
		for( ; angle < 360*subSteps; ++angle ) {
			control = MatT::createRotation( Vec3<T>( 0, (T)angle/(T)subSteps, 0 ) );
			src = MatT::createRotation( Vec3<T>( 0, (T)angle/(T)subSteps, 0 ) );
			src.invert();
			resMat = control*src;
			if( resMat != identMat ) {
				result = false;
				break;
			}
		}
		if( ! result ) {
			os << "Angle: " << angle << "\n";
			os << "Input Matrix:" << "\n";
			os << src;
			os << "Control Matrix: " << "\n";
			os << control;
			os << "Input result did not match control" << "\n";
		}
		else {
			os << "z-axis rotation inverse passed" << "\n";
		}
	}
	
	return true;
}