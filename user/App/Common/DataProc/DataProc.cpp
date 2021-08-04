#include "DataProc.h"
#include "../HAL/HAL.h"

/**
 *  创建数据中心
 *  修改为这样是因为在kendryte ide中出现全局对象不调用构造函数的问题
 *  网上看到的解释这个问题与ld相关
 **/
static DataCenter* dataCenter;
DataCenter* DataProc::Center()
{
    return dataCenter;
}

void DataProc_Init()
{
    dataCenter = new DataCenter("CENTER");
#define DP_DEF(NodeName, bufferSize)\
    Account* act##NodeName = new Account(#NodeName, dataCenter, bufferSize);
#  include "DP_LIST.inc"
#undef DP_DEF

#define DP_DEF(NodeName, bufferSize)\
do{\
    extern void _DP_##NodeName##_Init(Account* account);\
    _DP_##NodeName##_Init(act##NodeName);\
}while(0)
#  include "DP_LIST.inc"
#undef DP_DEF

}
