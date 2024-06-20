/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.c
	*
	* Проект "КомБО" (Открытые системы беспроводной коммуникации)
	* Игровая кнопка электронной системы для проведения интеллектуальных игр
	*	
	* Основной код программы.
	* Микроконтроллер: stm32f030f4p6
	* Периферия: радиомодуль nrf24l01+, кнопка, светодиод.
	* 
	* Краткое описание работы:
	* После включения питания, настройки периферии производится проверка 
	* соединения с радиомодулем, соответствующая сигнализация (пять морганий при 
	* исправном радиомодуле, 1 - при неисправности), а затем считывание напряжения 
	* питания с целью контроля уровня заряда батареи. 
	* При инициализации запускается отсчет времени.
	* В процессе работы постоянно проверяется наличие посылки от базы, периодически 
	* измеряется напряжение питания.
	* При срабатывании прерывания по нажатию кнопки фиксируется время нажатия.
	* При получении посылки от базы в зависимости от содержимого:
	* - отправка в ответ статуса кнопки, либо времени, а также значение измеренного 
	* напряжения питания
	* - изменение статуса светодиода - зажжен, погашен, либо моргает
	* - сброс состояния устройства
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

//флаги
uint8_t f_push_button	= 0;
uint8_t f_led_toggle 	= 0;

//переменные измерения времени
uint8_t	 ms_counter 	= 0;
uint32_t millis 		= 0;
uint32_t temp_millis 	= 0;
uint32_t time_ms 		= 0;

//переменные измерения напряжения питания
uint16_t adc_data 		= 0;
float vcc_data;

//внешние переменные для работы с радиомодулем
extern uint8_t tx_buf[TX_PLOAD_WIDTH];	//буфер передачи
extern uint8_t rx_buf[TX_PLOAD_WIDTH];	//буфер приема
extern volatile uint8_t f_rx, f_tx;			//флаги приема и передачи

//структура для преобразования данных
float_to_byte_t float_to_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Процедура моргания светодиодом
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

//Функция расчета контрольной суммы
uint16_t crc16_calc(volatile uint8_t buf[], uint8_t buf_size) 
{
	uint16_t crc = 0;			// возвращаемое значение
	uint16_t D;						// Индекс элемента массива для наложения 
	int16_t T;						// Элемент массива для наложения, вычисляется на лету
	uint16_t P = 0x1021;	// Полином - маска

	for (uint8_t i = 0; i < buf_size; i++)
	{
		D = crc >> 8; 			// Сдвиг crc на 8 бит вправо
		D = D ^ buf[i];			// Вычисление индекса элемента массива для наложения  

		T = D;							// Вычисление элемента массива
		T = T << 8;        	// Сдвиг элемента массива на 8 бит влево

		for (uint8_t j = 0; j < 8; j++)    // Повторение 8 раз
		{
			if (T < 0)				// Если старший бит = 1
			{
				T = T << 1;				// Сдвиг элемента массива на 1 бит влево
				T = T ^ P;				// Наложение полинома
			}
			else
			{
				T = T << 1;				// Сдвиг элемента массива на 1 бит влево
			}
		}
		crc = crc << 8;				// Сдвиг С на 8 бит влево
		crc = crc ^ T;				// Исключающе или между С и элементом массива
	}

	return crc;							// Возврат значения crc
}

//Подпрограмма тестирования радиомодуля
uint8_t nrf_test(void)
{
	uint8_t buf_0[TX_ADR_WIDTH] = {0};				
	uint8_t buf_1[TX_ADR_WIDTH] = {0};
	uint8_t test_addr_0[TX_ADR_WIDTH] = UNIQUE_ADDRESS_0;
	uint8_t test_addr_1[TX_ADR_WIDTH] = UNIQUE_ADDRESS_1;
	
	uint8_t err = ERR_OK;								
	
	//читаем значения регистров
	
	#ifdef MASTER
	nrf24_read_buf(RX_ADDR_P1,buf_0,3);	
	nrf24_read_buf(TX_ADDR,buf_1,3);
	#endif
	
	#ifdef SLAVE
	nrf24_read_buf(RX_ADDR_P1,buf_1,3);	
	nrf24_read_buf(TX_ADDR,buf_0,3);
	#endif

	//проверяем значения регистров записанных при инициализации и считанных 
	for (uint8_t i = 0; i < TX_ADR_WIDTH; i++)
	{
		if (test_addr_0[i] != buf_0[i]) err = NRF_ERR;
		if (test_addr_1[i] != buf_1[i]) err = NRF_ERR;
	}
	
	return err;
}

//Процедура тестирования радиомодуля и сигнализации
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

