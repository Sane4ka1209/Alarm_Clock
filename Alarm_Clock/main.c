#include "stm32f10x.h" // ����������� ���������� STM32
#include <stdio.h>
#include "Board_LED.h" // ����������� ���������� ��� ���������� ������������
#include "Board_Buttons.h" // ����������� ���������� ��� ������ � ��������

// ����������� ��������� �������
struct time{
	uint32_t hours; // ����
	uint32_t minutes; // ������
	uint32_t seconds; // �������
	uint32_t ms; // ������������
} typedef time;

// ������� ��� ���������� �������
void update_time(uint32_t ms, time *t){
	t->ms += ms; // ��������� ������������ � �������� �������
	if (t->ms >= 1000)
	{
			t->seconds += t->ms / 1000; // �������������� � �������
			t->ms %= 1000; // ����� �����������
			if (t->seconds >= 60) {
				t->minutes += t->seconds / 60; // �������������� � ������
				t->seconds %= 60; // ����� ������
				if (t->minutes >= 60) {
					t->hours += t->minutes / 60; // �������������� � ����
					t->minutes %= 60; // ����� �����
					if (t->hours >= 24) {
						t->seconds %= 24;	// ����� ����� (24-������� ������)
					}
				}
			}
	}
}
// ������� ��� ��������� �������
void set_time(time *st, uint32_t h, uint32_t m, uint32_t s) {
	st->hours = h; // ��������� �����
	st->minutes = m; // ��������� �����
	st->seconds = s; // ��������� ������
	st->ms = 0; // ���������� ������������
}

// ���������� ���������� ��� �������� ������� � ������� ����������
time cur_time;
time alarm_time;
uint32_t stand_up_time;
uint32_t time_signal = 1000; // �������� ������� �������
char standUP = 0; // ������ ��� ����������
uint32_t ms_in_min = 60;

// ������� ��� �������� ��������� �������
char is_equal(time *t1, time*t2){
	return (t1->hours == t2->hours) && (t1->minutes == t2->minutes);
}

// ������� ��� �������� ��������� �����
char btn_press(){
	return GPIOB->ODR;
}
// ������� ��������
void Delay(uint32_t dlyTicks){
	uint32_t curTicks;
	
	curTicks = cur_time.ms;
	while((cur_time.ms - curTicks) < dlyTicks) __NOP();
}

// ������� ���������� �����������
void led(char on){
	if (on) {
		LED_On(0); // ��������� ����������
	} else {
		LED_Off(0); // ���������� ����������
	}
}

// ���������� ���������� SysTick
void SysTick_Handler() {
	update_time(1, &cur_time); // ���������� �������� ������� ������ ������������
	// ��������, ����� �� �������� ���������
	if (standUP){
		stand_up_time++;
		if (stand_up_time == time_signal) {
			standUP = 0;
			stand_up_time = 0;
			led(0);
		}
	}
	// �������� �� ���������� �������� ������� � ����������
	if (is_equal(&cur_time, &alarm_time) && cur_time.seconds == 0){
		standUP = 1; // ��������� ������� ����������
		led(1); // ��������� ����������
	}
	// �������� ������� ������ ��� ���������� ������� ����������
	if (btn_press()){
		update_time(ms_in_min, &alarm_time);
	}
}

// ������� �������
int main(){
	set_time(&cur_time, 8, 29, 59); // ��������� �������� �������
	set_time(&alarm_time, 8, 30, 0); // ��������� ������� ����������
	
	LED_Initialize(); // ������������� �����������
	Buttons_Initialize(); // ������������� ������
	SysTick_Config(SystemCoreClock / 1000); // ������������ ������� SysTick
	
	while(1){
		// �������� ���� ���������
	}
}