#ifndef _DRAW_H_
#define _DRAW_H_
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include "dudef.h"

struct CUSTOMVERTEX   
{   
	FLOAT x, y, z, rhw;
	DWORD color;   
};   
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

class CDraw
{
public:
	CDraw();
	void	Release()	{ delete this;	}
	
	BOOL	Create(HWND hWnd,
				HWND* hSubWnd = NULL, DWORD* dwSubId = NULL, DWORD dwSubWindowCount = 0);
	BOOL	Delete();
	BOOL	Clear(DWORD dwColor);
	BOOL	ClearSubWindow(DWORD dwId, DWORD dwColor);

	BOOL	DrawYUV420P(DWORD dwWidth, DWORD dwHeight,
				PBYTE y, PBYTE u, PBYTE v, 
				const RECT* pDestRect =NULL,
				DWORD dwDisplayPositionFlag = DisplayFilledAspectRatio);
	BOOL	DrawYUV420PSubWindow(DWORD dwId,
				DWORD dwWidth, DWORD dwHeight,
				PBYTE y, PBYTE u, PBYTE v, 
				const RECT* pDestRect =NULL,
				DWORD dwDisplayPositionFlag = DisplayFilledAspectRatio);

	BOOL	DrawYV12(DWORD dwWidth, DWORD dwHeight,
				PBYTE y, PBYTE u, PBYTE v, 
				const RECT* pDestRect = NULL, 
				DWORD dwDisplayPositionFlag =DisplayFilledAspectRatio);

	BOOL	DrawYV12SubWindow(DWORD dwId,
				DWORD dwWidth, DWORD dwHeight,
				PBYTE y, PBYTE u, PBYTE v, 
				const RECT* pDestRect = NULL, 
				DWORD dwDisplayPositionFlag =DisplayFilledAspectRatio);

	BOOL	Draw2DLine(const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor);
	BOOL	Draw2DLineSubWindow(DWORD dwId,
				const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor);

	BOOL	Draw2DLine(const HV_Point2D* pStartPoints, const HV_Point2D* pEndPoints, 
				DWORD dwLineCount, DWORD dwColor);
	BOOL	Draw2DLineSubWindow(DWORD dwId,
				const HV_Point2D* pStartPoints, const HV_Point2D* pEndPoints, 
				DWORD dwLineCount, DWORD dwColor);

	//BOOL	Draw3DLine(const HV_Line3D* pLineArray, DWORD dwLineCount, DWORD dwColor);
	//BOOL	Draw3DLineSubWindow(DWORD dwId, 
	//			const HV_Line3D* pLineArray, DWORD dwLineCount, DWORD dwColor);

	BOOL	DrawText(LPCTSTR lpString, HV_Point2D* ptLeftTop, DWORD dwColor, 
				BYTE byWidth, BYTE byHeight, BOOL bUnderline = FALSE, BOOL bItalic = FALSE);
	BOOL	DrawTextSubWindow(DWORD dwId,
				LPCTSTR lpString, HV_Point2D* ptLeftTop, DWORD dwColor, 
				BYTE byWidth, BYTE byHeight, BOOL bUnderline = FALSE, BOOL bItalic = FALSE);

	BOOL	DrawCircleSubWindow(DWORD dwId, int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor);
	BOOL	DrawRectangleSubWindow(DWORD dwId, int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor);

	BOOL	Present();
	BOOL	PresentSubWindow(DWORD dwId);

private:
	~CDraw();

private:
	IDirect3D9			*mD3DInstance;
	IDirect3DDevice9	*mD3DDevice;
	IDirect3DSwapChain9	*mSwapChain[16];
	HWND				mMainWnd;
	HWND				mSwapChainWnd[16];
	DWORD				mSwapChainCount;
	CUSTOMVERTEX*		mCustomVertexBuf;
	int					mCustomVertexBufSize;
};

#endif


