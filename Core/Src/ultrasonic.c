#include "main.h"	// for GPIO, HAL
#include <stdio.h>

#define TRIG_PORT	GPIOC
#define TRIG_PIN	GPIO_PIN_5

// 여기는 cubeIDE와 만나지 않는 영역이다.
// 한글로 주석 달아도 깨지지 않는다.
int door_open=0;
int distance=10;			// 거리 측정값을 저장하는 변수
int input_capture_flag;	// rising edge/falling edge 를 detect 했을때 check 하는 flag
extern void delay_us(unsigned long us);

// rising edge/falling edge Interrupt 가 발생 되면 이곳으로 들어온다.
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t is_first_capture=0;

	if	(htim->Instance == TIM1)
	{
		if	(is_first_capture == 0)	// rising edge detect !!!
		{
			__HAL_TIM_SET_COUNTER(htim, 0);	// timer clear
			is_first_capture=1;	// rising edge detect indicator set
		}
		else if (is_first_capture == 1)	// falling edge detect !!!
		{
			is_first_capture=0;
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);	// 현재까지 COUNT 된 값을 distance 값으로
			input_capture_flag=1;
		}
	}
}

void make_trigger()
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

extern volatile int TIM11_10ms_ultrasonic_counter;

void ultrasonic_processing()
{
	if (TIM11_10ms_ultrasonic_counter >= 5)	// 50ms
	{
		TIM11_10ms_ultrasonic_counter=0;
		make_trigger();
		if (input_capture_flag)
		{
			input_capture_flag=0;
			distance = distance*0.034/2;	// 1us 당 0.034 cm 를 이동 // 왕복값 return --> /2
//			printf("distance: %d\n", distance);

			if (distance < 5)
			{
				door_open=1;
			}
		}
	}
}