//Процедура приема радиомодуля
void nrf24l01_receive(void)
{
	if(f_rx)	//если флаг приема поднят
	{
		//проверяем контрольную сумму
		uint16_t temp_crc = crc16_calc(rx_buf, TX_PLOAD_WIDTH - 2);
		uint16_t input_crc = (uint16_t)rx_buf[TX_PLOAD_WIDTH - 1] + (uint16_t)(rx_buf[TX_PLOAD_WIDTH - 2] << 8);
		
		if (temp_crc == input_crc)		//если контрольная сумма сходится
		{
			if (rx_buf[0] == OUR_ADDR)	//если адрес наш
			{
				if (f_push_button)				//если нажатие было
				{
					tx_buf[0] = OUR_ADDR;		//записываем в первый байт адрес
					
					(*(uint32_t*)&tx_buf[1]) = time_ms;	//в следующие 4 байта пишем значение времени
					
					float_to_byte.float_val = vcc_data;	//в следующие 4 байта - преобразованное значение напряжения питания
					tx_buf[5] = float_to_byte.char_val[0];
					tx_buf[6] = float_to_byte.char_val[1];
					tx_buf[7] = float_to_byte.char_val[2];
					tx_buf[8] = float_to_byte.char_val[3];
					//рассчитываем контрольную сумму, записываем в последние 4 байта
					temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
					tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
					tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
					
					nrf24_send(tx_buf);		//отправляем посылку в эфир
				}
				else										//если нажатия не было
				{
					tx_buf[0] = OUR_ADDR;	//записываем в первый байт адрес
					(*(uint32_t*)&tx_buf[1]) = NOT_PUSHED;	//в следующие 4 - признак ненажатой кнопки
										
					float_to_byte.float_val = vcc_data;			//в следующие 4 байта - преобразованное значение напряжения питания
					tx_buf[5] = float_to_byte.char_val[0];
					tx_buf[6] = float_to_byte.char_val[1];
					tx_buf[7] = float_to_byte.char_val[2];
					tx_buf[8] = float_to_byte.char_val[3];
					//рассчитываем контрольную сумму, записываем в последние 4 байта
					temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
					tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
					tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
					
					nrf24_send(tx_buf);		//отправляем посылку в эфир
				}
				
				if (rx_buf[1] == LED_STAT)	//если далее идет команда обновления состояния светодиода
				{
					if (rx_buf[2] == LED_STAT_ON)	//если зажечь светодиод
					{
						LED_ON();										//зажигаем
						f_led_toggle = 0;						//опускаем флаг моргания
					}
					else if (rx_buf[2] == LED_STAT_OFF)	//если погасить светодиод
					{
						LED_OFF();									//гасим
						f_led_toggle = 0;						//опускаем флаг моргания
					}
					else if (rx_buf[2] == LED_STAT_TGL)	//если режим моргания
					{
						f_led_toggle = 1;						//поднимаем флаг моргания
					}
				}
			}
			else if (rx_buf[0] == RESET_CMD)	//если команда общего сброса
			{
				blink_led(1);										//моргаем светодиодом 1 раз
				//сбрасываем состояние кнопки - обнуляем счетчики, опускаем флаги
				time_ms = 0;
				millis = 0;
				f_push_button = 0;
				f_led_toggle = 0;
			}
		}
		f_rx = 0;					//опускаем флаг приема
	}
}

//Процедура инициализации АЦП
void adc_init(void)
{
	uint32_t wait_loop_index = 0;	//переменная ожидания калибровки АЦП
	
	LL_ADC_SetCommonPathInternalCh(ADC1_COMMON,LL_ADC_PATH_INTERNAL_VREFINT);
	
	//---Кофигурирование АЦП--------------------------------------------------------
	LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B); 
  LL_ADC_StartCalibration(ADC1);											//запуск калибровки АЦП
  while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {}		//ждем окончания калибровки АЦП
	LL_ADC_Enable(ADC1);																//включение АЦП
	//формирование  времени ожидания настройки АЦП
	wait_loop_index = ((LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32) >> 1		); 
	while(wait_loop_index != 0)													//ожидание
  {
    wait_loop_index--;
  }
}

//Процедура чтения напряжения питания
float reading_vcc(void)
{
	LL_ADC_REG_StartConversion(ADC1);											//запуск преобразования АЦП
	while (!LL_ADC_IsActiveFlag_EOS(ADC1))  {}						//ожидание конца преобразования
	LL_ADC_ClearFlag_EOS(ADC1);														//сброс флага АЦП
	adc_data = LL_ADC_REG_ReadConversionData12(ADC1);			//чтение преобразования 12 бит
	float vcc = 0;
	vcc = (float)(CALIBR_VREF * *VREFINT_CAL) / adc_data; //CALIBR_VREF необходимо откалибровать по измеренному напряжению питания
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
	spi_init();				//инициализируем SPI
	nrf24_init();			//инициализируем радиомодуль
	test_and_info(err);

	LL_TIM_EnableIT_UPDATE(TIM1);
	LL_TIM_EnableCounter(TIM1);
	
	vcc_data = reading_vcc();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		nrf24l01_receive(); 	//прием
		
		//если прошло 10 секунд
		if ((millis - temp_millis) > 10000)
		{
			temp_millis = millis;
			//измеряем напряжение питания
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
//Процедура обработки прерывания по нажатию кнопки
void but_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(BUT_GPIO_Port, BUT_Pin))	
	{
		if (!f_push_button)				//если не поднят флаг нажатия
		{
			f_push_button = 1;			//поднимаем флаг нажатия 
			time_ms = millis;				//сохраняем время
		}
	}
}

//Процедура обработки прерывания таймера
void tim1_callback(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM1))	//если флаг прерывания т/с
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);			//опускаем флаг прерывания
		millis++;														//считаем прошедшую мс
		
		if (f_led_toggle)										//если режим моргания светодиода
		{
			ms_counter++;											//отсчитываем указанное количество мс
			if (ms_counter >= LED_TGL_MS)
			{
				ms_counter = 0;									//по истечении меняем состояние светодиода
				LED_TGL();
			}
		}
		else ms_counter = 0;								//сброс счетчика
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

