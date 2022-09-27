//$ nobt
//$ nocpp

/**
 * @file CDSPProcessor.h
 *
 * @brief The base virtual class for DSP processing algorithms.
 *
 * This file includes the base virtual class for DSP processing algorithm
 * classes like FIR filtering and interpolation.
 *
 * r8brain-free-src Copyright (c) 2013-2021 Aleksey Vaneev
 * See the "LICENSE" file for license.
 */

#ifndef R8B_CDSPPROCESSOR_INCLUDED
#define R8B_CDSPPROCESSOR_INCLUDED

#include "r8bbase.h"

namespace r8b {

/**
 * @brief The base virtual class for DSP processing algorithms.
 *
 * This class can be used as a base class for various DSP processing
 * algorithms (processors). DSP processors that are derived from this class
 * can be seamlessly integrated into various DSP processing graphs.
 */

class CDSPProcessor : public R8B_BASECLASS
{
	R8BNOCTOR( CDSPProcessor );

public:
	CDSPProcessor()
	{
	}

	virtual ~CDSPProcessor()
	{
	}

	/**
	 * @return The latency, in samples, which is present in the output signal.
	 * This value is usually zero if the DSP processor "consumes" the latency
	 * automatically.
	 */

	virtual int getLatency() const = 0;

	/**
	 * @return Fractional latency, in samples, which is present in the output
	 * signal. This value is usually zero if a linear-phase filtering is used.
	 * With minimum-phase filters in use, this value can be non-zero even if
	 * the getLatency() function returns zero.
	 */

	virtual double getLatencyFrac() const = 0;

	/**
	 * @param MaxInLen The number of samples planned to process at once, at
	 * most.
	 * @return The maximal length of the output buffer required when
	 * processing the "MaxInLen" number of input samples.
	 */

	virtual int getMaxOutLen( const int MaxInLen ) const = 0;

	/**
	 * Function clears (resets) the state of *this object and returns it to
	 * the state after construction. All input data accumulated in the
	 * internal buffer so far will be discarded.
	 */

	virtual void clear() = 0;

	/**
	 * Function performs DSP processing.
	 *
	 * @param ip Input data pointer.
	 * @param l0 How many samples to process.
	 * @param[out] op0 Output data pointer. The capacity of this buffer should
	 * be equal to the value returned by the getMaxOutLen() function for the
	 * given "l0". This buffer can be equal to "ip" only if the
	 * getMaxOutLen( l0 ) function returned a value lesser than "l0". This
	 * pointer can be incremented on function's return if latency compensation
	 * was performed by the processor. Note that on function's return, this
	 * pointer may point to some internal buffers, including the "ip" buffer,
	 * ignoring the originally passed value.
	 * @return The number of output samples written to the "op0" buffer and
	 * available after processing. This value can be smaller or larger in
	 * comparison to the original "l0" value due to processing and filter's
	 * latency compensation that took place, and due to resampling if it was
	 * performed.
	 */

	virtual int process( double* ip, int l0, double*& op0 ) = 0;
};

} // namespace r8b

#endif // R8B_CDSPPROCESSOR_INCLUDED
