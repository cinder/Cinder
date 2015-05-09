#define BOOST_TEST_MODULE "Cinder Audio2 Unit Tests"

#include <boost/test/included/unit_test.hpp>

// the single header variant of Boost Test cannot handle multiple .cpp's,
// so they are included as headers.

#include "BufferUnit.h"
#include "FftUnit.h"
#include "RingbufferUnit.h"
