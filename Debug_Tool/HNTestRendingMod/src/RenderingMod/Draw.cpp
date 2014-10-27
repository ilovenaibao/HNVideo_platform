#include "Draw.h"

struct SCREEN_VERTEX_UNTEX  
{  
	float x, y, z, h;  
	DWORD color;  
};

CDraw::CDraw()
	: mD3DInstance(NULL), mD3DDevice(NULL)
	, mMainWnd(NULL), mSwapChainCount(0)
{
	for (int i = 0; i < 16; i++)
	{
		mSwapChain[i] = NULL;
		mSwapChainWnd[i] = NULL;
	}

	mCustomVertexBufSize = 16 * 1024;
	mCustomVertexBuf = new CUSTOMVERTEX[mCustomVertexBufSize];
}


CDraw::~CDraw()
{
	Delete();

	if (mCustomVertexBuf)
		delete[] mCustomVertexBuf;
}


BOOL CDraw::Create(HWND hWnd,
				   HWND* hSubWnd /* = NULL */, DWORD* dwSubId /* = NULL */, DWORD dwSubWindowCount /* = 0 */)
{
	if (!hWnd)
		return FALSE;

	mD3DInstance = Direct3DCreate9( D3D_SDK_VERSION );    
	if(!mD3DInstance)	goto err;

	D3DDISPLAYMODE disp_mode;
	D3DPRESENT_PARAMETERS d3dpp;

	if (FAILED(mD3DInstance->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &disp_mode)))
		goto err;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );    
	d3dpp.Windowed = TRUE;    
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT_DISCARD;    
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = disp_mode.Format;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;//D3DPRESENT_INTERVAL_IMMEDIATE;

	if( FAILED( mD3DInstance->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, 
		(IDirect3DDevice9 **)&mD3DDevice ) ) )
	{
		// <BUG FIXED> Remove the release code of pD3D
		goto err;
	}

	if (mD3DInstance && mD3DDevice)
	{
		IDirect3DSurface9* pSurface = NULL;
		if( FAILED(mD3DDevice->CreateOffscreenPlainSurface(50, 50, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
			D3DPOOL_DEFAULT, &pSurface, NULL)))
		{
			goto err;
		}
		pSurface->Release();
	}

	mMainWnd = hWnd;

	// 创建交换缓冲用于子窗口
	for (int i = 0; i < dwSubWindowCount; i++)
	{
		D3DPRESENT_PARAMETERS d3dpp; 
		D3DDISPLAYMODE mode;
		IDirect3DSwapChain9* swapChain = NULL;

		ZeroMemory( &d3dpp, sizeof(d3dpp) );
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		// Use the current display mode.
		mD3DInstance->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
		d3dpp.BackBufferFormat = mode.Format;
		// m_hWnd contains child window handle
		d3dpp.hDeviceWindow = hSubWnd[i];
		d3dpp.Flags = D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		// m_pSwapChain is IDirect3DSwapChain * 
		mD3DDevice->CreateAdditionalSwapChain(&d3dpp, &swapChain);
		mSwapChain[i] = swapChain;
		mSwapChainWnd[i] = hSubWnd[i];
		dwSubId[i] = i;
	}

	return TRUE;

err:
	for (int i = 0; i < 16; i++)
	{
		if (mSwapChain[i])
			mSwapChain[i]->Release();
		mSwapChain[i] = NULL;
		mSwapChainWnd[i] = NULL;
	}
	mSwapChainCount = 0;

	if (mD3DDevice)
		mD3DDevice->Release();
	mD3DDevice = NULL;
	if (mD3DInstance)
		mD3DInstance->Release();
	mD3DInstance = NULL;

	return FALSE;
}


BOOL CDraw::Delete()
{	
	for (int i = 0; i < 16; i++)
	{
		if (mSwapChain[i])
			mSwapChain[i]->Release();
		mSwapChain[i] = NULL;
		mSwapChainWnd[i] = NULL;
	}
	mSwapChainCount = 0;

	if (mD3DDevice)
		mD3DDevice->Release();
	mD3DDevice = NULL;

	if (mD3DInstance)
		mD3DInstance->Release();
	mD3DInstance = NULL;

	return TRUE;
}


BOOL CDraw::Clear(DWORD dwColor)
{
	if (mD3DInstance && mD3DDevice)
	{
		HRESULT hRet;
		BYTE r = (dwColor & 0x00ff0000) >> 16;
		BYTE g = (dwColor & 0x0000ff00) >> 8;
		BYTE b = (dwColor & 0x000000ff) >> 0;

		hRet = mD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(r,g,b), 1.0f, 0 );

		return (hRet == S_OK);
	}

	return FALSE;
}


