Software RX UART
================

This implements a software UART for receiving data. It is designed to be
small and to run on a microcontroller that either does not have a
hardware UART or whose hardware UARTs are all in use for other purposes.

This software has not been tested.

How to Use the Software RX UART
-------------------------------

This software UART is implemented in the "SoftRxUart" class.

To use SoftRxUart, you must:
1. Instantiate a `SoftRxUart` object.
2. Call `SoftRxUart::sample()` with the state of the digital input pin.

The constructor takes the serial baud rate and interrupt rate as
arguments, so you must pass those when instantiating the object.
However, as SoftRxUart is designed to be small and simple, the baud rate
and interrupt rate both must fit in 8-bit values (less than 256 each).
In most cases you will need to scale both values by the same factor; the
only thing that matters is that the ratio between the two remains the
same.

To read bytes from the `SoftRxUart` object, call the `read` method. This
method returns the next byte or -1 if the receive queue is empty.

That's it! Note that the SoftRxUart class does not manage the timer
interrupts or input pin itself. Those must be set up and handled
separately.

Example
-------

Let's look at an example of using SoftRxUart on an AVR microcontroller.
The exact details on how to create an interrupt handler and read a
digital input pin depend on the particular microcontroller and on your
application's needs (as well as your compiler's brand), but this should
at least illustrate how to use SoftRxUart.

Say you are using Timer 1, running at 16000Hz, and reading from pin PD4.
You want to read at 2400 baud. Your code will look similar to the
following:

```cpp
#define BAUD_RATE     2400
#define INT_RATE     16000
#define INT_BAUD_GCD   100

SoftRxUart mySoftRxUart(BAUD_RATE/INT_BAUD_GCD, INT_RATE/INT_BAUD_GCD);

ISR(TIMER1_COMPA_vect)
{
    mySoftRxUart.sample(PIND & PD4);
}
```

The macros are not required, but they show what is happening more
clearly. As mentioned above, the SoftRxUart constructor takes the baud
and interrupt rates and that they must be limited to 8-bit values. The
`INT_BAUD_GCD` macro is used to scale the baud rate and interrupt rate
to fit them in 8 bits while maintaining the ratio between them. In this
example, the baud rate and interrupt rate are scaled to 24 and 160,
respectively.

Limitations
-----------

SoftRxUart implements only 8-bit, no parity, and 1 stop bit.
Technically, it can support more than 1 stop bit (as a stop bit is
simply a gap between frames), and it technically could support 7-bit
with even/odd/mark/space parity while leaving it up to the application
to handle the upper bit in the received byte.

The baud rate can be virtually anything as long as the timer interrupt
runs at least twice as fast as the baud, though ideally the interrupt
should run at least four times as fast as the baud rate. The baud rate
also cannot be less than 1/255 of the interrupt rate.

License
-------

Copyright 2017 Christopher Williams.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
