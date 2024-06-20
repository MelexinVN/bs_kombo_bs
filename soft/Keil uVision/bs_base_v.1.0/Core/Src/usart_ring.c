/**
********************************************************************************
*
*
********************************************************************************
*/

#include "usart_ring.h"
#include "main.h"

volatile uint16_t rx_buffer_head = 0;								//указатель заголовка буфера
volatile uint16_t rx_buffer_tail = 0;								//указатель конца буфера
unsigned char rx_buffer[UART_RX_BUFFER_SIZE] = {0,};//приемный буфер

//---Очистка буфера-------------------------------------------------------------
void clear_uart_buff(void)															
{
  LL_USART_DisableIT_RXNE(USART1);								//Запрещаем прерывания USART
	rx_buffer_head = 0;																//обнуляем заголовок
	rx_buffer_tail = 0;																//обнуляем конец буфера
	LL_USART_EnableIT_RXNE(USART1);									//Разрешаем прерывания USART
}

//---Определение наличия данных в буфере----------------------------------------
uint16_t uart_available(void)													
{
//Возвращаем остаток от деления, который будет равен количеству данных в буфере
	return ((uint16_t)(UART_RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail)) % UART_RX_BUFFER_SIZE;
}

//---Чтение из буфера-----------------------------------------------------------
uint8_t uart_read(void) 															
{
	if(rx_buffer_head == rx_buffer_tail) 							//если нечего читать
	{
		return 0;																				//возвращаем 0
	}
	else																							//если есть что читать
	{
		uint8_t c = rx_buffer[rx_buffer_tail]; 					//берем байт из конца буфера
//Перемещаем конец буфера
		rx_buffer_tail = (uint16_t)(rx_buffer_tail + 1) % UART_RX_BUFFER_SIZE; 
		return c;																				//возвращаем байт
	}
}
