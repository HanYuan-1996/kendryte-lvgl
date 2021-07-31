#include <stdio.h>

#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl/lvgl.h"

void setup() {}

int main() {
  setup();
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  while (1) {
    lv_task_handler();
  }

  return 1;
}
