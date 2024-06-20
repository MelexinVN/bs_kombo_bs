/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.h
	*
	* Проект "КомБО" (Открытые системы беспроводной коммуникации)
	* Игровая кнопка электронной системы для проведения интеллектуальных игр
	*
	* Заголовочный файл основного кода программы.
	* Микроконтроллер: stm32f030f4p6
	* Периферия: радиомодуль nrf24l01+, кнопка, светодиод.
	* 
	* Здесь задается:
	* - калибровка для измерения напряжения питания
	* - адрес кнопки
	*
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx_ll_adc.h"
#include "stm32f0xx_ll_crc.h"
#include "stm32f0xx_ll_iwdg.h"
#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kombo_spi.h"				//библиотека интерфейса SPI
#include "kombo_nrf24.h"			//библиотека радиомодуля nrf24l01
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
// Взаимное преобразование числа из массива 4 байт во float и наоборот
typedef union
{
	float float_val;
	uint8_t char_val[4];
} float_to_byte_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin LL_GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define IRQ_Pin LL_GPIO_PIN_2
#define IRQ_GPIO_Port GPIOA
#define IRQ_EXTI_IRQn EXTI2_3_IRQn
#define CE_Pin LL_GPIO_PIN_3
#define CE_GPIO_Port GPIOA
#define CSN_Pin LL_GPIO_PIN_4
#define CSN_GPIO_Port GPIOA
#define BUT_Pin LL_GPIO_PIN_1
#define BUT_GPIO_Port GPIOB
#define BUT_EXTI_IRQn EXTI0_1_IRQn
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */
#define STM32_LL

#define VERSION			1
#define SUBVERSION	0

#define VREFINT_CAL ((uint16_t*) VREFINT_CAL_ADDR)
#define CALIBR_VREF 3.316//2.994f//4.257f

#define ERR_OK					0x00				//отсутствие ошибок
#define NRF_ERR					0xFF				//ошибка радиомодуля

#define OUR_ADDR				0x14				//адрес кнопки (должен быть в списке адресов базы)

#define RESET_CMD				0xFF				//команда аппаратного сброса
#define NOT_PUSHED 			0xFFFFFFFF	//сообщение "кнопка не нажата"
#define LED_STAT				0x01				//команда задания состояния светодиода
#define LED_STAT_ON			0x01				//команда включения светодиода
#define LED_STAT_OFF		0x00				//команда отключения светодиода
#define LED_STAT_TGL		0x02				//команда запуска моргания светодиода

#define LED_TGL_MS			100					//полупериод моргания светодиода

//Макросы управления светодиодом
#define LED_OFF() LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_ON() LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_TGL() LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
