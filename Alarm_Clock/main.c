#include "stm32f10x.h" // Подключение библиотеки STM32
#include <stdio.h>
#include "Board_LED.h" // Подключение библиотеки для управления светодиодами
#include "Board_Buttons.h" // Подключение библиотеки для работы с кнопками

// Определение структуры времени
struct time{
	uint32_t hours; // Часы
	uint32_t minutes; // Минуты
	uint32_t seconds; // Секунды
	uint32_t ms; // Миллисекунды
} typedef time;

// Функция для обновления времени
void update_time(uint32_t ms, time *t){
	t->ms += ms; // Добавляем миллисекунды к текущему времени
	if (t->ms >= 1000)
	{
			t->seconds += t->ms / 1000; // Преобразование в секунды
			t->ms %= 1000; // Сброс миллисекунд
			if (t->seconds >= 60) {
				t->minutes += t->seconds / 60; // Преобразование в минуты
				t->seconds %= 60; // Сброс секунд
				if (t->minutes >= 60) {
					t->hours += t->minutes / 60; // Преобразование в часы
					t->minutes %= 60; // Сброс минут
					if (t->hours >= 24) {
						t->seconds %= 24;	// Сброс часов (24-часовой формат)
					}
				}
			}
	}
}
// Функция для установки времени
void set_time(time *st, uint32_t h, uint32_t m, uint32_t s) {
	st->hours = h; // Установка часов
	st->minutes = m; // Установка минут
	st->seconds = s; // Установка секунд
	st->ms = 0; // Сбрасываем миллисекунды
}

// Объявление переменных для текущего времени и времени будильника
time cur_time;
time alarm_time;
uint32_t stand_up_time;
uint32_t time_signal = 1000; // Интервал времени сигнала
char standUP = 0; // Сигнал для будильника
uint32_t ms_in_min = 60;

// Функция для проверки равенства времени
char is_equal(time *t1, time*t2){
	return (t1->hours == t2->hours) && (t1->minutes == t2->minutes);
}

// Функция для проверки состояния кнопк
char btn_press(){
	return GPIOB->ODR;
}
// Функция задержки
void Delay(uint32_t dlyTicks){
	uint32_t curTicks;
	
	curTicks = cur_time.ms;
	while((cur_time.ms - curTicks) < dlyTicks) __NOP();
}

// Функция управления светодиодом
void led(char on){
	if (on) {
		LED_On(0); // Включение светодиода
	} else {
		LED_Off(0); // Выключение светодиода
	}
}

// Обработчик прерывания SysTick
void SysTick_Handler() {
	update_time(1, &cur_time); // Обновление текущего времени каждую миллисекунду
	// Проверка, нужно ли включить будильник
	if (standUP){
		stand_up_time++;
		if (stand_up_time == time_signal) {
			standUP = 0;
			stand_up_time = 0;
			led(0);
		}
	}
	// Проверка на совпадение текущего времени и будильника
	if (is_equal(&cur_time, &alarm_time) && cur_time.seconds == 0){
		standUP = 1; // Включение сигнала будильника
		led(1); // Включение светодиода
	}
	// Проверка нажатия кнопки для обновления времени будильника
	if (btn_press()){
		update_time(ms_in_min, &alarm_time);
	}
}

// Главная функция
int main(){
	set_time(&cur_time, 8, 29, 59); // Установка текущего времени
	set_time(&alarm_time, 8, 30, 0); // Установка времени будильника
	
	LED_Initialize(); // Инициализация светодиодов
	Buttons_Initialize(); // Инициализация кнопок
	SysTick_Config(SystemCoreClock / 1000); // Конфигурация таймера SysTick
	
	while(1){
		// Основной цикл программы
	}
}