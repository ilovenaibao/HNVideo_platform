#include "stdafx.h"
#include "RenderObject.h"
#include "MacroDefine.h"


CRenderObject::CRenderObject(void)
	: frameNo(0)
	, buffer(NULL)
	, bufferLen(0)
	, dataLen(0)
	, width(0)
	, height(0)
	, type(0)
	, hour(0)
	, minute(0)
	, second(0)
	, tenmillisecond(0)
	, inPC(0)
	, outPC(0)
	, inFMPC(0)
	, outFMPC(0)
	, bSetFMPC(false)
	, algoET(0)
	, bSetObjLoc(false)
    , bSetObjTrack(false)
    , recvCounterData(NULL)
    , m_nlen(0)
{
    // 创建临界区
    ::InitializeCriticalSection(&hCs_recvData);
}


CRenderObject::~CRenderObject(void)
{
    ::DeleteCriticalSection(&hCs_recvData);
    if (recvCounterData != NULL)
    {
        delete []recvCounterData;
        recvCounterData = NULL;
    }
}


void CRenderObject::Init(void)
{
	if (!buffer)
	{
		buffer = new BYTE[352 * 288 * 2];
		bufferLen = 352 * 288 * 2;
		dataLen = 0;
	}
}


void CRenderObject::Clear(void)
{
	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
		bufferLen = 0;
		dataLen = 0;

		Reset();
	}
}


void CRenderObject::GetFrameData(LPBYTE& lpData, unsigned int& nLen)
{
	lpData = buffer;
	nLen = dataLen;
}


void CRenderObject::SetFrameData(const BYTE* lpData, unsigned int nLen)
{
	if (bufferLen < nLen)
	{
		delete[] buffer;
		buffer = new BYTE[nLen];
		bufferLen = nLen;
	}
	dataLen = nLen;
	memcpy_s(buffer, bufferLen, lpData, nLen);
}


unsigned int CRenderObject::GetFrameNo(void)
{
	return frameNo;
}


void CRenderObject::SetFrameNo(unsigned int nFrameNo)
{
	frameNo = nFrameNo;
}


int CRenderObject::GetWidth(void)
{
	return width != 0 ? width : 352;
}


void CRenderObject::SetWidth(int nWidth)
{
	width = nWidth;
}


int CRenderObject::GetHeight(void)
{
	return  height != 0 ? height : 288;
}


void CRenderObject::SetHeight(int nHeight)
{
	height = nHeight;
}


unsigned int CRenderObject::GetType(void)
{
	return type;
}


void CRenderObject::SetType(unsigned int nType)
{
	type = nType;
}

void CRenderObject::GetDevTime(unsigned char& nHour, unsigned char& nMinute, unsigned char& nSecond, unsigned char& nTenmillisecond)
{
	nHour = hour;
	nMinute = minute;
	nSecond = second;
	nTenmillisecond = tenmillisecond;
}


void CRenderObject::SetDevTime(unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenmillisecond)
{
	hour = nHour;
	minute = nMinute;
	second = nSecond;
	tenmillisecond = nTenmillisecond;
}


void CRenderObject::SetObjLoc(CObjLocInfo* vObj, unsigned int nLen)
{
	vObjLoc.clear();
	for (unsigned int count = 0; count < nLen; count++)
	{
		vObjLoc.push_back(vObj[count]);
	}
	bSetObjLoc = true;
}


bool CRenderObject::GetObjLoc(CObjLocInfo*& vObj, unsigned int& nLen)
{
	vObj = vObjLoc.size() ? &vObjLoc[0] : NULL;
	nLen = vObjLoc.size();
	return bSetObjLoc;
}

unsigned short CRenderObject::GetAlgoET(void)
{
	return algoET;
}


void CRenderObject::SetAlgoET(unsigned short nAlgoET)
{
	algoET = nAlgoET;
}


void CRenderObject::Reset(void)
{
	frameNo = 0;
	dataLen = 0;
	width = 0;
	height = 0;
	type = 0;

	hour = 0;
	minute = 0;
	second = 0;
	tenmillisecond = 0;

	inPC = 0;
	outPC = 0;
	inFMPC = 0;
	outFMPC = 0;
	bSetFMPC = false;
	vObjLoc.clear();
	bSetObjLoc = false;
    bSetObjTrack = false;
	algoET = 0;
}

void CRenderObject::SetAllCounterData(const char* allCounterData, int nlen )
{
    if (allCounterData == NULL)
    {
        return;
    }
    //这里处理所有计数器数据
    ::EnterCriticalSection(&hCs_recvData);

    if (recvCounterData != NULL)
    {
        delete []recvCounterData;
        recvCounterData = NULL;
    }

    recvCounterData = new char[nlen];
    m_nlen = nlen ;
    memset(recvCounterData, NULL, m_nlen);
    memcpy_s(recvCounterData, m_nlen, allCounterData, nlen);

    ::LeaveCriticalSection(&hCs_recvData);
}


void CRenderObject::GetAllCounterData( char* allCounterData, int nlength)
{
    ::EnterCriticalSection(&hCs_recvData);
    if (recvCounterData != NULL)
    {
        memcpy_s(allCounterData, nlength, recvCounterData, m_nlen);
    }
    ::LeaveCriticalSection(&hCs_recvData);
}

int CRenderObject::GetAllCounterDataSize()
{
    return m_nlen;
}
