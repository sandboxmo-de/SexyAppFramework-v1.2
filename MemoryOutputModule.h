#ifndef __MEMORYOUTPUTMODULE_H__
#define __MEMORYOUTPUTMODULE_H__

#include <decoder.h>
#include <vector>

typedef std::vector<unsigned char> ByteVector;

struct MemoryOutput
{
	int mNumChannels;
	int mBitsPerSample;
	int mSamplingRate;

	ByteVector mByteVector;
};

int memory_output_module_register(XA_OutputModule *module);

#endif //__MEMORYOUTPUTMODULE_H__