#include "Buffer.h"

#define POLYNOMIAL 0x04c11db7L

static BOOL 	     bCrcTableGenerated = FALSE;
static unsigned long crc_table[256];

using namespace Sexy;
using namespace std;

static char* gWebEncodeMap = ".-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static int gWebDecodeMap[256] = 
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, 0, -1, 1, 0, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1, -1
, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
, 30, 31, 32, 33, 34, 35, 36, 37, -1, -1, -1, -1, -1, -1, 38, 39, 40, 41, 42, 43
, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

//----------------------------------------------------------------------------
// Generate the table of CRC remainders for all possible bytes.
//----------------------------------------------------------------------------
static void GenerateCRCTable(void)
{
	bCrcTableGenerated = TRUE;

	register int i, j;
	register unsigned long crc_accum;
	for (i = 0;  i < 256;  i++)
	{
		crc_accum = ((unsigned long) i << 24);
		for ( j = 0;  j < 8;  j++ )
		{
			if (crc_accum & 0x80000000L)
				crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
			else
				crc_accum = (crc_accum << 1);
		}
		crc_table[i] = crc_accum;
	}
}

//----------------------------------------------------------------------------
// Update the CRC on the data block one byte at a time.
//----------------------------------------------------------------------------
static unsigned long UpdateCRC(unsigned long crc_accum,
						const char *data_blk_ptr,
						int data_blk_size)
{
	if (!bCrcTableGenerated)
		GenerateCRCTable();
	
	register int i, j;
	for (j = 0; j < data_blk_size; j++)
	{
		i = ((int) (crc_accum >> 24) ^ *data_blk_ptr++) & 0xff;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}
	return crc_accum;
}

Buffer::Buffer()
{
	mDataBitSize = 0;
	mReadBitPos = 0;
	mWriteBitPos = 0;	
}

Buffer::~Buffer()
{
}

std::string Buffer::ToWebString() const
{
	std::string aString;
	int aSizeBits = mWriteBitPos;
	
	int anOldReadBitPos = mReadBitPos;
	mReadBitPos = 0;

	char aStr[256];
	sprintf(aStr, "%08X", aSizeBits);
	aString += aStr;

	int aNumChars = (aSizeBits + 5) / 6;
	for (int aCharNum = 0; aCharNum < aNumChars; aCharNum++)
		aString += gWebEncodeMap[ReadNumBits(6, false)];
	
	mReadBitPos = anOldReadBitPos;
	
	return aString;
}

void Buffer::FromWebString(const std::string& theString)
{
	Clear();

	if (theString.size() < 4)
		return;
	
	int aSizeBits = 0;

	for (int aDigitNum = 0; aDigitNum < 8; aDigitNum++)
	{
		char aChar = theString[aDigitNum];
		int aVal = 0;

		if ((aChar >= '0') && (aChar <= '9'))
			aVal = aChar - '0';
		else if ((aChar >= 'A') && (aChar <= 'F'))
			aVal = (aChar - 'A') + 10;
		else if ((aChar >= 'a') && (aChar <= 'f'))
			aVal = (aChar - 'f') + 10;

		aSizeBits += (aVal << ((7 - aDigitNum) * 4));
	}

	int aCharIdx = 8;
	int aNumBitsLeft = aSizeBits;
	while (aNumBitsLeft > 0)
	{
		uchar aChar = theString[aCharIdx++];
		int aVal = gWebDecodeMap[aChar];
		int aNumBits = min(aNumBitsLeft, 6);
		WriteNumBits(aVal, aNumBits);
		aNumBitsLeft -= aNumBits;		
	}

	SeekFront();
}

void Buffer::SeekFront() const
{
	mReadBitPos = 0;	
}

void Buffer::Clear()
{
	mReadBitPos = 0;
	mWriteBitPos = 0;
	mDataBitSize = 0;
	mData.clear();
}

void Buffer::WriteByte(uchar theByte)
{	
	if (mWriteBitPos % 8 == 0)
		mData.push_back((char) theByte);
	else
	{		
		int anOfs = mWriteBitPos  % 8;
		mData[mWriteBitPos /8] |= theByte << anOfs;
		mData.push_back((char) (theByte >> (8-anOfs)));		
	}

	mWriteBitPos += 8;
	if (mWriteBitPos > mDataBitSize)
		mDataBitSize = mWriteBitPos;
}

void Buffer::WriteNumBits(int theNum, int theBits)
{
	for (int aBitNum = 0; aBitNum < theBits; aBitNum++)
	{
		if (mWriteBitPos % 8 == 0)
			mData.push_back(0);
		if ((theNum & (1<<aBitNum)) != 0)
			mData[mWriteBitPos/8] |= 1 << (mWriteBitPos  % 8);
		mWriteBitPos++;
	}

	if (mWriteBitPos > mDataBitSize)
		mDataBitSize = mWriteBitPos;
}

int Buffer::GetBitsRequired(int theNum, bool isSigned)
{
	if (theNum < 0) // two's compliment stuff
		theNum = -theNum - 1;
	
	int aNumBits = 0;
	while (theNum >= 1<<aNumBits)
		aNumBits++;
		
	if (isSigned)
		aNumBits++;
		
	return aNumBits;
}