BOOL CDraw::ClearSubWindow(DWORD dwId, DWORD dwColor)
{
	if (dwId >= 16)
		return FALSE;

	if (mD3DInstance && mD3DDevice && mSwapChain[dwId])
	{
		HRESULT hRet;
		BYTE r = (dwColor & 0x00ff0000) >> 16;
		BYTE g = (dwColor & 0x0000ff00) >> 8;
		BYTE b = (dwColor & 0x000000ff) >> 0;
		D3DLOCKED_RECT lockRect;
		RECT rc;
		IDirect3DSurface9* pSurface = NULL;

		::GetWindowRect(mSwapChainWnd[dwId], &rc);

		if( FAILED(mD3DDevice->CreateOffscreenPlainSurface(rc.right - rc.left, rc.bottom - rc.top, D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT, &pSurface, NULL)))
			return FALSE;

		if( FAILED(pSurface->LockRect(&lockRect,NULL,D3DLOCK_NOSYSLOCK)))//D3DLOCK_DONOTWAIT)))
			return FALSE;

		for (int i = 0; i < rc.bottom - rc.top; i++)
		{
			DWORD * p = (DWORD *) ((BYTE *)lockRect.pBits + lockRect.Pitch * i);
			for (int j = 0; j < rc.right - rc.left; j ++)
			{
				*p = dwColor;
				p ++;
			}
		}
		
		pSurface->UnlockRect();
		mD3DDevice->BeginScene();
		IDirect3DSurface9 * pBackBuffer = NULL;
		mSwapChain[dwId]->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
		hRet = mD3DDevice->StretchRect(pSurface, NULL, pBackBuffer, NULL, D3DTEXF_NONE);
		pBackBuffer->Release();
		mD3DDevice->EndScene();
		pSurface->Release();

		return (hRet == S_OK);
	}

	return FALSE;
}


