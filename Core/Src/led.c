#include "led.h"
#include "button.h"

int led_on=0;

extern uint8_t prev_button0_state; // button.c
extern uint8_t prev_button1_state; // button.c

extern volatile int TIM11_10ms_counter;

void button1_ledall_on_off()
{
	static int button1_count=0;   // 로컬변수 이지만 static를 붙이면 전역변수 처럼 동작

	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
	{
		button1_count++;
		button1_count %=2;
		if (button1_count)
		{
			led_all_on();
		}
		else
		{
			led_all_off();
		}
	}
}

void led2_toggle()
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // DEMO b'd  LED2

}
// 0 -> 1 ->  2 -> 3 --- 7
// 해당 되는 LED만 on  ( 왼쪽 <--- 오른쪽  )
void led_on1_up()
{
	for (int i=0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i, GPIO_PIN_SET);
		HAL_Delay(200);
	}
}
// 7 -> 6 -> 5 -> 4 ---- 0
// 해당 되는 LED만 on  ( 왼쪽 ---> 오른쪽  )
void led_on1_down()
{
	for (int i=0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, GPIO_PIN_SET);
		HAL_Delay(200);
	}
}

// 0 -> 01 -> 012 -> 0123 --- 01234567
void led_on_up()
{
	HAL_GPIO_WritePin(GPIOB, 0x01 << led_on, GPIO_PIN_SET);

	if (led_on >= 8)
	{
		led_on=0;
		led_all_off();
	}

}

// 7 -> 76 -> 765 -> 7654 --- 7654321
void led_on_down()
{
	HAL_GPIO_WritePin(GPIOB, 0x80 >> led_on, GPIO_PIN_SET);

	if (led_on >= 8)
	{
		led_on=0;
		led_all_off();
	}
}

void led_all_on()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
}

void led_all_off()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
}
