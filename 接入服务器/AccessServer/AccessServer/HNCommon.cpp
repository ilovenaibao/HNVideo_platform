#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "HNCommon.h"
#include "windows.h"
//#include "leak_detector_c.h"

// type convert
void int_to_byte4(int in, char *out)
{
	int tmp = in;
	int j = 0;
	char tmp_byte = 0x00;

	if (NULL == out) {
		return;
	}
	for (j = 3; 0<= j; j--) {
		out[j] = tmp_byte | tmp;
		tmp >>= 8;
	}
}

int byte4_to_int(char *in)
{
	int ret = 0;
	int j = 0;
	if (NULL == in) {
		return ret;
	}
	for (j = 0; j < 3; j++) {
		ret |= in[j];
		ret <<= 8;
	}
	ret |= in[j];
	return ret;
}

char *adjust_buffer_size(char *buf, int need_size)
{
	char *res = NULL;
	int len = 0;
	if(need_size > 0) {
		if(NULL == buf) {
			len = _MAX(need_size, 512);
			res = (char *)malloc(need_size);
		} else {
			len = _msize(buf);
			if (need_size > len) {
				res = (char*)realloc(buf, need_size);
			}
		}

		if(res == NULL) {
			return NULL;
		} else {
			buf = res;
		}
	}

	return res;
}

// 把UTF-8转换成Unicode
void UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}
// Unicode 转换成UTF-8 
void UnicodeToUTF_8(char* pOut, wchar_t* pText)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

// 把Unicode 转换成 GB2312 
void UnicodeToGB2312(char* pOut, wchar_t *uData)
{
	WideCharToMultiByte(CP_ACP, NULL, uData, 1, pOut, sizeof(WCHAR), NULL, NULL);
	return;
}     

// GB2312 转换成　Unicode
void Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return;
}

//GB2312 转为 UTF-8
void GB2312ToUTF_8(char *pOut, char *pText, int pLen)
{
	char buf[4];
	char* rst = new char[pLen + (pLen >> 2) + 2];

	memset(buf,0,4);
	memset(rst,0,pLen + (pLen >> 2) + 2);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		//如果是英文直接复制就可以 
		if ( *(pText + i) >= 0 )
		{
			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);

			UnicodeToUTF_8(buf,&pbuffer);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];


			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	//返回结果
	pOut = rst;             
	delete []rst;   

	return;
}

//UTF-8 转为 GB2312
void UTF_8ToGB2312(char *pOut, char *pText, int pLen)
{
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			pOut[j++] = pText[i++];                       
		}
		else                 
		{
			wchar_t Wtemp;
			UTF_8ToUnicode(&Wtemp, pText + i);

			UnicodeToGB2312(Ctemp, &Wtemp);

			pOut[j] = Ctemp[0];
			pOut[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	pOut[j] = '\0';

	return; 
}