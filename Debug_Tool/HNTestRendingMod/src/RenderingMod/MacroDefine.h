#pragma once
//#include "StatusDialogBase.h"
#include <list>
using namespace std;

//#define MAX_COUNTER_DATA_SIZE              4*1024*1024L

#define COUNTER_TYPE_F_NAME_PASSFLOW       _T("����������")
#define COUNTER_TYPE_F_NAME_DELAY          _T("����������")
#define COUNTER_TYPE_F_NAME_EVENT          _T("�¼�������")
#define COUNTER_TYPE_F_NAME_TRACK          _T("�켣������")
#define COUNTER_TYPE_F_NAME_CROSS          _T("���������")

#define COUNTER_TYPE_S_NAME_PASSFLOW         _T("����")
#define COUNTER_TYPE_S_NAME_DELAY            _T("����")
#define COUNTER_TYPE_S_NAME_EVENT            _T("�¼�")
#define COUNTER_TYPE_S_NAME_TRACK            _T("�켣")
#define COUNTER_TYPE_S_NAME_CROSS            _T("����")

#define COUNTER_TYPE_PASSENGERFLOW          1    //�����������͡�����������
#define COUNTER_TYPE_DELAY                  2    //�����������͡�����������
#define COUNTER_TYPE_EVENT                  3    //�����������͡��¼�������
#define COUNTER_TYPE_TRACK                  4    //�����������͡��켣������
#define COUNTER_TYPE_CROSS                  5    //�����������͡����������

#define COUNTER_REPORT_MODE_NONE            0    //���ϱ����͡��� 
#define COUNTER_REPORT_MODE_INTERVAL        1    //���ϱ����͡�ʵʱ�ϱ����
#define COUNTER_REPORT_MODE_PERIOD          2    //���ϱ����͡��ۼ��ϱ�����
#define COUNTER_REPORT_MODE_ALL             3    //���ϱ����͡�ʵʱ�ۼ�ͬʱ�ϱ�


//typedef list<CCounterBase*>           LIST_ALL_COUNTER;           //����������
//typedef LIST_ALL_COUNTER::iterator    LIST_ALL_COUNTER_ITERATOR;  //������������

