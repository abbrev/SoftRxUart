/*
 * Simple byte queue. Can have a capacity between 1 and 128 bytes.
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

#ifndef SIMPLEBYTEQUEUE_H
#define SIMPLEBYTEQUEUE_H

#include <stdint.h>

// NB. qsize must be a power of 2!
template <uint8_t qsize>
class SimpleByteQueue {
private:
	uint8_t buf_[qsize];
	// data goes into queue at tail and comes out at head
	uint8_t head_, tail_;
public:
	SimpleByteQueue(void) : head_(0), tail_(0) {}
	~SimpleByteQueue() {}
	uint8_t capacity(void) { return qsize; }
	uint8_t size(void) { return tail_ - head_; }
	bool isempty(void) { return size() == 0; }
	bool isfull(void) { return size() == capacity(); }
	bool enqueue(uint8_t b) {
		if (isfull()) return false;
		buf_[tail_++ & (qsize - 1)] = b;
		return true;
	}
	int dequeue(void) {
		if (isempty()) return -1;
		return buf_[head_++ & (qsize - 1)];
	}
};

#endif
