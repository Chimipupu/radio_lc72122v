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

void drv_lc72122v_am_fm_select(uint8_t select);
void drv_lc72122v_set_am_fm_freq(uint16_t data);

#endif // DRV_LC72122V