BOOL CDraw::DrawYUV420P(DWORD dwWidth, DWORD dwHeight, 
	PBYTE y, PBYTE u, PBYTE v,
	const RECT* pDestRect /* =NULL */, 
	DWORD dwDisplayPositionFlag /* = DisplayFilledAspectRatio */)
{
	D3DLOCKED_RECT d3d_rect;
	IDirect3DSurface9* pSurface = NULL;

	if (dwWidth <= 0 || dwHeight <= 0 || !y)
		return FALSE;
	if (!mD3DDevice)
		return FALSE;

	if( FAILED(mD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
		D3DPOOL_DEFAULT, &pSurface, NULL)))
		return FALSE;

	if( FAILED(pSurface->LockRect(&d3d_rect,NULL,D3DLOCK_NOSYSLOCK)))//D3DLOCK_DONOTWAIT)))
	{
		pSurface->Release();
		return FALSE;
	}

	const int w = dwWidth;
	const int h = dwHeight;
	BYTE * const p = (BYTE *)d3d_rect.pBits;
	const int stride = d3d_rect.Pitch;
	PBYTE dst = NULL;
	PBYTE src = NULL;

	// Y
	dst = p;
	src = y;
	for(int i = 0; i < h; i++)    
	{
		memcpy(dst, src, w);

		dst += stride;
		src += w;
	}    

	// V
	dst = p + stride * h;
	if (v)
	{
		src = v;
		for(int i = 0; i < h / 2; i++)    
		{
			memcpy(dst, src, (w >> 1));

			dst += (stride >> 1);
			src += (w >> 1);
		}   
	}
	else
	{
		for (int i = 0; i < h / 2; i++)
		{	
			memset(dst, 128, (w >> 1));
			dst += (stride >> 1);
		}
	}


	// U
	dst = p + stride * h + ((stride * h) >> 2);
	if (u)
	{
		src = u;
		for(int i = 0; i < h / 2; i++)    
		{
			memcpy(dst, src, (w >> 1));

			dst += (stride >> 1);
			src += (w >> 1);
		}    
	}
	else
	{
		for (int i = 0; i < h / 2; i++)
		{
			memset(dst, 128, (w >> 1));

			dst += (stride >> 1);
		}
	}


	if( FAILED(pSurface->UnlockRect()))    
	{
		pSurface->Release();
		return FALSE;
	}

	// 计算显示位置
	RECT rc_dst;
	int rc_w = 0;
	int rc_h = 0;
	int dst_w = 0;
	int dst_h = 0;
	float rc_aspect = 1.0f;

	::GetClientRect(mMainWnd, &rc_dst);
	rc_w = rc_dst.right - rc_dst.left;
	rc_h = rc_dst.bottom - rc_dst.top;
	rc_aspect = dwWidth / dwHeight;

	switch (dwDisplayPositionFlag)
	{
	case DisplayFilled:
		// 填满目标窗口
		rc_dst.left = rc_dst.top = 0;
		rc_dst.right = rc_w;
		rc_dst.bottom = rc_h;

		break;

	case DisplayFilledAspectRatio:
		// 保持长宽比的方式填满目标窗口
		if (rc_aspect * rc_h >= rc_w)
		{
			// 按目标窗口的宽度等比设定显示矩形
			dst_w = rc_w;
			dst_h = (LONG)((float)(dwHeight * rc_w) / (float)dwWidth);

			rc_dst.left = 0;
			rc_dst.right = dst_w;
			rc_dst.top = (rc_h - dst_h) >> 1;
			rc_dst.bottom = rc_dst.top + dst_h;
		}
		else
		{
			// 按目标窗口的高度等比设定显示矩形
			dst_w = (LONG)((float)(dwWidth * rc_h) / (float)dwHeight);
			dst_h = rc_h;

			rc_dst.left = (rc_w - dst_w) >> 1;
			rc_dst.right = rc_dst.left + dst_w;
			rc_dst.top = 0;
			rc_dst.bottom = dst_h;
		}

		break;

	case DisplayAdaptived:
		if (dwWidth >= rc_w || dwHeight >= rc_h)
		{
			if (rc_aspect * rc_h >= rc_w)
			{
				// 按目标窗口的宽度等比设定显示矩形
				dst_w = rc_w;
				dst_h = (LONG)((float)(dwHeight * rc_w) / (float)dwWidth);

				rc_dst.left = 0;
				rc_dst.right = dst_w;
				rc_dst.top = (rc_h - dst_h) >> 1;
				rc_dst.bottom = rc_dst.top + dst_h;
			}
			else
			{
				// 按目标窗口的高度等比设定显示矩形
				dst_w = (LONG)((float)(dwWidth * rc_h) / (float)dwHeight);
				dst_h = rc_h;

				rc_dst.left = (rc_w - dst_w) >> 1;
				rc_dst.right = rc_dst.left + dst_w;
				rc_dst.top = 0;
				rc_dst.bottom = dst_h;
			}
		}
		else
		{
			// 居中显示原始尺寸
			dst_w = dwWidth;
			dst_h = dwHeight;

			rc_dst.left = (rc_w - dst_w) >> 1;
			rc_dst.right = rc_dst.left + dst_w;
			rc_dst.top = (rc_h - dst_h) >> 1;
			rc_dst.bottom = rc_dst.top + dst_h;
		}

		break;

	case DisplayCustom:
		if (!pDestRect || 
			pDestRect->right > rc_w ||
			pDestRect->bottom > rc_h)
		{
			// 填满窗口
			rc_dst.left = rc_dst.top = 0;
			rc_dst.right = rc_w;
			rc_dst.bottom = rc_h;
		}
		else
		{
			rc_dst.left = pDestRect->left;
			rc_dst.right = pDestRect->right;
			rc_dst.top = pDestRect->top;
			rc_dst.bottom = pDestRect->bottom;
		}

		break;

	default:
		// 填满窗口
		rc_dst.left = rc_dst.top = 0;
		rc_dst.right = rc_w;
		rc_dst.bottom = rc_h;
	}

	// 把图像复制到后台缓冲
	mD3DDevice->BeginScene();
	IDirect3DSurface9 * pBackBuffer = NULL;
	mD3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	mD3DDevice->StretchRect(pSurface, NULL, pBackBuffer, &rc_dst, D3DTEXF_LINEAR);
	pBackBuffer->Release();
	mD3DDevice->EndScene();
	pSurface->Release();

	return TRUE;
}


