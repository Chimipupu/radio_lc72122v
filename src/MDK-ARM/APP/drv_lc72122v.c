/**
 * @file drv_lc72122v.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief LC72122Vドライバ
 * @version 0.1
 * @date 2025-09-26
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */

#include "drv_lc72122v.h"

static uint8_t s_tx_data_buf[4] = {0};
static uint8_t s_select_am_fm_flg = AM_SELECT;

static void tx_lc72122v_data(uint8_t *p_data);

static void tx_lc72122v_data(uint8_t *p_data)
{
    // TODO:LC72122Vのデータ送信
}

/**
 * @brief AM/FM切り替えAPI
 * 
 * @param select AM/FMの選択(0=AM, 1=FM)
 */
void drv_lc72122v_am_fm_select(uint8_t select)
{
    memset(&s_tx_data_buf[0], 0, sizeof(s_tx_data_buf));

    s_tx_data_buf[0] = 0x28;
    if(s_select_am_fm_flg == AM_SELECT) {
        s_tx_data_buf[3] = 0x23;
        s_select_am_fm_flg = AM_SELECT;
    } else {
        s_tx_data_buf[3] = 0x03;
        s_select_am_fm_flg = FM_SELECT;
    }

    tx_lc72122v_data(&s_tx_data_buf[0]);
}

/**
 * @brief AM/FM周波数設定API
 * 
 * @param data 周波数データ
 */
void drv_lc72122v_set_am_fm_freq(uint16_t data)
{
    memset(&s_tx_data_buf[0], 0, sizeof(s_tx_data_buf));

    s_tx_data_buf[0] = 0x28;
    s_tx_data_buf[1] = (uint8_t)(data & 0x00FF);
    s_tx_data_buf[2] = (uint8_t)((data & 0xFF00) >> 8);

    if(s_select_am_fm_flg == AM_SELECT) {
        s_tx_data_buf[3] = 0xC0;
    } else {
        s_tx_data_buf[3] = 0x33;
    }

    tx_lc72122v_data(&s_tx_data_buf[0]);
}