/**
 * @file drv_lc72122v.h
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief LC72122Vドライバ
 * @version 0.1
 * @date 2025-09-26
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */

#ifndef DRV_LC72122V_H
#define DRV_LC72122V_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define AM_FREQ_MIN     500     // AM周波数 Min(500kHz)
#define AM_FREQ_MAX     1600    // AM周波数 Max(1600KHz)

#define FM_FREQ_MIN     76.0    // FM周波数 Min(76.0MHz)
#define FM_FREQ_MAX     108.0   // FM周波数 Max(108.0MHz)

#define AM_SELECT       0x00
#define FM_SELECT       0x10

#define AM_TOKYO        0x01
#define AM_OSAKA        0x02
#define AM_NARA         0x03

#define FM_TOKYO        0x01
#define FM_OSAKA        0x02
#define FM_NARA         0x03

// AMの設定したい周波数[KHz]の設定データ算出マクロ
#define GET_AM_FREQ(freq_data)      (((freq_data) * 3 / 32) - 45)
#define GET_AM_FREQ_DATA(freq)      (((freq + 45.0f) / 1.5f) * 16.0f)

// FMの設定したい周波数[MHz]の設定データ算出マクロ
#define GET_FM_FREQ_X10(freq_data)  (((uint16_t)(((freq_data) / 4) - 1)))
#define GET_FM_FREQ_DATA(freq)      (((freq + 0.1f) / 2.0f) * 80.0f)

typedef struct {
    uint16_t freq;
    uint16_t freq_data;
}am_freq_data_t;

typedef struct {
    float freq;
    uint16_t freq_data;
}fm_freq_data_t;

void drv_lc72122v_am_fm_select(uint8_t select);
void drv_lc72122v_set_am_fm_freq(uint16_t data);
// void drv_lc72122v_set_am_fm_freq_data(uint16_t freq);
void drv_lc72122v_am_fm_channel_change(void);

// (DEBUG)デバッグ関連
#define DEBUG_DRV_LC72122V

#ifdef DEBUG_DRV_LC72122V
#include <stdio.h>
void dbg_drv_lc72122v_info_print(void);
#endif // DEBUG_DRV_LC72122V

#endif // DRV_LC72122V_H