BOOL CDraw::DrawYUV420PSubWindow(DWORD dwId, 
	DWORD dwWidth, DWORD dwHeight, 
	PBYTE y, PBYTE u, PBYTE v, 
	const RECT* pDestRect /* =NULL */, 
	DWORD dwDisplayPositionFlag /* = DisplayFilledAspectRatio */)
{
	D3DLOCKED_RECT d3d_rect;
	IDirect3DSurface9* pSurface = NULL;

	if (dwWidth <= 0 || dwHeight <= 0 || !y)
		return FALSE;
	if (!mD3DDevice)
		return FALSE;

	if( FAILED(mD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
		D3DPOOL_DEFAULT, &pSurface, NULL)))
		return FALSE;

	if( FAILED(pSurface->LockRect(&d3d_rect,NULL,D3DLOCK_NOSYSLOCK)))//D3DLOCK_DONOTWAIT)))
	{
		pSurface->Release();
		return FALSE;
	}

	const int w = dwWidth;
	const int h = dwHeight;
	BYTE * const p = (BYTE *)d3d_rect.pBits;
	const int stride = d3d_rect.Pitch;
	PBYTE dst = NULL;
	PBYTE src = NULL;

	// Y
	dst = p;
	src = y;
	for(int i = 0; i < h; i++)    
	{
		memcpy(dst, src, w);

		dst += stride;
		src += w;
	}    

	// V
	dst = p + stride * h;
	if (v)
	{
		src = v;
		for(int i = 0; i < h / 2; i++)    
		{
			memcpy(dst, src, (w >> 1));

			dst += (stride >> 1);
			src += (w >> 1);
		}   
	}
	else
	{
		for (int i = 0; i < h / 2; i++)
		{	
			memset(dst, 128, (w >> 1));
			dst += (stride >> 1);
		}
	}


	// U
	dst = p + stride * h + ((stride * h) >> 2);
	if (u)
	{
		src = u;
		for(int i = 0; i < h / 2; i++)    
		{
			memcpy(dst, src, (w >> 1));

			dst += (stride >> 1);
			src += (w >> 1);
		}    
	}
	else
	{
		for (int i = 0; i < h / 2; i++)
		{
			memset(dst, 128, (w >> 1));

			dst += (stride >> 1);
		}
	}


	if( FAILED(pSurface->UnlockRect()))    
	{
		pSurface->Release();
		return FALSE;
	}

	// 计算显示位置
	RECT rc_dst;
	int rc_w = 0;
	int rc_h = 0;
	int dst_w = 0;
	int dst_h = 0;
	float rc_aspect = 1.0f;

	::GetClientRect(mSwapChainWnd[dwId], &rc_dst);
	rc_w = rc_dst.right - rc_dst.left;
	rc_h = rc_dst.bottom - rc_dst.top;
	rc_aspect = dwWidth / dwHeight;

	switch (dwDisplayPositionFlag)
	{
	case DisplayFilled:
		// 填满目标窗口
		rc_dst.left = rc_dst.top = 0;
		rc_dst.right = rc_w;
		rc_dst.bottom = rc_h;

		break;

	case DisplayFilledAspectRatio:
		// 保持长宽比的方式填满目标窗口
		if (rc_aspect * rc_h >= rc_w)
		{
			// 按目标窗口的宽度等比设定显示矩形
			dst_w = rc_w;
			dst_h = (LONG)((float)(dwHeight * rc_w) / (float)dwWidth);

			rc_dst.left = 0;
			rc_dst.right = dst_w;
			rc_dst.top = (rc_h - dst_h) >> 1;
			rc_dst.bottom = rc_dst.top + dst_h;
		}
		else
		{
			// 按目标窗口的高度等比设定显示矩形
			dst_w = (LONG)((float)(dwWidth * rc_h) / (float)dwHeight);
			dst_h = rc_h;

			rc_dst.left = (rc_w - dst_w) >> 1;
			rc_dst.right = rc_dst.left + dst_w;
			rc_dst.top = 0;
			rc_dst.bottom = dst_h;
		}

		break;

	case DisplayAdaptived:
		if (dwWidth >= rc_w || dwHeight >= rc_h)
		{
			if (rc_aspect * rc_h >= rc_w)
			{
				// 按目标窗口的宽度等比设定显示矩形
				dst_w = rc_w;
				dst_h = (LONG)((float)(dwHeight * rc_w) / (float)dwWidth);

				rc_dst.left = 0;
				rc_dst.right = dst_w;
				rc_dst.top = (rc_h - dst_h) >> 1;
				rc_dst.bottom = rc_dst.top + dst_h;
			}
			else
			{
				// 按目标窗口的高度等比设定显示矩形
				dst_w = (LONG)((float)(dwWidth * rc_h) / (float)dwHeight);
				dst_h = rc_h;

				rc_dst.left = (rc_w - dst_w) >> 1;
				rc_dst.right = rc_dst.left + dst_w;
				rc_dst.top = 0;
				rc_dst.bottom = dst_h;
			}
		}
		else
		{
			// 居中显示原始尺寸
			dst_w = dwWidth;
			dst_h = dwHeight;

			rc_dst.left = (rc_w - dst_w) >> 1;
			rc_dst.right = rc_dst.left + dst_w;
			rc_dst.top = (rc_h - dst_h) >> 1;
			rc_dst.bottom = rc_dst.top + dst_h;
		}

		break;

	case DisplayCustom:
		if (!pDestRect || 
			pDestRect->right > rc_w ||
			pDestRect->bottom > rc_h)
		{
			// 填满窗口
			rc_dst.left = rc_dst.top = 0;
			rc_dst.right = rc_w;
			rc_dst.bottom = rc_h;
		}
		else
		{
			rc_dst.left = pDestRect->left;
			rc_dst.right = pDestRect->right;
			rc_dst.top = pDestRect->top;
			rc_dst.bottom = pDestRect->bottom;
		}

		break;

	default:
		// 填满窗口
		rc_dst.left = rc_dst.top = 0;
		rc_dst.right = rc_w;
		rc_dst.bottom = rc_h;
	}

	// 把图像复制到后台缓冲
	mD3DDevice->BeginScene();
	IDirect3DSurface9 * pBackBuffer = NULL;
	mSwapChain[dwId]->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	mD3DDevice->StretchRect(pSurface, NULL, pBackBuffer, &rc_dst, D3DTEXF_LINEAR);
	pBackBuffer->Release();
	mD3DDevice->EndScene();

	pSurface->Release();

	return TRUE;
}