void Buffer::WriteBoolean(bool theBool)
{
	WriteByte(theBool ? 1 : 0);
}

void Buffer::WriteShort(short theShort)
{
	WriteByte((uchar)theShort);
	WriteByte((uchar)(theShort >> 8));
}

void Buffer::WriteLong(long theLong)
{
	WriteByte((uchar)theLong);
	WriteByte((uchar)(theLong >> 8));
	WriteByte((uchar)(theLong >> 16));
	WriteByte((uchar)(theLong >> 24));
}

void Buffer::WriteString(const std::string& theString)
{
	WriteShort((short) theString.length());
	for (int i = 0; i < (int)theString.length(); i++)
		WriteByte(theString[i]);
}

void Buffer::WriteLine(const std::string& theString)
{
	WriteBytes((uchar*) (theString + "\r\n").c_str(), (int) theString.length() + 2);
}

void Buffer::WriteBuffer(const ByteVector& theBuffer)
{
	WriteLong((short) theBuffer.size());
	for (int i = 0; i < (int)theBuffer.size(); i++)
		WriteByte(theBuffer[i]);
}

void Buffer::WriteBytes(uchar* theByte, int theCount)
{
	for (int i = 0; i < theCount; i++)
		WriteByte(theByte[i]);
}

void Buffer::SetData(const ByteVector& theBuffer)
{
	mData = theBuffer;
	mDataBitSize = mData.size() * 8;
}

void Buffer::SetData(uchar* thePtr, int theCount)
{
	mData.clear();
	mData.insert(mData.begin(), thePtr, thePtr + theCount);
	mDataBitSize = mData.size() * 8;
}

uchar Buffer::ReadByte() const
{
	if ((mReadBitPos + 7)/8 >= (int)mData.size())
	{		
		return 0; // Underflow
	}

	if (mReadBitPos % 8 == 0)
	{
		uchar b = mData[mReadBitPos/8];
		mReadBitPos += 8;
		return b;
	}
	else
	{
		int anOfs = mReadBitPos % 8;
			
		uchar b = 0;
		
		b = mData[mReadBitPos/8] >> anOfs;
		b |= mData[(mReadBitPos/8)+1] << (8 - anOfs);
		
		mReadBitPos += 8;		
		
		return b;
	}
}

int Buffer::ReadNumBits(int theBits, bool isSigned) const
{	
	int aByteLength = (int) mData.size();

	int theNum = 0;
	bool bset = false;
	for (int aBitNum = 0; aBitNum < theBits; aBitNum++)
	{
		int aBytePos = mReadBitPos/8;

		if (aBytePos >= aByteLength)
			break;

		if (bset = (mData[aBytePos] & (1<<(mReadBitPos%8))) != 0)			
			theNum |= 1<<aBitNum;
		
		mReadBitPos++;
	}
	
	if ((isSigned) && (bset)) // sign extend
		for (int aBitNum = theBits; aBitNum < 32; aBitNum++)
			theNum |= 1<<aBitNum;
	
	return theNum;
}

bool Buffer::ReadBoolean() const
{
	return ReadByte() != 0;
}

short Buffer::ReadShort() const
{
	short aShort = ReadByte();
	aShort |= ((short) ReadByte() << 8);
	return aShort;	
}

long Buffer::ReadLong() const
{
	long aLong = ReadByte();
	aLong |= ((long) ReadByte()) << 8;
	aLong |= ((long) ReadByte()) << 16;
	aLong |= ((long) ReadByte()) << 24;

	return aLong;
}

std::string	Buffer::ReadString() const
{
	string aString;
	int aLen = ReadShort();

	for (int i = 0; i < aLen; i++)
		aString += (char) ReadByte();

	return aString;
}

std::string Buffer::ReadLine() const
{
	std::string aString;

	for (;;)
	{
		char c = ReadByte();

		if ((c == 0) || (c == '\n'))
			break;

		if (c != '\r')
			aString += c;
	}

	return aString;
}

void Buffer::ReadBytes(uchar* theData, int theLen) const
{
	for (int i = 0; i < theLen; i++)
		theData[i] = ReadByte();
}

void Buffer::ReadBuffer(ByteVector* theByteVector) const
{
	theByteVector->clear();
	
	ulong aLength = ReadLong();
	theByteVector->resize(aLength);
	ReadBytes(&(*theByteVector)[0], aLength);
}

const uchar* Buffer::GetDataPtr() const
{
	if (mData.size() == 0)
		return NULL;
	return &mData[0];
}

int Buffer::GetDataLen() const
{
	return (mDataBitSize + 7) / 8; // Round up
}

int Buffer::GetDataLenBits() const
{
	return mDataBitSize;
}

ulong Buffer::GetCRC32(ulong theSeed) const
{	
	ulong aCRC = theSeed;
	aCRC = UpdateCRC(aCRC, (const char*) &mData[0], (int) mData.size());	
	return aCRC;
}

bool Buffer::AtEnd() const
{ 
	//return mReadBitPos >= (int)mData.size()*8;
	return mReadBitPos >= mDataBitSize;
}

bool Buffer::PastEnd() const
{
	return mReadBitPos > mDataBitSize;
}
