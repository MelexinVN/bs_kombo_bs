/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.c
	*
	* ������ "�����" (�������� ������� ������������ ������������)
	* ������� ������ ����������� ������� ��� ���������� ���������������� ���
	*	
	* �������� ��� ���������.
	* ���������������: stm32f030f4p6
	* ���������: ����������� nrf24l01+, ������, ���������.
	* 
	* ������� �������� ������:
	* ����� ��������� �������, ��������� ��������� ������������ �������� 
	* ���������� � ������������, ��������������� ������������ (���� �������� ��� 
	* ��������� �����������, 1 - ��� �������������), � ����� ���������� ���������� 
	* ������� � ����� �������� ������ ������ �������. 
	* ��� ������������� ����������� ������ �������.
	* � �������� ������ ��������� ����������� ������� ������� �� ����, ������������ 
	* ���������� ���������� �������.
	* ��� ������������ ���������� �� ������� ������ ����������� ����� �������.
	* ��� ��������� ������� �� ���� � ����������� �� �����������:
	* - �������� � ����� ������� ������, ���� �������, � ����� �������� ����������� 
	* ���������� �������
	* - ��������� ������� ���������� - ������, �������, ���� �������
	* - ����� ��������� ����������
	*
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//�����
uint8_t f_push_button = 0;
uint8_t f_led_toggle 	= 0;

//���������� ��������� �������
uint8_t	 ms_counter 	= 0;
uint32_t millis 			= 0;
uint32_t temp_millis 	= 0;
uint32_t time_ms 			= 0;

//���������� ��������� ���������� �������
uint16_t adc_data 		= 0;
float vcc_data;

//������� ���������� ��� ������ � ������������
extern uint8_t tx_buf[TX_PLOAD_WIDTH];	//����� ��������
extern uint8_t rx_buf[TX_PLOAD_WIDTH];	//����� ������
extern volatile uint8_t f_rx, f_tx;			//����� ������ � ��������

//��������� ��� �������������� ������
float_to_byte_t float_to_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//��������� �������� �����������
void blink_led(uint8_t blink_counter)
{
	while (blink_counter--)			
	{
		LED_ON();					
		LL_mDelay(10);				
		LED_OFF();					
		if (blink_counter >= 1) LL_mDelay(50);
	}
}

//������� ������� ����������� �����
uint16_t crc16_calc(volatile uint8_t buf[], uint8_t buf_size) 
{
	uint16_t crc = 0;			// ������������ ��������
	uint16_t D;						// ������ �������� ������� ��� ��������� 
	int16_t T;						// ������� ������� ��� ���������, ����������� �� ����
	uint16_t P = 0x1021;	// ������� - �����

	for (uint8_t i = 0; i < buf_size; i++)
	{
		D = crc >> 8; 			// ����� crc �� 8 ��� ������
		D = D ^ buf[i];			// ���������� ������� �������� ������� ��� ���������  

		T = D;							// ���������� �������� �������
		T = T << 8;        	// ����� �������� ������� �� 8 ��� �����

		for (uint8_t j = 0; j < 8; j++)    // ���������� 8 ���
		{
			if (T < 0)				// ���� ������� ��� = 1
			{
				T = T << 1;				// ����� �������� ������� �� 1 ��� �����
				T = T ^ P;				// ��������� ��������
			}
			else
			{
				T = T << 1;				// ����� �������� ������� �� 1 ��� �����
			}
		}
		crc = crc << 8;				// ����� � �� 8 ��� �����
		crc = crc ^ T;				// ���������� ��� ����� � � ��������� �������
	}

	return crc;							// ������� �������� crc
}

//������������ ������������ �����������
uint8_t nrf_test(void)
{
	uint8_t buf_0[TX_ADR_WIDTH] = {0};				
	uint8_t buf_1[TX_ADR_WIDTH] = {0};
	uint8_t test_addr_0[TX_ADR_WIDTH] = UNIQUE_ADDRESS_0;
	uint8_t test_addr_1[TX_ADR_WIDTH] = UNIQUE_ADDRESS_1;
	
	uint8_t err = ERR_OK;								
	
	//������ �������� ���������
	
	#ifdef MASTER
	nrf24_read_buf(RX_ADDR_P1,buf_0,3);	
	nrf24_read_buf(TX_ADDR,buf_1,3);
	#endif
	
	#ifdef SLAVE
	nrf24_read_buf(RX_ADDR_P1,buf_1,3);	
	nrf24_read_buf(TX_ADDR,buf_0,3);
	#endif

	//��������� �������� ��������� ���������� ��� ������������� � ��������� 
	for (uint8_t i = 0; i < TX_ADR_WIDTH; i++)
	{
		if (test_addr_0[i] != buf_0[i]) err = NRF_ERR;
		if (test_addr_1[i] != buf_1[i]) err = NRF_ERR;
	}
	
	return err;
}

//��������� ������������ ����������� � ������������
void test_and_info(uint8_t err)
{
	err = nrf_test();		
	
	if (err == ERR_OK) 
	{
		blink_led(5);			
	}
	else if (err == NRF_ERR) 
	{
		blink_led(1);			
	}
}

