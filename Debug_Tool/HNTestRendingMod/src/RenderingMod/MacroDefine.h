#pragma once
//#include "StatusDialogBase.h"
#include <list>
using namespace std;

//#define MAX_COUNTER_DATA_SIZE              4*1024*1024L

#define COUNTER_TYPE_F_NAME_PASSFLOW       _T("客流计数器")
#define COUNTER_TYPE_F_NAME_DELAY          _T("滞留计数器")
#define COUNTER_TYPE_F_NAME_EVENT          _T("事件计数器")
#define COUNTER_TYPE_F_NAME_TRACK          _T("轨迹计数器")
#define COUNTER_TYPE_F_NAME_CROSS          _T("过店计数器")

#define COUNTER_TYPE_S_NAME_PASSFLOW         _T("客流")
#define COUNTER_TYPE_S_NAME_DELAY            _T("滞留")
#define COUNTER_TYPE_S_NAME_EVENT            _T("事件")
#define COUNTER_TYPE_S_NAME_TRACK            _T("轨迹")
#define COUNTER_TYPE_S_NAME_CROSS            _T("过店")

#define COUNTER_TYPE_PASSENGERFLOW          1    //【计数器类型】客流计数器
#define COUNTER_TYPE_DELAY                  2    //【计数器类型】滞留计数器
#define COUNTER_TYPE_EVENT                  3    //【计数器类型】事件计数器
#define COUNTER_TYPE_TRACK                  4    //【计数器类型】轨迹计数器
#define COUNTER_TYPE_CROSS                  5    //【计数器类型】过店计数器

#define COUNTER_REPORT_MODE_NONE            0    //【上报类型】空 
#define COUNTER_REPORT_MODE_INTERVAL        1    //【上报类型】实时上报间隔
#define COUNTER_REPORT_MODE_PERIOD          2    //【上报类型】累计上报周期
#define COUNTER_REPORT_MODE_ALL             3    //【上报类型】实时累计同时上报


//typedef list<CCounterBase*>           LIST_ALL_COUNTER;           //计数器容器
//typedef LIST_ALL_COUNTER::iterator    LIST_ALL_COUNTER_ITERATOR;  //计数器迭代器

