#include "DataProc.h"

#include "../HAL/HAL.h"

DataCenter* DataProc::Center() { return &dataCenter; }

void DataProc_Init() {
  static DataCenter dataCenter("CENTER");
#define DP_DEF(NodeName, bufferSize) \
  Account* act##NodeName = new Account(#NodeName, &dataCenter, bufferSize);
#include "DP_LIST.inc"
#undef DP_DEF

#define DP_DEF(NodeName, bufferSize)                     \
  do {                                                   \
    extern void _DP_##NodeName##_Init(Account* account); \
    _DP_##NodeName##_Init(act##NodeName);                \
  } while (0)
#include "DP_LIST.inc"
#undef DP_DEF
}
