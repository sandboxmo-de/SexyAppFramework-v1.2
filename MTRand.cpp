/* A C-program for MT19937: Integer     version                   */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) set initial values to the working area    */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer except for 0).        */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

#include <windows.h> 
#include "MTRand.h"
#include "Debug.h"
#include <stdio.h>

using namespace Sexy;

/* Period parameters */  
#define MTRAND_M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)


MTRand::MTRand(const std::string& theSerialData)
{
	SRand(theSerialData);
	mti=MTRAND_N+1; /* mti==N+1 means mt[MTRAND_N] is not initialized */
}

MTRand::MTRand(unsigned long seed)    
{
	SRand(seed);
}

MTRand::MTRand()
{
	SRand(4357);
}

static int gRandAllowed = 0;
void MTRand::SetRandAllowed(bool allowed)
{
	if (allowed)
	{
		if (gRandAllowed>0)
			gRandAllowed--;
		else
			DBG_ASSERT(false); // mismatched calls
	}
	else
		gRandAllowed++;
}

void MTRand::SRand(const std::string& theSerialData)
{
	if (theSerialData.size() == MTRAND_N*4)
	{
		memcpy(mt, theSerialData.c_str(), MTRAND_N*4);
	}
	else
		SRand(4357);
}

void MTRand::SRand(unsigned long seed)
{
	if (seed == 0)
		seed = 4357;

	/* setting initial seeds to mt[N] using         */
    /* the generator Line 25 of Table 1 in          */
    /* [KNUTH 1981, The Art of Computer Programming */
    /*    Vol. 2 (2nd Ed.), pp102]                  */
    mt[0]= seed & 0xffffffff;
    for (mti=1; mti<MTRAND_N; mti++)
        mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

unsigned long MTRand::Next()
{
	DBG_ASSERT(gRandAllowed==0);
	return NextNoAssert();
}

unsigned long MTRand::NextNoAssert()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= MTRAND_N) { /* generate N words at one time */
        int kk;        

        for (kk=0;kk<MTRAND_N-MTRAND_M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MTRAND_M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<MTRAND_N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MTRAND_M-MTRAND_N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[MTRAND_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[MTRAND_N-1] = mt[MTRAND_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

	y &= 0x7FFFFFFF;		

	/*char aStr[256];
	sprintf(aStr, "Rand=%d\r\n", y);
	OutputDebugString(aStr);*/

    return y; 
}

std::string MTRand::Serialize()
{
	std::string aString;

	aString.resize(MTRAND_N*4);
	memcpy((char*) aString.c_str(), mt, MTRAND_N*4);

	return aString;
}