BOOL CDraw::DrawYV12(DWORD dwWidth, DWORD dwHeight, PBYTE y, PBYTE u, PBYTE v, const RECT* pDestRect /* = NULL */, DWORD dwDisplayPositionFlag /* =DisplayFilledAspectRatio */)
{
	return DrawYUV420P(dwWidth, dwHeight, 
		y, v, u, 
		pDestRect, 
		dwDisplayPositionFlag);
}


BOOL CDraw::DrawYV12SubWindow(DWORD dwId, 
	DWORD dwWidth, DWORD dwHeight, 
	PBYTE y, PBYTE u, PBYTE v, 
	const RECT* pDestRect /* = NULL */, 
	DWORD dwDisplayPositionFlag /* =DisplayFilledAspectRatio */)
{
	return DrawYUV420PSubWindow(dwId,
		dwWidth, dwHeight,
		y, v, u, 
		pDestRect, 
		dwDisplayPositionFlag);
}


BOOL CDraw::Draw2DLine(const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor)
{
	if (mD3DDevice && pLineArray && dwLineCount > 0)
	{
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
		VOID* pTempVertexs = NULL;

		if (mCustomVertexBufSize < (dwLineCount << 1))
		{
			delete[] mCustomVertexBuf;
			mCustomVertexBufSize = dwLineCount << 1;
			mCustomVertexBuf = new CUSTOMVERTEX[mCustomVertexBufSize];
		}

		for (int i = 0; i < dwLineCount; i++)
		{
			int j = i << 1;
			mCustomVertexBuf[j].x = pLineArray[i].start.x;
			mCustomVertexBuf[j].y = pLineArray[i].start.y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;

			j++;
			mCustomVertexBuf[j].x = pLineArray[i].end.x;
			mCustomVertexBuf[j].y = pLineArray[i].end.y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;
		}

		do 
		{
			//创建顶点缓冲区    
			if( FAILED( mD3DDevice->CreateVertexBuffer( (dwLineCount << 1)* sizeof(CUSTOMVERTEX), 0, 
				D3DFVF_CUSTOMVERTEX,
				D3DPOOL_DEFAULT, &pVertexBuffer, NULL ) ) )
			{
				break;
			}

			//填充顶点缓冲区
			if( FAILED( pVertexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * dwLineCount << 1, (void**)&pTempVertexs, 0 ) ) )
			{
				break;
			}

			memcpy( pTempVertexs, mCustomVertexBuf, sizeof(CUSTOMVERTEX) * dwLineCount << 1 );
			pVertexBuffer->Unlock();

			mD3DDevice->BeginScene();
			mD3DDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(CUSTOMVERTEX) );
			mD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			mD3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, dwLineCount );
			mD3DDevice->EndScene();
		} 
		while (0);

		if (pVertexBuffer)
			pVertexBuffer->Release();

		return TRUE;
	}

	return FALSE;
}


