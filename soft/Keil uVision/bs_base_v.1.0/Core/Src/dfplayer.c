/*
* dfplayer.c
* Библиотека работы со звуковым модулем dfplayer.
* микроконтроллер stm32f103c8t6 используется usart3
*/

#include "dfplayer.h"
#include "main.h"

//массив для отправки модулю
uint8_t mp3[ARR_SIZE];

//Процедура отправки одного байта
void usart3_transmit(uint8_t data) 
{
	while (!LL_USART_IsActiveFlag_TXE(USART3)) {}		//ждем отправки предыдущего байта
  LL_USART_TransmitData8(USART3,data);						//отправляем очередной байт в порт
}

//Процедура отправки массива
void usart3_print(uint8_t *str)
{	
	for (int i = 0; i < ARR_SIZE; i++)
	{
		usart3_transmit(str[i]); //отправляем очередной байт
	}
}

//Процедура инициализации звукового модуля
void mp3_init(void)
{
	uint16_t crc = 0;
	uint16_t j = 0;
	
	//заполняем массив
	mp3[j++] = DF_START;
	mp3[j++] = DF_VER;
	mp3[j++] = DF_LEN;
	mp3[j++] = DF_CMD_INIT;
	mp3[j++] = 0x00;
	mp3[j++] = 0x00;
	mp3[j++] = 0x02;
	
	//рассчитываем контрольную сумму
	for (uint8_t i = 1; i < 7; i++)
	{
		crc += mp3[i];
	}
	crc = UINT16_MAX - crc + 1;

	mp3[j++] = (uint8_t)(crc >> 8);
	mp3[j++] = (uint8_t)crc;
	mp3[j++] = DF_ENF;
	
	//отправляем данные в порт
	usart3_print(mp3);
	
	//пауза в 1 секунду
	uint16_t count = 1000;
	while(count--)
	{
		#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
		#endif
		LL_mDelay(1);
	}
}

//Процедура воспроизведения файла под номером
void mp3_play(uint8_t mp3_num)
{
	uint16_t crc = 0;
	uint16_t j = 0;

	//заполняем массив
	mp3[j++] = DF_START;
	mp3[j++] = DF_VER;
	mp3[j++] = DF_LEN;
	mp3[j++] = DF_CMD_NUM;
	mp3[j++] = 0x00;
	mp3[j++] = (uint8_t)(mp3_num >> 8);
	mp3[j++] = (uint8_t)mp3_num;

	//рассчитываем контрольную сумму
	for (uint8_t i = 1; i < 7; i++)
	{
		crc += mp3[i];
	}
	crc = UINT16_MAX - crc + 1;

	mp3[j++] = (uint8_t)(crc >> 8);
	mp3[j++] = (uint8_t)crc;
	mp3[j++] = DF_ENF;
	
	//отправляем данные в порт
	usart3_print(mp3);
}

//Процедура установки громкости звука
void mp3_volume(uint8_t vol)
{
	uint16_t crc = 0;
	uint16_t j = 0;
		
	//заполняем массив
	mp3[j++] = DF_START;
	mp3[j++] = DF_VER;
	mp3[j++] = DF_LEN;
	mp3[j++] = DF_CMD_VOL;
	mp3[j++] = 0x00;
	mp3[j++] = 0x00;
	mp3[j++] = vol;

	//рассчитываем контрольную сумму
	for (uint8_t i = 1; i < 7; i++)
	{
		crc += mp3[i];
	}
	crc = UINT16_MAX - crc + 1;

	mp3[j++] = (uint8_t)(crc >> 8);
	mp3[j++] = (uint8_t)crc;
	mp3[j++] = DF_ENF;
	
	//отправляем данные в порт
	usart3_print(mp3);
	//пауза в 100 мс
	uint16_t count = 100;
	while(count--)
	{
		#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
		#endif
		LL_mDelay(1);
	}
}
