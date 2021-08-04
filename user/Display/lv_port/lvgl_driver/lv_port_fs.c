/**
 * @file lv_port_fs_templ.c
 *
 */

/*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs.h"

#include <printf.h>
#include <stdio.h>

#include "ff.h"
#include "sdcard.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int fs_init(void);

static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf,
                           uint32_t btr, uint32_t *br);
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf,
                            uint32_t btw, uint32_t *bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos,
                           lv_fs_whence_t whence);
static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);

static void *fs_dir_open(lv_fs_drv_t *drv, const char *path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void) {
  /*----------------------------------------------------
   * Initialize your storage device and File System
   * -------------------------------------------------*/
  fs_init();

  /*---------------------------------------------------
   * Register the file system interface in LVGL
   *--------------------------------------------------*/

  /*Add a simple drive to open images*/
  static lv_fs_drv_t fs_drv;
  lv_fs_drv_init(&fs_drv);

  /*Set up fields...*/
  fs_drv.letter = 'p';
  fs_drv.open_cb = fs_open;
  fs_drv.close_cb = fs_close;
  fs_drv.read_cb = fs_read;
  fs_drv.write_cb = fs_write;
  fs_drv.seek_cb = fs_seek;
  fs_drv.tell_cb = fs_tell;

  fs_drv.dir_close_cb = fs_dir_close;
  fs_drv.dir_open_cb = fs_dir_open;
  fs_drv.dir_read_cb = fs_dir_read;

  lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#include <fpioa.h>
/*Initialize your Storage device and File system.*/
static int fs_init(void) {
  /*E.g. for FatFS initialize the SD card and FatFS itself*/
  fpioa_set_function(28, FUNC_SPI1_D0);
  fpioa_set_function(26, FUNC_SPI1_D1);
  fpioa_set_function(27, FUNC_SPI1_SCLK);
  fpioa_set_function(29, FUNC_SPI1_SS3);

  if (sd_init() != 0) {
    printf("SD card err\n");
    return -1;
  }

  static FATFS sdcard_fs;
  FRESULT status;
  status = f_mount(&sdcard_fs, _T("0:"), 1);
  if (status != FR_OK) {
    printf("FAT32 err\n");
    return -1;
  }
  /*You code here*/
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g.
 * S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD |
 * FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
  FRESULT f;
  FIL *file = lv_mem_alloc(sizeof(FIL));
  if (mode == LV_FS_MODE_WR) {
    /*Open a file for write*/
    f = f_open(file, path, FA_WRITE); /*Add your code here*/
  } else if (mode == LV_FS_MODE_RD) {
    /*Open a file for read*/
    f = f_open(file, path, FA_READ); /*Add your code here*/
  } else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) {
    /*Open a file for read and write*/
    f = f_open(file, path, FA_READ | FA_WRITE); /*Add your code here*/
  }

  return (void *)file;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with lv_ufs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;

  /*Add your code here*/
  res = FRESULT_2_lv_fs_res_t[f_close(file_p)];
  lv_mem_free(file_p);
  return res;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf,
                           uint32_t btr, uint32_t *br) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;

  /*Add your code here*/
  res = FRESULT_2_lv_fs_res_t[f_read(file_p, buf, btr, br)];
  return res;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btr       Bytes To Write
 * @param br        the number of real written bytes (Bytes Written). NULL if
 * unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf,
                            uint32_t btw, uint32_t *bw) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;

  /*Add your code here*/
  res = FRESULT_2_lv_fs_res_t[f_write(file_p, buf, btw, bw)];
  return res;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos,
                           lv_fs_whence_t whence) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;

  /*Add your code here*/
  res = FRESULT_2_lv_fs_res_t[f_lseek(file_p, pos)];
  return res;
}
/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;
  /*Add your code here*/
  pos_p = (uint32_t *)f_tell((FIL *)file_p);
  return pos_p ? LV_FS_RES_OK : LV_FS_RES_NOT_IMP;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void *fs_dir_open(lv_fs_drv_t *drv, const char *path) {
  FRESULT ret;
  DIR *rddir_p = NULL;
  /*Add your code here*/
  ret = f_opendir(rddir_p, path); /*Add your code here*/
  return rddir_p;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;
  FILINFO fno;
  /*Add your code here*/
  res = FRESULT_2_lv_fs_res_t[f_readdir(rddir_p, &fno)];
  fn = fno.fname;
  return res;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p) {
  lv_fs_res_t res = LV_FS_RES_NOT_IMP;

  /*Add your code here*/
  res = FRESULT_2_lv_fs_res_t[f_closedir(rddir_p)];
  return res;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
