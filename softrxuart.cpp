/*
 * Software receive-only UART.
 *
 * Copyright 2017 Christopher Williams.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "softrxuart.h"

/*
 * A few design notes/peculiarities....
 *
 * When we detect the falling edge of the start bit, instead of setting
 * bit_sample_timer to 1.5 bit times (to sample the center of the first data
 * bit), we set bit_sample_timer to half of a bit time and treat the start bit
 * as an extra bit. That extra bit will fall off the bottom of the received
 * byte when the 8th data bit is received. This is to prevent overflowing
 * bit_sample_timer because int_rate may be greater than 170 (171*1.5=256.5).
 *
 * This actually treats bit_sample_timer as if it were negative, and it expires
 * when it overflows to 0 or positive.
 *
 * This uses bit_counter to tell us the current sampling state:
 *      bit_counter  = 10: reading start bit
 * 2 <= bit_counter <=  9: reading data bit
 *      bit_counter  =  1: reading/expecting stop bit
 *      bit_counter  =  0: waiting for start bit
 *
 * If bit_counter is not 0, it's decremented each bit time.
 */

//#define REDUCE_INT_RATE 1

SoftRxUart::SoftRxUart(uint32_t bit_rate, uint32_t int_rate)
: bit_counter(0)
{
#if REDUCE_INT_RATE
	// bit_rate must be less than int_rate/2
	if (bit_rate >= int_rate/2) return;

	/*
	 * Reduce bit_rate and int_rate if int_rate doesn't fit in 8-bit
	 * unsigned.
	 *
	 * XXX this should be done at compile time to minimize run-time
	 * overhead.
	 */
	while (int_rate > 255) {

#define REDUCE(n) \
	if ((int_rate % n) == 0 && (bit_rate % n) == 0) { \
		int_rate /= n; bit_rate /= n; \
	}

		/*
		 * Try to reduce int_rate and bit_rate by common factors of
		 * UART speeds (2, 3, 5).
		 */
		REDUCE(8) else
		REDUCE(2) else
		REDUCE(3) else
		REDUCE(5) else {
			// int_rate is too big but it or bit_rate are not
			// divisible by 2, 3, or 5 anymore. This introduces
			// some timing error.
			int_rate /= 2;
			bit_rate /= 2;
		}
#undef REDUCE_INT_RATE
	}
#endif
	bit_rate_ = bit_rate;
	int_rate_ = int_rate;
}

void SoftRxUart::sample(uint8_t pin_state) {
	if (bit_counter == 0) {
		if (pin_state == 0) {
			// leading edge of start bit
			bit_counter = 8 + 2;
			bit_sample_timer = -(int_rate_ / 2);
		}
	} else {
		/*
		 * This add and test hopefully will be optimized to an add and
		 * test for carry.
		 */
		if ((bit_sample_timer += bit_rate_) < bit_rate_) {
			bit_sample_timer -= int_rate_;
			// it's bit time!
			uint8_t d = data;
			if (--bit_counter == 0) {
				// this should be the stop bit
				if (pin_state == 0) {
					// framing error
					// TODO do something to tell the
					// application about this
				}
				rxq.enqueue(d);
				// TODO if receive queue is nearly full, do
				// something to tell the transmitter to stop
				// transmitting!
			} else {
				// save the data bit
				d /= 2;
				if (pin_state) d |= (uint8_t)(1<<7);
				data = d;
			}
		}
	}
}
