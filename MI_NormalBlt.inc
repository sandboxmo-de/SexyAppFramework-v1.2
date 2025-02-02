{
	ulong* aDestPixelsRow = ((ulong*) GetBits()) + (theY * mWidth) + theX;	

	if ((mHasAlpha) || (mHasTrans) || (theColor != Color::White))
	{
		if (theColor == Color::White)
		{
			for (int y = 0; y < theSrcRect.mHeight; y++)
			{
				ulong* aDestPixels = aDestPixelsRow;
				EACH_ROW;

				for (int x = 0; x < theSrcRect.mWidth; x++)
				{
					ulong src = NEXT_SRC_COLOR;
					ulong dest = *aDestPixels;
					
					int a = src >> 24;	
					
					if (a != 0)
					{
						int aDestAlpha = dest >> 24;
						int aNewDestAlpha = aDestAlpha + ((255 - aDestAlpha) * a) / 255;
											
						a = 255 * a / aNewDestAlpha;

						int oma = 256 - a;
						
						// TODO: To avoid rounding problems for fully opaque pixels use (a+1) instead of a in the code below since we are dividing by 256
						*(aDestPixels++) = (aNewDestAlpha << 24) |
							((((dest & 0x0000FF) * oma) >> 8) + (((src & 0x0000FF) * a) >> 8) & 0x0000FF) |
							((((dest & 0x00FF00) * oma) >> 8) + (((src & 0x00FF00) * a) >> 8) & 0x00FF00) |
							((((dest & 0xFF0000) * oma) >> 8) + (((src & 0xFF0000) * a) >> 8) & 0xFF0000);
					}
					else
						aDestPixels++;
				}

				aDestPixelsRow += mWidth;
				aSrcPixelsRow += theImage->mWidth;
			}
		}
		else
		{			
			int ca = theColor.mAlpha;
			int cr = theColor.mRed;
			int cg = theColor.mGreen;
			int cb = theColor.mBlue;

			for (int y = 0; y < theSrcRect.mHeight; y++)
			{
				ulong* aDestPixels = aDestPixelsRow;
				
				EACH_ROW;

				for (int x = 0; x < theSrcRect.mWidth; x++)
				{
					ulong src = NEXT_SRC_COLOR;
					ulong dest = *aDestPixels;
					
					int a = ((src >> 24) * ca) / 255;	
					
					if (a != 0)
					{
						int aDestAlpha = dest >> 24;
						int aNewDestAlpha = aDestAlpha + ((255 - aDestAlpha) * a) / 255;
											
						a = 255 * a / aNewDestAlpha;

						int oma = 256 - a;
						
						*(aDestPixels++) = (aNewDestAlpha << 24) |
							((((dest & 0x0000FF) * oma) >> 8) + (((src & 0x0000FF) * a * cb) >> 16) & 0x0000FF) |
							((((dest & 0x00FF00) * oma) >> 8) + (((src & 0x00FF00) * a * cg) >> 16) & 0x00FF00) |
							((((dest & 0xFF0000) * oma) >> 8) + (((((src & 0xFF0000) * a) >> 8) * cr) >> 8) & 0xFF0000);
					}
					else
						aDestPixels++;
				}

				aDestPixelsRow += mWidth;
				aSrcPixelsRow += theImage->mWidth;
			}
		}
	}
	else
	{
		uchar* aSrcRLAlphaData = aSrcMemoryImage->GetRLAlphaData();
		uchar* aRLAlphaDataRow = aSrcRLAlphaData + (theSrcRect.mY * theImage->mWidth) + theSrcRect.mX;

		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ulong* aDestPixels = aDestPixelsRow;
			
			EACH_ROW;
			
			uchar* aRLAlphaData = aRLAlphaDataRow;					

			for (int aSpanLeft = theSrcRect.mWidth; aSpanLeft > 0; )
			{
				ulong src = READ_SRC_COLOR;
				uchar rl = *aRLAlphaData;
				
				if (rl > aSpanLeft)
					rl = aSpanLeft;

				int oma = 256 - (src >> 24);

				if (oma == 1) // Fully opaque
				{
					for (int i = 0; i < rl; i++)
						*aDestPixels++ = NEXT_SRC_COLOR;
				}
				else if (oma == 256) // Fully transparent
				{
					aDestPixels += rl;							
					aSrcPtr += rl;
				}						
				else // Partially transparent
				{
					int a = 256 - oma;
				
					aSrcPtr++;
					ulong dest = *aDestPixels;
					*(aDestPixels++) = (0xFF000000) |
						((((dest & 0x0000FF) * oma) >> 8) + (((src & 0x0000FF) * a) >> 8) & 0x0000FF) |
						((((dest & 0x00FF00) * oma) >> 8) + (((src & 0x00FF00) * a) >> 8) & 0x00FF00) |
						((((dest & 0xFF0000) * oma) >> 8) + (((src & 0xFF0000) * a) >> 8) & 0xFF0000);

					for (int i = 1; i < rl; i++)
					{
						ulong src = NEXT_SRC_COLOR;
						int a = (src >> 24);
						int oma = 256 - a;

						ulong dest = *aDestPixels;
						*(aDestPixels++) = (0xFF000000) |
							((((dest & 0x0000FF) * oma) >> 8) + (((src & 0x0000FF) * a) >> 8) & 0x0000FF) |
							((((dest & 0x00FF00) * oma) >> 8) + (((src & 0x00FF00) * a) >> 8) & 0x00FF00) |
							((((dest & 0xFF0000) * oma) >> 8) + (((src & 0xFF0000) * a) >> 8) & 0xFF0000);
					}
				}
				
				aRLAlphaData += rl;
				aSpanLeft -= rl;
			}

			aDestPixelsRow += mWidth;
			aSrcPixelsRow += theImage->mWidth;
			aRLAlphaDataRow += theImage->mWidth;
		}
	}
}