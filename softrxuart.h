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

#ifndef SOFTRXUART_H
#define SOFTRXUART_H

#include "simplebytequeue.h"

#define SOFT_RX_UART_QUEUE_SIZE 32

class SoftRxUart {
private:
	uint8_t data;
	uint8_t bit_counter;

	/*
	 * bit_rate_ is the UART baud (e.g., 600) and int_rate_ is the
	 * interrupt rate (e.g., 16000). Each must be reduced to fit in
	 * unsigned 8-bit values. They can be reduced by their GCD or,
	 * if they still don't fit, by a larger factor as long as the
	 * ratio int_rate_/bit_rate_ is not in error by more than about
	 * 2% to ensure reliable receipt of data. With a "round"
	 * interrupt rate (e.g., 16000) and a common baud, this
	 * shouldn't be an issue.
	 */
	uint8_t bit_sample_timer;
	uint8_t bit_rate_;
	uint8_t int_rate_;
	SimpleByteQueue<SOFT_RX_UART_QUEUE_SIZE> rxq;

public:
	SoftRxUart(uint32_t bit_rate, uint32_t int_rate);
	// sample() must be called at the given interrupt rate
	void sample(uint8_t pin_state);
	int read(void) { return this->rxq.dequeue(); }
};

#endif
