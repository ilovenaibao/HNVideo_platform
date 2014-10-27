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


// ����Ŀ�괰��
const int DisplayFilled				= 0x00000000L;
// ���ֳ���ȵķ�ʽ����Ŀ�괰��
const int DisplayFilledAspectRatio	= 0x00000001L;
// ������ʾԭʼ�ߴ�, ����Ƶ�ߴ��Ŀ�괰�ڳߴ���򱣳ֳ��������Ŀ�괰��
const int DisplayAdaptived			= 0x00000002L; 
// �����û������Ŀ����ν�����ʾ������α�Ŀ�괰�ڴ��򱣳ֳ���ȵķ�ʽ����Ŀ�괰�ڣ�Ŀ���������Ϊ���ڵ��������
const int DisplayCustom				= 0x00000003L; 


#endif
