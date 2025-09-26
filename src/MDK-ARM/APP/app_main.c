/**
 * @file app_main.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief アプリメイン
 * @version 0.1
 * @date 2025-08-23
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */
#include "app_main.h"
#include "main.h"
#include "drv_lc72122v.h"

/**
 * @brief アプリメイン初期化
 * 
 */
void app_main_init(void)
{
    // FMに設定
    drv_lc72122v_am_fm_select(FM_SELECT);
}

/**
 * @brief アプリメイン
 * 
 */
void app_main(void)
{
    // ラジオChを変更
    drv_lc72122v_am_fm_channel_change();
}