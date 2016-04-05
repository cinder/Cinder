#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/MotionManager.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

class AngleAxis {
public:
	float 		angle;
	ci::vec3	axis;

	AngleAxis() {}

	AngleAxis( float aAngle, const ci::vec3& aAxis )
		: angle( aAngle ), axis( aAxis ) {}

	AngleAxis( const ci::quat& q  ) {
		ci::vec3 v = normalize( ci::vec3( q.x, q.y, q.z ) );
		axis = q.w > 0.0f ? v : -v;
   		angle = 2.0f * atan2( glm::length( axis ), ( q.w > 0.0f ? q.w : -q.w ) );
	}

	~AngleAxis() {}

	friend std::ostream& operator<<( std::ostream& os, const AngleAxis& obj ) {
		os << "[" << obj.angle << ", <" << obj.axis.x << ", " << obj.axis.y << ", " << obj.axis.z << ">]";
		return os;
	}
};


// We'll create a new Cinder Application by deriving from the App class
class Sensors : public App {
public:
	void setup();
    void touchesBegan( TouchEvent event );
    void touchesEnded( TouchEvent event );
	void draw();

private:
	bool 	mPrint = false;
};

std::string getOrientationString(int io)
{
	std::string result = "Unknown";
	switch( io ) {
		case ci::app::InterfaceOrientation::Portrait: {
			result = "Portrait";
		}
		break;

		case ci::app::InterfaceOrientation::PortraitUpsideDown: {
			result = "PortraitUpsideDown";
		}
		break;

		case ci::app::InterfaceOrientation::LandscapeLeft: {
			result = "LandscapeLeft";
		}
		break;

		case ci::app::InterfaceOrientation::LandscapeRight: {
			result = "LandscapeRight";
		}
		break;
	};
	return result;
}

void prepareSettings( Sensors::Settings* settings )
{
	settings->setFullScreen( true );
}

void Sensors::setup()
{
	MotionManager::enable( 60.0f, MotionManager::Accelerometer );

	int io = getOrientation();
	console() << "InterfaceOrientation: " << getOrientationString( io ) << " : " << io << std::endl;

	console() << "Window Size: " << getWindowSize() << std::endl;
}

void Sensors::touchesBegan( TouchEvent event )
{
	mPrint = true;
}

void Sensors::touchesEnded( TouchEvent event )
{
	mPrint = false;
}

void toAngleAxis( const ci::quat& q, float* angle, ci::vec3* axis )
{

}

void Sensors::draw()
{
	auto rotation = MotionManager::getRotation( getOrientation() );

	vec3 g = MotionManager::getGravity();
	float thetaRad = atan2( g.z, g.x );// - 3.141592f/2.0f;
	float theta =  thetaRad/3.141592f*180.0f;

	if( mPrint ) {
		//vec3 ng = normalize( g );

		//theta = dot( vec3( 0, ng.y, ng.z ), vec3( 0, 1, 0 ) );
		//theta = (acos(theta) - M_PI/2.0)/3.141592f*180.0f;
		//console() << "ng=" << ng << ", theta=" << theta << std::endl;

		auto q = MotionManager::getRotation( getOrientation() );
		//console() << "-\n" << "quat:\n" << q << "\nmat4:" << toMat4( q ) << std::endl;
		//console() << "AngleAxis: " << AngleAxis( q ) << std::endl;

		console() << "Euler=" << glm::eulerAngles( q ) << std::endl;

		//console() << "g=" << g << " : " << "theta=" << theta << std::endl;
		//console() << "rv=" << MotionManager::getRotationVector() << std::endl;
		//console() << "rotation: " << MotionManager::getRotation( getOrientation() ) << std::endl;
	}

	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 10000.0f );
	gl::pushModelMatrix();
	{
		glCullFace( GL_BACK );
		gl::translate( getWindowSize() / 2 );
#if defined( CINDER_ANDROID )
		gl::scale( vec3( 500.0f, 500.0f, 500.0f ) );
#else
		gl::scale( vec3( 200.0f, 200.0f, 200.0f ) );
#endif

		gl::rotate( rotation );

		//gl::rotate( -3.141592f/4.0f, 1, 1, 0 );
		gl::drawColorCube( vec3(0.0f), vec3(1.0f) );


		//gl::rotate( rotation );

		gl::lineWidth( 50.0f );

		gl::color( 1.0f, 0.0f, 0.0f );
		gl::drawLine( vec3( 0, 0, 0 ), vec3( 1.5f, 0, 0 ) );

		gl::color( 0.0f, 1.0f, 0.0f );
		gl::drawLine( vec3( 0, 0, 0 ), vec3( 0, 1.5f, 0 ) );

		gl::color( 0.0f, 0.0f, 1.0f );
		gl::drawLine( vec3( 0, 0, 0 ), vec3( 0, 0, -1.5f ) );

	}
	gl::popModelMatrix();

	/*
	gl::pushModelMatrix();
	{
		auto q = MotionManager::getRotation( getOrientation() );
		auto aa =  AngleAxis( q );
		vec3 dv = aa.axis;

		gl::translate( getWindowSize() / 2 );
		gl::scale( vec3( 1000.0f, 1000.0f, 1000.0f ) );

		gl::lineWidth( 50.0f );
		gl::color( 1.0f, 0.0f, 1.0f );
		gl::drawLine( -dv, dv );
	}
	gl::popModelMatrix();
	*/

	/*
	gl::pushModelMatrix();
	{
		vec3 dv = normalize( MotionManager::getGravity() );

		vec3 g = MotionManager::getGravity();
		float theta = atan2( g.y, g.x )/3.141592f*180.0f;

		gl::translate( getWindowSize() / 2 );
		gl::scale( vec3( 500.0f, 500.0f, 500.0f ) );

		gl::lineWidth( 20.0f );

		gl::color( 1.0f, 0.0f, 0.0f );
		gl::drawLine( vec3( 0, 0, 0 ), dv );
	}
	gl::popModelMatrix();
	*/
}

// This line tells Cinder to actually create the application
CINDER_APP( Sensors, RendererGl, prepareSettings )
