/**
 * @file main.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief  PY32F002A メイン
 * @version 0.1
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "py32f0xx.h"

#include "app_main.h"

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t *p_tx_buf = NULL;
volatile uint16_t g_tx_size = 0;
volatile uint16_t g_tx_cnt = 0;

uint8_t *p_rx_buf = NULL;
volatile uint16_t g_rx_size = 0;
volatile uint16_t g_rx_cnt = 0;

/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigUsart(USART_TypeDef *USARTx);

LL_UTILS_ClkInitTypeDef UTILS_ClkInitStruct = {LL_RCC_SYSCLK_DIV_1, LL_RCC_APB1_DIV_1};


// printf()用
int __io_putchar(int ch)
{
    APP_UsartTransmit_IT(USART1, (uint8_t *)&ch, 1);
    return ch;
}
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        __io_putchar(ptr[i]);
    }
    return len;
}
int fputc(int ch, FILE *f)
{
    return __io_putchar(ch);
}

/**
  * @brief  System clock configuration
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void)
{
    volatile ErrorStatus pll_init_status;

#if 0
    uint32_t fact_hsi_trim_val;
    uint32_t reg;

    // [工場出荷時のHSIのトリム値を読み出し]
    fact_hsi_trim_val = REG_READ_DWORD(0x1FFF0FA0, 0) & 0x00001FFF;
    reg = READ_REG(RCC->ICSCR);
    reg = reg & ~0x00001FFF;
    reg = reg | fact_hsi_trim_val;
    WRITE_REG(RCC->ICSCR, reg);
#endif

    LL_RCC_HSI_Enable();

#if (HSI_FREQ == 24000000) || (HSI_FREQ == 48000000)
    // [内部RCクロックのHSIを8MHzから24MHzに変更]
    LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_24MHz);
#endif

    while(LL_RCC_HSI_IsReady() != 1)
    {
        NOP();
    }

    // [HSI@24MHzをPLLで48MHzに逓倍]
    // ※PLLはx2の2逓倍固定
    pll_init_status = LL_PLL_ConfigSystemClock_HSI(&UTILS_ClkInitStruct);
    LL_Init1msTick(HSI_FREQ);
    LL_SetSystemCoreClock(HSI_FREQ);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);

    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
    {
        NOP();
    }

    /* Set APB1 prescaler*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_Init1msTick(HSI_FREQ);

    /* Update system clock global variable SystemCoreClock (can also be updated by calling SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(HSI_FREQ);

    // 内蔵RCレゾネータのLSI(32.768KHz)を有効
    LL_RCC_LSI_Enable();
    while(LL_RCC_LSI_IsReady() == 0)
    {
        NOP();
    }
}

/**
  * @brief  USART configuration.
  * @param  USARTx：USART module, can be USART1
  * @retval None
  */
static void APP_ConfigUsart(USART_TypeDef *USARTx)
{
    /*Enable clock, initialize pins, enable NVIC interrupt*/

    /*Enable GPIOA clock*/
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    /*Enable USART1 clock*/
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

    /*GPIOA configuration*/
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Select pin 2*/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    /* Select alternate function mode */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    /* Select output speed */
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    /* Select push-pull output mode */
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    /*Pull-up*/
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    /* Select USART1 function */
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
    /* Initialize GPIOA */
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);

    /*Select pin 3*/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    /* Select USART1 function */
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
    /* Initialize GPIOA */
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);

    /*Set USART1 interrupt priority*/
    NVIC_SetPriority(USART1_IRQn,0);
    /*Enable USART1 interrupt*/
    NVIC_EnableIRQ(USART1_IRQn);

    /*Configure USART*/
    LL_USART_InitTypeDef USART_InitStruct = {0};
    /*Set baud rate*/
    USART_InitStruct.BaudRate = 115200;
    /*Set data width*/
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    /* Set stop bits */
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    /* Set parity */
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    /* Initialize USART */
    LL_USART_Init(USARTx, &USART_InitStruct);

    /* Configure as full duplex asynchronous mode */
    LL_USART_ConfigAsyncMode(USARTx);

    /* Enable UART module */
    LL_USART_Enable(USARTx);
    /* Configure auto baud rate detection */
    LL_USART_SetAutoBaudRateMode(USARTx, LL_USART_AUTOBAUDRATE_ONFALLINGEDGE);
    LL_USART_EnableAutoBaudRate(USARTx);
}

