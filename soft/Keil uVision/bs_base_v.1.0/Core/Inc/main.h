/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.h
	* 
	* ������ "�����" (�������� ������� ������������ ������������)
	* ���� ����������� ������� ��� ���������� ���������������� ���
  *
	* ������������ ���� ��������� ��� ���������.
	* ���������������: stm32f103c8t6.
	*
	* ����� ��������:
	* - ���������� ������ (�� 20)
	* - ������ �������
	* - ��� �������
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

#include "stm32f1xx_ll_crc.h"
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
//���������� ���������������� ���������
#include "kombo_usart.h"			//���������� ����������������� �����
#include "kombo_spi.h"				//���������� ���������� SPI
#include "kombo_nrf24.h"			//���������� ����������� nrf24l01
#include "lcd.h"							//���������� �������LCD2004
#include "dfplayer.h"					//���������� ��������� ������
#include "st7735.h"						//���������� ������� ST7735
#include "fonts.h"						//���������� ������� ������� ST7735
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

#define NUM_OF_SLAVES 	20			//���������� ������� ���������

//#define DEBUG_MODE						//����� �������. ���������������� ��� ����������

//��������� ������ ����������
typedef struct Flags
{
	uint8_t		rst_push;					//���� ������� "�����"
  uint8_t		nxt_push;					//���� ������� "�����"
  uint8_t		set_push;					//���� ������� "���������"
	uint8_t		time_push;				//���� ������� "�����"
	uint8_t		f_timer;					//���� ������� �������
} Flags_t;

//��������� ������� ���������� ����������
typedef struct Variables
{
	uint32_t		seconds;					//������� ������
  uint32_t		temp_seconds;			//��������� �������� �������� ������
  uint32_t		millis;						//������� ����������
	uint32_t		temp_millis; 			//��������� �������� �������� ����������
	uint8_t 		slave_counter;		//������� ������
	uint8_t 		sound_switch;			//������������� ������
	uint8_t 		send_order_count;	//������� ������� ������� � ���������������� ����
} Variables_t;

//��������� ������ ������
typedef struct B_data
{
	uint8_t 	number;        		//�����
  uint8_t 	address;					//�����
	uint8_t 	pushed;						//���� �������
  uint32_t 	time;							//�����
	float		 	vcc;          		//���������� �������
	uint8_t 	led_status;				//��������� ����������
	uint8_t 	push_order;				//������� �������
  uint8_t 	cmd;							//�������
} B_data_t;

// �������� �������������� ����� ���� � ����� (4�) ���� �� float 
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
#define SUBVERSION	0

#define DEV_INFO "kombo_bs base"

#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR   *(volatile unsigned long *)0xE000EDFC

//���������
#define ADRESS_LIST		{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14}
#define CHARS_LIST		{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'}
#define NUM_LIST			{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}	
	
#define LCD_2004
//#define ST_7735
	
//������� ���������� �����������
#define LED_ON() LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_OFF() LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_TGL() LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)
	
#define RESET_CMD				0xFF		//������� ����������� ������
#define NOT_PUSHED 			0xFFFFFFFF	//��������� "������ �� ������"
#define	MAX_TIME 				4294967295	//������������ �������� �������
#define LED_STAT				0x01
#define LED_STAT_ON			0x01		//������� ��������� ����������
#define LED_STAT_OFF		0x00		//������� ���������� ����������
#define LED_STAT_TGL		0x02
	
#define USART_COMMAND		0x31
#define USART_RESET			0x31
#define USART_NEXT			0x32

#define ERR_OK					0x00
#define NRF_ERR					0xFF
	
//������ ���������
#define STRING_SIZE			64			//������ �����

#define SCAN_PERIOD 		1
	
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
