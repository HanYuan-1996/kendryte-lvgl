#include <ff.h>
#include <fpioa.h>
#include <plic.h>
#include <sleep.h>
#include <stdio.h>
#include <sysctl.h>
#include <time.h>

#include "App.h"
#include "bsp.h"
#include "lv_port_disp.h"
#include "lv_port_fs.h"
#include "lv_port_indev.h"
#include "lvgl/lvgl.h"
#include "printf.h"
#include "sdcard.h"
#include "timer.h"
#define STATUS_BAR_HEIGHT 22
#define COLOR_GREY lv_color_hex(0x333333)
#define COLOR_ORANGE lv_color_hex(0xff931e)

void setup() {
  // static lv_style_t style_bg;
  // static lv_style_t style_indic;

  // lv_style_init(&style_bg);
  // lv_style_set_border_color(&style_bg, lv_palette_main(LV_PALETTE_BLUE));
  // lv_style_set_border_width(&style_bg, 2);
  // lv_style_set_pad_all(&style_bg, 6); /*To make the indicator smaller*/
  // lv_style_set_radius(&style_bg, 6);
  // lv_style_set_anim_time(&style_bg, 1000);

  // lv_style_init(&style_indic);
  // lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
  // lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
  // lv_style_set_radius(&style_indic, 3);

  // lv_obj_t *bar = lv_bar_create(lv_scr_act());
  // lv_obj_remove_style_all(bar); /*To have a clean start*/
  // lv_obj_add_style(bar, &style_bg, 0);
  // lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);

  // lv_obj_set_size(bar, 200, 20);
  // lv_obj_center(bar);
  // lv_bar_set_value(bar, 100, LV_ANIM_ON);
  App_Init();
}

/* lvgl tick, 5ms */
int timer_callback(void *ctx) {
  lv_tick_inc(5);
  return 1;
}

corelock_t lock;

int main() {
  /* set cpu frequence to 400MHz */
  sysctl_cpu_set_freq(400 * 1000 * 1000);
  /* initialize external interrupt */
  plic_init();
  /* initialize fpioa module */
  fpioa_init();

  /* initialize lvgl lib */
  lv_init();
  lv_port_disp_init();
  lv_port_fs_init();

  /* setup app */
  corelock_lock(&lock);
  setup();
  corelock_unlock(&lock);
  /* enable irq and begining the lvgl tick */
  timer_init(TIMER_DEVICE_0);
  timer_set_interval(TIMER_DEVICE_0, TIMER_CHANNEL_0, 5 * 1000 * 1000);
  timer_irq_register(TIMER_DEVICE_0, TIMER_CHANNEL_0, 0, 1, timer_callback,
                     NULL);
  timer_set_enable(TIMER_DEVICE_0, TIMER_CHANNEL_0, 1);
  sysctl_enable_irq();

  /* lvgl main loop */
  while (1) {
    lv_task_handler();
    usleep(5 * 1000);
  }

  return 1;
}

void func(const char *__restrict __format, ...) {
  char printf_buff[256];
  va_list args;
  va_start(args, __format);
  int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
  va_end(args);
}

static int sdcard_init(void) {
  uint8_t status;

  printf("/******************sdcard test*****************/\n");
  status = sd_init();
  printf("sd init %d\n", status);
  if (status != 0) {
    return status;
  }

  printf("card info status %d\n", status);
  printf("CardCapacity:%ld\n", cardinfo.CardCapacity);
  printf("CardBlockSize:%d\n", cardinfo.CardBlockSize);
  return 0;
}

static int fs_init(void) {
  static FATFS sdcard_fs;
  FRESULT status;
  DIR dj;
  FILINFO fno;

  printf("/********************fs test*******************/\n");
  status = f_mount(&sdcard_fs, _T("0:"), 1);
  printf("mount sdcard:%d\n", status);
  if (status != FR_OK) return status;

  printf("printf filename\n");
  status = f_findfirst(&dj, &fno, _T("0:"), _T("*"));
  while (status == FR_OK && fno.fname[0]) {
    if (fno.fattrib & AM_DIR)
      printf("dir:%s\n", fno.fname);
    else
      printf("file:%s\n", fno.fname);
    status = f_findnext(&dj, &fno);
  }
  f_closedir(&dj);
  return 0;
}

int sd_test() {
  if (sdcard_init()) {
    printf("SD card err\n");
    return -1;
  }
  if (fs_init()) {
    printf("FAT32 err\n");
    return -1;
  }
  FIL file;
  FRESULT ret = FR_OK;

  char dir[] = "cannan";
  ret = f_mkdir(dir);
  if (ret == FR_OK)
    printf("Mkdir %s ok\n", dir);
  else
    printf("Mkdir %s err [%d]\n", dir, ret);

  char path[] = "cannan/test.txt";
  printf("/*******************sd read write test*******************/\n");
  uint32_t v_ret_len = 0;

  FILINFO v_fileinfo;
  if ((ret = f_stat(path, &v_fileinfo)) == FR_OK) {
    printf("%s length is %lld\n", path, v_fileinfo.fsize);
  } else {
    printf("%s fstat err [%d]\n", path, ret);
  }

  if ((ret = f_open(&file, path, FA_READ)) == FR_OK) {
    char v_buf[80] = {0};
    ret = f_read(&file, (void *)v_buf, 64, &v_ret_len);
    if (ret != FR_OK) {
      printf("Read %s err[%d]\n", path, ret);
    } else {
      printf("Read :> %s %d bytes lenth\n", v_buf, v_ret_len);
    }
    f_close(&file);
  }

  if ((ret = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK) {
    printf("open file %s err[%d]\n", path, ret);
    return ret;
  } else {
    printf("Open %s ok\n", path);
  }
  uint8_t hello[1024];
  uint32_t i;
  for (i = 0; i < 1024; i++) {
    hello[i] = 'A';
  }

  ret = f_write(&file, hello, sizeof(hello), &v_ret_len);
  if (ret != FR_OK) {
    printf("Write %s err[%d]\n", path, ret);
  } else {
    printf("Write %d bytes to %s ok\n", v_ret_len, path);
  }
  f_close(&file);
  return false;
}
