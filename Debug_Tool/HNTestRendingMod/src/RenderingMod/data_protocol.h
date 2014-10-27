#ifndef _IPVA_DEBUG_PROTOCOL_H_
#define _IPVA_DEBUG_PROTOCOL_H_

#include "../hv_types.h"
#include "../hv_versions.h"

#include <list>
using namespace std;


/**
* @brief	����Э�������ֶ���
*/
#define	IPVA_DT_DATA_LOGIN				0x0000		// ��¼����
#define	IPVA_DT_DATA_REG				0x0001		// ע��Ŀ��ͨ��
#define IPVA_DT_DATA_UNREG				0x0002		// ע��Ŀ��ͨ��
#define IPVA_DT_DATA_HEARTBEAT			0x0003		// ��������
#define IPVA_DT_DATA_REQDATA			0x0004		// ��������
// 0x0005 �C 0xFFFF					Э�鱣������


/**
* @brief	����Э�������ֶ���
*/
#define IPVA_DT_DATA_DATALINK_TOKEN		0x0000		// ������֤
// 0x0001 �C 0xFFFF					Э�鱣������

/**
* @brief	֧�ֵĴ���ģʽ
*/
#define IPVA_DT_DATA_MODE_TCP			0			// TCPģʽ
#define IPVA_DT_DATA_MODE_TCP_REUSE		1			// TCP����ģʽ
#define IPVA_DT_DATA_MODE_UDP			2			// UDP����
#define IPVA_DT_DATA_MODE_UDP_MULTICAST	3			// UDP�鲥


/**
* @brief	Ӧ���붨��
*/
#define S_DATA_OK						0x0000		// �����ɹ�
#define S_ERR_DATA_CONNECT_TIMEOUT		0x0001		// ���ӳ�ʱ
#define S_ERR_DATA_ILLEGALACCESS		0x0002		// ����ʧ�ܣ��Ƿ�����
#define S_ERR_DATA_USERORPWD_INCORRECT	0x0003		// ��¼ʧ�ܣ��û������������
#define S_ERR_DATA_PROT_NOTSUPP			0x0004		// ��¼ʧ�ܣ�Э��汾����֧��
#define S_ERR_DATA_CHNL_NOTSUPP			0x0005		// ע��ʧ�ܣ�Ŀ��ͨ������֧��
#define S_ERR_DATA_CHNL_CONNECT_TOOMUCH	0x0006		// ע��ʧ�ܣ�Ŀ��ͨ������������
#define S_ERR_DATA_REQ_NOTSUPP			0x0007		// ��������ʧ�ܣ���������֧��
#define S_ERR_DATA_LOGOFF_FAILED		0x0008		// ע��ʧ��
#define S_ERR_DATA_CMD_UNSUPPORT		0x0009		// ���֧��
#define S_ERR_DATA_CMD_TOKEN			0x000A		// ������֤ʧ��

/**
* @brief	��������
*/
#define IPVA_DT_DATA_DATATYPE_TEST		0x00		// �������������������
#define IPVA_DT_DATA_DATATYPE_VIDEO		0x01		// ʵʱ��Ƶ����
//#define IPVA_DT_DATA_DATATYPE_PC		0x02		// ʵʱ������������
//#define IPVA_DT_DATA_DATATYPE_FMPC		0x03		// ʵʱ5���ӽ�����������
#define IPVA_DT_DATA_DATATYPE_OBJLOC	0x04		// ʵʱĿ��λ������
#define IPVA_DT_DATA_DATATYPE_DEVTIME	0x05		// ʵʱ�豸ʱ��
#define IPVA_DT_DATA_DATATYPE_ET		0x06		// ͨ���㷨ƽ����ʱ����
#define IPVA_DT_DATA_DATATYPE_COUNTER   0x07        // �������������������� Add by WangJie [2014-8-26]
// 0x07 �� 0xFF	����


