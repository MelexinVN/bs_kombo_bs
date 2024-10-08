/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.h
	* 
	* Проект "КомБО" (Открытые системы беспроводной коммуникации)
	* База электронной системы для проведения интеллектуальных игр
  *
	* Заголовочный файл основного код программы.
	* Микроконтроллер: stm32f103c8t6.
	*
	* Здесь задается:
	* - количество кнопок (до 20)
	* - список адресов
	* - тип дисплея
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
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_iwdg.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//Добавление пользовательских библиотек
#include "kombo_usart.h"			//библиотека последовательного порта
#include "kombo_spi.h"				//библиотека интерфейса SPI
#include "kombo_nrf24.h"			//библиотека радиомодуля nrf24l01
#include "lcd.h"							//библиотека дисплеяLCD2004
#include "dfplayer.h"					//библиотека звукового модуля
#include "st7735.h"						//библиотека дисплея ST7735
#include "fonts.h"						//библиотека шрифтов дисплея ST7735
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

#define NUM_OF_SLAVES 	20			//количество ведомых устройств

//#define DEBUG_MODE						//режим отладки. Закомментировать для отключения

//Структура флагов устройства
typedef struct Flags
{
	uint8_t		rst_push;					//флаг команды "сброс"
	uint8_t		nxt_push;					//флаг команды "далее"
	uint8_t		set_push;					//флаг команды "установка"
	uint8_t		time_push;				//флаг команды "время"
	uint8_t		f_timer;					//флаг запуска таймера
} Flags_t;

//Структура рабочих переменных устройства
typedef struct Variables
{
	uint32_t		seconds;					//счетчик секунд
	uint32_t		temp_seconds;			//временное значение счетчика секунд
	uint32_t		millis;						//счетчик милисекунд
	uint32_t		temp_millis; 			//временное значение счетчика милисекунд
	uint8_t 		slave_counter;		//счетчик кнопок
	uint8_t 		sound_switch;			//переключатель звуков
	uint8_t 		send_order_count;	//счетчик посылок очереди в последовательный порт
	uint16_t 		bounce_count;	//счетчик для борьбы с дребезгом кнопки
} Variables_t;

//Структура данных кнопки
typedef struct B_data
{
	uint8_t 	number;        		//номер
	uint8_t 	address;					//адрес
	uint8_t 	pushed;						//флаг нажатия
	uint32_t 	time;							//время
	float		 	vcc;          		//напряжение питания
	uint8_t 	led_status;				//состояние светодиода
	uint8_t 	push_order;				//очередь нажатия
	uint8_t 	cmd;							//команда
} B_data_t;

// Взаимное преобразование числа либо в целое (4б) либо во float 
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
void usart_tx(uint8_t* dt, uint16_t sz);
void reset_button_pushed(void);
void push_order_shift(void);

void delay_us(uint32_t us);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin LL_GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define IRQ_Pin LL_GPIO_PIN_2
#define IRQ_GPIO_Port GPIOA
#define IRQ_EXTI_IRQn EXTI2_IRQn
#define CE_Pin LL_GPIO_PIN_3
#define CE_GPIO_Port GPIOA
#define CSN_Pin LL_GPIO_PIN_4
#define CSN_GPIO_Port GPIOA
#define TIME_Pin LL_GPIO_PIN_15
#define TIME_GPIO_Port GPIOA
#define TIME_EXTI_IRQn EXTI15_10_IRQn
#define NEXT_Pin LL_GPIO_PIN_3
#define NEXT_GPIO_Port GPIOB
#define NEXT_EXTI_IRQn EXTI3_IRQn
#define RST_Pin LL_GPIO_PIN_4
#define RST_GPIO_Port GPIOB
#define RST_EXTI_IRQn EXTI4_IRQn
#define SET_Pin LL_GPIO_PIN_5
#define SET_GPIO_Port GPIOB
#define SET_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */
#define STM32_LL

#define VERSION			1
#define SUBVERSION		04

#define DEV_INFO "kombo_bs base"

#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR   *(volatile unsigned long *)0xE000EDFC

//Настройки
#if NUM_OF_SLAVES == 20

#define ADRESS_LIST		{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14}
#define CHARS_LIST		{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'}
#define NUM_LIST		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}
	
#elif NUM_OF_SLAVES == 6

#define ADRESS_LIST		{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}
#define CHARS_LIST		{'0', '1', '2', '3', '4', '5'}
#define NUM_LIST		{0, 1, 2, 3, 4, 5}

#endif

#define LCD_2004
//#define ST_7735
	
//Макросы управления светодиодом
#define LED_ON() LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_OFF() LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_TGL() LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)
	
#define RESET_CMD				0xFF		//команда аппаратного сброса
#define NOT_PUSHED 			0xFFFFFFFF	//сообщение "кнопка не нажата"
#define	MAX_TIME 				4294967295	//максимальное значение времени
#define LED_STAT				0x01
#define LED_STAT_ON			0x01		//команда включения светодиода
#define LED_STAT_OFF		0x00		//команда отключения светодиода
#define LED_STAT_TGL		0x02
	
#define USART_COMMAND		0x31
#define USART_RESET			0x31
#define USART_NEXT			0x32

#define ERR_OK					0x00
#define NRF_ERR					0xFF
	
//Прочие настройки
#define STRING_SIZE			64			//размер строк

#if NUM_OF_SLAVES == 20

#define SCAN_PERIOD 		1

#elif NUM_OF_SLAVES == 6

#define SCAN_PERIOD 		2

#endif

#define BOUNCE_OFFSET		100
	
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
