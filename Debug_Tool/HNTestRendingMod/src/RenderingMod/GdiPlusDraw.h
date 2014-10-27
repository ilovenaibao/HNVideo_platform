#ifndef _GDIPLUS_DRAW_H_
#define _GDIPLUS_DRAW_H_

#include "dudef.h"
#include <GdiPlus.h>

class CGdiPlusDraw
{
public:
	CGdiPlusDraw();
	~CGdiPlusDraw();
	

	BOOL Create(HWND hwnd);

	BOOL Delete(void);

 	BOOL Clear(DWORD dwColor);

	BOOL Draw2Dline(const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor);

	BOOL DrawText(LPCTSTR lpString, HV_Point2D* ptLeftTop, DWORD dwColor, 
	 		BYTE byWidth, BYTE byHeight, BOOL bUnderline = FALSE, BOOL bItalic = FALSE);

	BOOL DrawYUV420P(DWORD dwWidth,DWORD dwHeight,unsigned char* yuv, const RECT* pDestRect =NULL,
						DWORD dwDisplayPositionFlag = DisplayFilledAspectRatio );

	BOOL DrawCircle(int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor);

	BOOL DrawRectangle(int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor);

	BOOL Present(void);

		
private:
	Gdiplus::GdiplusStartupInput	m_gdiplusStartupInput;
	ULONG_PTR						m_gdiplusToken;

	Gdiplus::Graphics*	m_memGraphics;		// 内存缓冲绘制对象
	Gdiplus::Bitmap*	m_memBmp;			// 内存缓冲位图
	unsigned char*		video_rgb;
	unsigned int		video_rgbSize;		// 缓冲区大小
	int					m_width, m_height;
	HWND				displayWnd;
};


#endif