BOOL CDraw::Draw2DLine(const HV_Point2D* pStartPoints, const HV_Point2D* pEndPoints, DWORD dwLineCount, DWORD dwColor)
{
	if (mD3DDevice && pStartPoints && pEndPoints && dwLineCount > 0)
	{
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
		VOID* pTempVertexs = NULL;
		int i = 0, j = 0;

		if (mCustomVertexBufSize < (dwLineCount << 1))
		{
			delete[] mCustomVertexBuf;
			mCustomVertexBufSize = dwLineCount << 1;
			mCustomVertexBuf = new CUSTOMVERTEX[mCustomVertexBufSize];
		}

		for (i = 0; i < dwLineCount; i++)
		{
			mCustomVertexBuf[j].x = pStartPoints[i].x;
			mCustomVertexBuf[j].y = pStartPoints[i].y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;
			j ++;

			mCustomVertexBuf[j].x = pEndPoints[i].x;
			mCustomVertexBuf[j].y = pEndPoints[i].y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;
			j ++;
		}

		do 
		{
			//创建顶点缓冲区    
			if( FAILED( mD3DDevice->CreateVertexBuffer( (dwLineCount << 1)* sizeof(CUSTOMVERTEX), 0, 
				D3DFVF_CUSTOMVERTEX,
				D3DPOOL_DEFAULT, &pVertexBuffer, NULL ) ) )
			{
				break;
			}

			//填充顶点缓冲区
			if( FAILED( pVertexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * dwLineCount << 1, (void**)&pTempVertexs, 0 ) ) )
			{
				break;
			}

			memcpy( pTempVertexs, mCustomVertexBuf, sizeof(CUSTOMVERTEX) * dwLineCount << 1 );
			pVertexBuffer->Unlock();

			mD3DDevice->BeginScene();
			mD3DDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(CUSTOMVERTEX) );
			mD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			mD3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, dwLineCount );
			mD3DDevice->EndScene();
		} 
		while (0);

		if (pVertexBuffer)
			pVertexBuffer->Release();

		return TRUE;
	}

	return FALSE;
}


BOOL CDraw::Draw2DLineSubWindow(DWORD dwId, const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor)
{
	if (dwId < 16 && mD3DDevice && mSwapChain[dwId] && pLineArray && dwLineCount > 0)
	{
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
		VOID* pTempVertexs = NULL;

		if (mCustomVertexBufSize < (dwLineCount << 1))
		{
			delete[] mCustomVertexBuf;
			mCustomVertexBufSize = dwLineCount << 1;
			mCustomVertexBuf = new CUSTOMVERTEX[mCustomVertexBufSize];
		}

		for (int i = 0; i < dwLineCount; i++)
		{
			int j = i << 1;
			mCustomVertexBuf[j].x = pLineArray[i].start.x;
			mCustomVertexBuf[j].y = pLineArray[i].start.y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;

			j++;
			mCustomVertexBuf[j].x = pLineArray[i].end.x;
			mCustomVertexBuf[j].y = pLineArray[i].end.y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;
		}

		do 
		{
			//创建顶点缓冲区    
			if( FAILED( mD3DDevice->CreateVertexBuffer( (dwLineCount << 1)* sizeof(CUSTOMVERTEX), 0, 
				D3DFVF_CUSTOMVERTEX,
				D3DPOOL_DEFAULT, &pVertexBuffer, NULL ) ) )
			{
				break;
			}

			//填充顶点缓冲区
			if( FAILED( pVertexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * dwLineCount << 1, (void**)&pTempVertexs, 0 ) ) )
			{
				break;
			}

			memcpy( pTempVertexs, mCustomVertexBuf, sizeof(CUSTOMVERTEX) * dwLineCount << 1 );
			pVertexBuffer->Unlock();

			mD3DDevice->BeginScene();
			IDirect3DSurface9* pBackBuffer = NULL;
			IDirect3DSurface9* pOldBuffer = NULL;
			mSwapChain[dwId]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
			mD3DDevice->GetRenderTarget(0, &pOldBuffer);
			mD3DDevice->SetRenderTarget(0, pBackBuffer);
			mD3DDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(CUSTOMVERTEX) );
			mD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			mD3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, dwLineCount );
			mD3DDevice->SetRenderTarget(0, pOldBuffer);
			pOldBuffer->Release();
			pBackBuffer->Release();
			mD3DDevice->EndScene();
		} 
		while (0);

		if (pVertexBuffer)
			pVertexBuffer->Release();

		return TRUE;
	}

	return FALSE;
}


