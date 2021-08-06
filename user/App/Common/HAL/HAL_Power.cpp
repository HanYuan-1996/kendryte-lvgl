#include "HAL.h"

void HAL::Power_GetInfo(Power_Info_t* info) {
  int usage = 70;

  if (usage > 100) {
    usage = 100;
  }

  info->isCharging = true;
  info->voltage = 3700;
  info->usage = usage;
}

void HAL::Power_SetEventCallback(Power_CallbackFunction_t callback) {}
