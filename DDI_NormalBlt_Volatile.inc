{
	if (mNoLock)
		return;

	// Special path that avoids generating cached native alpha data 
	
	LPDIRECTDRAWSURFACE aSurface = GetSurface();

	if (!LockSurface())
		return;
				
	if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 16)
	{
		ushort* aDestPixelsRow = ((ushort*) mLockedSurfaceDesc.lpSurface) + (theY * mLockedSurfaceDesc.lPitch/2) + theX;		
		SRC_TYPE* aSrcPixelsRow = aSrcBits + 
			(theSrcRect.mY * theImage->mWidth) + theSrcRect.mX;				

		ulong aRMask = mLockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
		ulong aGMask = mLockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
		ulong aBMask = mLockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

		ulong aRRoundAdd = aRMask >> 1;
		ulong aGRoundAdd = aGMask >> 1;
		ulong aBRoundAdd = aBMask >> 1;
	
		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ushort* aDestPixels = aDestPixelsRow;
			SRC_TYPE* aSrcPixels = aSrcPixelsRow;					

			for (int x = 0; x < theSrcRect.mWidth; x++)
			{
				ulong src = NEXT_SRC_COLOR;
				ulong dest = *aDestPixels;

				int a = src >> 24;
				int oma = 256 - a;
				
				*(aDestPixels++) = 
					(((((dest & aBMask) * oma) + (((src & 0x000000FF) * a * aBMask) >> 8) + aBRoundAdd) >> 8) & aBMask) +
					(((((dest & aGMask) * oma) + ((((src & 0x0000FF00) >> 8) * a * aGMask) >> 8) + aGRoundAdd) >> 8) & aGMask) +
					(((((dest & aRMask) * oma) + ((((src & 0x00FF0000) >> 16) * a * aRMask) >> 8) + aRRoundAdd) >> 8) & aRMask);
			}				
			
			aDestPixelsRow += mLockedSurfaceDesc.lPitch/2;
			aSrcPixelsRow += theImage->mWidth;					
		}
	}
	else if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		ulong* aDestPixelsRow = ((ulong*) mLockedSurfaceDesc.lpSurface) + (theY * mLockedSurfaceDesc.lPitch/4) + theX;		
		SRC_TYPE* aSrcPixelsRow = aSrcBits + 
			(theSrcRect.mY * theImage->mWidth) + theSrcRect.mX;				

		ulong aRMask = mLockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
		ulong aGMask = mLockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
		ulong aBMask = mLockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

		ulong aRRoundAdd = aRMask >> 1;
		ulong aGRoundAdd = aGMask >> 1;
		ulong aBRoundAdd = aBMask >> 1;
	
		for (int y = 0; y < theSrcRect.mHeight; y++)
		{
			ulong* aDestPixels = aDestPixelsRow;
			SRC_TYPE* aSrcPixels = aSrcPixelsRow;					

			for (int x = 0; x < theSrcRect.mWidth; x++)
			{
				ulong src = NEXT_SRC_COLOR;
				ulong dest = *aDestPixels;

				int a = src >> 24;
				int oma = 256 - a;
				
				*(aDestPixels++) = 
					(((((dest & aBMask) * oma) + ((((src & 0x000000FF) * a) >> 8) * aBMask) + aBRoundAdd) >> 8) & aBMask) +
					(((((dest & aGMask) * oma) + (((((src & 0x0000FF00) >> 8) * a) >> 8) * aGMask) + aGRoundAdd) >> 8) & aGMask) +
					(((((dest & aRMask) * oma) + (((((src & 0x00FF0000) >> 16) * a) >> 8) * aRMask) + aRRoundAdd) >> 8) & aRMask);
			}				
			
			aDestPixelsRow += mLockedSurfaceDesc.lPitch/4;
			aSrcPixelsRow += theImage->mWidth;					
		}
	}

	UnlockSurface();
}