BOOL CDraw::Draw2DLineSubWindow(DWORD dwId, const HV_Point2D* pStartPoints, const HV_Point2D* pEndPoints, DWORD dwLineCount, DWORD dwColor)
{
	if (dwId < 16 && mD3DDevice && mSwapChain[dwId] && pStartPoints && pEndPoints && dwLineCount > 0)
	{
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
		VOID* pTempVertexs = NULL;
		int i = 0, j = 0;

		if (mCustomVertexBufSize < (dwLineCount << 1))
		{
			delete[] mCustomVertexBuf;
			mCustomVertexBufSize = dwLineCount << 1;
			mCustomVertexBuf = new CUSTOMVERTEX[mCustomVertexBufSize];
		}

		for (i = 0; i < dwLineCount; i++)
		{
			mCustomVertexBuf[j].x = pStartPoints[i].x;
			mCustomVertexBuf[j].y = pStartPoints[i].y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;
			j ++;

			mCustomVertexBuf[j].x = pEndPoints[i].x;
			mCustomVertexBuf[j].y = pEndPoints[i].y;
			mCustomVertexBuf[j].z = 0.0f;
			mCustomVertexBuf[j].rhw = 1.0f;
			mCustomVertexBuf[j].color = 0x00FFFFFF & dwColor;
			j ++;
		}

		do 
		{
			//创建顶点缓冲区    
			if( FAILED( mD3DDevice->CreateVertexBuffer( (dwLineCount << 1)* sizeof(CUSTOMVERTEX), 0, 
				D3DFVF_CUSTOMVERTEX,
				D3DPOOL_DEFAULT, &pVertexBuffer, NULL ) ) )
			{
				break;
			}

			//填充顶点缓冲区
			if( FAILED( pVertexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * dwLineCount << 1, (void**)&pTempVertexs, 0 ) ) )
			{
				break;
			}

			memcpy( pTempVertexs, mCustomVertexBuf, sizeof(CUSTOMVERTEX) * dwLineCount << 1 );
			pVertexBuffer->Unlock();

			mD3DDevice->BeginScene();
			IDirect3DSurface9* pBackBuffer = NULL;
			IDirect3DSurface9* pOldBuffer = NULL;
			mSwapChain[dwId]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
			mD3DDevice->GetRenderTarget(0, &pOldBuffer);
			mD3DDevice->SetRenderTarget(0, pBackBuffer);
			mD3DDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(CUSTOMVERTEX) );
			mD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			mD3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, dwLineCount );
			mD3DDevice->SetRenderTarget(0, pOldBuffer);
			pOldBuffer->Release();
			pBackBuffer->Release();
			mD3DDevice->EndScene();
		} 
		while (0);

		if (pVertexBuffer)
			pVertexBuffer->Release();

		return TRUE;
	}

	return FALSE;
}


BOOL CDraw::DrawText(LPCTSTR lpString, HV_Point2D* ptLeftTop, DWORD dwColor, 
					 BYTE byWidth, BYTE byHeight, BOOL bUnderline /* = FALSE */, BOOL bItalic /* = FALSE */)
{
	if (!lpString || !ptLeftTop)
		return FALSE;

	if (mD3DDevice)
	{
		ID3DXFont* pD3DFont = NULL;
		LOGFONT lfFont;
		RECT rc;
		ZeroMemory(&lfFont, sizeof(lfFont));

		lfFont.lfHeight	= byHeight;        // in logical units
		lfFont.lfWidth	= byWidth;        // in logical units
		lfFont.lfWeight	= 500;        // boldness, range 0(light) - 1000(bold)
		lfFont.lfItalic	= bItalic;
		lfFont.lfCharSet	= DEFAULT_CHARSET;
		lfFont.lfOutPrecision	= 0;
		lfFont.lfQuality	= 0;
		lfFont.lfPitchAndFamily	= 0;
		lfFont.lfUnderline	= bUnderline;
		wcscpy_s(lfFont.lfFaceName, L"宋体");//Times New Roman");

		if( FAILED(D3DXCreateFontIndirect(mD3DDevice, &lfFont, &pD3DFont)) )
			return FALSE;

		::GetWindowRect(mMainWnd, &rc);
		rc.right = ptLeftTop->x + (rc.right - rc.left);
		rc.bottom = ptLeftTop->y + (rc.bottom - rc.top);
		rc.left = ptLeftTop->x;
		rc.top = ptLeftTop->y;

		mD3DDevice->BeginScene();
		pD3DFont->DrawText(lpString, -1, &rc, DT_TOP | DT_LEFT, 0xff000000 | dwColor);
		mD3DDevice->EndScene();

		if (pD3DFont)
			pD3DFont->Release();

		return TRUE;
	}
	
	return FALSE;
}



