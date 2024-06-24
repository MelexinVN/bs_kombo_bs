# БС ("Брейн система")

<img align="left" width=80 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/logo_red.png" />

В рамках проекта ["КомБО"](https://github.com/MelexinVN/bs_kombo) разработана электронная система для проведения интеллектуальных игр.
Система без использования проводной связи позволяет устанавливать точную последовательность нажатий до 20 игровых кнопок.

As part of the project ["КомБО"](https://github.com/MelexinVN/bs_kombo), an electronic system for conducting intellectual games has been developed.
The wireless system allows you to set the exact sequence of pressing up to 20 game buttons.

<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-20_09-03-27%20(2).jpg" />

### Основные характеристики системы

- беспроводная связь базы с игровыми кнопками
- частота радиоканала - 2,4-2,56 ГГц
- дальность связи - 30 м в помещении (может зависеть от зашумленности помещения радиосигналами, перегородок и преград)
- количество кнопок - до 20 шт
- питание каждой игровой кнопки осуществляется от аккумулятора
- питание базы может осуществляться через USB кабель, а также автономно - от аккумулятора

### Main characteristics of the system

- wireless connection of the base with game buttons
- radio channel frequency - 2.4-2.56 GHz
- communication range - 30 m indoors (may depend on the noise level of the room due to radio signals, partitions and obstacles)
- number of buttons - up to 20 pcs.
- each game button is powered by a battery
- the base can be powered via a USB cable, or autonomously - from a battery

### Основные функции системы

- отображение хода игры на дисплее и в специальной программе на ПК
- воспроизведение звуковых файлов (mp3) при различных событиях как через ПК, так и автономно
- возможность замены звуковых файлов
- отдельная индикация первой нажатой игровой кнопки
- возможность перехода очереди нажатых кнопок
- возможность запуска времени и фиксации фальстарта

### Basic system functions

- display of the game progress on the display and in a special program on the PC
- playback of sound files (mp3) for various events both via a PC and autonomously
- ability to replace sound files
- separate indication of the first pressed game button
- the ability to move through the queue of pressed buttons
- ability to start time and fix false start

### Состав системы System composition

Система состоит из базы и комплекта игровых кнопок.

The system consists of a base and a set of game buttons.

База содержит контроллер (stm32f103c8t6 на плате bluepill), дисплей LCD2004, радиомодуль nrf24l01+, кнопки управления, USB-мост CP2102, звуковой модуль DFPlayer mini и схему питания.

The base contains a controller (stm32f103c8t6 on a bluepill board), an LCD2004 display, an nrf24l01+ radio module, control buttons, a CP2102 USB bridge, a DFPlayer mini sound module and a power circuit.

<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-20_09-03-27%20(2).jpg" />
<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-20_09-03-27%20(3).jpg" />

Каждая игровая кнопка содержит схему управления на микроконтроллере stm32f030f4p6 с радиомодулем nrf24l01+, схему питания и аркадную кнопку.

Each game button contains a control circuit on a stm32f030f4p6 microcontroller with an nrf24l01+ radio module, a power circuit and an arcade button.

<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-19_20-01-37.jpg" />
<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-19_20-01-38%20(2).jpg" />
<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-20_09-03-27.png" />

### Схема соединений игровой кнопки Game Button Connection Diagram

<img align="center" width=500 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/button_schematic.png" />


### Программное обеспечение

- МикроПО базы и игровой кнопки написаны в Keil uVision, есть вариант в STM32CubeIDE.
- ПО для ОС Windows написано в Visual Studio.
Все [исходники](https://github.com/MelexinVN/bs_kombo_bs/blob/main/soft) доступны в репозитории.

### Software

- The microsoftware base and buttons are written in Keil uVision, there is an option in STM32CubeIDE.
- Software for Windows OS is written in Visual Studio.
All [sources](https://github.com/MelexinVN/bs_kombo_bs/blob/main/soft) are available in the repository.

### Печатные платы

Печатные платы разработаны в KiCad 7.0, [исходники и гребер файлы](https://github.com/MelexinVN/bs_kombo_bs/blob/main/pcb) также есть в репозитории - [плата контроллера кнопки](https://github.com/MelexinVN/bs_kombo_bs/blob/main/pcb/bs_button_v.1.0), [плата базы](https://github.com/MelexinVN/bs_kombo_bs/blob/main/pcb/bs_base_v.1.0)

### Printed circuit boards

The printed circuit boards were developed in KiCad 7.0, [source and comb files](https://github.com/MelexinVN/bs_kombo_bs/blob/main/pcb) also available in the repository - [button controller board](https://github.com/MelexinVN/bs_kombo_bs/blob/main/pcb/bs_button_v.1.0), [base board](https://github.com/MelexinVN/bs_kombo_bs/blob/main/pcb/bs_base_v.1.0)

### Корпуса

Корпуса и вспомогательные детали изготовлены при помощи 3Д печати.
Модели корпусов разработаны в программе OpenSCAD, [исходники и STL файлы](https://github.com/MelexinVN/bs_kombo_bs/blob/main/3d) доступны в репозитории - [корпус кнопки](https://github.com/MelexinVN/bs_kombo_bs/blob/main/3d/button_box_v.1.0), [корпус и детали базы](https://github.com/MelexinVN/bs_kombo_bs/blob/main/3d/base_box_v.1.0)

### Housings

The housings and auxiliary parts are made using 3D printing.
Case models were developed in the OpenSCAD program, [sources and STL files](https://github.com/MelexinVN/bs_kombo_bs/blob/main/3d) are available in the repository - [button case](https://github.com/MelexinVN/bs_kombo_bs/blob/main/3d/button_box_v.1.0), [body and base details](https://github.com/MelexinVN/bs_kombo_bs/blob/main/3d/base_box_v.1.0)


### Краткое описание работы системы

После включения питания каждой игровой кнопки, ее светодиод производит три коротких моргания (при неисправности радиомодуля - одно). После чего игровая кнопка готова к работе
*Примечание: таким образом можно оценить состояние питания игровой кнопки - при низкой яркости аккумулятор разряжен, при отсутствии морганий возможна неисправность схемы питания*

После включения питания базы на дисплей выводится информация о устройстве, версии ПО, состоянию радиомодуля, номере канала.
Через секунду база отправляет в эфир команду общего сброса. База готова к работе.

<img align="center" width=300 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-20_13-56-21.jpg" />
На верхней строчке дисплея номера кнопок от "0" до "j"

По нажатию кнопки сброса система возвращается в исходное состояние и также отправляет в эфир команду общего сброса.
**Рекомендуется нажимать кнопку сброса после включения всех кнопок. Если какая то из кнопок была включена после сброса системы, возможна ошибка установления последовательности нажатий!**
Каждая игровая кнопка получившая команду сброса моргает светодиодом один раз.

После того как вся система приведена в готовность, нажатие каждой кнопки будет составлять очередь на второй строке дисплея - также от "0" до "j" и будет вопроизводиться звуковой файл. Светодиод первой нажатой игровой кнопки будет моргать, остальные - светиться.

После нажатия кнопки перехода очереди светодиод первой в очереди игровой кнопки гаснет, моргать начинает следующая в очереди.

Нажатие кнопки пуска времени приводит к началу отсчета времени - воспроизводится соответствующий звуковой сигнал, а на дисплее появляется таймер, отсчитывающий секунды. 

Нажатие кнопки установки приводит к изменению звукового файла, воспроизводимого при нажатии игровой кнопки **ДО** пуска времени. При этом на дисплее появится название звука, один из пяти вариантов.
*Примечание: выбранный звук сохранится после нажатия кнопки сброса, но вернется к стандартному при отключении питания*

Источником звуков при автономной работе является звуковой модуль, аудиовыход которого выведен на корпус в виде аудио гнезда 3,5 мм. Звуковой модуль воспроизводит звуковые файлы формата mp3, записанные на карте памяти microSD. Файлы можно заменить, но они должны иметь те же имена. Стандартное содержимое карты памяти можно найти [тут](https://disk.yandex.ru/d/iv4f7-gtp3TiZA). Файлы должны находиться в корне карты памяти.

**Предусмотрена возможность отображения хода игры и управления системой с помощью программы верхнего уровня. Описание программы и инструкцию можно найти [тут](https://github.com/MelexinVN/bs_kombo_bs/tree/main/soft/Visual%20Studio)**

### Brief description of the system operation

After turning on the power of each game button, its LED produces three short blinks (if the radio module is faulty - one). After which the game button is ready for use
*Note: this way you can evaluate the power status of the game button - if the brightness is low, the battery is discharged, if there is no blinking, the power circuit may be faulty*

After turning on the power of the base, information about the device, software version, radio module status, and channel number is displayed.
A second later, the base sends a general reset command over the air. The base is ready for use.

<img align="center" width=300 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/photo_2024-06-20_13-56-21.jpg" />
On the top line of the display there are button numbers from “0” to “j”

When the reset button is pressed, the system returns to its original state and also sends a general reset command over the air.
**It is recommended to press the reset button after all buttons have been turned on. If any of the buttons was turned on after resetting the system, there may be an error in establishing the sequence of presses!**
Each game button that receives a reset command blinks its LED once.

Once the entire system is ready, pressing each button will line up on the second line of the display - also from "0" to "j" - and a sound file will be played. The LED of the first pressed game button will blink, the others will light up.

After pressing the queue switch button, the LED of the first game button in the queue goes out, and the next one in the queue starts blinking.

Pressing the start time button starts the countdown - a corresponding sound signal is played, and a timer appears on the display, counting down the seconds.

Pressing the set button changes the sound file played when the game button **BEFORE** the time starts is pressed. In this case, the name of the sound will appear on the display, one of five options.
*Note: The selected sound will remain after pressing the reset button, but will return to standard when the power is turned off*

The source of sounds during autonomous operation is the sound module, the audio output of which is connected to the case in the form of a 3.5 mm audio jack. The sound module plays mp3 audio files recorded on a microSD memory card. The files can be replaced, but they must have the same names. The standard contents of the memory card can be found [here](https://disk.yandex.ru/d/iv4f7-gtp3TiZA). The files must be located in the root of the memory card.

**It is possible to display the progress of the game and control the system using a top-level program. A description of the program and instructions can be found [here](https://github.com/MelexinVN/bs_kombo_bs/tree/main/soft/Visual%20Studio)**


### Планы по развитию:
- **Переход на более компактную базу с цветным дисплеем**
- **Отображение информации о наличии кнопок в эфире**
- **Отображение информации об уровнях заряда кнопок и базы**
- **Добавление возможности перехода на другие каналы в случе зашумленности помещения**
- **Более гибкая настройка программы верхнего уровня для большей информативности и для разных видов интеллектуальных игр**

### Development plans:
- **Transition to a more compact base with a color display**
- **Displaying information about the presence of buttons on air**
- **Display information about charge levels of buttons and base**
- **Add the ability to switch to other channels in case of a noisy room**
- **More flexible configuration of the top-level program for greater information content and for different types of intellectual games**


## Наши ресурсы:

### Основной репозиторий: [bs_kombo](https://github.com/MelexinVN/bs_kombo)

### Cайт проекта: [bs-kombo.tilda.ws](http://bs-kombo.tilda.ws/)

### Электронная почта: 		    [bskombo@yandex.ru](bskombo@yandex.ru)

### Социальные сети:

- паблик ВКонтакте: 			  [vk.com/bs_kombo](https://vk.com/bs_kombo)
- Telegram-канал: 	        [t.me/bskombo](https://t.me/bskombo)
- Telegram-чат: 	          [t.me/bs_kombo_chat](https://t.me/bs_kombo_chat)
- канал Яндекс Дзен: 	      [dzen.ru/bs_kombo](https://dzen.ru/bs_kombo)


<img align="left" width=150 src="https://github.com/MelexinVN/bs_kombo_bs/blob/main/media/%D0%9C%D0%9D%D0%A5%D0%A1.png" />

### Авторы всегда рады отзывам и любым конструктивным предложениям по улучшению проекта! 
### The authors are always happy to receive feedback and any constructive suggestions for improving the project!

## Лицензия
Проект распространяется под лицензией Creative Commons Attribution-ShareAlike 4.0 [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/), 
а программное обеспечение - под лицензией GNU General Public License v3.0 [GPL-3.0 license](https://github.com/MelexinVN/bs_kombo/blob/main/LICENSE)

## License
This project is licensed under the Creative Commons 4.0 license with Attribution-ShareAlike [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/), 
and the software is licensed under the GNU General Public License v3.0 [GPL-3.0 license](https://github.com/MelexinVN/bs_kombo/blob/main/LICENSE)
