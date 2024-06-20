/*
* dfplayer.c
* ���������� ������ �� �������� ������� dfplayer.
* ��������������� stm32f103c8t6 ������������ usart3
*/

#include "dfplayer.h"
#include "main.h"

//������ ��� �������� ������
uint8_t mp3[ARR_SIZE];

//��������� �������� ������ �����
void usart3_transmit(uint8_t data) 
{
	while (!LL_USART_IsActiveFlag_TXE(USART3)) {}		//���� �������� ����������� �����
  LL_USART_TransmitData8(USART3,data);						//���������� ��������� ���� � ����
}

//��������� �������� �������
void usart3_print(uint8_t *str)
{	
	for (int i = 0; i < ARR_SIZE; i++)
	{
		usart3_transmit(str[i]); //���������� ��������� ����
	}
}

//��������� ������������� ��������� ������
void mp3_init(void)
{
	uint16_t crc = 0;
	uint16_t j = 0;
	
	//��������� ������
	mp3[j++] = DF_START;
	mp3[j++] = DF_VER;
	mp3[j++] = DF_LEN;
	mp3[j++] = DF_CMD_INIT;
	mp3[j++] = 0x00;
	mp3[j++] = 0x00;
	mp3[j++] = 0x02;
	
	//������������ ����������� �����
	for (uint8_t i = 1; i < 7; i++)
	{
		crc += mp3[i];
	}
	crc = UINT16_MAX - crc + 1;

	mp3[j++] = (uint8_t)(crc >> 8);
	mp3[j++] = (uint8_t)crc;
	mp3[j++] = DF_ENF;
	
	//���������� ������ � ����
	usart3_print(mp3);
	
	//����� � 1 �������
	uint16_t count = 1000;
	while(count--)
	{
		#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
		#endif
		LL_mDelay(1);
	}
}

//��������� ��������������� ����� ��� �������
void mp3_play(uint8_t mp3_num)
{
	uint16_t crc = 0;
	uint16_t j = 0;

	//��������� ������
	mp3[j++] = DF_START;
	mp3[j++] = DF_VER;
	mp3[j++] = DF_LEN;
	mp3[j++] = DF_CMD_NUM;
	mp3[j++] = 0x00;
	mp3[j++] = (uint8_t)(mp3_num >> 8);
	mp3[j++] = (uint8_t)mp3_num;

	//������������ ����������� �����
	for (uint8_t i = 1; i < 7; i++)
	{
		crc += mp3[i];
	}
	crc = UINT16_MAX - crc + 1;

	mp3[j++] = (uint8_t)(crc >> 8);
	mp3[j++] = (uint8_t)crc;
	mp3[j++] = DF_ENF;
	
	//���������� ������ � ����
	usart3_print(mp3);
}

//��������� ��������� ��������� �����
void mp3_volume(uint8_t vol)
{
	uint16_t crc = 0;
	uint16_t j = 0;
		
	//��������� ������
	mp3[j++] = DF_START;
	mp3[j++] = DF_VER;
	mp3[j++] = DF_LEN;
	mp3[j++] = DF_CMD_VOL;
	mp3[j++] = 0x00;
	mp3[j++] = 0x00;
	mp3[j++] = vol;

	//������������ ����������� �����
	for (uint8_t i = 1; i < 7; i++)
	{
		crc += mp3[i];
	}
	crc = UINT16_MAX - crc + 1;

	mp3[j++] = (uint8_t)(crc >> 8);
	mp3[j++] = (uint8_t)crc;
	mp3[j++] = DF_ENF;
	
	//���������� ������ � ����
	usart3_print(mp3);
	//����� � 100 ��
	uint16_t count = 100;
	while(count--)
	{
		#ifndef DEBUG_MODE
		LL_IWDG_ReloadCounter(IWDG);
		#endif
		LL_mDelay(1);
	}
}
