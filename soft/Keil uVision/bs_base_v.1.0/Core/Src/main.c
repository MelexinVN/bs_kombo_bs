/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * main.c
	* 
	* ������ "�����" (�������� ������� ������������ ������������)
	* ���� ����������� ������� ��� ���������� ���������������� ���
	*	���������� ������� ������ - 20.
	* �������� ��� ���������.
	* ���������������: stm32f103c8t6
	* ���������: ����������� nrf24l01+, ������� (LCD2004 ��� ST7735), 
	* ������ ���������� (4 ��), mp3 ������ DFPlayer, ���� USB-UART.
	* 
	* ������� �������� ������:
	* ����� ��������� �������, ������������� ��������� � ���������� ������������ 
	* �������� ���������� � ������������, ��������������� ������������ (����� 
	* ���������� �� ����������, ������ ��������, ������ ����������)
	* ����� ���� ������� ������������ ������� ������ ������.
	* � �������� ������ ���������� ������������� �������� �������� � ������ ���� 
	* ������� �������, �������� ��������� ������ ����������, ������������ ������� 
	* �������, ����� ���������� �� �������, 
	* ��� ��������� ������� �� ������� ������, ����������� ������ �� �� ��������� 
	* ��� ������� ������ ���������� "�����" ���������� �������� ������� ������ 
	* ������ ���� �������, � ����� ����� ��������� ���� - ����������� � �������� 
	* ���������.
	* ��� ������� ������ "�����" ���������� �������� ������� �������.
	* ������� "�����" � "�����" ����� ���� ����� �������� �� ����������������� �����, 
	* �������� ��� ��������� �� ���������� �������� ��������������� ������ ����������.
	* ��� ������� ������ "�����", ����������� ������ ������� ������, ��������������� 
	* ��������������� �������� ����.
	* ��� ������� ������� "���������" �������� ����� �����, ���������������� 
	* �������� ������� ��� ����������� ������� ������� ������ �� ����� �������.
	* ��� ����������� ������� ������� ������ ���������� ��������������� ��������� 
	* ����� � �������� ���������������� ������ ������� � ���������������� ����.
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

//���������� � �������
char str[STRING_SIZE]								= {0};				//������ ��� ������ � ����
char input_str[STRING_SIZE] 				= {0};				//�������� ������ 
char char_to_send[NUM_OF_SLAVES] 		= CHARS_LIST;	//������ �������� ��� �������� � ����
uint8_t slave_addrs[NUM_OF_SLAVES] 	= ADRESS_LIST;//������ �������
uint8_t slave_nums[NUM_OF_SLAVES] 	= NUM_LIST;		//������ �������

//������ ��� ������ �� ������� LCD2004
char string_0[] 	= "0123456789abcdefghij";
char string_1[] 	= "                    ";
char string_2[] 	= "                    ";
char string_3[] 	= "            BS KomBO";

//������ ��� ������ �� ������� st7735
char w_string_1[] = "          ";
char w_string_2[] = "          ";

//������� ���������� ��� ������ � ������������
extern uint8_t 					tx_buf[TX_PLOAD_WIDTH];	//����� ��������
extern uint8_t 					rx_buf[TX_PLOAD_WIDTH];	//����� ������
extern volatile uint8_t f_rx, f_tx;							//����� ������ � ��������

//��������� ������ ����������
volatile Flags_t 			flags;
volatile Variables_t 	vars;

//������ �������� ������ ������
B_data_t							b_data[NUM_OF_SLAVES];

//��������� ��� �������������� ������
float_to_byte_t 			float_to_byte;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//��������� ������������� ����������, ������, ��������
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