//��������� ������ �����������
void nrf24l01_receive(void)
{
	if(f_rx)	//���� ���� ������ ������
	{
		//��������� ����������� �����
		uint16_t temp_crc = crc16_calc(rx_buf, TX_PLOAD_WIDTH - 2);
		uint16_t input_crc = (uint16_t)rx_buf[TX_PLOAD_WIDTH - 1] + (uint16_t)(rx_buf[TX_PLOAD_WIDTH - 2] << 8);
		
		if (temp_crc == input_crc)		//���� ����������� ����� ��������
		{
			if (rx_buf[0] == OUR_ADDR)	//���� ����� ���
			{
				if (f_push_button)				//���� ������� ����
				{
					tx_buf[0] = OUR_ADDR;		//���������� � ������ ���� �����
					
					(*(uint32_t*)&tx_buf[1]) = time_ms;	//� ��������� 4 ����� ����� �������� �������
					
					float_to_byte.float_val = vcc_data;	//� ��������� 4 ����� - ��������������� �������� ���������� �������
					tx_buf[5] = float_to_byte.char_val[0];
					tx_buf[6] = float_to_byte.char_val[1];
					tx_buf[7] = float_to_byte.char_val[2];
					tx_buf[8] = float_to_byte.char_val[3];
					//������������ ����������� �����, ���������� � ��������� 4 �����
					temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
					tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
					tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
					
					nrf24_send(tx_buf);		//���������� ������� � ����
				}
				else										//���� ������� �� ����
				{
					tx_buf[0] = OUR_ADDR;	//���������� � ������ ���� �����
					(*(uint32_t*)&tx_buf[1]) = NOT_PUSHED;	//� ��������� 4 - ������� ��������� ������
										
					float_to_byte.float_val = vcc_data;			//� ��������� 4 ����� - ��������������� �������� ���������� �������
					tx_buf[5] = float_to_byte.char_val[0];
					tx_buf[6] = float_to_byte.char_val[1];
					tx_buf[7] = float_to_byte.char_val[2];
					tx_buf[8] = float_to_byte.char_val[3];
					//������������ ����������� �����, ���������� � ��������� 4 �����
					temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
					tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
					tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
					
					nrf24_send(tx_buf);		//���������� ������� � ����
				}
				
				if (rx_buf[1] == LED_STAT)	//���� ����� ���� ������� ���������� ��������� ����������
				{
					if (rx_buf[2] == LED_STAT_ON)	//���� ������ ���������
					{
						LED_ON();										//��������
						f_led_toggle = 0;						//�������� ���� ��������
					}
					else if (rx_buf[2] == LED_STAT_OFF)	//���� �������� ���������
					{
						LED_OFF();									//�����
						f_led_toggle = 0;						//�������� ���� ��������
					}
					else if (rx_buf[2] == LED_STAT_TGL)	//���� ����� ��������
					{
						f_led_toggle = 1;						//��������� ���� ��������
					}
				}
			}
			else if (rx_buf[0] == RESET_CMD)	//���� ������� ������ ������
			{
				blink_led(1);										//������� ����������� 1 ���
				//���������� ��������� ������ - �������� ��������, �������� �����
				time_ms = 0;
				millis = 0;
				f_push_button = 0;
				f_led_toggle = 0;
			}
		}
		f_rx = 0;					//�������� ���� ������
	}
}

//��������� ������������� ���
void adc_init(void)
{
	uint32_t wait_loop_index = 0;	//���������� �������� ���������� ���
	
	LL_ADC_SetCommonPathInternalCh(ADC1_COMMON,LL_ADC_PATH_INTERNAL_VREFINT);
	
	//---��������������� ���--------------------------------------------------------
	LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B); 
  LL_ADC_StartCalibration(ADC1);											//������ ���������� ���
  while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {}		//���� ��������� ���������� ���
	LL_ADC_Enable(ADC1);																//��������� ���
	//������������  ������� �������� ��������� ���
	wait_loop_index = ((LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32) >> 1		); 
	while(wait_loop_index != 0)													//��������
  {
    wait_loop_index--;
  }
}

//��������� ������ ���������� �������
float reading_vcc(void)
{
	LL_ADC_REG_StartConversion(ADC1);											//������ �������������� ���
	while (!LL_ADC_IsActiveFlag_EOS(ADC1))  {}						//�������� ����� ��������������
	LL_ADC_ClearFlag_EOS(ADC1);														//����� ����� ���
	adc_data = LL_ADC_REG_ReadConversionData12(ADC1);			//������ �������������� 12 ���
	float vcc = 0;
	vcc = (float)(CALIBR_VREF * *VREFINT_CAL) / adc_data; //CALIBR_VREF ���������� ������������� �� ����������� ���������� �������
	return vcc;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t err = ERR_OK;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, 3);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_CRC_Init();
  MX_ADC_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	
	adc_init();
	spi_init();				//�������������� SPI
	nrf24_init();			//�������������� �����������
	test_and_info(err);

	LL_TIM_EnableIT_UPDATE(TIM1);
	LL_TIM_EnableCounter(TIM1);
	
	vcc_data = reading_vcc();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		nrf24l01_receive(); 	//�����
		
		//���� ������ 10 ������
		if ((millis - temp_millis) > 10000)
		{
			temp_millis = millis;
			//�������� ���������� �������
			vcc_data = reading_vcc();
		}
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		LL_IWDG_ReloadCounter(IWDG);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(48000000);
  LL_SetSystemCoreClock(48000000);
}

/* USER CODE BEGIN 4 */
//��������� ��������� ���������� �� ������� ������
void but_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(BUT_GPIO_Port, BUT_Pin))	
	{
		if (!f_push_button)				//���� �� ������ ���� �������
		{
			f_push_button = 1;			//��������� ���� ������� 
			time_ms = millis;				//��������� �����
		}
	}
}

//��������� ��������� ���������� �������
void tim1_callback(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM1))	//���� ���� ���������� �/�
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);			//�������� ���� ����������
		millis++;														//������� ��������� ��
		
		if (f_led_toggle)										//���� ����� �������� ����������
		{
			ms_counter++;											//����������� ��������� ���������� ��
			if (ms_counter >= LED_TGL_MS)
			{
				ms_counter = 0;									//�� ��������� ������ ��������� ����������
				LED_TGL();
			}
		}
		else ms_counter = 0;								//����� ��������
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

