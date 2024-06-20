/**
********************************************************************************
*
* Макетное ПО для сигнализатора ультразвукового СУЗ 1
* Микроконтроллер STM32f030F4P6
* Разработал: Мелехин В.Н.
* Модуль обработки сигналов и фильтрации
*
********************************************************************************
*/

#include "filters.h"

//---Среднее арифметическое-----------------------------------------------------
int midArifm(int *data) 
{
  long sum = 0;                       		// локальная переменная sum
  for (int i = 0; i < NUM_OF_DATA; i++)  	// согласно количеству усреднений
    sum += data[i];                  			// суммируем значения с любого датчика в переменную sum
  return ((float)sum / NUM_OF_DATA);			// вычисляем значение
}

//---Бегущее среднее арифметическое---------------------------------------------
float runMiddleArifm(int newVal) 
{  	// принимает новое значение
  static int idx = 0;                		// индекс
  static float valArray[NUM_OF_DATA];   // массив
  valArray[idx] = newVal;             	// пишем каждый раз в новую ячейку
  if (++idx >= NUM_OF_DATA) idx = 0;    // перезаписывая самое старое значение
  float average = 0;                  	// обнуляем среднее
  for (int i = 0; i < NUM_OF_DATA; i++) {
    average += valArray[i];           	// суммируем
  }
  return (float)average / NUM_OF_DATA;  // возвращаем
}
//---Оптимизированное бегущее среднее-------------------------------------------
float runMiddleArifmOptim(float newVal) 
{
  static int t = 0;								//вспомогательная переменная
  static int vals[NUM_OF_DATA];		//массив данных
  static int average = 0;					//переменная среднего
  if (++t >= NUM_OF_DATA) t = 0; 	// перемотка t
  average -= vals[t];         		// вычитаем старое
  average += newVal;          		// прибавляем новое
  vals[t] = newVal;           		// запоминаем в массив
  return ((float)average / NUM_OF_DATA);//возвращаем среднее
}

//---Медиана на три значения----------------------------------------------------
int median(int newVal) 
{
  static int buf[3];				//буфер для трех значений
  static int count = 0;			//счетчик
  buf[count] = newVal;			//записываем очередное значение буфера
  if (++count >= 3) count = 0;
  return (buf[0] < buf[1]) ? ((buf[1] < buf[2]) ? buf[1] : ((buf[2] < buf[0]) ? buf[0] : buf[2])) : ((buf[0] < buf[2]) ? buf[0] : ((buf[2] < buf[1]) ? buf[1] : buf[2])); //выбираем медианное значение
}

//---Mедианный фильтр на N значений---------------------------------------------
float findMedianN(int newVal) 
{
  static float buffer[NUM_MIDDLE];  // статический буфер
  static int count = 0;							// статический счетчик 
  buffer[count] = newVal;						//присваиваем новое значение нулевому элементу буфера
  //если текущий элемент не предпоследний и текущее его значение больше последующего
	if ((count < NUM_MIDDLE - 1) && (buffer[count] > buffer[count + 1])) 
	{
    for (int i = count; i < NUM_MIDDLE - 1; i++) 	//по всему буферу кроме последнего элемента
		{
      if (buffer[i] > buffer[i + 1]) 							//сравнение значения с последующим
			{
        float buff = buffer[i];										//сохраняем значение
        buffer[i] = buffer[i + 1];								//меняем местами текущее значение и последующее
        buffer[i + 1] = buff;
      }
    }
  } 
	//если элемент предпоследний или значение меньше последующего
	else 
	{
    if ((count > 0) && (buffer[count - 1] > buffer[count])) //если это не первый элемент и предыдущее значение больше него
		{
      for (int i = count; i > 0; i--) 				//от текущего элемента до нулевого
			{
        if (buffer[i] < buffer[i - 1]) 				//сравнение с предыдущим
				{//если новое меньше предыдущего, меняем местами
          float buff = buffer[i];							
          buffer[i] = buffer[i - 1];					
          buffer[i - 1] = buff;								
        }
      }
    }
  }
  if (++count >= NUM_MIDDLE) count = 0;				//обнуление счетчика
  return buffer[(int)NUM_MIDDLE / 2];					//возвращаем значение из середины буфера
}

//---Сортировка данных по возрастанию-------------------------------------------
void sort_data(uint32_t *data) 
{
	for(uint8_t i = 0 ; i < NUM_OF_DATA - 1; i++) 
	{ 
		for(uint8_t j = 0 ; j < NUM_OF_DATA - i - 1 ; j++) 
		{
			if(data[j] > data[j+1]) 			//сравниваем два соседних элемента.
			{           									//если они идут в неправильном порядке, то  
				uint32_t tmp = data[j];					//меняем их местами. 
        data[j] = data[j+1];
        data[j+1] = tmp; 
      }
    }
  }
}

