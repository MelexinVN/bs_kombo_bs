/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.c
	* 
	* Проект "КомБО" (Открытые системы беспроводной коммуникации)
	* База электронной системы для проведения интеллектуальных игр
	*	Количество игровых кнопок - 20.
	* Основной код программы.
	* Микроконтроллер: stm32f103c8t6
	* Периферия: радиомодуль nrf24l01+, дисплей (LCD2004 или ST7735), 
	* кнопки управления (4 шт), mp3 модуль DFPlayer, мост USB-UART.
	* 
	* Краткое описание работы:
	* После включения питания, инициализации периферии и переменных производится 
	* проверка соединения с радиомодулем, соответствующая сигнализация (вывод 
	* информации об устройстве, версия прошивки, статус соединения)
	* Затем всем кнопкам отправляется команда общего сброса.
	* В процессе работы происходит периодическая отправка запросов и команд всем 
	* игровым кнопкам, проверка состояний кнопок управления, формирование очереди 
	* нажатий, вывод информации на дисплей, 
	* При получении ответов на запросы кнопок, фиксируются данные об их состоянии 
	* При нажатии кнопки управления "сброс" происходит отправка команды общего 
	* сброса всем кнопкам, а также сброс состояния базы - возвращение в исходное 
	* состояние.
	* При нажатии кнопки "далее" происходит смещение очереди нажатий.
	* Команды "сброс" и "далее" могут быть также получены по последовательному порту, 
	* действия при получении их аналогичны нажатиям соответствующих кнопок управления.
	* При нажатии кнопки "время", запускается таймер отсчета секунд, воспроизводится 
	* соответствующий звуковой файл.
	* При нажатии команды "установка" меняется номер файла, воспроизводимого 
	* звуковым модулем при обнаружении нажатия игровой кнопки до пуска времени.
	* При обнаружении нажатия игровой кнопки происходит воспроизведение звукового 
	* файла и отправка соответствующего кнопке символа в последовательный порт.
	*
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
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

//переменные и массивы
char str[STRING_SIZE]								= {0};				//строка для вывода в порт
char input_str[STRING_SIZE] 				= {0};				//входящая строка 
char char_to_send[NUM_OF_SLAVES] 		= CHARS_LIST;	//массив символов для отправки в порт
uint8_t slave_addrs[NUM_OF_SLAVES] 	= ADRESS_LIST;//массив адресов
uint8_t slave_nums[NUM_OF_SLAVES] 	= NUM_LIST;		//массив номеров

//строки для вывода на дисплей LCD2004
char string_0[] 	= "0123456789abcdefghij";
char string_1[] 	= "                    ";
char string_2[] 	= "                    ";
char string_3[] 	= "            BS KomBO";

//строки для вывода на дисплей st7735
char w_string_1[] = "          ";
char w_string_2[] = "          ";

//внешние переменные для работы с радиомодулем
extern uint8_t 					tx_buf[TX_PLOAD_WIDTH];	//буфер передачи
extern uint8_t 					rx_buf[TX_PLOAD_WIDTH];	//буфер приема
extern volatile uint8_t f_rx, f_tx;							//флаги приема и передачи

//структура флагов устройства
volatile Flags_t 			flags;
volatile Variables_t 	vars;

//массив структур данных кнопок
B_data_t							b_data[NUM_OF_SLAVES];

//структура для преобразования данных
float_to_byte_t 			float_to_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Процедура инициализации переменных, флагов, массивов
void init_variables(void)
{
	flags.rst_push 					= 0;
	flags.nxt_push 					= 0;
	flags.set_push 					= 0;
	flags.time_push 				= 0;
	flags.f_timer 					= 0;
	
	vars.seconds 						= 0;
	vars.temp_seconds 			= 0;
	vars.millis 						= 0;
	vars.slave_counter 			= 0;
	vars.send_order_count		= 1;
	
	for (uint8_t i = 0; i < NUM_OF_SLAVES; i++)
	{
		b_data[i].number 			= i;
		b_data[i].address 		= slave_addrs[i];
		b_data[i].time 				= MAX_TIME;
		b_data[i].cmd 				= LED_STAT; 
		b_data[i].led_status 	= LED_STAT_OFF; 
		b_data[i].push_order 	= 0;
		b_data[i].pushed 			= 0;
	}
}