BOOL CDraw::DrawTextSubWindow(DWORD dwId, 
							  LPCTSTR lpString, HV_Point2D* ptLeftTop, DWORD dwColor, 
							  BYTE byWidth, BYTE byHeight, BOOL bUnderline, BOOL bItalic)
{
	if (dwId >= 16 || !lpString || !ptLeftTop)
		return FALSE;

	if (mD3DDevice && mSwapChain[dwId])
	{
		ID3DXFont* pD3DFont = NULL;
		LOGFONT lfFont;
		RECT rc;
		ZeroMemory(&lfFont, sizeof(LOGFONT));

		lfFont.lfHeight         = byHeight;        // in logical units
		lfFont.lfWidth          = byWidth;        // in logical units
		lfFont.lfWeight         = 500;        // boldness, range 0(light) - 1000(bold)
		lfFont.lfItalic         = bItalic;
        lfFont.lfUnderline      = bUnderline;
        lfFont.lfStrikeOut      = false;
	    lfFont.lfCharSet        = DEFAULT_CHARSET;
		lfFont.lfOutPrecision   = 0;
		lfFont.lfQuality        = 0;
		lfFont.lfPitchAndFamily = 0;
 
		wcscpy_s(lfFont.lfFaceName, L"Times New Roman");//Times New Roman");

		if( FAILED(D3DXCreateFontIndirect(mD3DDevice, &lfFont, &pD3DFont)) )
			return FALSE;

		::GetWindowRect(mSwapChainWnd[dwId], &rc);

        rc.right  = ptLeftTop->x + (rc.right - rc.left);
        rc.bottom = ptLeftTop->y + (rc.bottom - rc.top);
        rc.left   = ptLeftTop->x;
        rc.top    = ptLeftTop->y;

        mD3DDevice->BeginScene();
        IDirect3DSurface9* pBackBuffer = NULL;
        IDirect3DSurface9* pOldBuffer = NULL;
        mSwapChain[dwId]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
        mD3DDevice->GetRenderTarget(0, &pOldBuffer);
        mD3DDevice->SetRenderTarget(0, pBackBuffer);
        pD3DFont->DrawText(lpString, -1, &rc, DT_TOP | DT_LEFT | DT_NOCLIP, 0xff000000 | dwColor);
        mD3DDevice->SetRenderTarget(0, pOldBuffer);
        pOldBuffer->Release();
        pOldBuffer->Release();
        mD3DDevice->EndScene();

		if (pD3DFont)
			pD3DFont->Release();

		return TRUE;
	}

	return FALSE;
}




BOOL CDraw::DrawCircleSubWindow(DWORD dwId, int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor)
{
	IDirect3DSurface9* pBackBuffer = NULL;
	mSwapChain[dwId]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	HDC hDC = NULL;
	HRESULT hResult = pBackBuffer->GetDC(&hDC);
	HPEN hPen = CreatePen(PS_SOLID, nPixel, RGB((FrameColor >> 16) & 0x000000FF, (FrameColor >> 8) & 0x000000FF, FrameColor & 0x000000FF));
	HBRUSH hBr = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen); 
	HBRUSH hOldBr = (HBRUSH)SelectObject(hDC, hBr);

	// Draw ellipse.
	Ellipse(hDC, xCenter - nRadius, yCenter - nRadius, xCenter + nRadius, yCenter + nRadius);

	SelectObject(hDC, hOldBr);
	SelectObject(hDC, hOldPen);
	DeleteObject (hBr);
	DeleteObject (hPen);

	pBackBuffer->ReleaseDC(hDC);
	pBackBuffer->Release();

	return TRUE;
}


BOOL CDraw::DrawRectangleSubWindow(DWORD dwId, int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor)
{
	IDirect3DSurface9* pBackBuffer = NULL;
	mSwapChain[dwId]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	HDC hDC = NULL;
	HRESULT hResult = pBackBuffer->GetDC(&hDC);
	HPEN hPen = CreatePen(PS_SOLID, nPixel, RGB((FrameColor >> 16) & 0x000000FF, (FrameColor >> 8) & 0x000000FF, FrameColor & 0x000000FF));
	HBRUSH hBr = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen); 
	HBRUSH hOldBr = (HBRUSH)SelectObject(hDC, hBr);

	int halfLenOfSide = nLenOfSide / 2;
	Rectangle(hDC, xCenter - halfLenOfSide, yCenter - halfLenOfSide, xCenter + halfLenOfSide, yCenter + halfLenOfSide);

	SelectObject(hDC, hOldBr);
	SelectObject(hDC, hOldPen);
	DeleteObject (hBr);
	DeleteObject (hPen);

	pBackBuffer->ReleaseDC(hDC);
	pBackBuffer->Release();

	return TRUE;
}


BOOL CDraw::Present()
{
	if (mD3DDevice)
	{
		HRESULT hRet = mD3DDevice->Present(NULL, NULL, NULL, NULL);
		return (hRet == S_OK);
	}

	return FALSE;
}


BOOL CDraw::PresentSubWindow(DWORD dwId)
{
	if (dwId <= 15 && mD3DDevice && mSwapChain[dwId])
	{
		HRESULT hRet = mSwapChain[dwId]->Present(NULL, NULL, NULL, NULL, 0);
		return (hRet == S_OK);
	}

	return FALSE;
}