/**
* @brief	��������
*/
#define IPVA_DT_DATA_TYPE_TEST			0x00		// ���������԰�
#define IPVA_DT_DATA_TYPE_IDRFRAME		0x01		// H.264 IDR֡
#define IPVA_DT_DATA_TYPE_BPFRAME		0x02		// H.264 B/P֡
// 0x03 �C 0x0F	�������ͣ�������Ƶ����������չ
//#define IPVA_DT_DATA_TYPE_PC			0x10		// ʵʱ��������
//#define IPVA_DT_DATA_TYPE_FMPC			0x11		// ʵʱ5����ͳ�ƽ�������
#define IPVA_DT_DATA_TYPE_OBJLOC		0x12		// ʵʱĿ����������
// 0x13 �C 0x1F	�������ͣ������㷨��������չ
#define IPVA_DT_DATA_TYPE_DEVTIME		0x20		// ʵʱ�豸ʱ��
#define IPVA_DT_DATA_TYPE_AGLOTIME		0x21		// ʵʱ�㷨����ʱ�䣨ƽ��ʱ�䣬����/֡��     
#define IPVA_DT_DATA_TYPE_COUTER        0x22        // �������ĸ�������Add by WangJie [2014-8-26]
// 0x22 �� 0x2F	�������ͣ������豸��Ϣ����������Ϣ����չ
// 0x30 �� 0xFF	�������ͣ�����������������������չ


//�������ϱ����ݻ���
typedef struct
{
    HV_U32 CountId;
    HV_U32  Type;
}Counter_Base_Info;

typedef struct _TCPData_Payload_PFCount:public Counter_Base_Info
{
    HV_U32 nTotalIn;       //�ۼƽ�����
    HV_U32 nTotalOut;      //�ۼƳ�����
    HV_U32 nCurrentIn;     //ʵʱ������
    HV_U32 nCurrentOut;    //ʵʱ������
}
TCPData_Payload_PFCount;

typedef struct _TCPData_PayloadCrossCount:public Counter_Base_Info
{
    HV_U32 nTotalCrossNum;      // �ۼƹ������
    HV_U32 nCrossNum;           // ʵʱ�������
}
TCPData_PayloadCrossCount;

typedef struct _TCPData_Payload_DelayCount:public Counter_Base_Info
{
    HV_U32 nCurDelayCount;      //������������
    HV_U32 nMaxDelayTime;       //���ڸ����������ʱ��
    HV_U32 nTotalDelayTime;     //��������ʱ���ܺ�
}
TCPData_Payload_DelayCount;

typedef struct 
{	
    HV_U32   nId;                 //����Id
    HV_U8   nEventType;          //�����¼�����
    char     nEventTime[23];      //����ʱ��
}Event_Occur_Info;

typedef struct _TCPData_Payload_EventCount:public Counter_Base_Info
{
    HV_U32 nTotalIn;     //�ۼƽ�����
    HV_U32 nTotalOut;    //�ۼƳ�����
    HV_U32 nEvnetNum;    //�¼�����
    Event_Occur_Info* nEventInfoList;
}
TCPData_Payload_EventCount;


typedef struct
{
    HV_U32 nId;             // Ŀ��ID
    HV_U32 ntrack;           // ����
    char nSampleTime[24];
}Track_Point_Info;

typedef struct _TCPData_Payload_TrackCount:public Counter_Base_Info
{
    HV_U32 nObjectCoorNum;              //�켣����Ŀ
    Track_Point_Info* nObjectCoor;      //�켣���б�
}
TCPData_Payload_TrackCount;

typedef struct _TCPData_Payload_ALL_COUNTER
{
    HV_U8       nCountersNum;               //����������
    Counter_Base_Info* nCounterOutput[8];	//�����������Ϣ��
    _TCPData_Payload_ALL_COUNTER()
    {
        for(int i = 0;i < 8;i++)
        {
            nCounterOutput[i] = 0;
        }
    }
}TCPData_Payload_All_Counter;

typedef list<Counter_Base_Info*>              LIST_ALL_COUNTER_DATA;
typedef LIST_ALL_COUNTER_DATA::iterator       LIST_ALL_COUNTER_DATA_ITER;

#endif