//Процедура расчета контрольной суммы
uint16_t crc16_calc(volatile uint8_t buf[], uint8_t buf_size) 
{
	uint16_t	crc = 0;			// возвращаемое значение
	uint16_t	D;						// Индекс элемента массива для наложения 
	int16_t 	T;						// Элемент массива для наложения, вычисляется на лету
	uint16_t	P = 0x1021;		// Полином - маска

	for (uint8_t i = 0; i < buf_size; i++)
	{
		D = crc >> 8; 				// Сдвиг crc на 8 бит вправо
		D = D ^ buf[i];				// Вычисление индекса элемента массива для наложения  

		T = D;								// Вычисление элемента массива
		T = T << 8;        		// Сдвиг элемента массива на 8 бит влево

		for (uint8_t j = 0; j < 8; j++)    // Повторение 8 раз
		{
			if (T < 0)					// Если старший бит = 1
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

//Процедура приема радиомодуля
void nrf24l01_receive(void)
{
	if(f_rx)	//если флаг приема поднят (флаг поднимается по внешнему прерыванию от радиомодуля)
	{
		uint16_t temp_crc = crc16_calc(rx_buf, TX_PLOAD_WIDTH - 2);
		uint16_t input_crc = (uint16_t)rx_buf[TX_PLOAD_WIDTH - 1] + (uint16_t)(rx_buf[TX_PLOAD_WIDTH - 2] << 8);
		if (temp_crc == input_crc)
		{
			for (uint8_t i = 0; i < NUM_OF_SLAVES; i++)
			{	//если найден принятый адрес
				if (rx_buf[0] == slave_addrs[i]) 	//если найден принятый адрес
				{
					float vcc_data;
					float_to_byte.char_val[0] = rx_buf[5];
					float_to_byte.char_val[1] = rx_buf[6];
					float_to_byte.char_val[2] = rx_buf[7];
					float_to_byte.char_val[3] = rx_buf[8];
					vcc_data = float_to_byte.float_val;
					
					if ((*(uint32_t*)&rx_buf[1]) != NOT_PUSHED)
					{	
						uint32_t time = *(uint32_t*)&rx_buf[1];	//преобразуем оставшиеся байты во время

						if (b_data[i].pushed == 0)
						{
							b_data[i].time = time;
						}
					}	
					break;
				}
			}
		}
		f_rx = 0;					//опускаем флаг приема
	}
}

//Процедура обновления дисплея
void refresh_display()
{
	#ifdef LCD_2004
		LCD_Clear();

		LCD_SetPos(0,0);
		LCD_String(string_0);
		LCD_SetPos(0,1);
		LCD_String(string_1);
		LCD_SetPos(0,2);
		LCD_String(string_2);
		LCD_SetPos(0,3);
		LCD_String(string_3);
	#endif
	
	#ifdef ST_7735
		ST7735_FillScreen(ST7735_BLACK);
	
		ST7735_Write_W_String(0, 0,  "0123456789", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		ST7735_Write_W_String(0, 50, "abcdefghij", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	
		ST7735_WriteString(100, 120, "BS KomBO", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	#endif
}

//Процедура инициализации дисплея
void display_init(void)
{
	#ifdef LCD_2004
		LCD_init();
	#endif
	
	#ifdef ST_7735
		ST7735_Init();
		ST7735_FillScreen(ST7735_BLACK);
	#endif
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
	
	sprintf(str, "TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\nRX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n",
	buf_1[0], buf_1[1], buf_1[2], buf_0[0], buf_0[1], buf_0[2]);
	
	#ifdef DEBUG_MODE
		usart_print(str);	
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
	sprintf(str, "v. %d.%02d", VERSION, SUBVERSION);
	
	#ifdef DEBUG_MODE
		usart_println(DEV_INFO);
		usart_println(str);
	#endif
	
	#ifdef LCD_2004
		LCD_SetPos(0,0);
		LCD_String(DEV_INFO);
		LCD_SetPos(0,1);
		LCD_String(str);
	#endif
	
	#ifdef ST_7735
		ST7735_WriteString(0, 0, DEV_INFO, Font_11x18, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(0, 20, str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
	#endif
	
	#ifdef DEBUG_MODE
		usart_println(str);
	#endif	
	
	#ifndef DEBUG_MODE
	LL_IWDG_ReloadCounter(IWDG);
	#endif
	
	err = nrf_test();		//выводим значения регистров в порт
	
	if (err == ERR_OK) 
	{
		sprintf(str, "%s ch = %d", "nrf24 OK ", CHANNEL);
		
		#ifdef DEBUG_MODE
			usart_println(str);
		#endif		
		
		#ifdef LCD_2004
			LCD_SetPos(0,2);
			LCD_String(str);
		#endif
		
		#ifdef ST_7735
			ST7735_WriteString(0, 60, "nrf24 OK", Font_11x18, ST7735_WHITE, ST7735_BLACK);
			sprintf(str, "ch = %d", CHANNEL);
			ST7735_WriteString(0, 80, str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
		#endif
	}
	else  if (err == NRF_ERR) 
	{
		#ifdef DEBUG_MODE
			usart_println("nrf ERROR");
		#endif			
		
		#ifdef LCD_2004
			LCD_SetPos(0,2);
			LCD_String("nrf ERROR");
		#endif
		
		#ifdef ST_7735
			ST7735_WriteString(0, 60, "nrf24 ERROR", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		#endif
	}
}

//Процедура сортировки данных по возрастанию
void sort_data(uint32_t *data) 
{
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES - 1; i++) 
	{ 
		for(uint8_t j = 0 ; j < NUM_OF_SLAVES - i - 1 ; j++) 
		{
			if(data[j] > data[j+1]) 			//сравниваем два соседних элемента.
			{           									//если они идут в неправильном порядке, то  
				uint32_t tmp = data[j];			//меняем их местами. 
        data[j] = data[j+1];
        data[j+1] = tmp; 
      }
    }
  }
}

//Процедура инициализации модуля DWT
void dwt_init(void)
{
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

//Процедура микросекундной задержки
void delay_us(uint32_t us)
{
	uint32_t us_count_tic =  us * (SystemCoreClock / 1000000); // получаем кол-во тактов за 1 мкс и умножаем на наше значение
	DWT->CYCCNT = 0U; // обнуляем счётчик
	while(DWT->CYCCNT < us_count_tic);
}

//Процедура отправки общей команды сброса
void reset_all(void)
{
	tx_buf[0] = RESET_CMD;	//
	
	uint16_t temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
	
	tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
	tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
	
	nrf24_send(tx_buf);						//отправляем посылку в эфир
}

//Процедура псевдослучайного перемешивания данных
void data_mix(uint8_t *data)
{
	uint8_t temp_data[NUM_OF_SLAVES];
	uint8_t i_rnd;
	
	//прибавляем ко всем элементам 1 потому что алгоритм предполагает отсутствие нулей
	for(uint8_t i = 0; i < NUM_OF_SLAVES; i++) 
	{
			if (data[i] != 0xFF) data[i] = data[i] + 1;
	}
	
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{
		i_rnd = rand()%NUM_OF_SLAVES;	//генерируем случайный индекс
		
		while (data[i_rnd] == 0)			//в случае если элемент массива с полученным индексом нулевой
		{
				i_rnd = rand()%NUM_OF_SLAVES;		//генерируем новый случайный индекс пока не найдем ненулевой элемент
		}
		if (data[i_rnd] != 0)					
		{
				temp_data[i] = data[i_rnd];	//записываем случайный элемент массива во временный массив
				data[i_rnd] = 0;						//обнуляем сохраненный элемент
		}
	}
	
	//копируем перемешанные элементы массива обратно в исходный массив, вычитаем единицу
	for(uint8_t i = 0; i < NUM_OF_SLAVES; i++) 
	{
			if (data[i] != 0xFF) data[i] = temp_data[i] - 1;
	}
}

//Процедура реализации команды "сброс"
void rst_push(void)
{
	reset_all();		//отправляем команду общего сброса всем кнопкам

	f_rx = 0;					//опускаем флаг приема
	
	//приводим в исходное состояние дисплей
	#ifdef LCD_2004
		LCD_Clear();
		refresh_display();
	#endif
	
	#ifdef ST_7735
		refresh_display();
		for (uint8_t i = 0; i < 10; i++)
		{
			w_string_1[i] = ' ';
			w_string_2[i] = ' ';
		}
	#endif
	
	//перемешаваем массив с номерами кнопок
	data_mix(slave_nums);
	//приводим все переменные в исходное состояние
	init_variables();
	//делаем паузу
	LL_mDelay(SCAN_PERIOD * 5);
}

//Процедура команды "установка"
void set_push(void)
{
	//присваиваем новое значение переключателя звуковых файлов
	vars.sound_switch++;			
	if (vars.sound_switch >= 5) vars.sound_switch = 0;
	
	//отображаем на дисплее название нового звука
	#ifdef LCD_2004
		switch (vars.sound_switch)
		{
			case 0:
				LCD_SetPos(12,2);
				LCD_String("shoot   ");
			break;
			case 1:
				LCD_SetPos(12,2);
				LCD_String("open    ");
			break;
			case 2:
				LCD_SetPos(12,2);
				LCD_String("svist   ");
			break;
			case 3:
				LCD_SetPos(12,2);
				LCD_String("falstart");
			break;
			case 4:
				LCD_SetPos(12,2);
				LCD_String("zvon    ");
			break;
		}
	#endif
	
	#ifdef ST_7735
		switch (vars.sound_switch)
		{
			case 0:
				ST7735_WriteString(0, 90, "shoot   ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			break;
			case 1:
				ST7735_WriteString(0, 90, "open    ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			break;
			case 2:
				ST7735_WriteString(0, 90, "svist   ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			break;
			case 3:
				ST7735_WriteString(0, 90, "falstart", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			break;
			case 4:
				ST7735_WriteString(0, 90, "zvon    ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			break;
		}
	#endif

}

//Процедура выполнения команды "время"
void time_push(void)
{
	flags.f_timer = 1;		//поднимаем флаг таймера
	vars.seconds = 0;			//обнуляем счетчик секунд
	
	//Выводим на дисплей значение времени
	#ifdef LCD_2004
		LCD_SetPos(0,2);
		LCD_String(string_2);	//очищаем строку
		sprintf(str, "%02d:%02d", vars.seconds/60, vars.seconds%60);
		LCD_SetPos(0,2);
		LCD_String(str);
	#endif
	
	#ifdef ST_7735
		sprintf(str, "%02d:%02d", vars.seconds/60, vars.seconds%60);
		ST7735_WriteString(0, 110, str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
	#endif
	//воспроизводим нужный звуковой айл
	mp3_play(MP3_START);
}

//Процедура формирования очереди нажатий
void order_processing(void)
{
	uint32_t time_to_sort[NUM_OF_SLAVES];
	uint32_t buts_to_sort[NUM_OF_SLAVES];
	
	uint8_t flag_order[NUM_OF_SLAVES] = {0};
	
	uint8_t order = 0;
	uint8_t number = 0;
	
	//копируем времена кнопок для сортировки
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		time_to_sort[i] = b_data[i].time;
	}
	//сортируем времена
	sort_data(time_to_sort);
	
	//находим 
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		for(uint8_t j = 0 ; j < NUM_OF_SLAVES; j++) 
		{ 
			//в качестве индекса номер из перемешанного массива для обеспечения более объективного выбора очередности при совпадении времен
			//если время кнопки сопадает со следующим по величине, точно нажималась и соответствующий флаг учета очереди не поднят
			if ((b_data[slave_nums[j]].time == time_to_sort[i]) && (b_data[slave_nums[j]].time != MAX_TIME) && (!flag_order[slave_nums[j]]) )
			{
				order++;																	//переходим к следующему значению очереди
				b_data[slave_nums[j]].push_order = order;	//присваиваем соответствующий номер в очереди кнопке
				flag_order[slave_nums[j]] = 1;						//поднимаем флаг учета
			}
		}
	}
	
	//отсылаем в последовательный порт символы в нужной последовательности
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		//если номер в очереди соответствует текущему значению счетчика очереди и кнопка нажималась
		if ((b_data[i].push_order == vars.send_order_count) && (b_data[i].time != MAX_TIME))
		{
			sprintf(str,"%c",char_to_send[i]);		
			usart_print(str);					//высылаем символ в порт
			vars.send_order_count++;	//увеличиваем значение счетчика
		}
	}
	
	//расставляем состояния кнопок для отправки и выводим очередь на дисплей
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		if (b_data[i].push_order != 0)
		{
			if (b_data[i].push_order == 1)
			{
				b_data[i].led_status = LED_STAT_TGL;	//первому в очереди - моргать
			}
			else
			{
				b_data[i].led_status = LED_STAT_ON;		//оставшиеся нажатые - зажечь
			}
			//выводим данные на дисплей
			#ifdef LCD_2004
				LCD_SetPos(b_data[i].number, 1);
				LCD_SendChar(char_to_send[b_data[i].push_order - 1]);
			#endif
			
			#ifdef ST_7735
				if (i < 10)
				{
					w_string_1[i] = char_to_send[b_data[i].push_order - 1];
					ST7735_Write_W_String(0, 20, w_string_1, Font_11x18, ST7735_YELLOW, ST7735_BLACK);
				}
				else
				{
					w_string_2[i-10] = char_to_send[b_data[i].push_order - 1];
					ST7735_Write_W_String(0, 70, w_string_2, Font_11x18, ST7735_YELLOW, ST7735_BLACK);
				}
			#endif
			
			//воспроизводим звук нажатия в зависимости от состояния таймера и настроек
			if (!b_data[i].pushed)
			{
				b_data[i].pushed = 1;

				if (flags.f_timer)
				{
					mp3_play(MP3_ANSWER);
				}
				else
				{
					switch (vars.sound_switch)
					{
						case 0:
							mp3_play(MP3_SHOOT);
						break;
						case 1:
							mp3_play(MP3_ANSWER);
						break;
						case 2:
							mp3_play(MP3_SVIST);
						break;
						case 3:
							mp3_play(MP3_FALSTART);
						break;
						case 4:
							mp3_play(MP3_ZVON);
						break;
					}
				}
			}
		}
	}
}

//Процедура выполнения команды "далее"
void nxt_push(void)
{
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		//находим первого в очереди
		if (b_data[i].push_order == 1) 	
		{
			b_data[i].push_order = 0;						//обнуляем значение номера в очереди
			b_data[i].time = MAX_TIME;					//устанавливаем стандартное значение времени
			b_data[i].led_status = LED_STAT_OFF;//записываем команду на выключения для отправки
		}
		else if (b_data[i].push_order > 1) 		//уменьшаем номер в очереди всем остальным в очереди
		{
			b_data[i].push_order--;
		}
	}
	
	//очищаем отображение очереди на дисплее
	#ifdef LCD_2004
		LCD_SetPos(0,1);
		LCD_String(string_1);
	#endif
	
	#ifdef ST_7735
		ST7735_Write_W_String(0, 20, w_string_1, Font_11x18, ST7735_BLACK, ST7735_BLACK);
		ST7735_Write_W_String(0, 70, w_string_2, Font_11x18, ST7735_BLACK, ST7735_BLACK);
	#endif
	
	//формируем новую очередь
	//order_processing();
	
	LL_mDelay(100); //пауза против дребезга кнопки
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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_SPI2_Init();
  MX_CRC_Init();
  MX_USART3_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	
	dwt_init();					//инициализируем dwt
	spi_init();					//инициализируем SPI
	nrf24_init();				//инициализируем радиомодуль
	usart_init(103);		//инициализируем USART
	
	display_init();			//инициализируем дисплей

	test_and_info(err);	//тестируем радиомодуль и выводим информацию	
	
	#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
	#endif
	
	mp3_init();					//инициализируем звуковой модуль

	refresh_display();	//обновляем дисплей
	
	//разрешаем прерывания, запускаем таймеры
	LL_TIM_EnableIT_UPDATE(TIM1);
	LL_TIM_EnableCounter(TIM1);	
	LL_TIM_EnableIT_UPDATE(TIM2);
	LL_TIM_EnableCounter(TIM2);
	LL_USART_EnableIT_RXNE(USART1);
	
	//инициализируем переменные
	init_variables();
	
	//отправляем команду общего сброса
	reset_all();
	
	//устанавливаем громкость звукового модуля
	mp3_volume(15);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
		//проверяем флаг сброса 
		if (flags.rst_push)			//если поднят флаг
		{
			flags.rst_push = 0;		//опускаем флаг
			rst_push();						//выполняем команду "сброс"
		}
		
		//следующий блок выполняется с указанной периодичностью
		if ((vars.millis - vars.temp_millis) > SCAN_PERIOD)
		{
			vars.temp_millis = vars.millis;
			
			tx_buf[0] = b_data[vars.slave_counter].address;	//записываем в буфер очередной адрес
			tx_buf[1] = b_data[vars.slave_counter].cmd;
			tx_buf[2] = b_data[vars.slave_counter].led_status;
			
			uint16_t temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
			
			tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
			tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
			
			nrf24_send(tx_buf);						//отправляем посылку в эфир

			vars.slave_counter++;
			
			if (vars.slave_counter >= NUM_OF_SLAVES) 
			{
				vars.slave_counter = 0;			
				
				if (flags.nxt_push)
				{
					flags.nxt_push = 0;
					nxt_push();
				}
				
				if (flags.set_push)
				{
					flags.set_push = 0;
					set_push();
				}

				if (flags.time_push)
				{
					flags.time_push = 0;
					time_push();
				}
				
				order_processing();
			}
			
			if (flags.f_timer)	//если секундный таймер запущен
			{			
				//если прошла секунда
				if ((vars.seconds - vars.temp_seconds) > 0)
				{
					uint8_t dec = vars.seconds/60;
					uint8_t ed = vars.seconds%60;
					vars.temp_seconds = vars.seconds;
					
					//отображаем текущее время на дисплее
					#ifdef LCD_2004
						sprintf(str, "%02d:%02d", dec, ed);
						LCD_SetPos(0,2);
						LCD_String(str);
					#endif
					
						#ifdef ST_7735
						sprintf(str, "%02d:%02d", vars.seconds/60, vars.seconds%60);
						ST7735_WriteString(0, 110, str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
					#endif
				}
			}
		}

		nrf24l01_receive();		//прием
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
		#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
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
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(72000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//обработка прерывания по нажатию кнопки "сброс"
void rst_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(RST_GPIO_Port, RST_Pin))	//если состояние входа - 0, кнопка нажата
	{
		flags.rst_push = 1;	//поднимаем флаг нажатия 
	}
}

//обработка прерывания по нажатию кнопки "далее"
void nxt_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(NEXT_GPIO_Port, NEXT_Pin))	//если состояние входа - 0, кнопка нажата
	{
		flags.nxt_push = 1;	//поднимаем флаг нажатия 
	}
}

//обработка прерывания по нажатию кнопки "установка"
void set_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(SET_GPIO_Port, SET_Pin))	//если состояние входа - 0, кнопка нажата
	{
		flags.set_push = 1;	//поднимаем флаг нажатия 
	}
}

//обработка прерывания по нажатию кнопки "время"
void time_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(TIME_GPIO_Port, TIME_Pin))	//если состояние входа - 0, кнопка нажата
	{
		flags.time_push = 1;	//поднимаем флаг нажатия 
	}
}

//обработка прерывания таймера 1
void tim1_callback(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM1))			//если флаг прерывания т/с
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);					//опускаем флаг прерывания
		vars.seconds++;													//считаем секунду
	}
}

//обработка прерывания таймера 2
void tim2_callback(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM2))			//если флаг прерывания таймера поднят
	{
		LL_TIM_ClearFlag_UPDATE(TIM2);					//опускаем флаг прерывания
		vars.millis++;													//считаем милисекунду
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

