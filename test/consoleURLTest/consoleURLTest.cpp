#include <iostream>

#include "flint/URL.h"
#include "flint/ImageFile.h"

using namespace fli;

int main (int argc, char * const argv[]) {
    std::cout << "Hello, World!\n";

	shared_ptr<StreamURL> stream = loadURL( "http://www.syntheastwood.com/files/gimgs/40_13daquar-with-bank.jpg" );
	shared_ptr<StreamURL> stream2 = loadURL( "http://upload.wikimedia.org/wikipedia/commons/e/ee/Latin_alphabet.png" );
//	shared_ptr<StreamURL> stream2 = loadURL( "http://tempest.barbariangroup.com/trac/flint/chrome/common/tbg_banner.png" );
	shared_ptr<StreamURL> streamHTML = loadURL( "http://apple.com" );
	if( ( ! stream ) || ( ! stream2 ) || ( ! streamHTML ) ) {
		std::cout << "Failed to load URL" << std::endl;
		return -1;
	}

	std::cout << "Stream 1 size " << stream->size() << std::endl;

	std::cout << "HTML response code: " << streamHTML->getResponseCode() << std::endl;
	std::cout << "Effective HTML URL: " << streamHTML->getEffectiveURL() << std::endl;

	while( ! streamHTML->isEOF() ) {
		int8_t c;
		streamHTML->read( &c );
		std::cout << c;
	}

	shared_ptr<ImageFile> img = loadImage( stream );
	if( ! img ) {
		std::cout << "Failed to load image 1" << std::endl;
		return -1;
	}
	std::cout << "image 1: " << img->getWidth() << " x " << img->getHeight() << " and " << stream->size() << " bytes" << std::endl;

	shared_ptr<ImageFile> img2 = loadImage( stream2 );
	if( ! img2 ) {
		std::cout << "Failed to load image 2" << std::endl;
		return -1;
	}
	std::cout << "image 2: " << img2->getWidth() << " x " << img2->getHeight() << " and " << stream2->size() << " bytes" << std::endl;

    return 0;
}