/**
  * @brief  USART transmit function.
  * @param  USARTx：USART module, can be USART1
  * @param  pData：transmit buffer
  * @param  Size：Size of the transmit buffer
  * @retval None
  */
void APP_UsartTransmit_IT(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
    volatile uint8_t i;

    p_tx_buf = pData;
    g_tx_size = Size;
    g_tx_cnt = Size;

    LL_USART_EnableIT_TXE(USARTx);

    // [バッファの衝突回避の待ち処理]
    // ※48MHz = 20.83ns, 待ち時間 = 20.83ns * 96 ≒ 2us
    for(i = 0; i < 96; i++)
    {
        NOP();
    }
}

/**
  * @brief  USART receive function.
  * @param  USARTx：USART module, can be USART1
  * @param  pData：transmit buffer
  * @param  Size：Size of the transmit buffer
  * @retval None
  */
void APP_UsartReceive_IT(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
    p_rx_buf = pData;
    g_rx_size = Size;
    g_rx_cnt = Size;

    /*Enable parity error interrupt*/
    LL_USART_EnableIT_PE(USARTx);

    /*Enable error interrupt*/
    LL_USART_EnableIT_ERROR(USARTx);

    /*Enable receive data register not empty interrupt*/
    LL_USART_EnableIT_RXNE(USARTx);
}

/**
  * @brief  USART interrupt callback function
  * @param  USARTx：USART module, can be USART1
  * @retval None
  */
void APP_UsartIRQCallback(USART_TypeDef *USARTx)
{
    /*Receive data register not empty*/
    uint32_t errorflags = (LL_USART_IsActiveFlag_PE(USARTx) | LL_USART_IsActiveFlag_FE(USARTx) |\
                        LL_USART_IsActiveFlag_ORE(USARTx) | LL_USART_IsActiveFlag_NE(USARTx));
    if (errorflags == RESET)
    {
        if ((LL_USART_IsActiveFlag_RXNE(USARTx) != RESET) && (LL_USART_IsEnabledIT_RXNE(USARTx) != RESET))
        {
            *p_rx_buf = LL_USART_ReceiveData8(USARTx);
            p_rx_buf++;

            if (--g_rx_cnt == 0U)
            {
                LL_USART_DisableIT_RXNE(USARTx);
                LL_USART_DisableIT_PE(USARTx);
                LL_USART_DisableIT_ERROR(USARTx);
            }
            return;
        }
    }

    /*Receive error occurred*/
    if (errorflags != RESET)
    {
        /* Used for auto baud rate detection */
        if ((LL_USART_IsActiveFlag_RXNE(USARTx) != RESET) && (LL_USART_IsEnabledIT_RXNE(USARTx) != RESET))
        {
            *p_rx_buf = LL_USART_ReceiveData8(USARTx);
            p_rx_buf++;
            if (--g_rx_cnt == 0U)
            {
                LL_USART_DisableIT_RXNE(USARTx);
                LL_USART_DisableIT_PE(USARTx);
                LL_USART_DisableIT_ERROR(USARTx);
            }
            return;
        }
    }
    /*Transmit data register empty*/
    if ((LL_USART_IsActiveFlag_TXE(USARTx) != RESET) && (LL_USART_IsEnabledIT_TXE(USARTx) != RESET))
    {
        LL_USART_TransmitData8(USARTx, *p_tx_buf);
        p_tx_buf++;
        if (--g_tx_cnt == 0U)
        {
            LL_USART_DisableIT_TXE(USARTx);
            LL_USART_EnableIT_TC(USARTx);
        }
        return;
    }

    /*Transmission complete*/
    if ((LL_USART_IsActiveFlag_TC(USARTx) != RESET) && (LL_USART_IsEnabledIT_TC(USARTx) != RESET))
    {
        LL_USART_DisableIT_TC(USARTx);
        return;
    }
}

/**
  * @brief  メイン関数
  * @param  None
  * @retval int
  */
int main(void)
{
    // 標準ライブラリ関連初期化
    setbuf(stdout, NULL);

    // クロック初期化
    APP_SystemClockConfig();

    // アプリ初期化
    app_main_init();

    while (1)
    {
        // アプリメイン
        app_main();
        LL_mDelay(5000);
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
    /* infinite loop */
    while (1)
    {
        NOP();
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
        for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* infinite loop */
    while (1)
    {
        NOP();
    }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
