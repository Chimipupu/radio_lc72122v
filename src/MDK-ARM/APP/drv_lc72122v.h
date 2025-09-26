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

#ifndef DRV_LC72122V
#define DRV_LC72122V

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define AM_SELECT       0x00
#define FM_SELECT       0x10

// AMの設定したい周波数[KHz]の設定データ算出マクロ
#define GET_AM_FREQ_DATA(freq)      (((freq + 45) / 1.5) * 16)
// FMの設定したい周波数[MHz]の設定データ算出マクロ
#define GET_FM_FREQ_DATA(freq)      (((freq + 0.1) / 2) / 0.0125)

typedef struct {
    uint16_t freq;
    uint16_t freq_data;
} am_freq_data_t;

typedef struct {
    float freq;
    uint16_t freq_data;
} fm_freq_data_t;

void drv_lc72122v_am_fm_select(uint8_t select);
void drv_lc72122v_set_am_fm_freq_data(uint16_t freq);
void drv_lc72122v_set_am_fm_freq(uint16_t data);
void drv_lc72122v_am_fm_channel_change(void);

#endif // DRV_LC72122V