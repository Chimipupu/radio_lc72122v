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
static uint8_t s_am_ch = 0;
static uint8_t s_fm_ch = 0;
static uint16_t s_freq = 0;
static uint16_t s_freq_data = 0;

// (ROM) LC72122V用 AMラジオ周波数設定データテーブル
const am_freq_data_t g_am_freq_data_tbl[] = {
    { 558/*[kHz]*/,     /*設定値:*/ 6432}, // [AM] ラジオ関西
    { 666/*[kHz]*/,     /*設定値:*/ 7584}, // [AM] NHK第1
    { 828/*[kHz]*/,     /*設定値:*/ 9312}, // [AM] NHK第2
    {1008/*[kHz]*/,     /*設定値:*/11232}, // [AM] ABCラジオ
    {1179/*[kHz]*/,     /*設定値:*/13056}, // [AM] MBSラジオ
    {1314/*[kHz]*/,     /*設定値:*/11496}, // [AM] ラジオ大阪
};
const uint8_t g_am_freq_data_tbl_size = (sizeof(g_am_freq_data_tbl) / sizeof(g_am_freq_data_tbl[0]));

// (ROM) LC72122V用 FMラジオ周波数設定データテーブル
const fm_freq_data_t g_fm_freq_data_tbl[] = {
    {76.5/*[MHz]*/,     /*設定値:*/3064}, // [FM] FM COCOLO
    {80.2/*[MHz]*/,     /*設定値:*/3212}, // [FM] FM802
    {85.1/*[MHz]*/,     /*設定値:*/3408}, // [FM] FM大阪
    {88.1/*[MHz]*/,     /*設定値:*/3528}, // [FM] NHK-FM
    {90.6/*[MHz]*/,     /*設定値:*/3628}, // [FM] MBSラジオ
    {91.9/*[MHz]*/,     /*設定値:*/3680}, // [FM] ラジオ大阪
    {93.3/*[MHz]*/,     /*設定値:*/3736}, // [FM] ABCラジオ
};
const uint8_t g_fm_freq_data_tbl_size = (sizeof(g_fm_freq_data_tbl) / sizeof(g_fm_freq_data_tbl[0]));

static void tx_lc72122v_data(uint8_t *p_data);

static void tx_lc72122v_data(uint8_t *p_data)
{
    s_freq_data = p_data[1];
    s_freq_data |= (uint16_t)(p_data[2] << 8);
    if(s_select_am_fm_flg == AM_SELECT) {
        s_freq = GET_AM_FREQ(s_freq_data);
    } else {
        s_freq = GET_FM_FREQ(s_freq_data) * 10;
    }
    // TODO:LC72122Vのデータ送信
}

/**
 * @brief AM/FM切り替え関数
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
 * @brief AM/FM周波数設定関数
 * 
 * @param freq 周波数
 */
void drv_lc72122v_set_am_fm_freq_data(uint16_t freq)
{
    uint16_t freq_data;

    memset(&s_tx_data_buf[0], 0, sizeof(s_tx_data_buf));

    if(s_select_am_fm_flg == AM_SELECT) {
        freq_data = GET_AM_FREQ_DATA(freq);
        s_tx_data_buf[3] = 0xC0;
    } else {
        freq_data = GET_FM_FREQ_DATA(freq);
        s_tx_data_buf[3] = 0x33;
    }

    s_tx_data_buf[0] = 0x28;
    s_tx_data_buf[1] = (uint8_t)(freq_data & 0x00FF);
    s_tx_data_buf[2] = (uint8_t)((freq_data & 0xFF00) >> 8);

    tx_lc72122v_data(&s_tx_data_buf[0]);
}

/**
 * @brief AM/FM周波数設定関数
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

/**
 * @brief AM/FMでテーブルのCHを回していく関数
 * 
 */
void drv_lc72122v_am_fm_channel_change(void)
{
    if(s_select_am_fm_flg == AM_SELECT) {
        drv_lc72122v_set_am_fm_freq(g_am_freq_data_tbl[s_am_ch].freq_data);
        s_am_ch = (s_am_ch + 1) % g_am_freq_data_tbl_size;
    } else {
        drv_lc72122v_set_am_fm_freq(g_fm_freq_data_tbl[s_fm_ch].freq_data);
        s_fm_ch = (s_fm_ch + 1) % g_fm_freq_data_tbl_size;
    }
}

// (DEBUG)デバッグ関連
#ifdef DEBUG_DRV_LC72122V
void dbg_drv_lc72122v_info_print(void)
{
    if(s_select_am_fm_flg == AM_SELECT) {
        printf("[DEBUG] AM Radio Mode\n");
        printf("[DEBUG] AM Freq = %d.%d [KHz]\n", s_freq / 10, s_freq % 10);
    } else {
        printf("[DEBUG] FM Radio Mode\n");
        printf("[DEBUG] FM Freq = %d.%d [MHz]\n", s_freq / 10, s_freq % 10);
    }
}
#endif // DEBUG_DRV_LC72122V