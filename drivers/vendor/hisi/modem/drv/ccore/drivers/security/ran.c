/*-
* Copyright (c) 1992, 1993
*      The Regents of the University of California.  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of the University nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
*      @(#)random.c    8.1 (Berkeley) 6/10/93
*/
/*lint --e{713,732}*/

#include "ran.h"

#define NSHUFF 50       /* to drop some "seed -> 1st value" linearity */
extern unsigned int BSP_GetSliceValue(void);

static u_long randseed = 0; /* after srandom(1), NSHUFF counted */

void
srandom(u_long seed)
{
      int i;

      randseed = seed;
      for (i = 0; i < NSHUFF; i++)
	          /* coverity[secure_coding] */
              (void)random();/* [false alarm]: */
}

/*
* Pseudo-random number generator for randomizing the profiling clock,
* and whatever else we might use it for.  The result is uniform on
* [0, 2^31 - 1].
*/
u_long
random(void)
{
      register long x, hi, lo, t;

      /*
       * Compute x[n + 1] = (7^5 * x[n]) mod (2^31 - 1).
       * From "Random number generators: good ones are hard to find",
       * Park and Miller, Communications of the ACM, vol. 31, no. 10,
       * October 1988, p. 1195.
       */
      /* Can't be initialized with 0, so use another value. */
      if ((x = randseed) == 0)
              x = BSP_GetSliceValue();
      hi = x / 127773;
      lo = x % 127773;
      t = 16807 * lo - 2836 * hi;
      if (t < 0)
              t += 0x7fffffff;
      randseed = t;
      return (u_long)(t);
}

