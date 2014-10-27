#ifndef _DUDEF_H_
#define _DUDEF_H_
#include <WinDef.h>
#include <WTypes.h>

typedef struct HV_Point2D
{
	short x, y;
}
HV_Point2D;

typedef struct HV_Point3D
{
	float x,y,z;
}
HV_Point3D;

typedef struct HV_Line2D
{
	HV_Point2D start;
	HV_Point2D end;
}
HV_Line2D;

typedef struct HV_Line3D
{
	HV_Point3D start;
	HV_Point3D end;
}
HV_Line3D;

typedef HV_Point2D Point2D;
typedef HV_Point3D Point3D;
typedef HV_Line2D Line2D;
typedef HV_Line3D Line3D;


// 填满目标窗口
const int DisplayFilled				= 0x00000000L;
// 保持长宽比的方式填满目标窗口
const int DisplayFilledAspectRatio	= 0x00000001L;
// 居中显示原始尺寸, 如视频尺寸比目标窗口尺寸大，则保持长宽比填满目标窗口
const int DisplayAdaptived			= 0x00000002L; 
// 根据用户输入的目标矩形进行显示，如矩形比目标窗口大，则保持长宽比的方式填满目标窗口，目标矩形坐标为窗口的相对坐标
const int DisplayCustom				= 0x00000003L; 


#endif
