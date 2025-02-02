if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 16)
{
	ushort* aDestPixelsRow = ((ushort*) mLockedSurfaceDesc.lpSurface) + (theY * mLockedSurfaceDesc.lPitch/2) + theX;	
	uchar* aRLAlphaDataRow = aSrcRLAlphaData + (theSrcRect.mY * theImage->mWidth) + theSrcRect.mX;

	ulong aRMask = mLockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
	ulong aGMask = mLockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
	ulong aBMask = mLockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

	ulong aRRoundAdd = aRMask >> 1;
	ulong aGRoundAdd = aGMask >> 1;
	ulong aBRoundAdd = aBMask >> 1;

	if (theColor == Color::White)
	{
		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ushort* aDestPixels = aDestPixelsRow;			
			uchar* aRLAlphaData = aRLAlphaDataRow;
			
			aSrcPixels = aSrcPixelsRow;

			for (int aSpanLeft = theSrcRect.mWidth; aSpanLeft > 0; )
			{
				ulong src = PEEK_SRC_COLOR;
				uchar rl = *aRLAlphaData;
				
				if (rl > aSpanLeft)
					rl = aSpanLeft;

				int oma = 256 - (src >> 24);

				if (oma == 1) // Fully opaque
				{
					for (int i = 0; i < rl; i++)
						*aDestPixels _PLUSPLUS = NEXT_SRC_COLOR;
				}
				else if (oma == 256) // Fully transparent
				{
					aDestPixels _PLUSEQUALS rl;							
					aSrcPixels += rl;
				}						
				else // Partially transparent
				{
					aSrcPixels++;
					ulong dest = *aDestPixels;
					*(aDestPixels _PLUSPLUS) = src + 
						(((((dest & aRMask) * oma) ) >> 8) & aRMask) +
						(((((dest & aGMask) * oma) ) >> 8) & aGMask) +
						(((((dest & aBMask) * oma) ) >> 8) & aBMask);

					for (int i = 1; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;
						int oma = 256 - (src >> 24);

						ulong dest = *aDestPixels;
						*(aDestPixels _PLUSPLUS) = src + 
							(((((dest & aRMask) * oma) ) >> 8) & aRMask) +
							(((((dest & aGMask) * oma) ) >> 8) & aGMask) +
							(((((dest & aBMask) * oma) ) >> 8) & aBMask);
					}
				}
				
				aRLAlphaData += rl;
				aSpanLeft -= rl;
			}

			aDestPixelsRow += mLockedSurfaceDesc.lPitch/2;
			aSrcPixelsRow += theImage->mWidth;
			aRLAlphaDataRow += theImage->mWidth;
		}
	}
	else
	{
		int ca = theColor.mAlpha;
		int cr = (theColor.mRed * ca) / 255;
		int cg = (theColor.mGreen * ca) / 255;
		int cb = (theColor.mBlue * ca) / 255;

		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ushort* aDestPixels = aDestPixelsRow;			
			uchar* aRLAlphaData = aRLAlphaDataRow;	
			
			aSrcPixels = aSrcPixelsRow;

			for (int aSpanLeft = theSrcRect.mWidth; aSpanLeft > 0; )
			{
				ulong src = PEEK_SRC_COLOR;
				uchar rl = *aRLAlphaData;
				
				if (rl > aSpanLeft)
					rl = aSpanLeft;

				int a = src >> 24;																				

				if ((a == 255) && (ca == 255)) // Fully opaque
				{
					for (int i = 0; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;
						*(aDestPixels _PLUSPLUS) = 
							(((((src & aRMask) * cr) ) >> 8) & aRMask) +
							(((((src & aGMask) * cg) ) >> 8) & aGMask) +
							(((((src & aBMask) * cb) ) >> 8) & aBMask);
					}
				}
				else if (a == 0) // Fully transparent
				{
					aDestPixels _PLUSEQUALS rl;							
					aSrcPixels += rl;
				}						
				else // Partially transparent
				{																	
					for (int i = 0; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;

						a = ((src >> 24) * ca) / 255;
						int oma = 256 - a;									

						ulong dest = *aDestPixels;
						*(aDestPixels _PLUSPLUS) = 
							(((((dest & aRMask) * oma) + ((src & aRMask) * cr) ) >> 8) & aRMask) +
							(((((dest & aGMask) * oma) + ((src & aGMask) * cg) ) >> 8) & aGMask) +
							(((((dest & aBMask) * oma) + ((src & aBMask) * cb) ) >> 8) & aBMask);
					}
				}
				
				aRLAlphaData += rl;
				aSpanLeft -= rl;
			}

			aDestPixelsRow += mLockedSurfaceDesc.lPitch/2;
			aSrcPixelsRow += theImage->mWidth;
			aRLAlphaDataRow += theImage->mWidth;
		}
	}
}
else if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 32)
{
	ulong* aDestPixelsRow = ((ulong*) mLockedSurfaceDesc.lpSurface) + (theY * mLockedSurfaceDesc.lPitch/4) + theX;	
	uchar* aRLAlphaDataRow = aSrcRLAlphaData + (theSrcRect.mY * theImage->mWidth) + theSrcRect.mX;

	ulong aRMask = mLockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
	ulong aGMask = mLockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
	ulong aBMask = mLockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

	ulong aRRoundAdd = aRMask >> 1;
	ulong aGRoundAdd = aGMask >> 1;
	ulong aBRoundAdd = aBMask >> 1;

	if (theColor == Color::White)
	{
		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ulong* aDestPixels = aDestPixelsRow;			
			uchar* aRLAlphaData = aRLAlphaDataRow;
			
			aSrcPixels = aSrcPixelsRow;

			for (int aSpanLeft = theSrcRect.mWidth; aSpanLeft > 0; )
			{						
				ulong src = PEEK_SRC_COLOR;
				uchar rl = *aRLAlphaData;
				
				if (rl > aSpanLeft)
					rl = aSpanLeft;

				int oma = 256 - (src >> 24);

				if (oma == 1) // Fully opaque
				{
					for (int i = 0; i < rl; i++)							
						*aDestPixels _PLUSPLUS = NEXT_SRC_COLOR;
				}
				else if (oma == 256) // Fully transparent
				{
					aDestPixels _PLUSEQUALS rl;							
					aSrcPixels += rl;
				}						
				else // Partially transparent
				{
					aSrcPixels++;
					ulong dest = *aDestPixels;
					*(aDestPixels _PLUSPLUS) = src + 
						(((((dest & aRMask) * oma)) >> 8) & aRMask) +
						(((((dest & aGMask) * oma)) >> 8) & aGMask) +
						(((((dest & aBMask) * oma)) >> 8) & aBMask);

					for (int i = 1; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;
						int oma = 256 - (src >> 24);

						ulong dest = *aDestPixels;
						*(aDestPixels _PLUSPLUS) = src + 
							(((((dest & aRMask) * oma)) >> 8) & aRMask) +
							(((((dest & aGMask) * oma)) >> 8) & aGMask) +
							(((((dest & aBMask) * oma)) >> 8) & aBMask);
					}
				}
				
				aRLAlphaData += rl;
				aSpanLeft -= rl;
			}

			aDestPixelsRow += mLockedSurfaceDesc.lPitch/4;
			aSrcPixelsRow += theImage->mWidth;
			aRLAlphaDataRow += theImage->mWidth;
		}
	}
	else
	{
		int ca = theColor.mAlpha;
		int cr = (theColor.mRed * ca) / 255;
		int cg = (theColor.mGreen * ca) / 255;
		int cb = (theColor.mBlue * ca) / 255;

		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ulong* aDestPixels = aDestPixelsRow;			
			uchar* aRLAlphaData = aRLAlphaDataRow;
			
			aSrcPixels = aSrcPixelsRow;

			for (int aSpanLeft = theSrcRect.mWidth; aSpanLeft > 0; )
			{
				ulong src = PEEK_SRC_COLOR;
				uchar rl = *aRLAlphaData;
				
				if (rl > aSpanLeft)
					rl = aSpanLeft;

				int a = src >> 24;																				

				if ((a == 255) && (ca == 255)) // Fully opaque
				{
					for (int i = 0; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;
						*(aDestPixels _PLUSPLUS) = 
							(((((src & aRMask) * cr)) >> 8) & aRMask) +
							(((((src & aGMask) * cg)) >> 8) & aGMask) +
							(((((src & aBMask) * cb)) >> 8) & aBMask);
					}
				}
				else if (a == 0) // Fully transparent
				{
					aDestPixels _PLUSEQUALS rl;
					aSrcPixels += rl;
				}
				else // Partially transparent
				{
					for (int i = 0; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;

						a = ((src >> 24) * ca) / 255;
						int oma = 256 - a;									

						ulong dest = *aDestPixels;
						*(aDestPixels _PLUSPLUS) = 
							(((((dest & aRMask) * oma) + ((src & aRMask) * cr)) >> 8) & aRMask) +
							(((((dest & aGMask) * oma) + ((src & aGMask) * cg)) >> 8) & aGMask) +
							(((((dest & aBMask) * oma) + ((src & aBMask) * cb)) >> 8) & aBMask);
					}
				}
				
				aRLAlphaData += rl;
				aSpanLeft -= rl;
			}

			aDestPixelsRow += mLockedSurfaceDesc.lPitch/4;
			aSrcPixelsRow += theImage->mWidth;
			aRLAlphaDataRow += theImage->mWidth;
		}
	}
}