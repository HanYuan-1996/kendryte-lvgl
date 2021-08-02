#include <timer.h>

#include "App.h"
#include "lv_port_disp.h"
#include "lv_port_fs.h"
#include "lv_port_indev.h"
#include "lvgl/lvgl.h"

/* cpu enter sleep state */
void __wfi() { asm volatile("wfi"); }

/* setup the X-TRACK application */
void setup() { App_Init(); }

/* lvgl tick, 5ms */
int lv_timer_callback(void *ctx) {
  lv_tick_inc(5);
  return 1;
}

int main() {
  /**
   *    initialize bsp setting
   *    already done before enter main func
   */
  /* set cpu frequence to 400MHz */
  // sysctl_cpu_set_freq(400 * 1000 * 1000);
  /* initialize external interrupt */
  // plic_init();
  /* initialize fpioa module */
  // fpioa_init();

  /* initialize the lvgl lib */
  lv_init();
  lv_port_disp_init();
  lv_port_fs_init();

  /* setup app */
  setup();

  /* begining the lvgl tick */
  timer_init(TIMER_DEVICE_0);
  timer_set_interval(TIMER_DEVICE_0, TIMER_CHANNEL_0, 5 * 1000 * 1000);
  timer_irq_register(TIMER_DEVICE_0, TIMER_CHANNEL_0, 0, 1, lv_timer_callback,
                     NULL);
  timer_set_enable(TIMER_DEVICE_0, TIMER_CHANNEL_0, 1);

  /* lvgl main loop */
  while (1) {
    lv_task_handler();
    __wfi();
  }

  return 1;
}
