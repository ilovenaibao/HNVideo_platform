
#ifndef  LEAK_DETECTOR_C_H
#define  LEAK_DETECTOR_C_H

#include <stdio.h>
#include <conio.h>

#define CHECK_MEME_LEAK \
	if (1 == report_mem_leak()) { \
		printf("error: memory leak!\n"); \
		getch(); \
		exit(0); \
	} // ¼ì²âÄÚ´æÐ¹Â©


#define  FILE_NAME_LENGTH   	   256
#define  OUTPUT_FILE			   "leak_info.txt"
#define  malloc(size) 	    	   	xmalloc (size, __FILE__, __LINE__)
#define  calloc(elements, size)  	xcalloc (elements, size, __FILE__, __LINE__)
#define  free(mem_ref) 		  	 	xfree(mem_ref)

#define  new(size)						xnew (size, __FILE__, __LINE__)


struct _MEM_INFO
{
	void			*address;
	unsigned int	size;
	char			file_name[FILE_NAME_LENGTH];
	unsigned int	line;
};
typedef struct _MEM_INFO MEM_INFO;

struct _MEM_LEAK {
	MEM_INFO mem_info;
	struct _MEM_LEAK * next;
};
typedef struct _MEM_LEAK MEM_LEAK;

void add(MEM_INFO alloc_info);
void erase(unsigned pos);
void clear(void);

void * xmalloc(unsigned int size, const char * file, unsigned int line);
void * xcalloc(unsigned int elements, unsigned int size, const char * file, unsigned int line);
void xfree(void * mem_ref);

void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line);
void remove_mem_info (void * mem_ref);
int report_mem_leak(void);

void * xnew (unsigned int size, const char * file, unsigned int line);

#endif