//��������� ������� ����������� �����
uint16_t crc16_calc(volatile uint8_t buf[], uint8_t buf_size) 
{
	uint16_t	crc = 0;			// ������������ ��������
	uint16_t	D;						// ������ �������� ������� ��� ��������� 
	int16_t 	T;						// ������� ������� ��� ���������, ����������� �� ����
	uint16_t	P = 0x1021;		// ������� - �����

	for (uint8_t i = 0; i < buf_size; i++)
	{
		D = crc >> 8; 				// ����� crc �� 8 ��� ������
		D = D ^ buf[i];				// ���������� ������� �������� ������� ��� ���������  

		T = D;								// ���������� �������� �������
		T = T << 8;        		// ����� �������� ������� �� 8 ��� �����

		for (uint8_t j = 0; j < 8; j++)    // ���������� 8 ���
		{
			if (T < 0)					// ���� ������� ��� = 1
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

//��������� ������ �����������
void nrf24l01_receive(void)
{
	if(f_rx)	//���� ���� ������ ������ (���� ����������� �� �������� ���������� �� �����������)
	{
		uint16_t temp_crc = crc16_calc(rx_buf, TX_PLOAD_WIDTH - 2);
		uint16_t input_crc = (uint16_t)rx_buf[TX_PLOAD_WIDTH - 1] + (uint16_t)(rx_buf[TX_PLOAD_WIDTH - 2] << 8);
		if (temp_crc == input_crc)
		{
			for (uint8_t i = 0; i < NUM_OF_SLAVES; i++)
			{	//���� ������ �������� �����
				if (rx_buf[0] == slave_addrs[i]) 	//���� ������ �������� �����
				{
					float vcc_data;
					float_to_byte.char_val[0] = rx_buf[5];
					float_to_byte.char_val[1] = rx_buf[6];
					float_to_byte.char_val[2] = rx_buf[7];
					float_to_byte.char_val[3] = rx_buf[8];
					vcc_data = float_to_byte.float_val;
					
					if ((*(uint32_t*)&rx_buf[1]) != NOT_PUSHED)
					{	
						uint32_t time = *(uint32_t*)&rx_buf[1];	//����������� ���������� ����� �� �����

						if (b_data[i].pushed == 0)
						{
							b_data[i].time = time;
						}
					}	
					break;
				}
			}
		}
		f_rx = 0;					//�������� ���� ������
	}
}

//��������� ���������� �������
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

//��������� ������������� �������
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
	
	sprintf(str, "TX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\nRX_ADDR: 0x%02X, 0x%02X, 0x%02X\r\n",
	buf_1[0], buf_1[1], buf_1[2], buf_0[0], buf_0[1], buf_0[2]);
	
	#ifdef DEBUG_MODE
		usart_print(str);	
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
	
	err = nrf_test();		//������� �������� ��������� � ����
	
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

//��������� ���������� ������ �� �����������
void sort_data(uint32_t *data) 
{
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES - 1; i++) 
	{ 
		for(uint8_t j = 0 ; j < NUM_OF_SLAVES - i - 1 ; j++) 
		{
			if(data[j] > data[j+1]) 			//���������� ��� �������� ��������.
			{           									//���� ��� ���� � ������������ �������, ��  
				uint32_t tmp = data[j];			//������ �� �������. 
        data[j] = data[j+1];
        data[j+1] = tmp; 
      }
    }
  }
}

//��������� ������������� ������ DWT
void dwt_init(void)
{
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // ��������� ������������ �������
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // ��������� �������
}

//��������� �������������� ��������
void delay_us(uint32_t us)
{
	uint32_t us_count_tic =  us * (SystemCoreClock / 1000000); // �������� ���-�� ������ �� 1 ��� � �������� �� ���� ��������
	DWT->CYCCNT = 0U; // �������� �������
	while(DWT->CYCCNT < us_count_tic);
}

//��������� �������� ����� ������� ������
void reset_all(void)
{
	tx_buf[0] = RESET_CMD;	//
	
	uint16_t temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
	
	tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
	tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
	
	nrf24_send(tx_buf);						//���������� ������� � ����
}

//��������� ���������������� ������������� ������
void data_mix(uint8_t *data)
{
	uint8_t temp_data[NUM_OF_SLAVES];
	uint8_t i_rnd;
	
	//���������� �� ���� ��������� 1 ������ ��� �������� ������������ ���������� �����
	for(uint8_t i = 0; i < NUM_OF_SLAVES; i++) 
	{
			if (data[i] != 0xFF) data[i] = data[i] + 1;
	}
	
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{
		i_rnd = rand()%NUM_OF_SLAVES;	//���������� ��������� ������
		
		while (data[i_rnd] == 0)			//� ������ ���� ������� ������� � ���������� �������� �������
		{
				i_rnd = rand()%NUM_OF_SLAVES;		//���������� ����� ��������� ������ ���� �� ������ ��������� �������
		}
		if (data[i_rnd] != 0)					
		{
				temp_data[i] = data[i_rnd];	//���������� ��������� ������� ������� �� ��������� ������
				data[i_rnd] = 0;						//�������� ����������� �������
		}
	}
	
	//�������� ������������ �������� ������� ������� � �������� ������, �������� �������
	for(uint8_t i = 0; i < NUM_OF_SLAVES; i++) 
	{
			if (data[i] != 0xFF) data[i] = temp_data[i] - 1;
	}
}

//��������� ���������� ������� "�����"
void rst_push(void)
{
	reset_all();		//���������� ������� ������ ������ ���� �������

	f_rx = 0;					//�������� ���� ������
	
	//�������� � �������� ��������� �������
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
	
	//������������ ������ � �������� ������
	data_mix(slave_nums);
	//�������� ��� ���������� � �������� ���������
	init_variables();
	//������ �����
	LL_mDelay(SCAN_PERIOD * 5);
}

//��������� ������� "���������"
void set_push(void)
{
	//����������� ����� �������� ������������� �������� ������
	vars.sound_switch++;			
	if (vars.sound_switch >= 5) vars.sound_switch = 0;
	
	//���������� �� ������� �������� ������ �����
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

//��������� ���������� ������� "�����"
void time_push(void)
{
	flags.f_timer = 1;		//��������� ���� �������
	vars.seconds = 0;			//�������� ������� ������
	
	//������� �� ������� �������� �������
	#ifdef LCD_2004
		LCD_SetPos(0,2);
		LCD_String(string_2);	//������� ������
		sprintf(str, "%02d:%02d", vars.seconds/60, vars.seconds%60);
		LCD_SetPos(0,2);
		LCD_String(str);
	#endif
	
	#ifdef ST_7735
		sprintf(str, "%02d:%02d", vars.seconds/60, vars.seconds%60);
		ST7735_WriteString(0, 110, str, Font_11x18, ST7735_WHITE, ST7735_BLACK);
	#endif
	//������������� ������ �������� ���
	mp3_play(MP3_START);
}

//��������� ������������ ������� �������
void order_processing(void)
{
	uint32_t time_to_sort[NUM_OF_SLAVES];
	uint32_t buts_to_sort[NUM_OF_SLAVES];
	
	uint8_t flag_order[NUM_OF_SLAVES] = {0};
	
	uint8_t order = 0;
	uint8_t number = 0;
	
	//�������� ������� ������ ��� ����������
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		time_to_sort[i] = b_data[i].time;
	}
	//��������� �������
	sort_data(time_to_sort);
	
	//������� 
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		for(uint8_t j = 0 ; j < NUM_OF_SLAVES; j++) 
		{ 
			//� �������� ������� ����� �� ������������� ������� ��� ����������� ����� ������������ ������ ����������� ��� ���������� ������
			//���� ����� ������ �������� �� ��������� �� ��������, ����� ���������� � ��������������� ���� ����� ������� �� ������
			if ((b_data[slave_nums[j]].time == time_to_sort[i]) && (b_data[slave_nums[j]].time != MAX_TIME) && (!flag_order[slave_nums[j]]) )
			{
				order++;																	//��������� � ���������� �������� �������
				b_data[slave_nums[j]].push_order = order;	//����������� ��������������� ����� � ������� ������
				flag_order[slave_nums[j]] = 1;						//��������� ���� �����
			}
		}
	}
	
	//�������� � ���������������� ���� ������� � ������ ������������������
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		//���� ����� � ������� ������������� �������� �������� �������� ������� � ������ ����������
		if ((b_data[i].push_order == vars.send_order_count) && (b_data[i].time != MAX_TIME))
		{
			sprintf(str,"%c",char_to_send[i]);		
			usart_print(str);					//�������� ������ � ����
			vars.send_order_count++;	//����������� �������� ��������
		}
	}
	
	//����������� ��������� ������ ��� �������� � ������� ������� �� �������
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		if (b_data[i].push_order != 0)
		{
			if (b_data[i].push_order == 1)
			{
				b_data[i].led_status = LED_STAT_TGL;	//������� � ������� - �������
			}
			else
			{
				b_data[i].led_status = LED_STAT_ON;		//���������� ������� - ������
			}
			//������� ������ �� �������
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
			
			//������������� ���� ������� � ����������� �� ��������� ������� � ��������
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

//��������� ���������� ������� "�����"
void nxt_push(void)
{
	for(uint8_t i = 0 ; i < NUM_OF_SLAVES; i++) 
	{ 
		//������� ������� � �������
		if (b_data[i].push_order == 1) 	
		{
			b_data[i].push_order = 0;						//�������� �������� ������ � �������
			b_data[i].time = MAX_TIME;					//������������� ����������� �������� �������
			b_data[i].led_status = LED_STAT_OFF;//���������� ������� �� ���������� ��� ��������
		}
		else if (b_data[i].push_order > 1) 		//��������� ����� � ������� ���� ��������� � �������
		{
			b_data[i].push_order--;
		}
	}
	
	//������� ����������� ������� �� �������
	#ifdef LCD_2004
		LCD_SetPos(0,1);
		LCD_String(string_1);
	#endif
	
	#ifdef ST_7735
		ST7735_Write_W_String(0, 20, w_string_1, Font_11x18, ST7735_BLACK, ST7735_BLACK);
		ST7735_Write_W_String(0, 70, w_string_2, Font_11x18, ST7735_BLACK, ST7735_BLACK);
	#endif
	
	//��������� ����� �������
	//order_processing();
	
	LL_mDelay(100); //����� ������ �������� ������
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
	
	dwt_init();					//�������������� dwt
	spi_init();					//�������������� SPI
	nrf24_init();				//�������������� �����������
	usart_init(103);		//�������������� USART
	
	display_init();			//�������������� �������

	test_and_info(err);	//��������� ����������� � ������� ����������	
	
	#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
	#endif
	
	mp3_init();					//�������������� �������� ������

	refresh_display();	//��������� �������
	
	//��������� ����������, ��������� �������
	LL_TIM_EnableIT_UPDATE(TIM1);
	LL_TIM_EnableCounter(TIM1);	
	LL_TIM_EnableIT_UPDATE(TIM2);
	LL_TIM_EnableCounter(TIM2);
	LL_USART_EnableIT_RXNE(USART1);
	
	//�������������� ����������
	init_variables();
	
	//���������� ������� ������ ������
	reset_all();
	
	//������������� ��������� ��������� ������
	mp3_volume(15);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
		//��������� ���� ������ 
		if (flags.rst_push)			//���� ������ ����
		{
			flags.rst_push = 0;		//�������� ����
			rst_push();						//��������� ������� "�����"
		}
		
		//��������� ���� ����������� � ��������� ��������������
		if ((vars.millis - vars.temp_millis) > SCAN_PERIOD)
		{
			vars.temp_millis = vars.millis;
			
			tx_buf[0] = b_data[vars.slave_counter].address;	//���������� � ����� ��������� �����
			tx_buf[1] = b_data[vars.slave_counter].cmd;
			tx_buf[2] = b_data[vars.slave_counter].led_status;
			
			uint16_t temp_crc = crc16_calc(tx_buf, TX_PLOAD_WIDTH - 2);
			
			tx_buf[TX_PLOAD_WIDTH - 2] = (uint8_t)(temp_crc >> 8);
			tx_buf[TX_PLOAD_WIDTH - 1] = (uint8_t)temp_crc;
			
			nrf24_send(tx_buf);						//���������� ������� � ����

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
			
			if (flags.f_timer)	//���� ��������� ������ �������
			{			
				//���� ������ �������
				if ((vars.seconds - vars.temp_seconds) > 0)
				{
					uint8_t dec = vars.seconds/60;
					uint8_t ed = vars.seconds%60;
					vars.temp_seconds = vars.seconds;
					
					//���������� ������� ����� �� �������
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

		nrf24l01_receive();		//�����
		
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

//��������� ���������� �� ������� ������ "�����"
void rst_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(RST_GPIO_Port, RST_Pin))	//���� ��������� ����� - 0, ������ ������
	{
		flags.rst_push = 1;	//��������� ���� ������� 
	}
}

//��������� ���������� �� ������� ������ "�����"
void nxt_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(NEXT_GPIO_Port, NEXT_Pin))	//���� ��������� ����� - 0, ������ ������
	{
		flags.nxt_push = 1;	//��������� ���� ������� 
	}
}

//��������� ���������� �� ������� ������ "���������"
void set_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(SET_GPIO_Port, SET_Pin))	//���� ��������� ����� - 0, ������ ������
	{
		flags.set_push = 1;	//��������� ���� ������� 
	}
}

//��������� ���������� �� ������� ������ "�����"
void time_callback(void)
{
	if (!LL_GPIO_IsInputPinSet(TIME_GPIO_Port, TIME_Pin))	//���� ��������� ����� - 0, ������ ������
	{
		flags.time_push = 1;	//��������� ���� ������� 
	}
}

//��������� ���������� ������� 1
void tim1_callback(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM1))			//���� ���� ���������� �/�
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);					//�������� ���� ����������
		vars.seconds++;													//������� �������
	}
}

//��������� ���������� ������� 2
void tim2_callback(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM2))			//���� ���� ���������� ������� ������
	{
		LL_TIM_ClearFlag_UPDATE(TIM2);					//�������� ���� ����������
		vars.millis++;													//������� �����������
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

