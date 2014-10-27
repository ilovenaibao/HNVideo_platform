#include "stdafx.h"
#include "GdiPlusDraw.h"

#pragma  comment(lib,"gdiplus.lib")
using namespace Gdiplus;
static void YUV420P_TO_RGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight);

CGdiPlusDraw::CGdiPlusDraw()
	: m_gdiplusToken(NULL)
	, m_memGraphics(NULL)
	, m_memBmp(NULL)
	, m_width(0)
	, m_height(0)
	, video_rgb(NULL)
	, video_rgbSize(0)
{
}


CGdiPlusDraw::~CGdiPlusDraw()
{
}


BOOL CGdiPlusDraw::Create(HWND hwnd)
{
	displayWnd =  hwnd;

	if (m_gdiplusToken == NULL && GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL) == Ok)
	{
		RECT rect;
		::GetClientRect(hwnd, &rect);
		m_width = rect.right - rect.left;
		m_height = rect.bottom - rect.top;
		if (video_rgb == NULL)
		{
			video_rgbSize = 352 * 288 * 3;
			video_rgb = new unsigned char[video_rgbSize];
			ZeroMemory(video_rgb, video_rgbSize);
		}
		if (m_memBmp == NULL)
		{
			m_memBmp = new Bitmap(m_width, m_height, (3 * m_width) % 4 + 3 * m_width, PixelFormat24bppRGB, NULL);
		}
		if (m_memGraphics == NULL)
		{
			m_memGraphics = new Graphics(m_memBmp);
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CGdiPlusDraw::Delete(void)
{
	if (m_memGraphics != NULL)
	{
		delete m_memGraphics;
		m_memGraphics = NULL;
	}
	if (m_memBmp != NULL)
	{
		delete m_memBmp;
		m_memBmp = NULL;
	}
	if (video_rgb != NULL)
	{
		delete[] video_rgb;
		video_rgb = NULL;
		video_rgbSize = 0;
	}
	if (m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}
	return TRUE;
}


BOOL CGdiPlusDraw::Clear( DWORD dwColor )
{
	if (m_memGraphics)
	{
		Color color(dwColor | 0xFF000000);
		m_memGraphics->Clear(color);
		return TRUE;
	}
	return FALSE;
}


BOOL CGdiPlusDraw::Draw2Dline(const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor)
{
	if (!m_memGraphics)
	{
		return FALSE;
	}

	Color color(dwColor | 0xFF000000);
	Pen pen(color);
	for (unsigned int i = 0; i< dwLineCount; i++)
	{
		m_memGraphics->DrawLine(&pen, Point(pLineArray[i].start.x,pLineArray[i].start.y),Point(pLineArray[i].end.x,pLineArray[i].end.y));
	}
	return TRUE;
}


BOOL CGdiPlusDraw::DrawText( LPCTSTR lpString, HV_Point2D* ptLeftTop, 
	DWORD dwColor, BYTE byWidth, BYTE byHeight,
	BOOL bUnderline /*= FALSE*/, BOOL bItalic /*= FALSE*/ )
{
	if (!m_memGraphics)
	{
		return FALSE;
	}
	int fontStyle = FontStyleRegular;
	if (bUnderline)
	{
		fontStyle = FontStyleUnderline;
	}
	if (bItalic)
	{
		fontStyle = fontStyle | FontStyleItalic;
	}
	Gdiplus::Font font(L"ËÎÌו", byHeight, fontStyle, UnitPoint, NULL);
	Color color(dwColor | 0xFF000000);
	SolidBrush brush(color);

	int strlen = _tcslen(lpString);
	PointF pf(ptLeftTop->x,ptLeftTop->y);
	return m_memGraphics->DrawString(lpString, strlen, &font, pf, &brush) == Ok;

}


BOOL CGdiPlusDraw::DrawYUV420P(DWORD dwWidth,DWORD dwHeight,unsigned char* yuv, const RECT* pDestRect /*=NULL*/, DWORD dwDisplayPositionFlag /*= DisplayFilledAspectRatio*/ )
{
	if (!m_memGraphics)
	{
		return FALSE;
	}
	if (video_rgbSize < dwWidth * dwHeight * 3)
	{
		delete[] video_rgb;
		video_rgbSize = dwWidth * dwHeight * 3;
		video_rgb = new unsigned char[video_rgbSize];
	}
	YUV420P_TO_RGB24(yuv,video_rgb,dwWidth	,dwHeight);
	
	Bitmap bmp(dwWidth, dwHeight, 3 * dwWidth + (3 * dwWidth) % 4, PixelFormat24bppRGB, video_rgb);
	return m_memGraphics->DrawImage(&bmp, 0, 0, m_width, m_height) == Ok;
}


BOOL CGdiPlusDraw::DrawCircle(int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor)
{
	if (!m_memGraphics)
	{
		return FALSE;
	}
	Color color(FrameColor | 0xFF000000);
	Pen pen(color, static_cast<REAL>(nPixel));
	int width, height;
	width = 2 * nRadius;
	height = width;
	return m_memGraphics->DrawEllipse(&pen, xCenter, yCenter, width, height);
}


BOOL CGdiPlusDraw::DrawRectangle(int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor)
{
	if (!m_memGraphics)
	{
		return FALSE;
	}
	Color color(FrameColor | 0xFF000000);
	Pen pen(color, static_cast<REAL>(nPixel));
	int halfLenOfSide = nLenOfSide / 2;
	return m_memGraphics->DrawRectangle(&pen, xCenter - halfLenOfSide, yCenter - halfLenOfSide, nLenOfSide, nLenOfSide);
}


BOOL CGdiPlusDraw::Present(void)
{
	HDC hDC = ::GetDC(displayWnd);
	Graphics graphics(hDC);
	CachedBitmap cachedBmp(m_memBmp, &graphics);
	Status result = graphics.DrawCachedBitmap(&cachedBmp, 0, 0);
	::ReleaseDC(displayWnd, hDC);
	return result == Ok;
}
	

inline byte ADJUST(double tmp)
{
	return (byte)((tmp >= 0 && tmp <= 255)?tmp:(tmp < 0 ? 0 : 255));
}


static void YUV420P_TO_RGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight)
{
    byte Y,U,V,R,G,B;
    byte* y_planar,*u_planar,*v_planar;
    int C,D,E;
    int rgb_width , u_width;
    rgb_width = nWidth * 3;
    u_width = (nWidth >> 1);
    int ypSize = nWidth * nHeight;
    int upSize = (ypSize>>2);
    int offSet = 0;
    
    y_planar = yuv_src;
    u_planar = yuv_src + ypSize;
    v_planar = u_planar + upSize;

   
    for(int i = 0; i < nHeight; i++)
    {
        for(int j = 0; j < nWidth; j ++)
        {
            // Get the Y value from the y planar
            Y = *(y_planar + nWidth * i + j);
            // Get the V value from the u planar
            offSet = (i>>1) * (u_width) + (j>>1);
            V = *(u_planar + offSet);
            // Get the U value from the v planar
            U = *(v_planar + offSet);
            
        
            C = Y - 16;
            D = U - 128;
            E = V - 128;
            
            R = ADJUST(( 298 * C          + 409 * E + 128) >> 8);
            G = ADJUST(( 298 * C - 100 * D - 208 * E + 128) >> 8);
            B = ADJUST(( 298 * C + 516 * D          + 128) >> 8);
         
            offSet = rgb_width * i + j * 3;
       
            rgb_dst[offSet] = R;
            rgb_dst[offSet + 1] = G;
            rgb_dst[offSet + 2] = B;
        }
    }
  
}


