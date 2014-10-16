#ifndef _HNCOMMON_H_
#define _HNCOMMON_H_

#ifndef _IN
#define _IN
#endif

#ifndef _OUT
#define _OUT
#endif

typedef unsigned char _byte_;

#define AGENT_MALLOC(to, size) to = (char*)malloc(size)
#define AGENT_REALLOC(tmp, to, new_size) do { \
	if(to) {\
	tmp = (char*)realloc(to, new_size); \
	if(tmp) \
	to = tmp; \
	} else { \
	tmp = NULL; \
	} \
} while(0)

#define _MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define _MIN(a,b)    (((a) < (b)) ? (a) : (b))

typedef struct {
	int buffer_size;
	int data_len;
	char *data;

	void _char_buffer()
	{
		buffer_size = 0;
		data_len = 0;
		data = NULL;
	}

} _char_buffer;

// type convert
void int_to_byte4(int in, char *out);

int byte4_to_int(char *in);

char *adjust_buffer_size(char *buf, int need_size);


// 把UTF-8转换成Unicode
void UTF_8ToUnicode(wchar_t* pOut, char *pText);

// Unicode 转换成UTF-8 
void UnicodeToUTF_8(char* pOut, wchar_t* pText);

// 把Unicode 转换成 GB2312 
void UnicodeToGB2312(char* pOut, wchar_t *uData);

// GB2312 转换成　Unicode
void Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer);

//GB2312 转为 UTF-8
void GB2312ToUTF_8(char *pOut, char *pText, int pLen);

//UTF-8 转为 GB2312
void UTF_8ToGB2312(char *pOut, char *pText, int pLen);

#endif