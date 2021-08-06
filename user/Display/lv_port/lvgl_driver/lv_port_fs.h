/**
 * @file lv_port_fs_templ.h
 *
 */

/*Copy this file as "lv_port_fs.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_FS_TEMPL_H
#define LV_PORT_FS_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
/*********************
 *      DEFINES
 *********************/

/*File system interface*/
#define LV_USE_FS_IF	1
#if LV_USE_FS_IF
#  define LV_FS_IF_FATFS    '/'
#  define LV_FS_IF_PC       '\0'
#  define LV_FS_IF_POSIX    '\0'
#endif  /*LV_USE_FS_IF*/

const lv_fs_res_t FRESULT_2_lv_fs_res_t[] = {
    LV_FS_RES_OK,      /* (0) Succeeded */
    LV_FS_RES_HW_ERR,  /* (1) A hard error occurred in the low level disk I/O
                          layer */
    LV_FS_RES_UNKNOWN, /* (2) Assertion failed */
    LV_FS_RES_NOT_EX,  /* (3) The physical drive cannot work */
    LV_FS_RES_NOT_EX,  /* (4) Could not find the file */
    LV_FS_RES_NOT_EX,  /* (5) Could not find the path */
    LV_FS_RES_NOT_EX,  /* (6) The path name format is invalid */
    LV_FS_RES_DENIED, /* (7) Access denied due to prohibited access or directory
                         full */
    LV_FS_RES_DENIED, /* (8) Access denied due to prohibited access */
    LV_FS_RES_INV_PARAM, /* (9) The file/directory object is invalid */
    LV_FS_RES_DENIED,    /* (10) The physical drive is write protected */
    LV_FS_RES_INV_PARAM, /* (11) The logical drive number is invalid */
    LV_FS_RES_INV_PARAM, /* (12) The volume has no work area */
    LV_FS_RES_INV_PARAM, /* (13) There is no valid FAT volume */
    LV_FS_RES_UNKNOWN,   /* (14) The f_mkfs() aborted due to any problem */
    LV_FS_RES_TOUT,   /* (15) Could not get a grant to access the volume within
                         defined period */
    LV_FS_RES_LOCKED, /* (16) The operation is rejected according to the file
                         sharing policy */
    LV_FS_RES_OUT_OF_MEM, /* (17) LFN working buffer could not be allocated */
    LV_FS_RES_NOT_IMP,    /* (18) Number of open files > _FS_LOCK */
    LV_FS_RES_INV_PARAM   /* (19) Given parameter is invalid */
};
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/
void lv_port_fs_init(void);
void lv_fs_if_fatfs_init(void);
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_FS_TEMPL_H*/

#endif /*Disable/Enable content*/
