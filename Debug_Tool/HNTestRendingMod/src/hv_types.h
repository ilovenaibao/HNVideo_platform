#ifndef _HV_TYPES_H_
#define _HV_TYPES_H_

//基础数据类型定义
typedef char					HV_S8,	*HV_S8PTR;
typedef unsigned char			HV_U8,	*HV_U8PTR;
typedef short					HV_S16,	*HV_S16PTR;
typedef unsigned short			HV_U16,	*HV_U16PTR;
typedef int						HV_S32,	*HV_S32PTR;
typedef unsigned int			HV_U32,	*HV_U32PTR;
typedef long					HV_SLONG,	*HV_SLPTR;
typedef unsigned long			HV_ULONG,*HV_ULPTR;
typedef long long				HV_S64,	*HV_S64PTR;
typedef unsigned long long		HV_U64,	*HV_U64PTR;
typedef float					HV_FLOAT32;
typedef double					HV_FLOAT64;
typedef void					HV_VOID, *HV_PTR, *HV_HANDLE;

#ifndef HV_BOOL
#define HV_BOOL					HV_S32
#endif

#ifndef HV_RESULT
#define HV_RESULT				HV_S32
#endif

#ifndef HV_CONST
#define HV_CONST				const
#endif
#ifndef HV_TRUE
#define HV_TRUE					1
#endif
#ifndef HV_FALSE
#define HV_FALSE				0
#endif


#ifndef HV_MAX
#define HV_MAX(a, b)	( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef HV_MIN
#define HV_MIN(a, b)	( ((a) < (b)) ? (a) : (b) )
#endif
#ifndef HV_ABS
#define HV_ABS(a)		( ((a) > 0) ? (a) : (0 - (a)) )
#endif // !HV_ABS





#endif

