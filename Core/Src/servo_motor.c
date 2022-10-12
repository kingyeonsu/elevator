#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

extern volatile int TIM11_10ms_servo_motor_counter;
extern TIM_HandleTypeDef htim2;
//extern int distance;
extern int door_open;

int arm_rotate_indicator=0;

// 84MHz / 1680 = 50kHz
// 1 pulse time T = 1/f = 1/50kHz = 0.02ms = 20us
// 1ms		= 20us * 10		--> 0	도 회전
// 1.5ms 	= 20us * 75 	--> 90	도 회전
// 2ms	 	= 20us * 110 	--> 180 도 회전

void servo_motor_control_main()
{
#if 1
	if (!door_open)
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 120);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 60);
	}
#endif

#if 0
	while(1)
	{
		if (TIM11_10ms_servo_motor_counter >= 100)	// 1sec
		{
			TIM11_10ms_servo_motor_counter=0;

			if (arm_rotate_indicator)
			{
				//	180	도 회전
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 100);	//	100 pulse
			}
			else
			{
				//	0	도 회전
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 50);	//	50	pulse
			}
			arm_rotate_indicator = !arm_rotate_indicator;
		}
	}
#endif
#if 0
	while(1)
	{
		//	180	도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 100);	//	100 pulse
		HAL_Delay(1000);	//	1초 유지
		//	0	도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 50);	//	50	pulse
		HAL_Delay(1000);	//	1초 유지
		//	90	도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 75);	//	75	pulse
		HAL_Delay(1000);	//	1초 유지
	}
#endif
}
