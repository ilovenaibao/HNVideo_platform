#include "StdAfx.h"
#include "AviWriter.h"

#pragma comment(lib, "Vfw32.lib")

CAviWriter::CAviWriter(void)
	: avi(NULL)
	, stream(NULL)
	, frameNo(0)
	, setFormat(false)
{
	::InitializeCriticalSection(&hCs);
}


CAviWriter::~CAviWriter(void)
{
	::DeleteCriticalSection(&hCs);
}


bool CAviWriter::AVI_Create_Video_file(LPCTSTR filename)
{
	bool result = true;
	::EnterCriticalSection(&hCs);
	do 
	{
		AVIFileInit();
		if (0 != AVIFileOpen(&avi, filename, OF_CREATE, NULL))
		{
			AVIFileExit();
			avi = NULL;
			result = false;
		}
	} while (false);
	::LeaveCriticalSection(&hCs);
	return result;
}


bool CAviWriter::AVI_Set_Video(int width, int height, char* compressor)
{
	bool result = false;
	::EnterCriticalSection(&hCs);
	do 
	{
		AVISTREAMINFO aviStreamInfo;
		ZeroMemory(&aviStreamInfo, sizeof(aviStreamInfo));
		aviStreamInfo.fccType = streamtypeVIDEO;
		aviStreamInfo.fccHandler = NULL;
		aviStreamInfo.dwRate = 25; 
		aviStreamInfo.dwScale = 1;
		aviStreamInfo.dwQuality = -1;
		aviStreamInfo.dwSuggestedBufferSize = width * height * 3;
		SetRect(&aviStreamInfo.rcFrame, 0, 0, width, height);

		if (avi == NULL || 0 != AVIFileCreateStream(avi, &stream, &aviStreamInfo))
		{
			stream = NULL;
			break;
		}
		BITMAPINFOHEADER bmpInfoHdr;
		ZeroMemory(&bmpInfoHdr,sizeof(BITMAPINFOHEADER));
		bmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfoHdr.biWidth = width;
		bmpInfoHdr.biHeight = height;
		bmpInfoHdr.biBitCount = 3;
		int compression;
		memcpy_s(&compression, sizeof(compression), compressor, 4);
		bmpInfoHdr.biCompression = compression;
		bmpInfoHdr.biSizeImage = 0;
		bmpInfoHdr.biPlanes = 1;
		setFormat = (0 == AVIStreamSetFormat(stream, 0, &bmpInfoHdr, sizeof(bmpInfoHdr)));
		result = setFormat;
	} while (false);
	::LeaveCriticalSection(&hCs);
	return result;
}


bool CAviWriter::AVI_Write_frame(const char* pVideo,long byteLength)
{
	bool result = false;
	::EnterCriticalSection(&hCs);
	if (setFormat && 0 == AVIStreamWrite(stream, frameNo, 1, (LPBYTE)pVideo, byteLength, AVIIF_KEYFRAME, NULL, NULL))
	{
		frameNo++;
		result = true;
	}
	::LeaveCriticalSection(&hCs);
	return result;
}


void CAviWriter::AVI_Close_file()
{
	::EnterCriticalSection(&hCs);
	if (stream != NULL)
	{
		AVIStreamClose(stream);
		stream = NULL;		
	}
	if (avi != NULL)
	{
		AVIFileClose(avi);
		avi = NULL;
		AVIFileExit();
	}
	frameNo = 0;
	setFormat = false;
	::LeaveCriticalSection(&hCs);
}


bool CAviWriter::isOpen()
{
	return avi != NULL;
}
